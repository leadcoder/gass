#include "PaintTool.h"

#include <memory>
#include "MouseToolController.h"
#include "Modules/Editor/EditorSceneManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"

namespace GASS
{

	PaintTool::PaintTool(MouseToolController* controller): m_MouseIsDown(false),
		m_Controller(controller),m_Active(false)
	{
		controller->GetEditorSceneManager()->GetScene()->RegisterForMessage(REG_TMESS(PaintTool::OnSelectionChanged,EditorSelectionChangedEvent,0));
	}

	PaintTool::~PaintTool()
	{

	}

	void PaintTool::MouseMoved(const MouseData &/*data*/, const SceneCursorInfo &info)
	{
		int from_id = GASS_PTR_TO_INT(this);
		SceneObjectPtr selected = m_SelectedObject.lock();
		if(m_MouseIsDown)
		{
			if(selected)
			{
				//selected->GetParentSceneObject()->PostRequest(std::make_shared<TerrainHeightModifyRequest>(TerrainHeightModifyRequest::MT_DEFORM,info.m_3DPos,116.0f, 90.0f,1.0f,0.0f));
			}
			SceneMessagePtr paint_msg(new PaintRequest(info.m_3DPos, selected, from_id));
			m_Controller->GetEditorSceneManager()->GetScene()->SendImmediate(paint_msg);
		}
		SceneObjectPtr gizmo = GetMasterGizmo();
		if(gizmo)
		{
			gizmo->GetFirstComponentByClass<ILocationComponent>()->SetWorldPosition(info.m_3DPos);
		}
	}

	void PaintTool::MouseDown(const MouseData &/*data*/, const SceneCursorInfo &/*info*/)
	{
		m_MouseIsDown = true;
	}

	void PaintTool::MouseUp(const MouseData &/*data*/, const SceneCursorInfo &/*info*/)
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
		SceneObjectPtr gizmo = m_MasterGizmoObject.lock();
		if(!gizmo &&  m_Controller->GetEditorSceneManager()->GetScene())
		{
			ScenePtr scene = m_Controller->GetEditorSceneManager()->GetScene();
			std::string gizmo_name = "PaintGizmo";
			SceneObjectPtr scene_object = m_Controller->GetEditorSceneManager()->GetScene()->LoadObjectFromTemplate(gizmo_name,m_Controller->GetEditorSceneManager()->GetScene()->GetRootSceneObject());
			m_MasterGizmoObject = scene_object;
			gizmo = scene_object;

			//Send selection message to inform gizmo about current object
			if(gizmo)
			{
				SceneObjectPtr current = m_SelectedObject.lock();
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
		if (gizmo)
		{
			auto iter = gizmo->GetChildren();
			while (iter.hasMoreElements())
			{
				auto child = iter.getNext();
				child->GetFirstComponentByClass<ILocationComponent>()->SetVisible(value);
			}
		}
	}

	void PaintTool::OnSelectionChanged(EditorSelectionChangedEventPtr message)
	{
		m_SelectedObject.reset();
		for (size_t i = 0; i< message->m_Selection.size(); i++)
		{
			SceneObjectPtr obj = message->m_Selection[i].lock();
			if(obj && obj->GetFirstComponentByClass<IHeightmapTerrainComponent>())
			{
				m_SelectedObject = obj;
			}
		}
	}

	void PaintTool::SendMessageRec(SceneObjectPtr obj,SceneObjectRequestMessagePtr msg)
	{
		obj->PostRequest(msg);
		auto iter = obj->GetChildren();
		while(iter.hasMoreElements())
		{
			auto child = iter.getNext();
			SendMessageRec(child,msg);
		}
	}
}
