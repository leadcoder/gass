//#include <stdafx.h>
#include "Modules/Editor/EditorSceneManager.h"
#include "Modules/Editor/EditorMessages.h"
#include "EditorComponent.h"

#include <memory>
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSComponentFactory.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSIBillboardComponent.h"
#include "Core/MessageSystem/GASSMessageManager.h"

namespace GASS
{
	EditorComponent::EditorComponent() :  
		m_SelectedColor(1.0, 1.0, 0.0, 1.0)
		
	{
		
	}

	EditorComponent::~EditorComponent()
	{

	}

	void EditorComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<EditorComponent>();
		RegisterGetSet("Lock",&EditorComponent::GetLock, &EditorComponent::SetLock);
		RegisterGetSet("Visible",&EditorComponent::GetVisible, &EditorComponent::SetVisible);
		RegisterGetSet("ChangeMaterialWhenSelected",&EditorComponent::GetChangeMaterialWhenSelected, &EditorComponent::SetChangeMaterialWhenSelected);
		RegisterGetSet("ShowBBWhenSelected",&EditorComponent::GetShowBBWhenSelected, &EditorComponent::SetShowBBWhenSelected);
		RegisterGetSet("VisibilityTransparency",&EditorComponent::GetVisibilityTransparency, &EditorComponent::SetVisibilityTransparency);
		RegisterGetSet("IconFile",&EditorComponent::GetIconFile, &EditorComponent::SetIconFile);
		RegisterGetSet("ShowInTree",&EditorComponent::GetShowInTree, &EditorComponent::SetShowInTree);
		RegisterGetSet("AllowRemove",&EditorComponent::GetAllowRemove, &EditorComponent::SetAllowRemove);
		RegisterGetSet("AllowDragAndDrop",&EditorComponent::GetAllowDragAndDrop, &EditorComponent::SetAllowDragAndDrop);
		RegisterMember("SelectedColor",&EditorComponent::m_SelectedColor);
		RegisterGetSet("EditName",&EditorComponent::GetEditName, &EditorComponent::SetEditName);
	}

	void EditorComponent::OnInitialize()
	{
		
		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS(EditorComponent::OnObjectLock,ObjectLockChangedEvent,0));
		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS(EditorComponent::OnObjectVisible,ObjectVisibilityChangedEvent,0));
		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS(EditorComponent::OnSelectionChanged,EditorSelectionChangedEvent,0));
		m_EditorSceneManager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<EditorSceneManager>();

		SetLock(m_Lock); 
		SetVisible(m_Visible);
	}

	void EditorComponent::OnDelete()
	{
		GetSceneObject()->GetScene()->UnregisterForMessage(UNREG_TMESS(EditorComponent::OnObjectLock,ObjectLockChangedEvent));
		GetSceneObject()->GetScene()->UnregisterForMessage(UNREG_TMESS(EditorComponent::OnObjectVisible,ObjectVisibilityChangedEvent));
		GetSceneObject()->GetScene()->UnregisterForMessage(UNREG_TMESS(EditorComponent::OnSelectionChanged,EditorSelectionChangedEvent));
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
			SceneObject::ComponentVector components;
			GetSceneObject()->GetComponentsByClass<IGeometryComponent>(components, false);
			for (auto comp : components)
			{
				std::dynamic_pointer_cast<IGeometryComponent>(comp)->SetVisible(m_Visible);
			}
			GetSceneObject()->PostRequest(std::make_shared<CollisionSettingsRequest>(m_Visible));
		}
	}

	void EditorComponent::OnSelectionChanged(EditorSelectionChangedEventPtr message)
	{
		if(!m_ChangeMaterialWhenSelected)
			return;
		auto bb = GetSceneObject()->GetFirstComponentByClass<IBillboardComponent>();
		if(message->IsSelected(GetSceneObject()))
		{
			m_Selected = true;
			if(bb)
				bb->SetColor(m_SelectedColor);
		}
		else if(m_Selected)
		{
			if (bb)
				bb->SetColor(ColorRGBA(1, 1, 1, 1));
			m_Selected = false;
		}
	}
}

