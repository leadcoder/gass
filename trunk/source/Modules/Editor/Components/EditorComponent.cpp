//#include <stdafx.h>
#include "../EditorManager.h"
#include "../EditorMessages.h"
#include "EditorComponent.h"
#include "Sim/Scene/GASSCoreSceneObjectMessages.h"
#include "Sim/Scene/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Scene/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Scene/GASSSceneObject.h"
#include "Sim/Systems/GASSSimSystemManager.h"

#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Utils/GASSLogManager.h"

	
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
		GetSceneObject()->RegisterForMessage(REG_TMESS(EditorComponent::OnLoad,LoadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(EditorComponent::OnUnload,UnloadComponentsMessage,0));
		EditorManager::GetPtr()->GetMessageManager()->RegisterForMessage(REG_TMESS(EditorComponent::OnObjectLock,ObjectLockMessage,0));
		EditorManager::GetPtr()->GetMessageManager()->RegisterForMessage(REG_TMESS(EditorComponent::OnObjectVisible,ObjectVisibleMessage,0));
		EditorManager::GetPtr()->GetMessageManager()->RegisterForMessage(REG_TMESS(EditorComponent::OnSceneObjectSelected,ObjectSelectedMessage,0));
	}
	
	void EditorComponent::OnLoad(LoadComponentsMessagePtr message)
	{
		//update loaded settings
		SetLock(m_Lock); 
		SetVisible(m_Visible);
	}

	void EditorComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		EditorManager::GetPtr()->GetMessageManager()->UnregisterForMessage(UNREG_TMESS(EditorComponent::OnObjectLock,ObjectLockMessage));
		EditorManager::GetPtr()->GetMessageManager()->UnregisterForMessage(UNREG_TMESS(EditorComponent::OnObjectVisible,ObjectVisibleMessage));
		EditorManager::GetPtr()->GetMessageManager()->UnregisterForMessage(UNREG_TMESS(EditorComponent::OnSceneObjectSelected,ObjectSelectedMessage));
	}

	void EditorComponent::SetLock(bool value) 
	{
		m_Lock = value;
		SceneObjectPtr obj = GetSceneObject();
		if(obj)
		{
			if(m_Lock)
				EditorManager::GetPtr()->LockObject(obj);
			else
				EditorManager::GetPtr()->UnlockObject(obj);

		}
	}

	void EditorComponent::OnObjectLock(ObjectLockMessagePtr message)
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
			EditorManager::GetPtr()->GetMessageManager()->PostMessage(MessagePtr(new ObjectVisibleMessage(obj,m_Visible)));
		}
	}

	void EditorComponent::SetVisibilityTransparency(float value) 
	{
		m_VisibilityTransparency = value;
	}

	void EditorComponent::OnObjectVisible(ObjectVisibleMessagePtr message)
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

	void EditorComponent::OnSceneObjectSelected(ObjectSelectedMessagePtr message)
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

