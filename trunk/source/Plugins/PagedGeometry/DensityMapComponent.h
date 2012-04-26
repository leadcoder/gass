/****************************************************************************
* This file is part of GASS.                                                *
* See http://sourceforge.net/projects/gass/                                 *
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
#ifndef DENSITY_MAP_COMPONENT_H
#define DENSITY_MAP_COMPONENT_H

#include "PagedGeometry.h"
#include "Sim/Components/GASSBaseSceneComponent.h"
#include "Sim/Scene/GASSCoreSceneObjectMessages.h"
#include "Sim/Scene/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Components/Graphics/Geometry/GASSIGeometryComponent.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "PGMessages.h"
#include <OgreRenderTargetListener.h>
namespace Ogre
{
    class PixelBox;
}
namespace Forests
{
	class PagedGeometry;
	class TreeLoader2D;
	class TreeLoader3D;

}
class RandomTable;

using namespace Forests;


namespace GASS
{
	class ITerrainComponent;

	class DensityMapComponent : public Reflection<DensityMapComponent,BaseSceneComponent>  , public Ogre::RenderTargetListener
	{
	public:
		DensityMapComponent(void);
		~DensityMapComponent(void);
		static void RegisterReflection();
		virtual void OnCreate();
		virtual void SaveXML(TiXmlElement *obj_elem);
		float GetDensityAt(float x, float z);

		void SetMapBounds(TBounds bounds) {m_MapBounds = bounds;}
	protected:
		void Paint(const Vec3 &world_pos, float brush_size, float brush_inner_size , float intensity);
		void SetImport(const std::string &dm);
		std::string GetImport() const {return std::string("");}
		//void LoadDensityMap(const std::string &mapFile, int channel);
		void OnLoad(LoadGFXComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		void OnPaint(GrassPaintMessagePtr message);
		void OnRoadMessage(RoadMessagePtr message);

		std::string GetDensityMap() const
		{
			return m_DensityMapFilename;
		}
		void SetDensityMap(const std::string &colormap)
		{
			m_DensityMapFilename = colormap;
		}
		//Ogre::PixelBox *m_DensityMap;
		std::string m_DensityMapFilename;
		TBounds m_MapBounds;
		Ogre::Image m_DensityImage;
	};
	typedef boost::shared_ptr<DensityMapComponent> DensityMapComponentPtr; 
}

#endif
