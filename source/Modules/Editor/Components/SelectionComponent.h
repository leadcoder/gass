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
		~SelectionComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
		void SetActive(bool active)  {m_Active =active;}
		bool GetActive() const {return m_Active;}
		void SceneManagerTick(double delta_time) override;

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
		bool m_Active{false};
		AABox m_BBox;
	};
	using SelectionComponentPtr = std::shared_ptr<SelectionComponent>;
}