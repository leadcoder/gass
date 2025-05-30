#include "DistanceScaleComponent.h"

#include <memory>
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/GASSComponent.h"
#include "Sim/Interface/GASSICameraComponent.h"
#include "Sim/Interface/GASSIBillboardComponent.h"
#include "Sim/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Math/GASSMath.h"
#include "Core/Utils/GASSException.h"


#define MOVMENT_EPSILON 0.0000001
#define DISTANCE_SENDER 997

namespace GASS
{
	DistanceScaleComponent::DistanceScaleComponent() 
		
	{

	}

	DistanceScaleComponent::~DistanceScaleComponent()
	{

	}

	void DistanceScaleComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<DistanceScaleComponent>();
		RegisterGetSet("MaxDistance",&DistanceScaleComponent::GetMaxDistance, &DistanceScaleComponent::SetMaxDistance);
		RegisterGetSet("MinDistance",&DistanceScaleComponent::GetMinDistance, &DistanceScaleComponent::SetMinDistance);
		RegisterGetSet("ScaleLocation",&DistanceScaleComponent::GetScaleLocation, &DistanceScaleComponent::SetScaleLocation);
	}

	void DistanceScaleComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(DistanceScaleComponent::OnTransformation,TransformationChangedEvent,0));
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(DistanceScaleComponent::OnCameraChanged,CameraChangedEvent,1));

		CameraComponentPtr camera = GetSceneObject()->GetScene()->GetRootSceneObject()->GetFirstComponentByClass<ICameraComponent>(true);
		if(camera)
		{
			SceneObjectPtr cam_obj = camera->GetSceneObject();
			m_ActiveCameraObject = cam_obj;
			if(cam_obj)
			{
				cam_obj->RegisterForMessage(REG_TMESS(DistanceScaleComponent::OnCameraMoved, TransformationChangedEvent,1));
			}
		}
	}

	void DistanceScaleComponent::OnDelete()
	{
		SimEngine::Get().GetSimSystemManager()->UnregisterForMessage(UNREG_TMESS(DistanceScaleComponent::OnCameraChanged,CameraChangedEvent));
		if(SceneObjectPtr prev_camera = m_ActiveCameraObject.lock())
		{
			prev_camera->UnregisterForMessage(UNREG_TMESS(DistanceScaleComponent::OnCameraMoved, TransformationChangedEvent));
		}
	}


	void DistanceScaleComponent::OnCameraChanged(CameraChangedEventPtr message)
	{
		//Unregister from previous camera
		if(SceneObjectPtr prev_camera = m_ActiveCameraObject.lock())
		{
			prev_camera->UnregisterForMessage(UNREG_TMESS(DistanceScaleComponent::OnCameraMoved, TransformationChangedEvent));
		}
		CameraComponentPtr camera = message->GetViewport()->GetCamera();
		auto cam_obj = camera->GetSceneObject();

		m_ActiveCameraObject = cam_obj;
		cam_obj->RegisterForMessage(REG_TMESS(DistanceScaleComponent::OnCameraMoved, TransformationChangedEvent,1));
	}

	void DistanceScaleComponent::OnTransformation(TransformationChangedEventPtr /*message*/)
	{
		UpdateScale();
	}

	void DistanceScaleComponent::OnCameraMoved(TransformationChangedEventPtr /*message*/)
	{
		UpdateScale();
	}

	void DistanceScaleComponent::UpdateScale()
	{
		SceneObjectPtr camera = m_ActiveCameraObject.lock();
		if(camera)
		{
			LocationComponentPtr cam_location = camera->GetFirstComponentByClass<ILocationComponent>();
			Vec3 cam_pos = cam_location->GetWorldPosition();


			LocationComponentPtr gizmo_location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
			Vec3 gizmo_pos = gizmo_location->GetWorldPosition();
			
			Float dist = (gizmo_pos-cam_pos).Length();

			if(fabs(dist - m_LastDist) > MOVMENT_EPSILON)
			{
				m_LastDist = dist;
				//clamp
				if(dist > m_MaxDistance)
					dist = m_MaxDistance;
				if(dist < m_MinDistance)
					dist = m_MinDistance;


				float fov = 45;
				CameraComponentPtr camera_comp = camera->GetFirstComponentByClass<ICameraComponent>();	
				if(camera_comp)
				{
					fov = camera_comp->GetFov();
				}
			
				double scale_factor = 0.06;
				scale_factor = scale_factor*tan(Math::Deg2Rad(fov));
				Vec3 scale(scale_factor * dist,scale_factor* dist,scale_factor* dist);
				if(m_ScaleLocation)
					gizmo_location->SetScale(scale);
				else
				{
					auto bb = GetSceneObject()->GetFirstComponentByClass<IBillboardComponent>();
					if(bb)
						bb->SetScale(static_cast<float>(scale.x), static_cast<float>(scale.y));
				}
			}
		}
	}
}
