/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                 *
*                                                                           *
* Copyright (c) 2008-2009 GASS team. See Contributors.txt for details.      *
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
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Core/MessageSystem/GASSIMessage.h"

namespace GASS
{

	/**
		Component that impl. IHeightmapTerrainComponent interface used for terrain interaction
	*/

	class Heightmap;
	class HeightmapComponent : public Reflection<HeightmapComponent,BaseSceneComponent> , public IHeightmapTerrainComponent, public IGeometryComponent
	{
	public:
		HeightmapComponent(void);
		~HeightmapComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void SaveXML(TiXmlElement *obj_elem);

		//IHeightmapTerrainComponent
		virtual Float GetHeightAtPoint(int x, int z) const;
		virtual Float GetHeightAtWorldLocation(Float x, Float z) const;
		virtual unsigned int GetSamples() const;
		virtual float* GetHeightData() const;
		virtual AABox GetBoundingBox() const;
		virtual Sphere GetBoundingSphere() const;
		virtual GeometryFlags GetGeometryFlags() const;
		virtual void SetGeometryFlags(GeometryFlags flags){}
	protected:
		void SetUpdate(bool value);
		bool  GetUpdate() const;
		ADD_PROPERTY(Vec2,Size)
		ADD_PROPERTY(Float,Resolution)

		//internal
		void _UpdateData();
		FilePath _GetFilePath() const;
	private:
		Heightmap* m_HM;
	};
}
#endif
