#pragma once
#include "Sim/GASSCommon.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Core/Utils/GASSColorRGBA.h"
#include "Modules/Editor/EditorCommon.h"
#include "Modules/Editor/EditorMessages.h"
#include "Modules/Editor/EditorSceneManager.h"


namespace GASS
{
	class EditorModuleExport EditorComponent : public Reflection<EditorComponent,BaseSceneComponent>
	{
	public:
		EditorComponent();
		virtual ~EditorComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
		bool GetShowBBWhenSelected() const {return m_ShowBBWhenSelected;}
		std::string GetIconFile() const { return m_IconFile;}
		void SetIconFile(const std::string &value) { m_IconFile = value;}
		void SetShowInTree(bool value) {m_ShowInTree = value;}
		bool GetShowInTree() const {return m_ShowInTree;}
		bool GetAllowRemove() const {return m_AllowRemove;}
		void SetAllowRemove(bool value) {m_AllowRemove = value;}
		bool GetAllowDragAndDrop() const {return m_AllowDragAndDrop;}
		bool GetVisible() const{return m_Visible;}
		void SetVisible(bool value);
		bool GetLock() const {return m_Lock;}
		void SetLock(bool value);
		ADD_PROPERTY(bool,EditName)
	private:
		ADD_PROPERTY(ColorRGBA,SelectedColor)
		void OnObjectVisible(ObjectVisibilityChangedEventPtr message);
		void OnObjectLock(ObjectLockChangedEventPtr message);
		float GetVisibilityTransparency() const{return m_VisibilityTransparency;}
		void SetVisibilityTransparency(float value);
		void OnSelectionChanged(EditorSelectionChangedEventPtr message);
		bool GetChangeMaterialWhenSelected() const {return m_ChangeMaterialWhenSelected;}
		void SetChangeMaterialWhenSelected(bool value) {m_ChangeMaterialWhenSelected = value;}
		void SetShowBBWhenSelected(bool value) {m_ShowBBWhenSelected = value;}
		void SetAllowDragAndDrop(bool value) {m_AllowDragAndDrop = value;}
		
		bool m_Lock;
		bool m_Visible;
		float m_VisibilityTransparency;
		bool m_Selected;
		
		bool m_ChangeMaterialWhenSelected;
		bool m_ShowBBWhenSelected;
		std::string m_IconFile;
		bool m_ShowInTree;
		bool m_AllowRemove;
		bool m_AllowDragAndDrop;
		EditorSceneManagerPtr m_EditorSceneManager;
	};
	typedef GASS_SHARED_PTR<EditorComponent> EditorComponentPtr;
}


