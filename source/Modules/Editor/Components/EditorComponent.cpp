//#include <stdafx.h>
#include "Modules/Editor/EditorSceneManager.h"
#include "Modules/Editor/EditorMessages.h"
#include "EditorComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"

namespace GASS
{
	EditorComponent::EditorComponent() : m_Lock (false), 
		m_Visible(true), 
		m_VisibilityTransparency(0.3f), 
		m_Selected(false), 
		m_SelectedColor(1.0, 1.0, 0.0, 1.0),
		m_ChangeMaterialWhenSelected(false),
		m_ShowBBWhenSelected(true),
		m_ShowInTree(false),
		m_AllowRemove(false),
		m_AllowDragAndDrop(false),
		m_EditName(true)
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
			GetSceneObject()->PostRequest(GeometryVisibilityRequestPtr(new GeometryVisibilityRequest(m_Visible)));
			GetSceneObject()->PostRequest(CollisionSettingsRequestPtr(new CollisionSettingsRequest(m_Visible)));
			
			/*if(m_Visible)
			{

				//if(m_ChangeMaterialWhenSelected)
				{
					if(m_Selected && m_ChangeMaterialWhenSelected)
						GetSceneObject()->PostRequest(MessagePtr(new MaterialMessage(m_SelectedColor,Vec3(-1,-1,-1))));
					else
						GetSceneObject()->PostRequest(MessagePtr(new MaterialMessage(Vec4(1,1,1,1),Vec3(-1,-1,-1))));
				}

				GetSceneObject()->PostRequest(MessagePtr(new CollisionSettingsRequest(true)));
			}
			else
			{
				GetSceneObject()->PostRequest(MessagePtr(new MaterialMessage(Vec4(1,1,1,m_VisibilityTransparency),Vec3(-1,-1,-1))));
				GetSceneObject()->PostRequest(MessagePtr(new CollisionSettingsRequest(false)));
			}*/
		}
	}

	void EditorComponent::OnSelectionChanged(EditorSelectionChangedEventPtr message)
	{
		if(!m_ChangeMaterialWhenSelected)
			return;
		if(message->IsSelected(GetSceneObject()))
		{
			m_Selected = true;
			GetSceneObject()->PostRequest(BillboardColorRequestPtr(new BillboardColorRequest(m_SelectedColor)));
		}
		else if(m_Selected)
		{
			GetSceneObject()->PostRequest(BillboardColorRequestPtr(new BillboardColorRequest(ColorRGBA(1,1,1,1))));
			m_Selected = false;
		}
	}
}

