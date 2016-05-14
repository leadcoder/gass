#pragma once

#include "Sim/GASSCommon.h"
#include "Core/Math/GASSAABox.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"

namespace GASS
{
	class EditorModuleExport MultiSelectionComponent : public Reflection<MultiSelectionComponent,BaseSceneComponent>
	{
	public:
		MultiSelectionComponent();
		virtual ~MultiSelectionComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
		void SetActive(bool active)  {m_Active =active;}
		bool GetActive() const {return m_Active;}
		void SceneManagerTick(double delta_time);
	private:
		void UpdateSelection();
		void BuildMesh();
		std::string GetType() const {return m_Type;}
		void SetType(const std::string &value) {m_Type = value;}
		float GetSize() const{return m_Size;}
		void SetSize(float value){m_Size =value;}
		ColorRGBA GetColor() const{return m_Color;}
		void SetColor(const ColorRGBA &value){m_Color =value;}
		void OnSelectionChanged(EditorSelectionChangedEventPtr message);
		void OnNewCursorInfo(CursorMovedOverSceneEventPtr message);
		void OnTransformation(TransformationChangedEventPtr message);
		//void OnGeometryChanged(GeometryChangedEventPtr message);
		//void OnSelectedTransformation(TransformationChangedEventPtr message);

		ColorRGBA m_Color;
		float m_Size;
		std::string m_Type;
		std::vector<GASS::SceneObjectWeakPtr> m_Selection;
		std::string m_Mode;
		bool m_Active;
		AABox m_BBox;
	};
	typedef GASS_SHARED_PTR<MultiSelectionComponent> MultiSelectionComponentPtr;
}


