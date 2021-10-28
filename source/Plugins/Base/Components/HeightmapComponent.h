/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
*                                                                           *
* GASS is free software: you can redistribute it and/or modify              *
* it under the terms of the GNU Lesser General Public License as published  *
* by the Free Software Foundation, either version 3 of the License, or      *
* (at your option) any later version.                                       *
*                                                                           *
* GASS is distributed in the hope that it will be useful,                   *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU Lesser General Public License for more details.                       *
*                                                                           *
* You should have received a copy of the GNU Lesser General Public License  *
* along with GASS. If not, see <http://www.gnu.org/licenses/>.              *
*****************************************************************************/
#ifndef HEIGHTMAP_COMPONENT_H
#define HEIGHTMAP_COMPONENT_H

#include "Sim/GASSCommon.h"
#include "Sim/GASSComponent.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/Interface/GASSIGeometryComponent.h"

namespace GASS
{

	/**
		Component that impl. IHeightmapTerrainComponent interface used for terrain interaction
	*/

	class HeightField;
	class HeightmapComponent : public Reflection<HeightmapComponent,Component> , public IHeightmapTerrainComponent, public IGeometryComponent
	{
	public:
		HeightmapComponent(void);
		~HeightmapComponent(void) override;
		static void RegisterReflection();
		void OnInitialize() override;
		void SaveXML(tinyxml2::XMLElement *obj_elem) override;
	
		//IHeightmapTerrainComponent
		Float GetHeightAtSample(int x, int z) const override;
		Float GetHeightAtWorldLocation(Float x, Float z) const override;
		unsigned int GetNumSamplesW() const override;
		unsigned int GetNumSamplesH() const override;
		AABox GetBoundingBox() const override;
		Sphere GetBoundingSphere() const override;
		GeometryFlags GetGeometryFlags() const override;
		void SetGeometryFlags(GeometryFlags flags) override{(void) flags;}
		bool GetCollision() const override;
		void SetCollision(bool value) override;
		bool GetVisible() const override { return false; }
		void SetVisible(bool /*value*/) override {}
		AABoxd GetExtent() const;
		void SetExtent(const AABoxd& extent);
	protected:
		void SetUpdate(bool value);
		bool  GetUpdate() const;
		bool GetUpdateExtentFromGeometry() const;
		void SetUpdateExtentFromGeometry(bool value);
		
		//internal
		void UpdateData();
		FilePath GetFilePath() const;
		SceneObjectPtr GetOrCreateDebugObject();
		void UpdateDebugObject(const AABoxd& extent);
		void SetDebug(bool value);
		bool GetDebug() const;
	private:
		HeightField* m_HM{nullptr};
		AABoxd m_Extent;
		double m_SampleStep{1.0};
		bool m_AutoBBoxGeneration{false};
		bool m_Debug{false};
	};
}
#endif
