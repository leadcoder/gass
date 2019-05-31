#pragma once

#include "Sim/GASSCommon.h"
#include "Core/Math/GASSAABox.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"

namespace GASS
{
	class EditorModuleExport SelectionComponent : public Reflection<SelectionComponent,BaseSceneComponent>
	{
	public:
		SelectionComponent();
		virtual ~SelectionComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
		void SetActive(bool active)  {m_Active =active;}
		bool GetActive() const {return m_Active;}
		void SceneManagerTick(double delta_time);

		std::string GetType() const { return m_Type; }
		void SetType(const std::string &value) { m_Type = value; }
		ColorRGBA GetColor() const { return m_Color; }
		void SetColor(const ColorRGBA &value) { m_Color = value; }
	private:
		void UpdateSelection();
		
		void OnSelectionChanged(EditorSelectionChangedEventPtr message);
	
		ColorRGBA m_Color;
		//float m_Size;
		std::string m_Type;
		std::vector<GASS::SceneObjectWeakPtr> m_Selection;
		std::string m_Mode;
		bool m_Active;
		AABox m_BBox;
	};
	typedef GASS_SHARED_PTR<SelectionComponent> SelectionComponentPtr;
}