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
#pragma once
#include "Sim/GASS.h"
#include "Plugins/OSG/OSGCommon.h"

namespace osg
{
	class Billboard;
	class Image;
	class Drawable;
	class Geometry;
	class TextureCubeMap;
	class Node;
}

namespace GASS
{
	class OSGSkyboxComponent : public Reflection<OSGSkyboxComponent,BaseSceneComponent>
	{
	public:
		OSGSkyboxComponent (void);
		~OSGSkyboxComponent (void) override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
	protected:
		void SetMaterial(const std::string &mat) {m_Material = mat;}
		std::string GetMaterial()const {return m_Material;}
		std::string GetTexturePath(const std::string &side) const;
		osg::TextureCubeMap* ReadCubeMap();
		osg::Node* CreateSkyBox();

		std::string m_Material;
		osg::Node* m_Node{nullptr};
		Float m_Size{200};
	};
}