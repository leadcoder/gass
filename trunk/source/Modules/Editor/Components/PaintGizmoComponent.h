#pragma once
#include "Sim/Components/BaseSceneComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Sim/Components/Graphics/MeshData.h"
#include "Sim/Scenario/Scene/ScenarioSceneMessages.h"

namespace GASS
{
	class EditorModuleExport PaintGizmoComponent : public Reflection<PaintGizmoComponent,BaseSceneComponent>
	{
	public:
		PaintGizmoComponent();
		virtual ~PaintGizmoComponent();
		static void RegisterReflection();
		virtual void OnCreate();
		
		void SetActive(bool active)  {m_Active =active;}
		bool GetActive() const {return m_Active;}
	private:
		void BuildMesh();
		std::string GetType() const {return m_Type;}
		void SetType(const std::string &value) {m_Type = value;}
		void OnLoad(LoadCoreComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		float GetSize() const{return m_Size;}
		void SetSize(float value){m_Size =value;}
		Vec4 GetColor() const{return m_Color;}
		void SetColor(const Vec4 &value){m_Color =value;}
		void OnNewCursorInfo(CursorMoved3DMessagePtr message);
		void OnTransformation(TransformationNotifyMessagePtr message);

		ManualMeshDataPtr m_MeshData;	
		Vec4 m_Color;
		float m_Size;
		std::string m_Type;
		//helpers
		std::string m_Mode;
		bool m_Active;
	};

	typedef boost::shared_ptr<PaintGizmoComponent> PaintGizmoComponentPtr;
}


