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
#include "Modules/Graphics/OSGCommon.h"
#include "Modules/Graphics/OSGGraphicsSystem.h"
#include "Sim/Interface/GASSIManualMeshComponent.h"

namespace GASS
{
	class OSGManualMeshComponent : public Reflection<OSGManualMeshComponent , Component> , public IGeometryComponent , public IManualMeshComponent
	{
	public:
		OSGManualMeshComponent(void);
		~OSGManualMeshComponent(void) override;
		static void RegisterReflection();
		void OnInitialize() override;

		//IGeometryComponent interface
		AABox GetBoundingBox() const override;
		Sphere GetBoundingSphere() const override;
		GeometryFlags GetGeometryFlags() const override;
		void SetGeometryFlags(GeometryFlags flags) override;
		bool GetCollision() const override;
		void SetCollision(bool value) override;
		void SetVisible(bool value) override;
		bool GetVisible() const override;

		//IManualMeshComponent interface
		GraphicsMesh GetMeshData() const override;
		void SetMeshData(const GraphicsMesh &mesh) override;
		void SetSubMeshMaterial(const std::string &material_name, int sub_mesh_index) override;
		void SetSubMeshMaterial(IGfxMaterialConfig* config, int sub_mesh_index)override;

		void Clear() override;
		bool GetCastShadow() const { return m_CastShadow; }
		bool GetReceiveShadow()const { return m_ReceiveShadow; }
	protected:
		
		osg::ref_ptr<osg::Geometry>  CreateSubMesh(GraphicsSubMeshPtr sm);
		void SetCastShadow(bool value);
		void SetReceiveShadow(bool value);
		void OnLocationLoaded(LocationLoadedEventPtr message);
		void OnCollisionSettings(CollisionSettingsRequestPtr message);
		
		
		std::vector<osg::ref_ptr<osg::Geometry> > m_OSGGeometries;
		osg::ref_ptr<osg::Geode> m_GeoNode;
		std::vector<GraphicsMesh> m_MeshData;
		bool m_CastShadow{false};
		bool m_ReceiveShadow{false};
		OSGGraphicsSystemPtr m_GFXSystem;
		bool m_Collision{true};
		GeometryFlagsBinder m_GeometryFlagsBinder;
	};

	using OSGManualMeshComponentWeakPtr = std::weak_ptr<OSGManualMeshComponent>;
	using OSGManualMeshComponentPtr = std::shared_ptr<OSGManualMeshComponent>;
}