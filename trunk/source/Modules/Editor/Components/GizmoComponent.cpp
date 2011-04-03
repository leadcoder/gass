#include "../EditorMessages.h"
#include "../EditorManager.h"
#include "../ToolSystem/MouseToolController.h"
#include "GizmoComponent.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"

#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Systems/SimSystemManager.h"

#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Core/Utils/Log.h"



#define MOVMENT_EPSILON 0.0000001
#define GIZMO_SENDER 999

namespace GASS
{

	GizmoComponent::GizmoComponent() : m_MeshData(new ManualMeshData), m_Color(1,0,0,1),
		m_Size(5),
		m_Type("arrow"),
		m_Highlight(true),
		m_LastDist(0), 
		m_Mode("World"),
		m_GridDist(1.0)
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

	void GizmoComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(GizmoComponent::OnLoad,LoadCoreComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(GizmoComponent::OnUnload,UnloadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(GizmoComponent::OnTransformation,TransformationNotifyMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(GizmoComponent::OnWorldPosition,WorldPositionMessage,0));
		EditorManager::GetPtr()->GetMessageManager()->RegisterForMessage(REG_TMESS(GizmoComponent::OnNewCursorInfo, CursorMoved3DMessage, 1000));
		EditorManager::GetPtr()->GetMessageManager()->RegisterForMessage(REG_TMESS(GizmoComponent::OnSceneObjectSelected,ObjectSelectedMessage,0));
		EditorManager::GetPtr()->GetMessageManager()->RegisterForMessage(REG_TMESS(GizmoComponent::OnEditMode,EditModeMessage,0));
		EditorManager::GetPtr()->GetMessageManager()->RegisterForMessage(REG_TMESS(GizmoComponent::OnGridMessage,GridMessage,0));
	}

	void GizmoComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		EditorManager::GetPtr()->GetMessageManager()->UnregisterForMessage(UNREG_TMESS(GizmoComponent::OnNewCursorInfo, CursorMoved3DMessage));
		EditorManager::GetPtr()->GetMessageManager()->UnregisterForMessage(UNREG_TMESS(GizmoComponent::OnSceneObjectSelected,ObjectSelectedMessage));
		EditorManager::GetPtr()->GetMessageManager()->UnregisterForMessage(UNREG_TMESS(GizmoComponent::OnEditMode,EditModeMessage));
		EditorManager::GetPtr()->GetMessageManager()->UnregisterForMessage(UNREG_TMESS(GizmoComponent::OnGridMessage,GridMessage));
		GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->UnregisterForMessage(UNREG_TMESS(GizmoComponent::OnChangeCamera,ChangeCameraMessage));
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
		if(m_Mode == "Local")
		{
			SceneObjectPtr  selected(m_SelectedObject,boost::detail::sp_nothrow_tag());
			if(selected)
			{
				LocationComponentPtr selected_lc = selected->GetFirstComponent<ILocationComponent>();
				GetSceneObject()->SendImmediate(MessagePtr(new WorldRotationMessage(m_BaseRot*selected_lc->GetWorldRotation(),GIZMO_SENDER)));
			}
		}
		else if(m_Mode == "World")
		{
			GetSceneObject()->SendImmediate(MessagePtr(new WorldRotationMessage(m_BaseRot,GIZMO_SENDER)));
		}
	}

	

	void GizmoComponent::OnGridMessage(GridMessagePtr message)
	{
		if(m_Type == "grid")
			BuildMesh();
	}

	void GizmoComponent::OnChangeCamera(ChangeCameraMessagePtr message)
	{
		//Unregister from previos camera
		if(SceneObjectPtr(m_ActiveCameraObject,boost::detail::sp_nothrow_tag()))
		{
			SceneObjectPtr prev_camera = SceneObjectPtr(m_ActiveCameraObject,boost::detail::sp_nothrow_tag());
			prev_camera->UnregisterForMessage(UNREG_TMESS(GizmoComponent::OnCameraMoved, TransformationNotifyMessage));
		}
		SceneObjectPtr cam_obj =  message->GetCamera();
		m_ActiveCameraObject = cam_obj;
		cam_obj->RegisterForMessage(REG_TMESS(GizmoComponent::OnCameraMoved, TransformationNotifyMessage,1));
	}

