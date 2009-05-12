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


#include "PagedGeometryManager.h"
#include "TreeGeometry.h"
#include "GrassGeometry.h"
#include "Main/Root.h"
#include "Main/SceneNodes/FreeCamera.h"
#include "Main/Level.h"



namespace HiFi
{

	PagedGeometryManager::PagedGeometryManager(void)
	{

	}
	PagedGeometryManager::~PagedGeometryManager(void)
	{

	}

	float PagedGeometryManager::GetTerrainHeight(float x, float z)
	{
		return HiFi::Root::Get().GetLevel()->GetTerrainHeight(x,z);
	}
	
	void PagedGeometryManager::Init()
	{
		REGISTER_INTERFACE(IPagedGeometry);
		REGISTER_OBJECT(TreeGeometry);
		REGISTER_OBJECT(GrassGeometry);
	}
}
