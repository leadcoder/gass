#include "KeyframeAnimationComponent.h"
#include "TriggerComponent.h"
#include "AITargetComponent.h"
#include "AnimationKeyComponent.h"


namespace GASS
{
	KeyframeAnimationComponent::KeyframeAnimationComponent(void) : m_Initialized(false), m_CurrentTime(0)
	{

	}	

	KeyframeAnimationComponent::~KeyframeAnimationComponent(void)
	{

	}

	void KeyframeAnimationComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("KeyframeAnimationComponent",new Creator<KeyframeAnimationComponent, IComponent>);
	}

	void KeyframeAnimationComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(KeyframeAnimationComponent::OnLoad,LocationLoadedMessage,1)); //load after agent
		GetSceneObject()->RegisterForMessage(REG_TMESS(KeyframeAnimationComponent::OnTransformationChanged,TransformationNotifyMessage ,0));

		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<AISceneManager>()->Register(listener);
	}

	void KeyframeAnimationComponent::OnLoad(LocationLoadedMessagePtr message)
	{
		m_Initialized = true;
		UpdateAnimation();
	}
	void KeyframeAnimationComponent::OnTransformationChanged(TransformationNotifyMessagePtr message)
	{
		m_Rotation = message->GetRotation();
		m_Position = message->GetPosition();
	}

	void KeyframeAnimationComponent::SceneManagerTick(double delta_time)
	{
		m_CurrentTime += delta_time;
		//loop?
		if(m_CurrentTime > m_Animation.m_Length)
			m_CurrentTime = 0;
		Key key = m_Animation.GetInterpolatedKeyFrame(m_CurrentTime);
		Vec3 pos = key.m_Pos;
		GetSceneObject()->GetParentSceneObject()->PostMessage(MessagePtr(new WorldPositionMessage(pos )));
	}

	void KeyframeAnimationComponent::UpdateAnimation()
	{
		std::vector<AnimationKeyComponentPtr> wp_vec;
		IComponentContainer::ComponentContainerIterator children = GetSceneObject()->GetChildren();
		m_Animation.m_KeyVector.clear();
		m_Animation.m_InterpolateMode = IM_LINEAR;
		while(children.hasMoreElements())
		{
			SceneObjectPtr child_obj =  boost::shared_static_cast<SceneObject>(children.getNext());
			AnimationKeyComponentPtr comp = child_obj->GetFirstComponentByClass<AnimationKeyComponent>();
			if(comp)
			{
				Key key = comp->GetKey();
				m_Animation.m_Length = key.m_Time;
				m_Animation.m_KeyVector.push_back(key);
			}
		}
		
		m_Animation.BuildInterpolationSplines();
	}
}


