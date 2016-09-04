#pragma once

#include "Sim/GASSCommon.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"

namespace GASS
{
	GASS_IFORWARD_DECL(HeightmapTerrainComponent)

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
		ColorRGBA GetColor() const{return m_Color;}
		void SetColor(const ColorRGBA &value){m_Color =value;}
		void OnTransformation(TransformationChangedEventPtr message);
		GraphicsMeshPtr m_MeshData;	
		ColorRGBA m_Color;
		float m_Size;
		float m_InnerSize;
		std::string m_Type;
		//helpers
		std::string m_Mode;
		bool m_Active;
		Vec3 m_Pos;
		HeightmapTerrainComponentPtr m_HMTerrain;
	};

	typedef GASS_SHARED_PTR<PaintGizmoComponent> PaintGizmoComponentPtr;
}


