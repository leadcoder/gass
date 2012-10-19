#pragma once
#include "Sim/GASSBaseSceneComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Sim/GASSMeshData.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"

namespace GASS
{
	class EditorModuleExport PaintGizmoComponent : public Reflection<PaintGizmoComponent,BaseSceneComponent>
	{
	public:
		PaintGizmoComponent();
		virtual ~PaintGizmoComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
		void SetSize(float value){m_Size =value;}
		void SetInnerSize(float value){m_InnerSize =value;}
		void SetActive(bool active)  {m_Active =active;}
		bool GetActive() const {return m_Active;}
		void BuildMesh();
	private:
		std::string GetType() const {return m_Type;}
		void SetType(const std::string &value) {m_Type = value;}
		float GetSize() const{return m_Size;}
		
		Vec4 GetColor() const{return m_Color;}
		void SetColor(const Vec4 &value){m_Color =value;}
		void OnNewCursorInfo(CursorMoved3DMessagePtr message);
		void OnTransformation(TransformationNotifyMessagePtr message);

		ManualMeshDataPtr m_MeshData;	
		Vec4 m_Color;
		float m_Size;
		float m_InnerSize;
		std::string m_Type;
		//helpers
		std::string m_Mode;
		bool m_Active;
	};

	typedef boost::shared_ptr<PaintGizmoComponent> PaintGizmoComponentPtr;
}


