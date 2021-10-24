#pragma once

#include "Sim/GASSCommon.h"
#include "Core/Math/GASSAABox.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"

namespace GASS
{
	class EditorModuleExport MultiSelectionComponent : public Reflection<MultiSelectionComponent,BaseSceneComponent>
	{
	public:
		MultiSelectionComponent();
		~MultiSelectionComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
		void SetActive(bool active)  {m_Active =active;}
		bool GetActive() const {return m_Active;}
		void SceneManagerTick(double delta_time) override;
	private:
		void UpdateSelection();
		std::string GetType() const {return m_Type;}
		void SetType(const std::string &value) {m_Type = value;}
		float GetSize() const{return m_Size;}
		void SetSize(float value){m_Size =value;}
		ColorRGBA GetColor() const{return m_Color;}
		void SetColor(const ColorRGBA &value){m_Color =value;}
		void OnSelectionChanged(EditorSelectionChangedEventPtr message);
	
		ColorRGBA m_Color;
		float m_Size;
		std::string m_Type;
		std::vector<GASS::SceneObjectWeakPtr> m_Selection;
		std::string m_Mode;
		bool m_Active{false};
		AABox m_BBox;
	};
	using MultiSelectionComponentPtr = std::shared_ptr<MultiSelectionComponent>;
}


