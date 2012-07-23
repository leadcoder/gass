#include "AnimationKeyComponent.h"
#include "KeyframeAnimationComponent.h"


namespace GASS
{
	AnimationKeyComponent::AnimationKeyComponent(void) : m_Initlized(false)
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
		GetSceneObject()->RegisterForMessage(REG_TMESS(AnimationKeyComponent::OnLoad,LoadComponentsMessage,1)); //load after agent
		GetSceneObject()->RegisterForMessage(REG_TMESS(AnimationKeyComponent::OnUnload,UnloadComponentsMessage,0));
	}

	void AnimationKeyComponent::OnLoad(LoadComponentsMessagePtr message)
	{
		m_Initlized = true;
	}
	
	void AnimationKeyComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		
	}

	void AnimationKeyComponent::UpdateAnimation()
	{
		if(!m_Initlized)
			return;

		KeyframeAnimationComponentPtr animation_comp = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<KeyframeAnimationComponent>();

		if(animation_comp)
			animation_comp->UpdateAnimation();
	}
}


