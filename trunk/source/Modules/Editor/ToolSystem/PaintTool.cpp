#include "PaintTool.h"
#include "MouseToolController.h"
#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/EditorSceneManager.h"
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
		m_Controller(controller),m_Active(false)
	{
		//SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(PaintTool::OnSceneObjectSelected,ObjectSelectionChangedEvent,0));
		controller->GetEditorSceneManager()->GetScene()->RegisterForMessage(REG_TMESS(PaintTool::OnSceneObjectSelected,ObjectSelectionChangedEvent,0));
	}

	PaintTool::~PaintTool()
	{

	}

	void PaintTool::MouseMoved(const MouseData &data, const SceneCursorInfo &info)
	{
		int from_id = (int) this;
		SceneObjectPtr selected(m_SelectedObject,NO_THROW);
		if(m_MouseIsDown)// && selected)
		{
			if(selected)
			{
				HeightmapTerrainComponentPtr terrain = selected->GetFirstComponentByClass<IHeightmapTerrainComponent>();
				if(terrain)
				{
					selected->GetParentSceneObject()->PostRequest(TerrainHeightModifyRequestPtr(new TerrainHeightModifyRequest(TerrainHeightModifyRequest::MT_DEFORM,info.m_3DPos,116, 90,1.0)));
				}
			}

			GASS::SceneMessagePtr paint_msg(new PaintRequest(info.m_3DPos,selected,from_id));
			m_Controller->GetEditorSceneManager()->GetScene()->SendImmediate(paint_msg);
		}
		SceneObjectPtr gizmo = GetMasterGizmo();
		if(gizmo)
		{
			gizmo->PostRequest(WorldPositionRequestPtr(new WorldPositionRequest(info.m_3DPos,from_id)));
		}
	}

	void PaintTool::MouseDown(const MouseData &data, const SceneCursorInfo &info)
	{
		m_MouseIsDown = true;
	}

	void PaintTool::MouseUp(const MouseData &data, const SceneCursorInfo &info)
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
		SceneObjectPtr gizmo(m_MasterGizmoObject,NO_THROW);
		if(!gizmo &&  m_Controller->GetEditorSceneManager()->GetScene())
		{
			ScenePtr scene = m_Controller->GetEditorSceneManager()->GetScene();
			std::string gizmo_name = "PaintGizmo";
			GASS::SceneObjectPtr scene_object = m_Controller->GetEditorSceneManager()->GetScene()->LoadObjectFromTemplate(gizmo_name,m_Controller->GetEditorSceneManager()->GetScene()->GetRootSceneObject());
			m_MasterGizmoObject = scene_object;
			gizmo = scene_object;

			//Send selection message to inform gizmo about current object
			if(gizmo)
			{
				SceneObjectPtr current (m_SelectedObject,NO_THROW);
				if(current)
				{
					m_Controller->GetEditorSceneManager()->SelectSceneObject(current);
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
			SendMessageRec(gizmo,CollisionSettingsRequestPtr(new CollisionSettingsRequest(value,from_id)));
			SendMessageRec(gizmo,VisibilityRequestPtr(new VisibilityRequest(value,from_id)));
		}
	}

	void PaintTool::OnSceneObjectSelected(ObjectSelectionChangedEventPtr message)
	{
		/*if(m_Active)
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
		}*/
		m_SelectedObject = message->GetSceneObject();
	}


	void PaintTool::SendMessageRec(SceneObjectPtr obj,SceneObjectRequestMessagePtr msg)
	{
		obj->PostRequest(msg);
		GASS::IComponentContainer::ComponentContainerIterator iter = obj->GetChildren();
		while(iter.hasMoreElements())
		{
			SceneObjectPtr child = STATIC_PTR_CAST<SceneObject>(iter.getNext());
			SendMessageRec(child,msg);
		}
	}
}
