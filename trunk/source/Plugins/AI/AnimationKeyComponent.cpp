#include "AnimationKeyComponent.h"
#include "KeyframeAnimationComponent.h"


namespace GASS
{
	AnimationKeyComponent::AnimationKeyComponent(void) : m_Initialized(false)
	{
		m_Key.m_Rot = Quaternion::IDENTITY;
	}	

	AnimationKeyComponent::~AnimationKeyComponent(void)
	{

	}

	void AnimationKeyComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("AnimationKeyComponent",new Creator<AnimationKeyComponent, IComponent>);
		RegisterProperty<Float>("Time", &GetTime, &SetTime);
		RegisterProperty<Vec3>("Position", &GetPosition, &SetPosition);
		RegisterProperty<Quaternion>("Rotation", &GetRotation, &SetRotation);
		RegisterProperty<Vec3>("Scale", &GetScale, &SetScale);
		RegisterProperty<bool>("UsePosition", &GetUsePosition, &SetUsePosition);
		RegisterProperty<bool>("UseRotation", &GetUseRotation, &SetUseRotation);
	}

	void AnimationKeyComponent::OnInitialize()
	{
		m_Initialized = false;
	}

	void AnimationKeyComponent::UpdateAnimation()
	{
		if(!m_Initialized)
			return;

		KeyframeAnimationComponentPtr animation_comp = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<KeyframeAnimationComponent>();

		if(animation_comp)
			animation_comp->UpdateAnimation();
	}
}