	void GizmoComponent::OnSceneObjectSelected(ObjectSelectedMessagePtr message)
	{
		//Unregister form previos
		SceneObjectPtr  previos_selected(m_SelectedObject,boost::detail::sp_nothrow_tag());
		if(previos_selected)
		{
			previos_selected->UnregisterForMessage(UNREG_TMESS(GizmoComponent::OnSelectedTransformation,TransformationNotifyMessage));
		}

		SceneObjectPtr  new_selected = message->GetSceneObject();
		if(new_selected)
		{
			//move gismo to position
			LocationComponentPtr lc = new_selected->GetFirstComponent<ILocationComponent>();
			if(lc)
			{
				//move to selecetd location
				GetSceneObject()->PostMessage(MessagePtr(new WorldPositionMessage(lc->GetWorldPosition(),GIZMO_SENDER)));

				//rotate to selecetd rotation
				if(m_Mode == "Local")
				{
					GetSceneObject()->PostMessage(MessagePtr(new WorldRotationMessage(m_BaseRot*lc->GetWorldRotation(),GIZMO_SENDER)));
				}
				else
				{
					GetSceneObject()->PostMessage(MessagePtr(new WorldRotationMessage(m_BaseRot,GIZMO_SENDER)));
				}
			}
			new_selected->RegisterForMessage(REG_TMESS(GizmoComponent::OnSelectedTransformation,TransformationNotifyMessage,1));
			m_SelectedObject = new_selected;
		}
		else
		{
			m_SelectedObject.reset();
		}
	}

	void GizmoComponent::OnSelectedTransformation(TransformationNotifyMessagePtr message)
	{
		//move gizmo
		LocationComponentPtr lc = GetSceneObject()->GetFirstComponent<ILocationComponent>();
		if(lc &&  ((lc->GetWorldPosition() - message->GetPosition()).Length()) > MOVMENT_EPSILON)
		{
			//move to selecetd location
			GetSceneObject()->SendImmediate(MessagePtr(new WorldPositionMessage(message->GetPosition(),GIZMO_SENDER)));
		}

		if(m_Mode == "Local")
		{
			GetSceneObject()->SendImmediate(MessagePtr(new WorldRotationMessage(m_BaseRot*message->GetRotation(),GIZMO_SENDER)));
		}

		
	}

	void GizmoComponent::OnTransformation(TransformationNotifyMessagePtr message)
	{
		UpdateScale();
	}

	void GizmoComponent::OnWorldPosition(WorldPositionMessagePtr message)
	{
		if(GIZMO_SENDER!= message->GetSenderID())
		{
			SceneObjectPtr  selected(m_SelectedObject,boost::detail::sp_nothrow_tag());
			if(selected)
			{
				LocationComponentPtr selected_lc = selected->GetFirstComponent<ILocationComponent>();
				//LocationComponentPtr gizmo_lc = GetSceneObject()->GetFirstComponent<ILocationComponent>();
				if(selected_lc && ((message->GetPosition() - selected_lc->GetWorldPosition()).Length()) > MOVMENT_EPSILON)
				{
					selected->SendImmediate(MessagePtr(new WorldPositionMessage(message->GetPosition(),GIZMO_SENDER)));
				
				}
			}
		}
	}

	void GizmoComponent::OnCameraMoved(TransformationNotifyMessagePtr message)
	{
		UpdateScale();
	}

	void GizmoComponent::UpdateScale()
	{
		if(m_Type == "grid")
			return;
		SceneObjectPtr camera(m_ActiveCameraObject,boost::detail::sp_nothrow_tag());
		if(camera)
		{
			LocationComponentPtr cam_location = camera->GetFirstComponent<ILocationComponent>();
			Vec3 cam_pos = cam_location->GetWorldPosition();

			LocationComponentPtr gizmo_location = GetSceneObject()->GetFirstComponent<ILocationComponent>();
			Vec3 gizmo_pos = gizmo_location->GetWorldPosition();
			

			Float dist = (gizmo_pos-cam_pos).Length();


			if(abs(dist - m_LastDist) > MOVMENT_EPSILON)
			{
				m_LastDist = dist;
				//clamp
				if(dist > 200)
					dist = 200;
				if(dist < 0.1)
					dist = 0.1;

				float scale_factor = 0.06;
				Vec3 scale(scale_factor * dist,scale_factor* dist,scale_factor* dist);
				GetSceneObject()->PostMessage(MessagePtr(new ScaleMessage(scale)));
			}
		}
	}

	void GizmoComponent::OnLoad(LoadCoreComponentsMessagePtr message)
	{
		BuildMesh();
		GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->RegisterForMessage(REG_TMESS(GizmoComponent::OnChangeCamera,ChangeCameraMessage,1));

		m_ActiveCameraObject = EditorManager::GetPtr()->GetMouseToolController()->GetActiveCameraObject();
		SceneObjectPtr cam_obj(m_ActiveCameraObject,boost::detail::sp_nothrow_tag());
		if(cam_obj)
			cam_obj->RegisterForMessage(REG_TMESS(GizmoComponent::OnCameraMoved, TransformationNotifyMessage,1));

		LocationComponentPtr lc = GetSceneObject()->GetFirstComponent<ILocationComponent>();
		m_BaseRot = Quaternion(Math::Deg2Rad(lc->GetEulerRotation()));
	}




