#include "TerrainDeformTool.h"
#include "MouseToolController.h"
#include "../EditorManager.h"
#include "../Components/PaintGizmoComponent.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/ComponentSystem/IComponent.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Core/ComponentSystem/BaseComponentContainerTemplateManager.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Components/Graphics/Geometry/ITerrainComponent.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/Input/ControlSettingsManager.h"
#include "Sim/Systems/Input/ControlSetting.h"


namespace GASS
{

	TerrainDeformTool::TerrainDeformTool(MouseToolController* controller): m_MouseIsDown(false),
		m_Controller(controller),m_BrushSize(116),m_BrushInnerSize(90), m_Intensity(1)
	{
		EditorManager::GetPtr()->GetMessageManager()->RegisterForMessage(REG_TMESS(TerrainDeformTool::OnSceneObjectSelected,ObjectSelectedMessage,0));

		ControlSetting* cs = SimEngine::Get().GetControlSettingsManager()->GetControlSetting("EditorInputSettings");
		if(cs)
		{
			cs->GetMessageManager()->RegisterForMessage(REG_TMESS(TerrainDeformTool::OnInput,ControllerMessage,0));
		}
		
	}

	TerrainDeformTool::~TerrainDeformTool()
	{

	}

	void TerrainDeformTool::MoveTo(const CursorInfo &info)
	{
		int from_id = (int) this;
		SceneObjectPtr selected(m_SelectedObject,boost::detail::sp_nothrow_tag());
		if(m_MouseIsDown)// && selected)
		{
			if(selected)
			{
				TerrainComponentPtr terrain = selected->GetFirstComponentByClass<ITerrainComponent>();
				if(terrain)
				{
					selected->GetParentSceneObject()->PostMessage(MessagePtr(new TerrainHeightModifyMessage(TerrainHeightModifyMessage::MT_DEFORM,info.m_3DPos,m_BrushSize, m_BrushInnerSize,m_Intensity)));
				}
			}
			GASS::MessagePtr paint_msg(new PaintMessage(info.m_3DPos,selected,from_id));
			EditorManager::GetPtr()->GetMessageManager()->SendImmediate(paint_msg);
			/*int from_id = (int) this;
			boost::shared_ptr<GASS::Message> rot_msg(new GASS::Message(GASS::ScenarioScene::OBJECT_MESSAGE_ROTATION,from_id));
			rot_msg->SetData("Rotation",Quaternion(new_rot));
			m_SelectedObject->GetMessageManager()->SendImmediate(rot_msg);*/
		}
		SceneObjectPtr gizmo = GetMasterGizmo();
		if(gizmo)
		{
			GASS::MessagePtr pos_msg(new GASS::WorldPositionMessage(info.m_3DPos,from_id));
			gizmo->PostMessage(pos_msg);
		}
	}

	void TerrainDeformTool::MouseDown(const CursorInfo &info)
	{
		m_MouseIsDown = true;
	}

	void TerrainDeformTool::MouseUp(const CursorInfo &info)
	{
		m_MouseIsDown = false;
	}

	void TerrainDeformTool::Stop()
	{
		SetGizmoVisiblity(false);
		m_Active = false;
	}

	void TerrainDeformTool::Start() 
	{
		SetGizmoVisiblity(true);
		m_Active = true;
	}

	SceneObjectPtr TerrainDeformTool::GetMasterGizmo()
	{
		SceneObjectPtr gizmo(m_MasterGizmoObject,boost::detail::sp_nothrow_tag());
		if(!gizmo &&  m_Controller->GetScene())
		{
			ScenarioScenePtr scene = m_Controller->GetScene();
			std::string gizmo_name = "PaintGizmo";
			GASS::SceneObjectPtr scene_object = m_Controller->GetScene()->GetObjectManager()->LoadFromTemplate(gizmo_name);
			m_MasterGizmoObject = scene_object;
			gizmo = scene_object;

			//Send selection message to inform gizmo about current object
			if(gizmo)
			{
				SceneObjectPtr current (m_SelectedObject,boost::detail::sp_nothrow_tag());
				if(current)
				{
					MessagePtr selection_msg(new ObjectSelectedMessage(current,(int) this));
					EditorManager::GetPtr()->GetMessageManager()->PostMessage(selection_msg);
				}

				PaintGizmoComponentPtr comp = gizmo->GetFirstComponentByClass<PaintGizmoComponent>();
				comp->SetSize(m_BrushSize*0.5);
				comp->BuildMesh();
			}
		}
		return gizmo;
	}

	void TerrainDeformTool::SetGizmoVisiblity(bool value)
	{
		SceneObjectPtr gizmo = GetMasterGizmo();
		if(gizmo)
		{
			int from_id = (int) this;
			MessagePtr col_msg(new GASS::CollisionSettingsMessage(value,from_id));
			SendMessageRec(gizmo,col_msg);
			MessagePtr vis_msg(new GASS::VisibilityMessage(value,from_id));
			SendMessageRec(gizmo,vis_msg);
		}
	}

	void TerrainDeformTool::OnSceneObjectSelected(ObjectSelectedMessagePtr message)
	{
		if(m_Active)
		{
			//hide gizmo
			/*if(message->GetSceneObject())
			{
				LocationComponentPtr lc = message->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
				if(lc) //only support gizmo for objects with location component
				{
					SetGizmoVisiblity(true);
				}
				else
				{
					SetGizmoVisiblity(false);
				}
			}
			else
			{
				SetGizmoVisiblity(false);
			}*/
		}
		m_SelectedObject = message->GetSceneObject();
	}


	void TerrainDeformTool::SendMessageRec(SceneObjectPtr obj,MessagePtr msg)
	{
		obj->PostMessage(msg);
		GASS::IComponentContainer::ComponentContainerIterator iter = obj->GetChildren();
		while(iter.hasMoreElements())
		{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject>(iter.getNext());
			SendMessageRec(child,msg);
		}
	}

	void TerrainDeformTool::OnInput(ControllerMessagePtr message)
	{
		if(m_Active)
		{
			std::string name = message->GetController();
			float value = message->GetValue();
			if(name == "ChangeBrushSize" && fabs(value)  > 0.5)
			{
				//std::cout << value << "\n";
				if(value > 0.5) 
					m_BrushSize += m_BrushSize*0.3;
				else if(value < -0.5) 
					m_BrushSize -= m_BrushSize*0.3;

				SceneObjectPtr gizmo = GetMasterGizmo();
				if(gizmo)
				{
					PaintGizmoComponentPtr comp = gizmo->GetFirstComponentByClass<PaintGizmoComponent>();
					comp->SetSize(m_BrushSize*0.5);
					comp->BuildMesh();
				
				}
			}
		}
	}
}
