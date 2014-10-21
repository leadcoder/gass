#include "DistanceScaleComponent.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Interface/GASSIViewport.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"

#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSException.h"


#define MOVMENT_EPSILON 0.0000001
#define DISTANCE_SENDER 997

namespace GASS
{
	DistanceScaleComponent::DistanceScaleComponent() : m_MaxDistance(30000), 
		m_MinDistance(0.1),
		m_ScaleLocation(false)
	{

	}

	DistanceScaleComponent::~DistanceScaleComponent()
	{

	}

	void DistanceScaleComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("DistanceScaleComponent",new Creator<DistanceScaleComponent, Component>);
		RegisterProperty<float>("MaxDistance",&DistanceScaleComponent::GetMaxDistance, &DistanceScaleComponent::SetMaxDistance);
		RegisterProperty<float>("MinDistance",&DistanceScaleComponent::GetMinDistance, &DistanceScaleComponent::SetMinDistance);
		RegisterProperty<bool>("ScaleLocation",&DistanceScaleComponent::GetScaleLocation, &DistanceScaleComponent::SetScaleLocation);
	}

	void DistanceScaleComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(DistanceScaleComponent::OnTransformation,TransformationChangedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(DistanceScaleComponent::OnWorldPosition,WorldPositionRequest,0));
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(DistanceScaleComponent::OnCameraChanged,CameraChangedEvent,1));

		//EditorSceneManagerPtr esm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<EditorSceneManager>();
		//m_ActiveCameraObject = esm->GetActiveCameraObject();
		CameraComponentPtr camera = GetSceneObject()->GetScene()->GetRootSceneObject()->GetFirstComponentByClass<ICameraComponent>(true);
		if(camera)
		{
			BaseSceneComponentPtr bsc = DYNAMIC_PTR_CAST<BaseSceneComponent>(camera);
			SceneObjectPtr cam_obj = bsc->GetSceneObject();
			m_ActiveCameraObject = cam_obj;
			if(cam_obj)
			{
				cam_obj->RegisterForMessage(REG_TMESS(DistanceScaleComponent::OnCameraMoved, TransformationChangedEvent,1));
				cam_obj->RegisterForMessage(REG_TMESS(DistanceScaleComponent::OnCameraParameter,CameraParameterRequest,1));
			}
		}
	}

	void DistanceScaleComponent::OnDelete()
	{
		SimEngine::Get().GetSimSystemManager()->UnregisterForMessage(UNREG_TMESS(DistanceScaleComponent::OnCameraChanged,CameraChangedEvent));
		if(SceneObjectPtr(m_ActiveCameraObject,NO_THROW))
		{
			SceneObjectPtr prev_camera = SceneObjectPtr(m_ActiveCameraObject,NO_THROW);
			prev_camera->UnregisterForMessage(UNREG_TMESS(DistanceScaleComponent::OnCameraMoved, TransformationChangedEvent));
		}
	}


	void DistanceScaleComponent::OnCameraChanged(CameraChangedEventPtr message)
	{
		//Unregister from previous camera
		if(SceneObjectPtr(m_ActiveCameraObject,NO_THROW))
		{
			SceneObjectPtr prev_camera = SceneObjectPtr(m_ActiveCameraObject,NO_THROW);
			prev_camera->UnregisterForMessage(UNREG_TMESS(DistanceScaleComponent::OnCameraMoved, TransformationChangedEvent));
			prev_camera->UnregisterForMessage(UNREG_TMESS(DistanceScaleComponent::OnCameraParameter,CameraParameterRequest));
		}
		CameraComponentPtr camera = message->GetViewport()->GetCamera();
		SceneObjectPtr cam_obj = DYNAMIC_PTR_CAST<BaseSceneComponent>(camera)->GetSceneObject();

		m_ActiveCameraObject = cam_obj;
		cam_obj->RegisterForMessage(REG_TMESS(DistanceScaleComponent::OnCameraMoved, TransformationChangedEvent,1));
		cam_obj->RegisterForMessage(REG_TMESS(DistanceScaleComponent::OnCameraParameter,CameraParameterRequest,1));
	}

	void DistanceScaleComponent::OnCameraParameter(CameraParameterRequestPtr message)
	{
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

				if(m_ScaleLocation)
					GetSceneObject()->PostRequest(ScaleRequestPtr(new ScaleRequest(scale)));
				else
				{
					GetSceneObject()->PostRequest(ScaleRequestPtr(new ScaleRequest(Vec3(1,1,1))));
					GetSceneObject()->PostRequest(GeometryScaleRequestPtr(new GeometryScaleRequest(scale)));
				}

			}
			break;
		case CameraParameterRequest::CAMERA_CLIP_DISTANCE:
			{
				
			}
			break;
		}
	}

	void DistanceScaleComponent::OnTransformation(TransformationChangedEventPtr message)
	{
		UpdateScale();
	}

	void DistanceScaleComponent::OnWorldPosition(WorldPositionRequestPtr message)
	{
		
	}

	void DistanceScaleComponent::OnCameraMoved(TransformationChangedEventPtr message)
	{
		UpdateScale();
	}

	void DistanceScaleComponent::UpdateScale()
	{
		SceneObjectPtr camera(m_ActiveCameraObject,NO_THROW);
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
					GetSceneObject()->PostRequest(ScaleRequestPtr(new ScaleRequest(scale)));
				else
					GetSceneObject()->PostRequest(GeometryScaleRequestPtr(new GeometryScaleRequest(scale)));
			}
		}
	}
}
