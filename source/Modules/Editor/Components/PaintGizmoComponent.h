#pragma once

#include "Sim/GASSCommon.h"
#include "Sim/GASSComponent.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"

namespace GASS
{
	GASS_IFORWARD_DECL(HeightmapTerrainComponent)

	class EditorModuleExport PaintGizmoComponent : public Reflection<PaintGizmoComponent,Component>
	{
	public:
		PaintGizmoComponent();
		~PaintGizmoComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
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
		float m_Size{30};
		float m_InnerSize{20};
		std::string m_Type;
		//helpers
		std::string m_Mode;
		bool m_Active{false};
		Vec3 m_Pos;
		HeightmapTerrainComponentPtr m_HMTerrain;
	};

	using PaintGizmoComponentPtr = std::shared_ptr<PaintGizmoComponent>;
}


