#pragma once
#include "Sim/Components/BaseSceneComponent.h"
#include "Plugins/Base/CoreMessages.h"
namespace GASS
{
	class EditorModuleExport EditorComponent : public Reflection<EditorComponent,BaseSceneComponent>
	{
	public:
		EditorComponent();
		virtual ~EditorComponent();
		static void RegisterReflection();
		virtual void OnCreate();
		bool GetShowBBWhenSelected() const {return m_ShowBBWhenSelected;}
	private:
		bool GetVisible() const{return m_Visible;}
		void SetVisible(bool value) ;
		void OnObjectVisible(ObjectVisibleMessagePtr message);
		void OnObjectLock(ObjectLockMessagePtr message);
		bool GetLock() const {return m_Lock;}
		void SetLock(bool value);
		void OnLoad(LoadCoreComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		float GetVisibilityTransparency() const{return m_VisibilityTransparency;}
		void SetVisibilityTransparency(float value);
		void OnSceneObjectSelected(ObjectSelectedMessagePtr message);
		bool GetChangeMaterialWhenSelected() const {return m_ChangeMaterialWhenSelected;}
		void SetChangeMaterialWhenSelected(bool value) {m_ChangeMaterialWhenSelected = value;}

		
		void SetShowBBWhenSelected(bool value) {m_ShowBBWhenSelected = value;}
		
		bool m_Lock;
		bool m_Visible;
		float m_VisibilityTransparency;
		bool m_Selected;
		Vec4 m_SelectedColor;
		bool m_ChangeMaterialWhenSelected;
		bool m_ShowBBWhenSelected;
	};
	typedef boost::shared_ptr<EditorComponent> EditorComponentPtr;
}


