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
#include "Sim/Interface/GASSIBillboardComponent.h"
#include "Modules/Graphics/OSGCommon.h"

namespace osg
{
	class Billboard;
	class Image;
	class Drawable;
	class Geometry;
}

namespace GASS
{
	class OSGBillboardComponent : public Reflection<OSGBillboardComponent, Component>, public IBillboardComponent
	{
	public:
		OSGBillboardComponent(void);
		~OSGBillboardComponent(void) override;
		static void RegisterReflection();
		void OnInitialize() override;

		//IGeometryComponent interface
		AABox GetBoundingBox()const override;
		Sphere GetBoundingSphere()const override;
		virtual void GetMeshData(GraphicsMeshPtr mesh_data);
		GeometryFlags GetGeometryFlags() const override;
		void SetGeometryFlags(GeometryFlags flags) override;
		bool GetCollision() const override;
		void SetCollision(bool value) override;
		void SetVisible(bool value) override;
		bool GetVisible() const override;
		void SetColor(const ColorRGBA& color) override;
		void SetSize(float width, float height) override;
		float GetWidth() const override;
		void SetWidth(float width) override;
		float GetHeight() const override;
		void SetHeight(float height) override;
		void SetScale(float width, float height);
	protected:
		void OnCollisionSettings(CollisionSettingsRequestPtr message);
		void OnLocationLoaded(LocationLoadedEventPtr message);

		void SetGeometryFlagsBinder(GeometryFlagsBinder value);
		GeometryFlagsBinder GetGeometryFlagsBinder() const;

		void SetMaterial(const std::string& mat) { m_Material = mat; }
		std::string GetMaterial()const { return m_Material; }
		bool GetCastShadow()const { return m_CastShadow; }
		void SetCastShadow(bool castShadow);


		osg::ref_ptr<osg::Geometry> CreateSquare(const osg::Vec3& corner, const osg::Vec3& width, const osg::Vec3& height, osg::Image* image = nullptr);
		std::string m_Material;
		bool m_CastShadow{ false };
		float m_Width{ 1.0f };
		float m_Height{ 1.0f };

		float m_ScaleWidth{ 1.0f };
		float m_ScaleHeight{ 1.0f };
		osg::ref_ptr<osg::Billboard> m_OSGBillboard;
		osg::Geometry* m_Geom{ nullptr };
		GeometryFlags m_GeomFlags{ GEOMETRY_FLAG_UNKNOWN };
		double m_GroundOffset{ 0.1 };
		bool m_Collision{ true };
	};
}