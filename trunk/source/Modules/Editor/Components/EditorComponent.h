#pragma once
#include "Sim/Components/GASSBaseSceneComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "../EditorCommon.h"
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
		void SetShowInTree(bool value) {m_ShowInTree = value;}
		bool GetShowInTree() const {return m_ShowInTree;}
		bool GetAllowRemove() const {return m_AllowRemove;}
		void SetAllowRemove(bool value) {m_AllowRemove = value;}
		bool GetAllowDragAndDrop() const {return m_AllowDragAndDrop;}
		bool GetVisible() const{return m_Visible;}
		void SetVisible(bool value);
		bool GetLock() const {return m_Lock;}
		void SetLock(bool value);
	private:
		ADD_ATTRIBUTE(Vec4,SelectedColor)
		
		void OnObjectVisible(ObjectVisibilityChangedMessagePtr message);
		void OnObjectLock(ObjectLockChangedMessagePtr message);
		float GetVisibilityTransparency() const{return m_VisibilityTransparency;}
		void SetVisibilityTransparency(float value);
		void OnSceneObjectSelected(ObjectSelectionChangedMessagePtr message);
		bool GetChangeMaterialWhenSelected() const {return m_ChangeMaterialWhenSelected;}
		void SetChangeMaterialWhenSelected(bool value) {m_ChangeMaterialWhenSelected = value;}
		void SetIconFile(const std::string &value) { m_IconFile = value;}
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
		EditorSystemPtr m_EditorSystem;
	};
	typedef boost::shared_ptr<EditorComponent> EditorComponentPtr;
}


