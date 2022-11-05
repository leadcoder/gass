
#include "CreateTool.h"
#include "MouseToolController.h"
#include "Modules/Editor/EditorSystem.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Utils/GASSException.h"
#include "Core/Math/GASSPath.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Interface/GASSIWaypointListComponent.h"
#include "Sim/Interface/GASSILocationComponent.h"
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

		SceneObjectPtr parent_obj = m_ParentObject.lock();
		if(info.m_HasCollision && parent_obj)
		{
			if (!parent_obj->GetScene())
				GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Failed to get scene from parent", "MouseToolController::MouseDown");

			SceneObjectPtr scene_object = SimEngine::Get().CreateObjectFromTemplate(m_ObjectName);
			if(scene_object)
			{
				WaypointListComponentPtr wp_list = parent_obj->GetFirstComponentByClass<IWaypointListComponent>();
				if (m_AllowWPInsert && wp_list)
				{
					
					//NOTE: This dont work with splines, fix this!!
					int index = -1;
					if (!wp_list->GetEnableSpline())
					{
						std::vector<Vec3> points = wp_list->GetWaypoints(false);
						if (points.size() > 1)
						{
							Vec3 point;
							int t_index;

							const bool is_projected = !parent_obj->GetScene()->GetGeocentric();
							if (Path::GetClosestPointOnPath(info.m_3DPos, points, t_index, point))
							{
								Vec3 dist_vec = (info.m_3DPos - point);
								//if projected scene, only respect projected distance to make picking easy when line is below ground level
								if (is_projected)
									dist_vec.y = 0;
								const Float dist_to_path = dist_vec.Length();
								const Float max_distance = 1.0f;
								if (dist_to_path < max_distance && t_index < static_cast<int>(points.size()) - 1)
								{
									index = t_index + 1;
								}
							}
						}
					}

					if (index > 0)
						parent_obj->InsertChildSceneObject(scene_object, index, true);
					else
						parent_obj->AddChildSceneObject(scene_object, true);
					
					if (LocationComponentPtr location = scene_object->GetFirstComponentByClass<ILocationComponent>())
					{
						location->SetWorldPosition(info.m_3DPos);
					}
				}
				else
				{
					parent_obj->AddChildSceneObject(scene_object, true);
					if(LocationComponentPtr location = scene_object->GetFirstComponentByClass<ILocationComponent>())
					{
						location->SetWorldPosition(info.m_3DPos);
					}
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
