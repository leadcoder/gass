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
#pragma once

#include "Sim/GASS.h"
#include <osg/Geometry>

namespace GASS
{
	
	class OSGManualMeshComponent : public Reflection<OSGManualMeshComponent , BaseSceneComponent> , public IGeometryComponent
	{
	public:
		OSGManualMeshComponent(void);
		~OSGManualMeshComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual AABox GetBoundingBox() const;
		virtual Sphere GetBoundingSphere() const;
		virtual GeometryFlags GetGeometryFlags() const;
		virtual void SetGeometryFlags(GeometryFlags flags);
	protected:
		void OnLocationLoaded(LocationLoadedMessagePtr message);
		void OnDataMessage(ManualMeshDataMessagePtr message);
		void OnClearMessage(ClearManualMeshMessagePtr message);
		void OnMaterialMessage(MaterialMessagePtr message);
		void OnCollisionSettings(CollisionSettingsMessagePtr message);
		void CreateMesh(ManualMeshDataPtr data);
		void Clear();
		
		osg::ref_ptr<osg::Geometry> m_OSGGeometry;
		osg::ref_ptr<osg::Geode> m_GeoNode;
		std::vector<MeshData> m_MeshData;
		GeometryFlags m_GeomFlags;
		std::string m_CurrentMaterial; //book keep current material file
	};
}

