//#include <stdafx.h>
#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/EditorSceneManager.h"
#include "Modules/Editor/EditorMessages.h"
#include "EditorComponent.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimSystemManager.h"
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
		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS(EditorComponent::OnObjectLock,ObjectLockChangedEvent,0));
		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS(EditorComponent::OnObjectVisible,ObjectVisibilityChangedEvent,0));
		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS(EditorComponent::OnSceneObjectSelected,ObjectSelectionChangedEvent,0));
		m_EditorSceneManager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<EditorSceneManager>();
		SetLock(m_Lock); 
		SetVisible(m_Visible);
	}

	void EditorComponent::OnDelete()
	{
		GetSceneObject()->GetScene()->UnregisterForMessage(UNREG_TMESS(EditorComponent::OnObjectLock,ObjectLockChangedEvent));
		GetSceneObject()->GetScene()->UnregisterForMessage(UNREG_TMESS(EditorComponent::OnObjectVisible,ObjectVisibilityChangedEvent));
		GetSceneObject()->GetScene()->UnregisterForMessage(UNREG_TMESS(EditorComponent::OnSceneObjectSelected,ObjectSelectionChangedEvent));
	}

	void EditorComponent::SetLock(bool value) 
	{
		m_Lock = value;
		SceneObjectPtr obj = GetSceneObject();
		if(obj && m_EditorSceneManager)
		{
			if(m_Lock)
				m_EditorSceneManager->LockObject(obj);
			else
				m_EditorSceneManager->UnlockObject(obj);

		}
	}

	void EditorComponent::OnObjectLock(ObjectLockChangedEventPtr message)
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
		if(obj && m_EditorSceneManager)
		{
			if(m_Visible)
				m_EditorSceneManager->UnhideObject(obj);
			else
				m_EditorSceneManager->HideObject(obj);
		}
	}

	void EditorComponent::SetVisibilityTransparency(float value) 
	{
		m_VisibilityTransparency = value;
	}

	void EditorComponent::OnObjectVisible(ObjectVisibilityChangedEventPtr message)
	{
		if(message->GetSceneObject() == GetSceneObject())
		{
			m_Visible = message->GetVisible();
			GetSceneObject()->PostMessage(MessagePtr(new MeshVisibilityMessage(m_Visible)));
			GetSceneObject()->PostMessage(MessagePtr(new CollisionSettingsMessage(m_Visible)));
			/*
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
			}*/
		}
	}

	void EditorComponent::OnSceneObjectSelected(ObjectSelectionChangedEventPtr message)
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

