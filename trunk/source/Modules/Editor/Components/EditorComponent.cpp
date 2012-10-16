//#include <stdafx.h>
#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/EditorMessages.h"
#include "EditorComponent.h"
#include "Sim/Scene/GASSCoreSceneObjectMessages.h"
#include "Sim/Scene/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Scene/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Scene/GASSSceneObject.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"

#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSException.h"

	
namespace GASS
{

	EditorComponent::EditorComponent() : m_Lock (false), 
		m_Visible(true), 
		m_VisibilityTransparency(0.3), 
		m_Selected(false), 
		m_SelectedColor(1,1,0,1),
		m_ChangeMaterialWhenSelected(false),
		m_ShowBBWhenSelected(true),
		m_ShowInTree(false),
		m_AllowRemove(false),
		m_AllowDragAndDrop(false)
	{
		
	}

	EditorComponent::~EditorComponent()
	{

	}

	void EditorComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("EditorComponent",new Creator<EditorComponent, IComponent>);
		RegisterProperty<bool>("Lock",&EditorComponent::GetLock, &EditorComponent::SetLock);
		RegisterProperty<bool>("Visible",&EditorComponent::GetVisible, &EditorComponent::SetVisible);
		RegisterProperty<bool>("ChangeMaterialWhenSelected",&EditorComponent::GetChangeMaterialWhenSelected, &EditorComponent::SetChangeMaterialWhenSelected);
		RegisterProperty<bool>("ShowBBWhenSelected",&EditorComponent::GetShowBBWhenSelected, &EditorComponent::SetShowBBWhenSelected);
		RegisterProperty<float>("VisibilityTransparency",&EditorComponent::GetVisibilityTransparency, &EditorComponent::SetVisibilityTransparency);
		RegisterProperty<std::string>("IconFile",&EditorComponent::GetIconFile, &EditorComponent::SetIconFile);
		RegisterProperty<bool>("ShowInTree",&EditorComponent::GetShowInTree, &EditorComponent::SetShowInTree);
		RegisterProperty<bool>("AllowRemove",&EditorComponent::GetAllowRemove, &EditorComponent::SetAllowRemove);
		RegisterProperty<bool>("AllowDragAndDrop",&EditorComponent::GetAllowDragAndDrop, &EditorComponent::SetAllowDragAndDrop);
		RegisterProperty<Vec4>("SelectedColor",&EditorComponent::GetSelectedColor, &EditorComponent::SetSelectedColor);
		
	}

	void EditorComponent::OnInitialize()
	{
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(EditorComponent::OnObjectLock,ObjectLockChangedMessage,0));
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(EditorComponent::OnObjectVisible,ObjectVisibilityChangedMessage,0));
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(EditorComponent::OnSceneObjectSelected,ObjectSelectionChangedMessage,0));
		m_EditorSystem = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<EditorSystem>();
		if(!m_EditorSystem)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get EditorSystem", " EditorComponent::OnInitialize");

		SetLock(m_Lock); 
		SetVisible(m_Visible);
	}

	void EditorComponent::OnDelete()
	{
		SimEngine::Get().GetSimSystemManager()->UnregisterForMessage(UNREG_TMESS(EditorComponent::OnObjectLock,ObjectLockChangedMessage));
		SimEngine::Get().GetSimSystemManager()->UnregisterForMessage(UNREG_TMESS(EditorComponent::OnObjectVisible,ObjectVisibilityChangedMessage));
		SimEngine::Get().GetSimSystemManager()->UnregisterForMessage(UNREG_TMESS(EditorComponent::OnSceneObjectSelected,ObjectSelectionChangedMessage));
	}

	void EditorComponent::SetLock(bool value) 
	{
		m_Lock = value;
		SceneObjectPtr obj = GetSceneObject();
		if(obj)
		{
			
			if(m_Lock)
				m_EditorSystem->LockObject(obj);
			else
				m_EditorSystem->UnlockObject(obj);

		}
	}

	void EditorComponent::OnObjectLock(ObjectLockChangedMessagePtr message)
	{
		if(message->GetSceneObject() == GetSceneObject())
		{
			m_Lock = message->GetLock();
		}
	}

	void EditorComponent::SetVisible(bool value) 
	{
		m_Visible = value;
		SceneObjectPtr obj = GetSceneObject();
		if(obj)
		{
			if(m_Visible)
				m_EditorSystem->UnhideObject(obj);
			else
				m_EditorSystem->HideObject(obj);
		}
	}

	void EditorComponent::SetVisibilityTransparency(float value) 
	{
		m_VisibilityTransparency = value;
	}

	void EditorComponent::OnObjectVisible(ObjectVisibilityChangedMessagePtr message)
	{
		if(message->GetSceneObject() == GetSceneObject())
		{
			m_Visible = message->GetVisible();
			if(m_Visible)
			{

				//if(m_ChangeMaterialWhenSelected)
				{
					if(m_Selected && m_ChangeMaterialWhenSelected)
						GetSceneObject()->PostMessage(MessagePtr(new MaterialMessage(m_SelectedColor,Vec3(-1,-1,-1))));
					else
						GetSceneObject()->PostMessage(MessagePtr(new MaterialMessage(Vec4(1,1,1,1),Vec3(-1,-1,-1))));
				}

				GetSceneObject()->PostMessage(MessagePtr(new CollisionSettingsMessage(true)));
			}
			else
			{
				GetSceneObject()->PostMessage(MessagePtr(new MaterialMessage(Vec4(1,1,1,m_VisibilityTransparency),Vec3(-1,-1,-1))));
				GetSceneObject()->PostMessage(MessagePtr(new CollisionSettingsMessage(false)));
			}
		}
	}

	void EditorComponent::OnSceneObjectSelected(ObjectSelectionChangedMessagePtr message)
	{
		if(!m_ChangeMaterialWhenSelected)
			return;
		SceneObjectPtr new_selection = message->GetSceneObject();
		if(GetSceneObject() == new_selection)
		{
			m_Selected = true;
			if(m_Visible)
				GetSceneObject()->PostMessage(MessagePtr(new MaterialMessage(m_SelectedColor,Vec3(1,1,1))));
		}
		else if(m_Selected)
		{
			if(m_Visible)
				GetSceneObject()->PostMessage(MessagePtr(new MaterialMessage(Vec4(1,1,1,1),Vec3(1,1,1))));
			m_Selected = false;
		}
	}
}

