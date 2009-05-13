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
#include "IPagedGeometry.h"
#include "PagedGeometry.h"

class Ogre::PixelBox;
class PagedGeometry;
namespace HiFi
{
	class ITerrainGeometry;
	class HFEPluginExport TreeGeometry : public IPagedGeometry
	{
	public:
		DECLARE_RTTI;
		DECLARE_PROPERTIES(TreeGeometry,CExtraProp);
		GEOM_DUPLICATE(TreeGeometry)
	public:
		TreeGeometry(void);
		~TreeGeometry(void);
		virtual void Init(ISceneNode* node);
		virtual void Shutdown();
		virtual void Update();
		void LoadDensityMap(const std::string &mapFile, int channel);
		float GetDensityAt(float x, float z);
	protected:
		Ogre::PixelBox *m_DensityMap;
		Vec2 m_MaxMinScale;
		bool m_CastShadows;
		float m_MeshDist;
		float m_ImposterDist;
		float m_MeshFadeDist;
		float m_ImposterFadeDist;
		bool m_PrecalcHeight;
		std::string m_ColorMapFilename;
		bool m_CreateShadowMap;
		

	};
}
