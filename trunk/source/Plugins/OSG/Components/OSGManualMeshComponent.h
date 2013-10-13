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
#include "Plugins/OSG/OSGGraphicsSystem.h"
#include "Sim/Interface/GASSIMeshComponent.h"
#include <osg/Geometry>

namespace GASS
{
	class OSGManualMeshComponent : public Reflection<OSGManualMeshComponent , BaseSceneComponent> , public IGeometryComponent , public IMeshComponent
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
		virtual GraphicsMesh GetMeshData() const;
	protected:
		void CreateSubMesh(GraphicsSubMeshPtr sm, osg::ref_ptr<osg::Geometry> geom );
		void SetCastShadow(bool value);
		bool GetCastShadow() const {return m_CastShadow;}
		void OnLocationLoaded(LocationLoadedMessagePtr message);
		void OnDataMessage(ManualMeshDataMessagePtr message);
		void OnClearMessage(ClearManualMeshMessagePtr message);
		void OnMaterialMessage(ReplaceMaterialMessagePtr message);
		void OnCollisionSettings(CollisionSettingsMessagePtr message);
		void CreateMesh(GraphicsMeshPtr data);
		void Clear();
		
		std::vector<osg::ref_ptr<osg::Geometry> > m_OSGGeometries;
		osg::ref_ptr<osg::Geode> m_GeoNode;
		std::vector<GraphicsMesh> m_MeshData;
		GeometryFlags m_GeomFlags;
		bool m_CastShadow;
		OSGGraphicsSystemPtr m_GFXSystem;
	};
}

