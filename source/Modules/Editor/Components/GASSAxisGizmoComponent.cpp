#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/EditorSceneManager.h"
#include "Modules/Editor/EditorMessages.h"
#include "Modules/Editor/ToolSystem/MouseToolController.h"
#include "GASSAxisGizmoComponent.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Utils/GASSException.h"
#include "Core/Math/GASSRay.h"
#include "Core/Math/GASSPlane.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSIViewport.h"
#include "Sim/Interface/GASSIGraphicsSystem.h"

#define MOVMENT_EPSILON 0.0000001
#define MAX_SCALE_DISTANCE 300.0 //meters
#define MIN_SCALE_DISTANCE 0.1 //meters
#define GIZMO_SENDER 999

namespace GASS
{
	AxisGizmoComponent::AxisGizmoComponent() : m_MeshData(new GraphicsMesh), m_Color(1,0,0,1),
		m_Size(5),
		m_Type(GT_AXIS),
		m_Highlight(true),
		m_LastDist(0), 
		m_Mode(GM_WORLD),
		m_GridDist(1.0),
		m_Active(false)
	{

	}

	AxisGizmoComponent::~AxisGizmoComponent()
	{

	}

	void AxisGizmoComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("AxisGizmoComponent",new Creator<AxisGizmoComponent, Component>);
		RegisterProperty<float>("Size",&AxisGizmoComponent::GetSize, &AxisGizmoComponent::SetSize);
		RegisterProperty<ColorRGBA>("Color",&AxisGizmoComponent::GetColor, &AxisGizmoComponent::SetColor);
		RegisterProperty<std::string>("Type",&AxisGizmoComponent::GetType, &AxisGizmoComponent::SetType);
	}

	void AxisGizmoComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(AxisGizmoComponent::OnLocationLoaded,LocationLoadedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(AxisGizmoComponent::OnTransformation,TransformationChangedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(AxisGizmoComponent::OnWorldPosition,WorldPositionRequest,0));
		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS(AxisGizmoComponent::OnNewCursorInfo, CursorMovedOverSceneEvent, 1000));
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(AxisGizmoComponent::OnEditMode,EditModeChangedEvent,0));
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(AxisGizmoComponent::OnChangeGridRequest,ChangeGridRequest,0));

		m_EditorSceneManager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<EditorSceneManager>();
		m_Mode = m_EditorSceneManager->GetMouseToolController()->GetEditMode();

		//Change geometry flags
		GeometryComponentPtr gc = GetSceneObject()->GetFirstComponentByClass<IGeometryComponent>();
		if(m_Type == GT_AXIS || m_Type == GT_PLANE)
			gc->SetGeometryFlags(GEOMETRY_FLAG_GIZMO);
		else
			gc->SetGeometryFlags(GEOMETRY_FLAG_TRANSPARENT_OBJECT);

		// create materials
		m_RegularMat = GetSceneObject()->GetName() + "GizmoRegular";
		m_HighlightMat = GetSceneObject()->GetName() + "GizmoHiglight";
		
		GraphicsMaterial regmat;
		regmat.Name = m_RegularMat;
		regmat.Diffuse.Set(0,0,0,1);
		regmat.Ambient.Set(0,0,0);
		regmat.SelfIllumination.Set(m_Color.r*0.5,m_Color.g*0.5,m_Color.b*0.5);
		regmat.DepthTest = (m_Type == GT_GRID || m_Type == GT_FIXED_GRID);
		regmat.DepthWrite = true;
		
		
		GraphicsMaterial hlmat;
		hlmat.Name = m_HighlightMat;
		hlmat.Diffuse.Set(0,0,0,1);
		hlmat.Ambient.Set(0,0,0);
		hlmat.SelfIllumination.Set(m_Color.r,m_Color.g,m_Color.b);
		hlmat.DepthTest = (m_Type == GT_GRID || m_Type == GT_FIXED_GRID);
		hlmat.DepthWrite = true;
		
		
		GraphicsSystemPtr gfx_sys = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IGraphicsSystem>();
		if(!gfx_sys->HasMaterial(m_RegularMat))
			gfx_sys->AddMaterial(regmat,"GizmoArrowMat");

		if(!gfx_sys->HasMaterial(m_HighlightMat))
			gfx_sys->AddMaterial(hlmat,"GizmoArrowMat");
	}

	void AxisGizmoComponent::OnDelete()
	{
		GetSceneObject()->GetScene()->UnregisterForMessage(UNREG_TMESS(AxisGizmoComponent::OnNewCursorInfo, CursorMovedOverSceneEvent));
		SimEngine::Get().GetSimSystemManager()->UnregisterForMessage(UNREG_TMESS(AxisGizmoComponent::OnEditMode,EditModeChangedEvent));
		SimEngine::Get().GetSimSystemManager()->UnregisterForMessage(UNREG_TMESS(AxisGizmoComponent::OnChangeGridRequest,ChangeGridRequest));
		SimEngine::Get().GetSimSystemManager()->UnregisterForMessage(UNREG_TMESS(AxisGizmoComponent::OnCameraChanged,CameraChangedEvent));
		if(SceneObjectPtr prev_camera = m_ActiveCameraObject.lock())
		{
			prev_camera->UnregisterForMessage(UNREG_TMESS(AxisGizmoComponent::OnCameraMoved, TransformationChangedEvent));
		}

		
		if(SceneObjectPtr  selected = m_SelectedObject.lock())
		{
			selected->UnregisterForMessage(UNREG_TMESS(AxisGizmoComponent::OnSelectedTransformation,TransformationChangedEvent));
		}
	}

	void AxisGizmoComponent::OnEditMode(EditModeChangedEventPtr message)
	{
		m_Mode = message->GetEditMode();
		if(m_Mode == GM_LOCAL)
		{
			if(SceneObjectPtr  selected = m_SelectedObject.lock())
			{
				LocationComponentPtr selected_lc = selected->GetFirstComponentByClass<ILocationComponent>();
				if (selected_lc)
					GetSceneObject()->SendImmediateRequest(WorldRotationRequestPtr(new WorldRotationRequest(selected_lc->GetWorldRotation()*m_BaseRot,GIZMO_SENDER)));
			}
		}
		else if(m_Mode == GM_WORLD)
		{
			GetSceneObject()->SendImmediateRequest(WorldRotationRequestPtr(new WorldRotationRequest(m_BaseRot,GIZMO_SENDER)));
		}
	}

	void AxisGizmoComponent::OnChangeGridRequest(ChangeGridRequestPtr message)
	{
		if(m_Type == GT_GRID || m_Type == GT_FIXED_GRID)
			BuildMesh();
	}

	void AxisGizmoComponent::OnCameraChanged(CameraChangedEventPtr message)
	{
		//Unregister from previous camera
		if(SceneObjectPtr prev_camera = m_ActiveCameraObject.lock())
		{
			prev_camera->UnregisterForMessage(UNREG_TMESS(AxisGizmoComponent::OnCameraMoved, TransformationChangedEvent));
			prev_camera->UnregisterForMessage(UNREG_TMESS(AxisGizmoComponent::OnCameraParameter,CameraParameterRequest));
		}
		CameraComponentPtr camera = message->GetViewport()->GetCamera();
		SceneObjectPtr cam_obj = GASS_DYNAMIC_PTR_CAST<BaseSceneComponent>(camera)->GetSceneObject();

		m_ActiveCameraObject = cam_obj;
		cam_obj->RegisterForMessage(REG_TMESS(AxisGizmoComponent::OnCameraMoved, TransformationChangedEvent,1));
		cam_obj->RegisterForMessage(REG_TMESS(AxisGizmoComponent::OnCameraParameter,CameraParameterRequest,1));
	}

	void AxisGizmoComponent::OnCameraParameter(CameraParameterRequestPtr message)
	{
		if(m_Type == GT_GRID || m_Type == GT_FIXED_GRID)
			return;
		CameraParameterRequest::CameraParameterType type = message->GetParameter();
		switch(type)
		{
		case CameraParameterRequest::CAMERA_FOV:
			{
				float value = message->GetValue1();
			}
			break;
		case CameraParameterRequest::CAMERA_ORTHO_WIN_SIZE:
			{
				float value = message->GetValue1();
				float scale_factor = 0.06;
				Vec3 scale(scale_factor * value,scale_factor* value,scale_factor* value);
				GetSceneObject()->PostRequest(ScaleRequestPtr(new ScaleRequest(scale)));
			}
			break;
		case CameraParameterRequest::CAMERA_CLIP_DISTANCE:
			{

			}
			break;
		}
	}

	void AxisGizmoComponent::SetSelection(std::vector<SceneObjectWeakPtr>  selection)
	{
		if(m_Type == GT_FIXED_GRID)
			return;

		//Create selection box

		//Unregister form previous
		/*SceneObjectPtr  previous_selected = m_SelectedObject.lock();
		if(previous_selected)
		{
			previous_selected->UnregisterForMessage(UNREG_TMESS(AxisGizmoComponent::OnSelectedTransformation,TransformationChangedEvent));
		}*/
		SceneObjectPtr object;
		if(selection.size() > 0)
			object = selection[0].lock();
		if(object)
		{
			//move gizmo to position
			LocationComponentPtr lc = object->GetFirstComponentByClass<ILocationComponent>();
			if(lc)
			{
				//move to selected location
				GetSceneObject()->PostRequest(WorldPositionRequestPtr(new WorldPositionRequest(lc->GetWorldPosition(),GIZMO_SENDER)));

				//rotate to selected rotation
				if(m_Mode == GM_LOCAL)
				{
					GetSceneObject()->PostRequest(WorldRotationRequestPtr(new WorldRotationRequest(lc->GetWorldRotation()*m_BaseRot,GIZMO_SENDER)));
				}
				else
				{
					GetSceneObject()->PostRequest(WorldRotationRequestPtr(new WorldRotationRequest(m_BaseRot,GIZMO_SENDER)));
				}
			}
			object->RegisterForMessage(REG_TMESS(AxisGizmoComponent::OnSelectedTransformation,TransformationChangedEvent,1));
			m_SelectedObject = object;
		}
		else
		{
			m_SelectedObject.reset();
		}
	}

	void AxisGizmoComponent::OnSelectedTransformation(TransformationChangedEventPtr message)
	{
		if(m_Type == GT_FIXED_GRID)
			return;

		//move gizmo
		LocationComponentPtr lc = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		if(lc &&  ((lc->GetWorldPosition() - message->GetPosition()).Length()) > MOVMENT_EPSILON)
		{
			//move to selecetd location
			GetSceneObject()->SendImmediateRequest(WorldPositionRequestPtr(new WorldPositionRequest(message->GetPosition(),GIZMO_SENDER)));
		}

		if(m_Mode == GM_LOCAL)
		{
			GetSceneObject()->SendImmediateRequest(WorldRotationRequestPtr(new WorldRotationRequest(message->GetRotation()*m_BaseRot,GIZMO_SENDER)));
		}
	}

	void AxisGizmoComponent::OnTransformation(TransformationChangedEventPtr message)
	{
		if(m_Type == GT_FIXED_GRID)
			return;
		UpdateScale();
	}

	void AxisGizmoComponent::OnWorldPosition(WorldPositionRequestPtr message)
	{
		if(GIZMO_SENDER != message->GetSenderID())
		{
			SceneObjectPtr  selected= m_SelectedObject.lock();
			if(selected)
			{
				LocationComponentPtr selected_lc = selected->GetFirstComponentByClass<ILocationComponent>();
				//LocationComponentPtr gizmo_lc = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
				if(selected_lc && ((message->GetPosition() - selected_lc->GetWorldPosition()).Length()) > MOVMENT_EPSILON)
				{
					selected->SendImmediateRequest(WorldPositionRequestPtr(new WorldPositionRequest(message->GetPosition(),GIZMO_SENDER)));
				}
			}
		}
	}

	void AxisGizmoComponent::OnCameraMoved(TransformationChangedEventPtr message)
	{
		if(m_Type == GT_FIXED_GRID)
			return;
		UpdateScale();
	}

	void AxisGizmoComponent::UpdateScale()
	{
		if(m_Type == GT_FIXED_GRID)
			return;

		if(m_Type == GT_GRID)
			return;
		SceneObjectPtr camera = m_ActiveCameraObject.lock();
		if(camera)
		{
			LocationComponentPtr cam_location = camera->GetFirstComponentByClass<ILocationComponent>();
			Vec3 cam_pos = cam_location->GetWorldPosition();

			LocationComponentPtr gizmo_location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
			Vec3 gizmo_pos = gizmo_location->GetWorldPosition();


			Float dist = (gizmo_pos-cam_pos).Length();


			if(abs(dist - m_LastDist) > MOVMENT_EPSILON)
			{
				m_LastDist = dist;
				//clamp
				if(dist > MAX_SCALE_DISTANCE)
					dist = MAX_SCALE_DISTANCE;
				if(dist < MIN_SCALE_DISTANCE)
					dist = MIN_SCALE_DISTANCE;

				float scale_factor = 0.06;
				Vec3 scale(scale_factor * dist,scale_factor* dist,scale_factor* dist);
				GetSceneObject()->PostRequest(ScaleRequestPtr(new ScaleRequest(scale)));
			}
		}
	}

	void AxisGizmoComponent::OnLocationLoaded(LocationLoadedEventPtr message)
	{
		BuildMesh();
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(AxisGizmoComponent::OnCameraChanged,CameraChangedEvent,1));
		m_ActiveCameraObject = m_EditorSceneManager->GetActiveCameraObject();
		SceneObjectPtr cam_obj = m_ActiveCameraObject.lock();
		if(cam_obj)
		{
			cam_obj->RegisterForMessage(REG_TMESS(AxisGizmoComponent::OnCameraMoved, TransformationChangedEvent,1));
			cam_obj->RegisterForMessage(REG_TMESS(AxisGizmoComponent::OnCameraParameter,CameraParameterRequest,0));
		}
		LocationComponentPtr lc = message->GetLocation();
		m_BaseRot = Quaternion(Math::Deg2Rad(lc->GetEulerRotation()));
		//std::vector<SceneObjectWeakPtr> selected = m_EditorSceneManager->GetSelectedObjects();
		//if(selected.size()  > 0 )
		{
			
			SetSelection(m_EditorSceneManager->GetSelectedObjects());
		}
	}




	void AxisGizmoComponent::BuildMesh()
	{
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		m_MeshData->SubMeshVector.clear();
		m_MeshData->SubMeshVector.push_back(sub_mesh_data);
		sub_mesh_data->MaterialName = m_RegularMat;

		//Arrow
		if(m_Type == GT_AXIS)
		{
			sub_mesh_data->Type = TRIANGLE_LIST;

			float box_volume = m_Size * 0.01;
			Vec3 offset(0,-box_volume,box_volume);
			Vec3 pos = Vec3(0,box_volume,box_volume)  + offset;
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(0,box_volume,-box_volume) + offset;
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(0,-box_volume,-box_volume)+ offset;;
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(0,-box_volume,box_volume) + offset;
			sub_mesh_data->PositionVector.push_back(pos);

			pos = Vec3(m_Size ,box_volume,box_volume) + offset;;
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(m_Size ,box_volume,-box_volume)  + offset;
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(m_Size ,-box_volume,-box_volume) + offset;
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(m_Size ,-box_volume,box_volume)  + offset;
			sub_mesh_data->PositionVector.push_back(pos);



			sub_mesh_data->IndexVector.push_back(0);
			sub_mesh_data->IndexVector.push_back(4);
			sub_mesh_data->IndexVector.push_back(5);
			sub_mesh_data->IndexVector.push_back(0);
			sub_mesh_data->IndexVector.push_back(5);
			sub_mesh_data->IndexVector.push_back(1);


			sub_mesh_data->IndexVector.push_back(1);
			sub_mesh_data->IndexVector.push_back(5);
			sub_mesh_data->IndexVector.push_back(6);
			sub_mesh_data->IndexVector.push_back(1);
			sub_mesh_data->IndexVector.push_back(6);
			sub_mesh_data->IndexVector.push_back(2);


			sub_mesh_data->IndexVector.push_back(2);
			sub_mesh_data->IndexVector.push_back(6);
			sub_mesh_data->IndexVector.push_back(7);
			sub_mesh_data->IndexVector.push_back(2);
			sub_mesh_data->IndexVector.push_back(7);
			sub_mesh_data->IndexVector.push_back(3);

			sub_mesh_data->IndexVector.push_back(3);
			sub_mesh_data->IndexVector.push_back(7);
			sub_mesh_data->IndexVector.push_back(4);
			sub_mesh_data->IndexVector.push_back(3);
			sub_mesh_data->IndexVector.push_back(4);
			sub_mesh_data->IndexVector.push_back(0);

			sub_mesh_data->IndexVector.push_back(0);
			sub_mesh_data->IndexVector.push_back(1);
			sub_mesh_data->IndexVector.push_back(2);
			sub_mesh_data->IndexVector.push_back(0);
			sub_mesh_data->IndexVector.push_back(2);
			sub_mesh_data->IndexVector.push_back(3);

			//hat
			box_volume = box_volume*4;
			pos = Vec3(m_Size ,box_volume,box_volume) + offset;;
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(m_Size ,box_volume,-box_volume)  + offset;
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(m_Size ,-box_volume,-box_volume) + offset;
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(m_Size ,-box_volume,box_volume)  + offset;
			sub_mesh_data->PositionVector.push_back(pos);

			pos= Vec3(m_Size + box_volume*5,0,0)  + offset;
			sub_mesh_data->PositionVector.push_back(pos);

			sub_mesh_data->IndexVector.push_back(8);
			sub_mesh_data->IndexVector.push_back(9);
			sub_mesh_data->IndexVector.push_back(10);
			sub_mesh_data->IndexVector.push_back(8);
			sub_mesh_data->IndexVector.push_back(10);
			sub_mesh_data->IndexVector.push_back(11);

			sub_mesh_data->IndexVector.push_back(8);
			sub_mesh_data->IndexVector.push_back(12);
			sub_mesh_data->IndexVector.push_back(9);

			sub_mesh_data->IndexVector.push_back(9);
			sub_mesh_data->IndexVector.push_back(12);
			sub_mesh_data->IndexVector.push_back(10);


			sub_mesh_data->IndexVector.push_back(10);
			sub_mesh_data->IndexVector.push_back(12);
			sub_mesh_data->IndexVector.push_back(11);

			sub_mesh_data->IndexVector.push_back(11);
			sub_mesh_data->IndexVector.push_back(12);
			sub_mesh_data->IndexVector.push_back(8);
			
		}
		else if(m_Type == GT_PLANE)
		{
			sub_mesh_data->Type = TRIANGLE_LIST;

			float thickness = 0.01;
			
			Vec3 pos = Vec3(0,-thickness,0);
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(m_Size,-thickness,0);
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(m_Size,-thickness,m_Size);
			sub_mesh_data->PositionVector.push_back(pos);

			pos = Vec3(0,-thickness,0);
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(m_Size,-thickness,m_Size);
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(0,-thickness,m_Size);
			sub_mesh_data->PositionVector.push_back(pos);

			sub_mesh_data->IndexVector.push_back(0);
			sub_mesh_data->IndexVector.push_back(1);
			sub_mesh_data->IndexVector.push_back(2);
			sub_mesh_data->IndexVector.push_back(3);
			sub_mesh_data->IndexVector.push_back(4);
			sub_mesh_data->IndexVector.push_back(5);

			sub_mesh_data->IndexVector.push_back(2);
			sub_mesh_data->IndexVector.push_back(1);
			sub_mesh_data->IndexVector.push_back(0);
			sub_mesh_data->IndexVector.push_back(5);
			sub_mesh_data->IndexVector.push_back(4);
			sub_mesh_data->IndexVector.push_back(3);

			pos = Vec3(0,thickness,0);
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(m_Size,thickness,0);
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(m_Size,thickness,m_Size);
			sub_mesh_data->PositionVector.push_back(pos);

			pos = Vec3(0,-thickness,0);
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(m_Size,thickness,m_Size);
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(0,thickness,m_Size);
			sub_mesh_data->PositionVector.push_back(pos);

			sub_mesh_data->IndexVector.push_back(6);
			sub_mesh_data->IndexVector.push_back(7);
			sub_mesh_data->IndexVector.push_back(8);
			sub_mesh_data->IndexVector.push_back(9);
			sub_mesh_data->IndexVector.push_back(10);
			sub_mesh_data->IndexVector.push_back(11);

			sub_mesh_data->IndexVector.push_back(8);
			sub_mesh_data->IndexVector.push_back(7);
			sub_mesh_data->IndexVector.push_back(6);
			sub_mesh_data->IndexVector.push_back(11);
			sub_mesh_data->IndexVector.push_back(10);
			sub_mesh_data->IndexVector.push_back(9);
		}


		else if (m_Type == GT_GRID || m_Type == GT_FIXED_GRID)
		{

			
			sub_mesh_data->Type = LINE_LIST;

			Vec3 pos(0,0,0);
			
			float grid_size = 0;
			if(m_Type == GT_FIXED_GRID)
				grid_size = m_Size;
			else
				grid_size = m_EditorSceneManager->GetMouseToolController()->GetGridSize();
			float half_grid_size = grid_size/2.0;
			float grid_spacing = m_EditorSceneManager->GetMouseToolController()->GetGridSpacing();
			int n = (grid_size / grid_spacing)/2;
			int index = 0;
			for(int i = -n ;  i <= n; i++)
			{
				pos = Vec3(-half_grid_size,0,i*grid_spacing);
				sub_mesh_data->PositionVector.push_back(pos);
				sub_mesh_data->ColorVector.push_back(m_Color);
				pos = Vec3(half_grid_size,0,i*grid_spacing);
				sub_mesh_data->PositionVector.push_back(pos);
				sub_mesh_data->ColorVector.push_back(m_Color);
				sub_mesh_data->IndexVector.push_back(index++);
				sub_mesh_data->IndexVector.push_back(index++);
			}
			for(int i = -n ;  i <= n; i++)
			{
				pos = Vec3(i*grid_spacing,0,-half_grid_size);
				sub_mesh_data->PositionVector.push_back(pos);
				sub_mesh_data->ColorVector.push_back(m_Color);
				pos = Vec3(i*grid_spacing,0,half_grid_size);
				sub_mesh_data->PositionVector.push_back(pos);
				sub_mesh_data->ColorVector.push_back(m_Color);
				sub_mesh_data->IndexVector.push_back(index++);
				sub_mesh_data->IndexVector.push_back(index++);
			}
			
		}
		
		GetSceneObject()->PostRequest(ManualMeshDataRequestPtr(new ManualMeshDataRequest(m_MeshData)));
	}

	void AxisGizmoComponent::OnNewCursorInfo(CursorMovedOverSceneEventPtr message)
	{
		if(m_Type == GT_FIXED_GRID)
			return;

		//bool grid = false;
		//if(m_Type == GT_GRID)
		//	grid = true;
		SceneObjectPtr obj_under_cursor = message->GetSceneObjectUnderCursor();
		if(m_Active || obj_under_cursor == GetSceneObject())
		{
			if(!m_Highlight)
			{
				GetSceneObject()->PostRequest(ReplaceMaterialRequestPtr(new ReplaceMaterialRequest(m_HighlightMat)));
			}
			m_Highlight = true;
		}
		else
		{
			if(m_Highlight)
			{
				GetSceneObject()->PostRequest(ReplaceMaterialRequestPtr(new ReplaceMaterialRequest(m_RegularMat)));
			}
			m_Highlight = false;
		}
	}

	Vec3 AxisGizmoComponent::GetPosition(const Ray &ray)
	{
		Quaternion rot = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetRotation();
		Mat4 rot_mat;
		rot_mat.Identity();
		rot.ToRotationMatrix(rot_mat);

		Vec3 c_pos = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
		Vec3 r_vec = rot_mat.GetXAxis();
		Vec3 v_vec = rot_mat.GetZAxis();
		Vec3 up_vec = rot_mat.GetYAxis();


		//select projection plane
		if(m_Type == GT_AXIS)
		{
			Float v1 = fabs(Math::Dot(ray.m_Dir, up_vec));
			Float v2 = fabs(Math::Dot(ray.m_Dir, v_vec));
			Float value;
			if(v1 > v2)
				value = Math::IsectRayPlane(ray,Plane(c_pos,up_vec));
			else 
				value  = Math::IsectRayPlane(ray,Plane(c_pos,v_vec));

			if(value > 0)
			{	
				Vec3 isect_pos = ray.m_Origin + ray.m_Dir*value;

				Vec3 ret = ProjectPointOnAxis(c_pos, r_vec, isect_pos);
				return ret;

			}
		}
		else
		{
			Float value = Math::IsectRayPlane(ray,Plane(c_pos,up_vec));
			if(value > 0)
			{
				Vec3 isect_pos = ray.m_Origin + ray.m_Dir*value;

				Vec3 proj_r = ProjectPointOnAxis(c_pos, r_vec, isect_pos);
				Vec3 proj_v = ProjectPointOnAxis(c_pos, v_vec, isect_pos);

				Vec3 ret = c_pos + (proj_r - c_pos ) + (proj_v - c_pos);

				return ret;
			}
		
		}
		return c_pos;
	}

	Quaternion  AxisGizmoComponent::GetRotation(float delta) const
	{

		SceneObjectPtr selected = m_SelectedObject.lock();
		if(selected)
		{
			LocationComponentPtr location = selected->GetFirstComponentByClass<ILocationComponent>();
			if(location)
			{
				Quaternion selected_rot = location->GetWorldRotation();
				Quaternion rot = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldRotation();
				Mat4 rot_mat;
				rot_mat.Identity();
				rot.ToRotationMatrix(rot_mat);

				Vec3 r_vec = rot_mat.GetXAxis();
				//Vec3 v_vec = rot_mat.GetZAxis();
				//Vec3 up_vec = rot_mat.GetYAxis();

				r_vec.Normalize();
				//v_vec = v_vec* delta;
				static float rest_angle = 0;
				Quaternion final_rot;
				float angle = delta;
				angle = m_EditorSceneManager->GetMouseToolController()->SnapAngle(angle+rest_angle);
				if(angle == 0)
				{
					rest_angle += delta;
				}
				else
				{
					rest_angle= 0;
				}

				final_rot.FromAngleAxis(angle,r_vec);
				return final_rot*selected_rot;	
			}
		}
		return Quaternion::IDENTITY;
	}


	Vec3 AxisGizmoComponent::ProjectPointOnAxis(const Vec3 &axis_origin, const Vec3 &axis_dir, const Vec3 &p) const
	{
		Vec3 c = p-axis_origin;
		Float t = Math::Dot(axis_dir,c);
		if(m_Mode == GM_LOCAL)
		{
			t = m_EditorSceneManager->GetMouseToolController()->SnapPosition(t);
		}
	
		Vec3 point_on_axis = axis_dir*t;

		Vec3 res = (axis_origin + point_on_axis);

		if(m_Mode == GM_WORLD)
		{
			res.x = m_EditorSceneManager->GetMouseToolController()->SnapPosition(res.x);
			res.z = m_EditorSceneManager->GetMouseToolController()->SnapPosition(res.z);
			res.y = m_EditorSceneManager->GetMouseToolController()->SnapPosition(res.y);
		}

		return res;
	}

	Float AxisGizmoComponent::SnapValue(Float value, Float snap)
	{
		Float new_value = value/snap;
		new_value = int(new_value);
		return new_value*snap;
	}
}
