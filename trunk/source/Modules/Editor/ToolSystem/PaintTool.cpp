#include "PaintTool.h"
#include "MouseToolController.h"
#include "Modules/Editor/EditorSystem.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Core/ComponentSystem/GASSBaseComponentContainerTemplateManager.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"


namespace GASS
{

	PaintTool::PaintTool(MouseToolController* controller): m_MouseIsDown(false),
		m_Controller(controller)
	{
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(PaintTool::OnSceneObjectSelected,ObjectSelectionChangedMessage,0));
	}

	PaintTool::~PaintTool()
	{

	}

	void PaintTool::MouseMoved(const CursorInfo &info)
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
					selected->GetParentSceneObject()->PostMessage(MessagePtr(new TerrainHeightModifyMessage(TerrainHeightModifyMessage::MT_DEFORM,info.m_3DPos,116, 90,1.0)));
				}
			}

			GASS::MessagePtr paint_msg(new PaintMessage(info.m_3DPos,selected,from_id));
			SimEngine::Get().GetSimSystemManager()->SendImmediate(paint_msg);
			/*int from_id = (int) this;
			boost::shared_ptr<GASS::Message> rot_msg(new GASS::Message(GASS::Scene::OBJECT_MESSAGE_ROTATION,from_id));
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

	void PaintTool::MouseDown(const CursorInfo &info)
	{
		m_MouseIsDown = true;
	}

	void PaintTool::MouseUp(const CursorInfo &info)
	{
		m_MouseIsDown = false;
	}

	void PaintTool::Stop()
	{
		SetGizmoVisiblity(false);
		m_Active = false;
	}

	void PaintTool::Start() 
	{
		SetGizmoVisiblity(true);
		m_Active = true;
	}

	SceneObjectPtr PaintTool::GetMasterGizmo()
	{
		SceneObjectPtr gizmo(m_MasterGizmoObject,boost::detail::sp_nothrow_tag());
		if(!gizmo &&  m_Controller->GetEditorSystem()->GetScene())
		{
			ScenePtr scene = m_Controller->GetEditorSystem()->GetScene();
			std::string gizmo_name = "PaintGizmo";
			GASS::SceneObjectPtr scene_object = m_Controller->GetEditorSystem()->GetScene()->LoadObjectFromTemplate(gizmo_name,m_Controller->GetEditorSystem()->GetScene()->GetRootSceneObject());
			m_MasterGizmoObject = scene_object;
			gizmo = scene_object;

			//Send selection message to inform gizmo about current object
			if(gizmo)
			{
				SceneObjectPtr current (m_SelectedObject,boost::detail::sp_nothrow_tag());
				if(current)
				{
					m_Controller->GetEditorSystem()->SelectSceneObject(current);
				}
			}
		}
		return gizmo;
	}

	void PaintTool::SetGizmoVisiblity(bool value)
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

	void PaintTool::OnSceneObjectSelected(ObjectSelectionChangedMessagePtr message)
	{
		if(m_Active)
		{
			//hide gizmo
			if(message->GetSceneObject())
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
			}
		}
		m_SelectedObject = message->GetSceneObject();
	}


	void PaintTool::SendMessageRec(SceneObjectPtr obj,MessagePtr msg)
	{
		obj->PostMessage(msg);
		GASS::IComponentContainer::ComponentContainerIterator iter = obj->GetChildren();
		while(iter.hasMoreElements())
		{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject>(iter.getNext());
			SendMessageRec(child,msg);
		}
	}
}
