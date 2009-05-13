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
#pragma once 
#include "Main/Geometry/IGeometry.h"
#include "PagedGeometry.h"
#include "Main/SimulationListener.h"
#include <OgreRenderTargetListener.h>
class PagedGeometry;

namespace HiFi
{
	class ITerrainGeometry;

	class HFEPluginExport IPagedGeometry : public IGeometry , Ogre::RenderTargetListener
	{
	public:
		IPagedGeometry(void);
		~IPagedGeometry(void);
		virtual void Init(ISceneNode* node);
		virtual void Shutdown();
		virtual void Update();
		virtual void preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt);
		virtual void SimulationUpdate(float delta);
		virtual void LevelChanged(){};
	protected:
		ITerrainGeometry * FindTerrain();
		PagedGeometry *m_PagedGeometry;
		TBounds m_MapBounds;
		float m_PageSize;
		float m_DensityFactor;
		std::string m_DensityMapFilename;
		Vec4 m_Bounds;
		float m_ImposterAlphaRejectionValue;
	};
}
