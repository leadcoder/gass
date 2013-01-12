#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/EditorSceneManager.h"
#include "Modules/Editor/EditorMessages.h"
#include "Modules/Editor/ToolSystem/MouseToolController.h"
#include "GizmoComponent.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSIViewport.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSException.h"

#define MOVMENT_EPSILON 0.0000001
#define MAX_SCALE_DISTANCE 300.0 //meters
#define MIN_SCALE_DISTANCE 0.1 //meters
#define GIZMO_SENDER 999

namespace GASS
{
	GizmoComponent::GizmoComponent() : m_MeshData(new ManualMeshData), m_Color(1,0,0,1),
		m_Size(5),
		m_Type(GT_AXIS),
		m_Highlight(true),
		m_LastDist(0), 
		m_Mode(GM_WORLD),
		m_GridDist(1.0),
		m_Active(false)
	{

	}

	GizmoComponent::~GizmoComponent()
	{

	}

	void GizmoComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("GizmoComponent",new Creator<GizmoComponent, IComponent>);
		RegisterProperty<float>("Size",&GizmoComponent::GetSize, &GizmoComponent::SetSize);
		RegisterProperty<Vec4>("Color",&GizmoComponent::GetColor, &GizmoComponent::SetColor);
		RegisterProperty<std::string>("Type",&GizmoComponent::GetType, &GizmoComponent::SetType);
	}

	void GizmoComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(GizmoComponent::OnLocationLoaded,LocationLoadedMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(GizmoComponent::OnTransformation,TransformationNotifyMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(GizmoComponent::OnWorldPosition,WorldPositionMessage,0));
		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS(GizmoComponent::OnNewCursorInfo, CursorMovedOverSceneEvent, 1000));
		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS(GizmoComponent::OnSceneObjectSelected,ObjectSelectionChangedEvent,0));
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(GizmoComponent::OnEditMode,EditModeMessage,0));
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(GizmoComponent::OnChangeGridRequest,ChangeGridRequest,0));


		m_EditorSceneManager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<EditorSceneManager>();

		//Change geomtry flags
		GeometryComponentPtr gc = GetSceneObject()->GetFirstComponentByClass<IGeometryComponent>();
		if(m_Type == GT_AXIS || m_Type == GT_PLANE)
			gc->SetGeometryFlags(GEOMETRY_FLAG_GIZMO);
		else
			gc->SetGeometryFlags(GEOMETRY_FLAG_TRANSPARENT_OBJECT);
	
	}

	void GizmoComponent::OnDelete()
	{
		GetSceneObject()->GetScene()->UnregisterForMessage(UNREG_TMESS(GizmoComponent::OnNewCursorInfo, CursorMovedOverSceneEvent));
		GetSceneObject()->GetScene()->UnregisterForMessage(UNREG_TMESS(GizmoComponent::OnSceneObjectSelected,ObjectSelectionChangedEvent));
		SimEngine::Get().GetSimSystemManager()->UnregisterForMessage(UNREG_TMESS(GizmoComponent::OnEditMode,EditModeMessage));
		SimEngine::Get().GetSimSystemManager()->UnregisterForMessage(UNREG_TMESS(GizmoComponent::OnChangeGridRequest,ChangeGridRequest));
		SimEngine::Get().GetSimSystemManager()->UnregisterForMessage(UNREG_TMESS(GizmoComponent::OnCameraChanged,CameraChangedEvent));
		if(SceneObjectPtr(m_ActiveCameraObject,boost::detail::sp_nothrow_tag()))
		{
			SceneObjectPtr prev_camera = SceneObjectPtr(m_ActiveCameraObject,boost::detail::sp_nothrow_tag());
			prev_camera->UnregisterForMessage(UNREG_TMESS(GizmoComponent::OnCameraMoved, TransformationNotifyMessage));
		}

		SceneObjectPtr  selected(m_SelectedObject,boost::detail::sp_nothrow_tag());
		if(selected)
		{
			selected->UnregisterForMessage(UNREG_TMESS(GizmoComponent::OnSelectedTransformation,TransformationNotifyMessage));
		}
	}

	void GizmoComponent::OnEditMode(EditModeMessagePtr message)
	{
		m_Mode = message->GetEditMode();
		if(m_Mode == GM_LOCAL)
		{
			SceneObjectPtr  selected(m_SelectedObject,boost::detail::sp_nothrow_tag());
			if(selected)
			{
				LocationComponentPtr selected_lc = selected->GetFirstComponentByClass<ILocationComponent>();
				if (selected_lc)
					GetSceneObject()->SendImmediate(MessagePtr(new WorldRotationMessage(m_BaseRot*selected_lc->GetWorldRotation(),GIZMO_SENDER)));
			}
		}
		else if(m_Mode == GM_WORLD)
		{
			GetSceneObject()->SendImmediate(MessagePtr(new WorldRotationMessage(m_BaseRot,GIZMO_SENDER)));
		}
	}

	

	void GizmoComponent::OnChangeGridRequest(ChangeGridRequestPtr message)
	{
		if(m_Type == GT_GRID || m_Type == GT_FIXED_GRID)
			BuildMesh();
	}

	void GizmoComponent::OnCameraChanged(CameraChangedEventPtr message)
	{
		//Unregister from previous camera
		if(SceneObjectPtr(m_ActiveCameraObject,boost::detail::sp_nothrow_tag()))
		{
			SceneObjectPtr prev_camera = SceneObjectPtr(m_ActiveCameraObject,boost::detail::sp_nothrow_tag());
			prev_camera->UnregisterForMessage(UNREG_TMESS(GizmoComponent::OnCameraMoved, TransformationNotifyMessage));
			prev_camera->UnregisterForMessage(UNREG_TMESS(GizmoComponent::OnCameraParameter,CameraParameterMessage));
		}
		CameraComponentPtr camera = message->GetViewport()->GetCamera();
		SceneObjectPtr cam_obj = boost::shared_dynamic_cast<BaseSceneComponent>(camera)->GetSceneObject();

		m_ActiveCameraObject = cam_obj;
		cam_obj->RegisterForMessage(REG_TMESS(GizmoComponent::OnCameraMoved, TransformationNotifyMessage,1));
		cam_obj->RegisterForMessage(REG_TMESS(GizmoComponent::OnCameraParameter,CameraParameterMessage,1));
	}

	void GizmoComponent::OnCameraParameter(CameraParameterMessagePtr message)
	{
		if(m_Type == GT_GRID || m_Type == GT_FIXED_GRID)
			return;
		CameraParameterMessage::CameraParameterType type = message->GetParameter();
		switch(type)
		{
		case CameraParameterMessage::CAMERA_FOV:
			{
				float value = message->GetValue1();
			}
			break;
		case CameraParameterMessage::CAMERA_ORTHO_WIN_SIZE:
			{
				float value = message->GetValue1();
				float scale_factor = 0.06;
				Vec3 scale(scale_factor * value,scale_factor* value,scale_factor* value);
				GetSceneObject()->PostMessage(MessagePtr(new ScaleMessage(scale)));
			}
			break;
		case CameraParameterMessage::CAMERA_CLIP_DISTANCE:
			{
				
			}
			break;
		}
	}

	void GizmoComponent::OnSceneObjectSelected(ObjectSelectionChangedEventPtr message)
	{
		SetSelection(message->GetSceneObject());
	}

	void GizmoComponent::SetSelection(SceneObjectPtr  object)
	{
		if(m_Type == GT_FIXED_GRID)
			return;
		//Unregister form previous
		SceneObjectPtr  previous_selected(m_SelectedObject,boost::detail::sp_nothrow_tag());
		if(previous_selected)
		{
			previous_selected->UnregisterForMessage(UNREG_TMESS(GizmoComponent::OnSelectedTransformation,TransformationNotifyMessage));
		}

		if(object)
		{
			//move gismo to position
			LocationComponentPtr lc = object->GetFirstComponentByClass<ILocationComponent>();
			if(lc)
			{
				//move to selecetd location
				GetSceneObject()->PostMessage(MessagePtr(new WorldPositionMessage(lc->GetWorldPosition(),GIZMO_SENDER)));

				//rotate to selecetd rotation
				if(m_Mode == GM_LOCAL)
				{
					GetSceneObject()->PostMessage(MessagePtr(new WorldRotationMessage(m_BaseRot*lc->GetWorldRotation(),GIZMO_SENDER)));
				}
				else
				{
					GetSceneObject()->PostMessage(MessagePtr(new WorldRotationMessage(m_BaseRot,GIZMO_SENDER)));
				}
			}
			object->RegisterForMessage(REG_TMESS(GizmoComponent::OnSelectedTransformation,TransformationNotifyMessage,1));
			m_SelectedObject = object;
		}
		else
		{
			m_SelectedObject.reset();
		}
	}

	void GizmoComponent::OnSelectedTransformation(TransformationNotifyMessagePtr message)
	{
		if(m_Type == GT_FIXED_GRID)
			return;

		//move gizmo
		LocationComponentPtr lc = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		if(lc &&  ((lc->GetWorldPosition() - message->GetPosition()).Length()) > MOVMENT_EPSILON)
		{
			//move to selecetd location
			GetSceneObject()->SendImmediate(MessagePtr(new WorldPositionMessage(message->GetPosition(),GIZMO_SENDER)));
		}

		if(m_Mode == GM_LOCAL)
		{
			GetSceneObject()->SendImmediate(MessagePtr(new WorldRotationMessage(m_BaseRot*message->GetRotation(),GIZMO_SENDER)));
		}
	}

	void GizmoComponent::OnTransformation(TransformationNotifyMessagePtr message)
	{
		if(m_Type == GT_FIXED_GRID)
			return;
		UpdateScale();
	}

	void GizmoComponent::OnWorldPosition(WorldPositionMessagePtr message)
	{
		if(GIZMO_SENDER!= message->GetSenderID())
		{
			SceneObjectPtr  selected(m_SelectedObject,boost::detail::sp_nothrow_tag());
			if(selected)
			{
				LocationComponentPtr selected_lc = selected->GetFirstComponentByClass<ILocationComponent>();
				//LocationComponentPtr gizmo_lc = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
				if(selected_lc && ((message->GetPosition() - selected_lc->GetWorldPosition()).Length()) > MOVMENT_EPSILON)
				{
					selected->SendImmediate(MessagePtr(new WorldPositionMessage(message->GetPosition(),GIZMO_SENDER)));
				}
			}
		}
	}

	void GizmoComponent::OnCameraMoved(TransformationNotifyMessagePtr message)
	{
		if(m_Type == GT_FIXED_GRID)
			return;
		UpdateScale();
	}

	void GizmoComponent::UpdateScale()
	{
		if(m_Type == GT_FIXED_GRID)
			return;

		if(m_Type == GT_GRID)
			return;
		SceneObjectPtr camera(m_ActiveCameraObject,boost::detail::sp_nothrow_tag());
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
				GetSceneObject()->PostMessage(MessagePtr(new ScaleMessage(scale)));
			}
		}
	}

	void GizmoComponent::OnLocationLoaded(LocationLoadedMessagePtr message)
	{
		BuildMesh();
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(GizmoComponent::OnCameraChanged,CameraChangedEvent,1));
		//GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS(GizmoComponent::OnChangeCamera,ChangeCameraRequest,1));
		m_ActiveCameraObject = m_EditorSceneManager->GetActiveCameraObject();
		SceneObjectPtr cam_obj(m_ActiveCameraObject,boost::detail::sp_nothrow_tag());
		if(cam_obj)
		{
			cam_obj->RegisterForMessage(REG_TMESS(GizmoComponent::OnCameraMoved, TransformationNotifyMessage,1));
			cam_obj->RegisterForMessage(REG_TMESS(GizmoComponent::OnCameraParameter,CameraParameterMessage,0));
		}
		LocationComponentPtr lc = message->GetLocation();
		m_BaseRot = Quaternion(Math::Deg2Rad(lc->GetEulerRotation()));
		SetSelection(m_EditorSceneManager->GetSelectedObject());
	}




	void GizmoComponent::BuildMesh()
	{
		m_MeshData->VertexVector.clear();
		m_MeshData->IndexVector.clear();

		//Arrow
		if(m_Type == GT_AXIS)
		{

			MeshVertex vertex;
			float box_volume = m_Size * 0.01;
			
			vertex.TexCoord.Set(0,0);
			vertex.Color  = Vec4(1,1,1,1);
			Vec3 offset(0,-box_volume,box_volume);
		
			vertex.Pos = Vec3(0,box_volume,box_volume)  + offset;
			m_MeshData->VertexVector.push_back(vertex);
			vertex.Pos = Vec3(0,box_volume,-box_volume) + offset;
			m_MeshData->VertexVector.push_back(vertex);
			vertex.Pos = Vec3(0,-box_volume,-box_volume)+ offset;;
			m_MeshData->VertexVector.push_back(vertex);
			vertex.Pos = Vec3(0,-box_volume,box_volume) + offset;
			m_MeshData->VertexVector.push_back(vertex);

			vertex.Pos = Vec3(m_Size ,box_volume,box_volume) + offset;;
			m_MeshData->VertexVector.push_back(vertex);
			vertex.Pos= Vec3(m_Size ,box_volume,-box_volume)  + offset;
			m_MeshData->VertexVector.push_back(vertex);
			vertex.Pos= Vec3(m_Size ,-box_volume,-box_volume) + offset;
			m_MeshData->VertexVector.push_back(vertex);
			vertex.Pos= Vec3(m_Size ,-box_volume,box_volume)  + offset;
			m_MeshData->VertexVector.push_back(vertex);



			m_MeshData->IndexVector.push_back(0);
			m_MeshData->IndexVector.push_back(4);
			m_MeshData->IndexVector.push_back(5);
			m_MeshData->IndexVector.push_back(0);
			m_MeshData->IndexVector.push_back(5);
			m_MeshData->IndexVector.push_back(1);


			m_MeshData->IndexVector.push_back(1);
			m_MeshData->IndexVector.push_back(5);
			m_MeshData->IndexVector.push_back(6);
			m_MeshData->IndexVector.push_back(1);
			m_MeshData->IndexVector.push_back(6);
			m_MeshData->IndexVector.push_back(2);


			m_MeshData->IndexVector.push_back(2);
			m_MeshData->IndexVector.push_back(6);
			m_MeshData->IndexVector.push_back(7);
			m_MeshData->IndexVector.push_back(2);
			m_MeshData->IndexVector.push_back(7);
			m_MeshData->IndexVector.push_back(3);

			m_MeshData->IndexVector.push_back(3);
			m_MeshData->IndexVector.push_back(7);
			m_MeshData->IndexVector.push_back(4);
			m_MeshData->IndexVector.push_back(3);
			m_MeshData->IndexVector.push_back(4);
			m_MeshData->IndexVector.push_back(0);


			m_MeshData->IndexVector.push_back(0);
			m_MeshData->IndexVector.push_back(1);
			m_MeshData->IndexVector.push_back(2);
			m_MeshData->IndexVector.push_back(0);
			m_MeshData->IndexVector.push_back(2);
			m_MeshData->IndexVector.push_back(3);


			
			//hat
			box_volume = box_volume*4;
			vertex.Pos = Vec3(m_Size ,box_volume,box_volume) + offset;;
			m_MeshData->VertexVector.push_back(vertex);
			vertex.Pos= Vec3(m_Size ,box_volume,-box_volume)  + offset;
			m_MeshData->VertexVector.push_back(vertex);
			vertex.Pos= Vec3(m_Size ,-box_volume,-box_volume) + offset;
			m_MeshData->VertexVector.push_back(vertex);
			vertex.Pos= Vec3(m_Size ,-box_volume,box_volume)  + offset;
			m_MeshData->VertexVector.push_back(vertex);

			vertex.Pos= Vec3(m_Size + box_volume*5,0,0)  + offset;
			m_MeshData->VertexVector.push_back(vertex);


			m_MeshData->IndexVector.push_back(8);
			m_MeshData->IndexVector.push_back(9);
			m_MeshData->IndexVector.push_back(10);
			m_MeshData->IndexVector.push_back(8);
			m_MeshData->IndexVector.push_back(10);
			m_MeshData->IndexVector.push_back(11);


			
			m_MeshData->IndexVector.push_back(8);
			m_MeshData->IndexVector.push_back(12);
			m_MeshData->IndexVector.push_back(9);

			m_MeshData->IndexVector.push_back(9);
			m_MeshData->IndexVector.push_back(12);
			m_MeshData->IndexVector.push_back(10);


			m_MeshData->IndexVector.push_back(10);
			m_MeshData->IndexVector.push_back(12);
			m_MeshData->IndexVector.push_back(11);

			m_MeshData->IndexVector.push_back(11);
			m_MeshData->IndexVector.push_back(12);
			m_MeshData->IndexVector.push_back(8);


			m_MeshData->Material = "GizmoArrowMat";
			m_MeshData->Type = TRIANGLE_LIST;

			/*MeshVertex vertex;
			vertex.Pos = Vec3(0,0,0);
			vertex.TexCoord.Set(0,0);
			vertex.Color  = m_Color;
			m_MeshData->VertexVector.push_back(vertex);
			vertex.Pos = Vec3(m_Size,0,0);
			m_MeshData->VertexVector.push_back(vertex);

			m_MeshData->VertexVector.push_back(vertex);

			vertex.Pos = Vec3(m_Size*0.9,0.1,m_Size*0.1);
			m_MeshData->VertexVector.push_back(vertex);

			vertex.Pos = Vec3(m_Size,0,0);
			m_MeshData->VertexVector.push_back(vertex);

			vertex.Pos = Vec3(m_Size*0.9,0.1,-m_Size*0.1);
			m_MeshData->VertexVector.push_back(vertex);

			m_MeshData->IndexVector.push_back(0);
			m_MeshData->IndexVector.push_back(1);
			m_MeshData->IndexVector.push_back(2);
			m_MeshData->IndexVector.push_back(3);
			m_MeshData->IndexVector.push_back(4);
			m_MeshData->IndexVector.push_back(5);
			m_MeshData->Material = "WhiteTransparentNoLighting";
			m_MeshData->Type = LINE_LIST;*/
		}
		else if(m_Type == GT_PLANE)
		{
			float thickness = 0.01;
			MeshVertex vertex;
			vertex.Pos = Vec3(0,-thickness,0);
			vertex.TexCoord.Set(0,0);
			vertex.Color  = Vec4(1,1,1,1);
			m_MeshData->VertexVector.push_back(vertex);
			vertex.Pos = Vec3(m_Size,-thickness,0);
			m_MeshData->VertexVector.push_back(vertex);
			vertex.Pos = Vec3(m_Size,-thickness,-m_Size);
			m_MeshData->VertexVector.push_back(vertex);

			vertex.Pos = Vec3(0,-thickness,0);
			m_MeshData->VertexVector.push_back(vertex);
			vertex.Pos = Vec3(m_Size,-thickness,-m_Size);
			m_MeshData->VertexVector.push_back(vertex);
			vertex.Pos = Vec3(0,-thickness,-m_Size);
			m_MeshData->VertexVector.push_back(vertex);

			m_MeshData->IndexVector.push_back(0);
			m_MeshData->IndexVector.push_back(1);
			m_MeshData->IndexVector.push_back(2);
			m_MeshData->IndexVector.push_back(3);
			m_MeshData->IndexVector.push_back(4);
			m_MeshData->IndexVector.push_back(5);

			m_MeshData->IndexVector.push_back(2);
			m_MeshData->IndexVector.push_back(1);
			m_MeshData->IndexVector.push_back(0);
			m_MeshData->IndexVector.push_back(5);
			m_MeshData->IndexVector.push_back(4);
			m_MeshData->IndexVector.push_back(3);


			vertex.Pos = Vec3(0,thickness,0);
			vertex.TexCoord.Set(0,0);
			vertex.Color  = Vec4(1,1,1,1);
			m_MeshData->VertexVector.push_back(vertex);
			vertex.Pos = Vec3(m_Size,thickness,0);
			m_MeshData->VertexVector.push_back(vertex);
			vertex.Pos = Vec3(m_Size,thickness,-m_Size);
			m_MeshData->VertexVector.push_back(vertex);

			vertex.Pos = Vec3(0,-thickness,0);
			m_MeshData->VertexVector.push_back(vertex);
			vertex.Pos = Vec3(m_Size,thickness,-m_Size);
			m_MeshData->VertexVector.push_back(vertex);
			vertex.Pos = Vec3(0,thickness,-m_Size);
			m_MeshData->VertexVector.push_back(vertex);

			m_MeshData->IndexVector.push_back(6);
			m_MeshData->IndexVector.push_back(7);
			m_MeshData->IndexVector.push_back(8);
			m_MeshData->IndexVector.push_back(9);
			m_MeshData->IndexVector.push_back(10);
			m_MeshData->IndexVector.push_back(11);

			m_MeshData->IndexVector.push_back(8);
			m_MeshData->IndexVector.push_back(7);
			m_MeshData->IndexVector.push_back(6);
			m_MeshData->IndexVector.push_back(11);
			m_MeshData->IndexVector.push_back(10);
			m_MeshData->IndexVector.push_back(9);


			m_MeshData->Material = "GizmoArrowMat";
			m_MeshData->Type = TRIANGLE_LIST;
		}

		
		else if (m_Type == GT_GRID || m_Type == GT_FIXED_GRID)
		{
			MeshVertex vertex;
			
			vertex.Pos = Vec3(0,0,0);
			vertex.TexCoord.Set(0,0);
			vertex.Color  = m_Color;

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
				vertex.Pos = Vec3(-half_grid_size,0,i*grid_spacing);
				m_MeshData->VertexVector.push_back(vertex);
				vertex.Pos = Vec3(half_grid_size,0,i*grid_spacing);
				m_MeshData->VertexVector.push_back(vertex);
				m_MeshData->IndexVector.push_back(index++);
				m_MeshData->IndexVector.push_back(index++);
			}
			for(int i = -n ;  i <= n; i++)
			{
				vertex.Pos = Vec3(i*grid_spacing,0,-half_grid_size);
				m_MeshData->VertexVector.push_back(vertex);
				vertex.Pos = Vec3(i*grid_spacing,0,half_grid_size);
				m_MeshData->VertexVector.push_back(vertex);
				m_MeshData->IndexVector.push_back(index++);
				m_MeshData->IndexVector.push_back(index++);
			}
			m_MeshData->Material = "PlaneGeometry";
			m_MeshData->Type = LINE_LIST;
		}
		MessagePtr mesh_message(new ManualMeshDataMessage(m_MeshData));
		GetSceneObject()->PostMessage(mesh_message);
	}

	void GizmoComponent::OnNewCursorInfo(CursorMovedOverSceneEventPtr message)
	{
		if(m_Type == GT_FIXED_GRID)
			return;
		
		bool grid = false;
		if(m_Type == GT_GRID)
			grid = true;
		SceneObjectPtr obj_under_cursor = message->GetSceneObjectUnderCursor();
		if(m_Active || obj_under_cursor == GetSceneObject())
		{
			if(!m_Highlight)
			{
				MessagePtr mat_mess(new MaterialMessage(Vec4(0,0,0,m_Color.w),
					Vec3(0,0,0),
					Vec3(0,0,0),
					Vec3(m_Color.x,m_Color.y,m_Color.z),
					0,
					grid));
				GetSceneObject()->PostMessage(mat_mess);
			}
			m_Highlight = true;
		}
		else
		{
			if(m_Highlight)
			{
				Vec3 color(m_Color.x*0.5, m_Color.y*0.5,m_Color.z*0.5);

				MessagePtr mat_mess(new MaterialMessage(Vec4(0,0,0,m_Color.w),
					Vec3(0,0,0),
					Vec3(0,0,0),
					color,
					0,
					grid));
				GetSceneObject()->PostMessage(mat_mess);
			}
			m_Highlight = false;
		}
	}

	Vec3 GizmoComponent::GetPosition(const Vec3 &ray_start, const Vec3 &ray_dir)
	{
		Quaternion rot = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetRotation();
		Mat4 rot_mat;
		rot_mat.Identity();
		rot.ToRotationMatrix(rot_mat);

		Vec3 c_pos = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
		Vec3 r_vec = rot_mat.GetRightVector();

		Vec3 v_vec = rot_mat.GetViewDirVector();
		Vec3 up_vec = rot_mat.GetUpVector();

		//check all planes

		
		
		//float value = Math::IsectRayPlane(ray_start,ray_dir,c_pos,up_vec);
		//float value2 = Math::IsectRayPlane(ray_start,ray_dir,c_pos,v_vec);
		//if(value2 < value)
		//	value = value2;
		


		//select projection plane
		if(m_Type == GT_AXIS)
		{
			float v1 = fabs(Math::Dot(ray_dir,up_vec));
			float v2 = fabs(Math::Dot(ray_dir,v_vec));
			float value;
			if(v1 > v2)
				value = Math::IsectRayPlane(ray_start,ray_dir,c_pos,up_vec);
			else 
				value  = Math::IsectRayPlane(ray_start,ray_dir,c_pos,v_vec);

			if(value > 0)
			{	
				Vec3 isect_pos = ray_start + ray_dir*value;

				Vec3 ret = ProjectPointOnAxis(c_pos, r_vec, isect_pos);
				return ret;

			}
		}
		else
		{
			float value = Math::IsectRayPlane(ray_start,ray_dir,c_pos,up_vec);
			if(value > 0)
			{
				Vec3 isect_pos = ray_start + ray_dir*value;

				Vec3 proj_r = ProjectPointOnAxis(c_pos, r_vec, isect_pos);
				Vec3 proj_v = ProjectPointOnAxis(c_pos, v_vec, isect_pos);

				Vec3 ret = c_pos + (proj_r - c_pos ) + (proj_v - c_pos);
				
				return ret;
			}
			//return isect_pos;
		}
		return c_pos;
		}

	Quaternion  GizmoComponent::GetRotation(float delta)
	{
		
		SceneObjectPtr  selected(m_SelectedObject,boost::detail::sp_nothrow_tag());
		if(selected)
		{
			Quaternion selected_rot = selected->GetFirstComponentByClass<ILocationComponent>()->GetWorldRotation();
			Quaternion rot = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldRotation();
			Mat4 rot_mat;
			rot_mat.Identity();
			rot.ToRotationMatrix(rot_mat);
			
			Vec3 r_vec = rot_mat.GetRightVector();
			Vec3 v_vec = rot_mat.GetViewDirVector();
			Vec3 up_vec = rot_mat.GetUpVector();

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
			return selected_rot*final_rot;	
		}
		return Quaternion::IDENTITY;
	}

	
	Vec3 GizmoComponent::ProjectPointOnAxis(const Vec3 &axis_origin, const Vec3 &axis_dir, const Vec3 &p)
	{
		Vec3 c = p-axis_origin;
		Float t = Math::Dot(axis_dir,c);
		if(m_Mode == GM_LOCAL)
		{
			t = m_EditorSceneManager->GetMouseToolController()->SnapPosition(t);
		}
			//t = SnapValue(t,m_MovmentSnap);
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

	Float GizmoComponent::SnapValue(Float value, Float snap)
	{
		Float new_value = value/snap;
		new_value = int(new_value);
		return new_value*snap;
	}
}
