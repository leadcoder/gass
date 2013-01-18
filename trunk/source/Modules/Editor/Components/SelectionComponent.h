#pragma once
#include "Core/Math/GASSAABox.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Sim/GASSMeshData.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"
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
	private:
		void BuildMesh();
		std::string GetType() const {return m_Type;}
		void SetType(const std::string &value) {m_Type = value;}
		float GetSize() const{return m_Size;}
		void SetSize(float value){m_Size =value;}
		Vec4 GetColor() const{return m_Color;}
		void SetColor(const Vec4 &value){m_Color =value;}
		void OnSceneObjectSelected(ObjectSelectionChangedEventPtr message);
		void OnNewCursorInfo(CursorMovedOverSceneEventPtr message);
		void OnSelectedTransformation(TransformationNotifyMessagePtr message);
		void OnGeometryChanged(GeometryChangedMessagePtr message);
		Vec4 m_Color;
		float m_Size;
		std::string m_Type;

		GASS::SceneObjectWeakPtr m_SelectedObject;
		std::string m_Mode;
		
		bool m_Active;
		AABox m_BBox;
	};

	typedef boost::shared_ptr<SelectionComponent> SelectionComponentPtr;
}


