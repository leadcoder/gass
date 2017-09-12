
#include "CreateTool.h"
#include "MouseToolController.h"
#include "Modules/Editor/EditorSystem.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Utils/GASSException.h"
#include "Core/Math/GASSPath.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Interface/GASSIWaypointListComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"

namespace GASS
{
	CreateTool::CreateTool(MouseToolController* controller): m_MouseIsDown(false),
		m_Controller(controller),
		m_AllowWPInsert(true)
	{
		
	}

	CreateTool::~CreateTool()
	{

	}

	void CreateTool::MouseMoved(const MouseData &/*data*/, const SceneCursorInfo &/*info*/)
	{
	}


	void CreateTool::MouseDown(const MouseData &/*data*/, const SceneCursorInfo &info)
	{
		m_MouseIsDown = true;
		SceneObjectPtr obj_under_cursor =info.m_ObjectUnderCursor.lock();

		SceneObjectPtr parent_obj = m_ParentObject.lock();
		if(obj_under_cursor && parent_obj)
		{
			SceneObjectPtr scene_object = SimEngine::Get().CreateObjectFromTemplate(m_ObjectName);
			if(scene_object)
			{
				WaypointListComponentPtr wp_list = parent_obj->GetFirstComponentByClass<IWaypointListComponent>();
				if (m_AllowWPInsert && wp_list)
				{
					std::vector<Vec3> points = wp_list->GetWaypoints(false);
					int index = -1;
					if (points.size() > 1)
					{
						GASS::Float dist_to_path;
						int t_index;
						Path::GetPathDistance(info.m_3DPos, points, t_index, dist_to_path);
						if (dist_to_path < 1 && t_index < static_cast<int>(points.size()) - 1)
						{
							index = t_index+1;
						}
					}

					if (index > 0)
						parent_obj->InsertChildSceneObject(scene_object, index, true);
					else
						parent_obj->AddChildSceneObject(scene_object, true);
					
					int from_id = GASS_PTR_TO_INT(this);
					scene_object->SendImmediateRequest(WorldPositionRequestPtr(new WorldPositionRequest(info.m_3DPos, from_id)));
				}
				else
				{
					parent_obj->AddChildSceneObject(scene_object, true);
					int from_id = GASS_PTR_TO_INT(this);
					scene_object->SendImmediateRequest(WorldPositionRequestPtr(new WorldPositionRequest(info.m_3DPos, from_id)));
				}
			}
			else
			{
				//failed to create object
				GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"Failed to create object","MouseToolController::Init()");
			}
		}
	}

	void CreateTool::MouseUp(const MouseData &/*data*/, const SceneCursorInfo &/*info*/)
	{
		m_MouseIsDown = false;
	}
}
