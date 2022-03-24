#pragma once
#include "Sim/GASSCommon.h"
#include "Sim/GASSComponent.h"
#include "Core/Utils/GASSColorRGBA.h"
#include "Modules/Editor/EditorCommon.h"
#include "Modules/Editor/EditorMessages.h"
#include "Modules/Editor/EditorSceneManager.h"


namespace GASS
{
	class EditorModuleExport EditorComponent : public Reflection<EditorComponent,Component>
	{
	public:
		EditorComponent();
		~EditorComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
		bool GetShowBBWhenSelected() const {return m_ShowBBWhenSelected;}
		void SetShowBBWhenSelected(bool value) { m_ShowBBWhenSelected = value; }
		std::string GetIconFile() const { return m_IconFile;}
		void SetIconFile(const std::string &value) { m_IconFile = value;}
		void SetShowInTree(bool value) {m_ShowInTree = value;}
		bool GetShowInTree() const {return m_ShowInTree;}
		bool GetAllowRemove() const {return m_AllowRemove;}
		void SetAllowRemove(bool value) {m_AllowRemove = value;}
		bool GetAllowDragAndDrop() const {return m_AllowDragAndDrop;}
		void SetAllowDragAndDrop(bool value) { m_AllowDragAndDrop = value; }
		bool GetChangeMaterialWhenSelected() const { return m_ChangeMaterialWhenSelected; }
		void SetChangeMaterialWhenSelected(bool value) { m_ChangeMaterialWhenSelected = value; }
		bool GetVisible() const{return m_Visible;}
		void SetVisible(bool value);
		bool GetLock() const {return m_Lock;}
		void SetLock(bool value);
		void SetEditName(bool value) { m_EditName = value; }
		bool GetEditName() const { return m_EditName; }
	private:
		
		void OnObjectVisible(ObjectVisibilityChangedEventPtr message);
		void OnObjectLock(ObjectLockChangedEventPtr message);
		float GetVisibilityTransparency() const{return m_VisibilityTransparency;}
		void SetVisibilityTransparency(float value);
		void OnSelectionChanged(EditorSelectionChangedEventPtr message);
		
		bool m_EditName{true};
		bool m_Lock{false};
		bool m_Visible{true};
		float m_VisibilityTransparency{0.3f};
		bool m_Selected{false};
		
		bool m_ChangeMaterialWhenSelected{false};
		bool m_ShowBBWhenSelected{true};
		std::string m_IconFile;
		bool m_ShowInTree{false};
		bool m_AllowRemove{false};
		bool m_AllowDragAndDrop{false};
		ColorRGBA m_SelectedColor;
		EditorSceneManagerPtr m_EditorSceneManager;
	};
	using EditorComponentPtr = std::shared_ptr<EditorComponent>;
}


