#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/EditorMessages.h"
#include "Modules/Editor/ToolSystem/MouseToolController.h"
#include "DistanceScaleComponent.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"

#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSException.h"


#define MOVMENT_EPSILON 0.0000001
#define DISTANCE_SENDER 997

namespace GASS
{

	DistanceScaleComponent::DistanceScaleComponent() : m_MaxDistance(30000), m_MinDistance(0.1),m_ScaleLocation(false)
	{

	}

	DistanceScaleComponent::~DistanceScaleComponent()
	{

	}

	void DistanceScaleComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("DistanceScaleComponent",new Creator<DistanceScaleComponent, IComponent>);
		RegisterProperty<float>("MaxDistance",&DistanceScaleComponent::GetMaxDistance, &DistanceScaleComponent::SetMaxDistance);
		RegisterProperty<float>("MinDistance",&DistanceScaleComponent::GetMinDistance, &DistanceScaleComponent::SetMinDistance);
		RegisterProperty<bool>("ScaleLocation",&DistanceScaleComponent::GetScaleLocation, &DistanceScaleComponent::SetScaleLocation);
	}

	void DistanceScaleComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(DistanceScaleComponent::OnTransformation,TransformationNotifyMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(DistanceScaleComponent::OnWorldPosition,WorldPositionMessage,0));

		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS(DistanceScaleComponent::OnChangeCamera,ChangeCameraRequest,1));

		EditorSystemPtr es = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<EditorSystem>();
		if(!es)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get EditorSystem", "DistanceScaleComponent::OnLoad");

		m_ActiveCameraObject = es->GetActiveCameraObject();
		SceneObjectPtr cam_obj(m_ActiveCameraObject,boost::detail::sp_nothrow_tag());
		if(cam_obj)
		{
			cam_obj->RegisterForMessage(REG_TMESS(DistanceScaleComponent::OnCameraMoved, TransformationNotifyMessage,1));
			cam_obj->RegisterForMessage(REG_TMESS(DistanceScaleComponent::OnCameraParameter,CameraParameterMessage,1));
		}
	}

	void DistanceScaleComponent::OnDelete()
	{
		GetSceneObject()->GetScene()->UnregisterForMessage(UNREG_TMESS(DistanceScaleComponent::OnChangeCamera,ChangeCameraRequest));
		if(SceneObjectPtr(m_ActiveCameraObject,boost::detail::sp_nothrow_tag()))
		{
			SceneObjectPtr prev_camera = SceneObjectPtr(m_ActiveCameraObject,boost::detail::sp_nothrow_tag());
			prev_camera->UnregisterForMessage(UNREG_TMESS(DistanceScaleComponent::OnCameraMoved, TransformationNotifyMessage));
		}
	}


	void DistanceScaleComponent::OnChangeCamera(ChangeCameraRequestPtr message)
	{
		//Unregister from previous camera
		if(SceneObjectPtr(m_ActiveCameraObject,boost::detail::sp_nothrow_tag()))
		{
			SceneObjectPtr prev_camera = SceneObjectPtr(m_ActiveCameraObject,boost::detail::sp_nothrow_tag());
			prev_camera->UnregisterForMessage(UNREG_TMESS(DistanceScaleComponent::OnCameraMoved, TransformationNotifyMessage));
			prev_camera->UnregisterForMessage(UNREG_TMESS(DistanceScaleComponent::OnCameraParameter,CameraParameterMessage));
		}
		SceneObjectPtr cam_obj =  message->GetCamera();
		m_ActiveCameraObject = cam_obj;
		cam_obj->RegisterForMessage(REG_TMESS(DistanceScaleComponent::OnCameraMoved, TransformationNotifyMessage,1));
		cam_obj->RegisterForMessage(REG_TMESS(DistanceScaleComponent::OnCameraParameter,CameraParameterMessage,1));
	}

	void DistanceScaleComponent::OnCameraParameter(CameraParameterMessagePtr message)
	{
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

				if(m_ScaleLocation)
					GetSceneObject()->PostMessage(MessagePtr(new ScaleMessage(scale)));
				else
				{
					GetSceneObject()->PostMessage(MessagePtr(new ScaleMessage(Vec3(1,1,1))));
					GetSceneObject()->PostMessage(MessagePtr(new GeometryScaleMessage(scale)));
				}

			}
			break;
		case CameraParameterMessage::CAMERA_CLIP_DISTANCE:
			{
				
			}
			break;
		}
	}

	void DistanceScaleComponent::OnTransformation(TransformationNotifyMessagePtr message)
	{
		UpdateScale();
	}

	void DistanceScaleComponent::OnWorldPosition(WorldPositionMessagePtr message)
	{
		
	}

	void DistanceScaleComponent::OnCameraMoved(TransformationNotifyMessagePtr message)
	{
		UpdateScale();
	}

	void DistanceScaleComponent::UpdateScale()
	{
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
				if(dist > m_MaxDistance)
					dist = m_MaxDistance;
				if(dist < m_MinDistance)
					dist = m_MinDistance;

				float scale_factor = 0.06;
				Vec3 scale(scale_factor * dist,scale_factor* dist,scale_factor* dist);
				if(m_ScaleLocation)
					GetSceneObject()->PostMessage(MessagePtr(new ScaleMessage(scale)));
				else
					GetSceneObject()->PostMessage(MessagePtr(new GeometryScaleMessage(scale)));
			}
		}
	}
}