	void GizmoComponent::BuildMesh()
	{
		m_MeshData->VertexVector.clear();
		m_MeshData->IndexVector.clear();

		//Arrow
		if(m_Type == "arrow")
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
		else if(m_Type == "plane")
		{
			MeshVertex vertex;
			vertex.Pos = Vec3(0,0,0);
			vertex.TexCoord.Set(0,0);
			vertex.Color  = Vec4(1,1,1,1);
			m_MeshData->VertexVector.push_back(vertex);
			vertex.Pos = Vec3(m_Size,0,0);
			m_MeshData->VertexVector.push_back(vertex);
			vertex.Pos = Vec3(m_Size,0,-m_Size);
			m_MeshData->VertexVector.push_back(vertex);

			vertex.Pos = Vec3(0,0,0);
			m_MeshData->VertexVector.push_back(vertex);
			vertex.Pos = Vec3(m_Size,0,-m_Size);
			m_MeshData->VertexVector.push_back(vertex);
			vertex.Pos = Vec3(0,0,-m_Size);
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

			m_MeshData->Material = "GizmoArrowMat";
			m_MeshData->Type = TRIANGLE_LIST;
		}

		//Arrow
		else if (m_Type == "grid")
		{
			MeshVertex vertex;
			
			vertex.Pos = Vec3(0,0,0);
			vertex.TexCoord.Set(0,0);
			vertex.Color  = m_Color;

			float grid_size = EditorManager::Get().GetMouseToolController()->GetGridSize();
			float half_grid_size = grid_size/2.0;
			float grid_spacing = EditorManager::Get().GetMouseToolController()->GetGridSpacing();
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
			m_MeshData->Material = "GizmoPlaneMat";
			m_MeshData->Type = LINE_LIST;
		}
		MessagePtr mesh_message(new ManualMeshDataMessage(m_MeshData));
		GetSceneObject()->PostMessage(mesh_message);
	}

	void GizmoComponent::OnNewCursorInfo(CursorMoved3DMessagePtr message)
	{
		bool grid = true;
		if(m_Type == "grid")
			grid = true;
		SceneObjectPtr obj_under_cursor = message->GetSceneObjectUnderCursor();
		if(obj_under_cursor == GetSceneObject())
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
				//GetSceneObject()->PostMessage(MessagePtr(new MaterialMessage(Vec4(0,0,0,m_Color.w),Vec3(0,0,0),Vec3(0,0,0),color)));
			}
			m_Highlight = false;
		}
	}


	Vec3 GizmoComponent::GetPosition(const Vec3 &ray_start, const Vec3 &ray_dir)
	{
		Quaternion rot = GetSceneObject()->GetFirstComponent<ILocationComponent>()->GetRotation();
		Mat4 rot_mat;
		rot_mat.Identity();
		rot.ToRotationMatrix(rot_mat);

		Vec3 c_pos = GetSceneObject()->GetFirstComponent<ILocationComponent>()->GetPosition();
		Vec3 r_vec = rot_mat.GetRightVector();

		Vec3 v_vec = rot_mat.GetViewDirVector();
		Vec3 up_vec = rot_mat.GetUpVector();

		float value = Math::IsectRayPlane(ray_start,ray_dir,c_pos,up_vec);
		if(value > 0)
		{
			Vec3 isect_pos = ray_start + ray_dir*value;
			
			if(m_Type == "arrow")
			{
				Vec3 ret = ProjectPointOnAxis(c_pos, r_vec, isect_pos);
				return ret;
			}
			else
			{
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
			Quaternion selected_rot = selected->GetFirstComponent<ILocationComponent>()->GetWorldRotation();
			Quaternion rot = GetSceneObject()->GetFirstComponent<ILocationComponent>()->GetWorldRotation();
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
			angle = EditorManager::Get().GetMouseToolController()->SnapAngle(angle+rest_angle);
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
		// Determine t (the length of the vector from a to p)
		Vec3 c = p-axis_origin;
		Float t = Math::Dot(axis_dir,c);
		t = EditorManager::Get().GetMouseToolController()->SnapPosition(t);
			//t = SnapValue(t,m_MovmentSnap);
		Vec3 point_on_axis = axis_dir*t;
		return (axis_origin + point_on_axis);
	}

	Float GizmoComponent::SnapValue(Float value, Float snap)
	{
		Float new_value = value/snap;
		new_value = int(new_value);
		return new_value*snap;
	}
}
