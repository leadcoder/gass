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
//#   pragma warning (disable : 4541)
#include "Sim/GASS.h"
#include "Plugins/OSG/OSGCommon.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/IOSGMesh.h"

namespace osg
{
	class Geode;
}
namespace GASS
{
	class OSGMeshEnumerationMetaData : public EnumerationPropertyMetaData
	{
	public:
		OSGMeshEnumerationMetaData(const std::string &annotation, PropertyFlags flags) : EnumerationPropertyMetaData(annotation, flags, false){}
		std::vector<std::string> GetEnumeration(BaseReflectionObjectPtr object) const override;
	};
	typedef GASS_SHARED_PTR<OSGMeshEnumerationMetaData> OSGMeshEnumerationMetaDataPtr;

	class OSGMeshComponent : public Reflection<OSGMeshComponent,BaseSceneComponent> , public IMeshComponent , public IGeometryComponent, public IResourceComponent, public IOSGMesh
	{
	public:
		OSGMeshComponent (void);
		~OSGMeshComponent (void) override;
		static void RegisterReflection();
		void OnInitialize() override;

		//IGeometryComponent
		AABox GetBoundingBox() const override;
		Sphere GetBoundingSphere() const override;
		GeometryFlags GetGeometryFlags() const override;
		void SetGeometryFlags(GeometryFlags flags) override;
		bool GetCollision() const override;
		void SetCollision(bool value) override;

		//IResourceComponent
		ResourceHandle GetResource() const override {return m_MeshResource;}

		//IMeshComponent
		GraphicsMesh GetMeshData() const override;

		//set external mesh
		void SetMeshNode(osg::ref_ptr<osg::Node> mesh);
		osg::ref_ptr<osg::Node> GetNode() const override {return m_MeshNode ;}
		std::vector<std::string> GetAvailableMeshFiles() const;
	protected:
		ADD_PROPERTY(std::string,EnumerationResourceGroup)
		ADD_PROPERTY(bool,FlipDDS)
		bool GetLighting() const;
		void SetLighting(bool value);
		ResourceHandle GetMeshResource() const {return m_MeshResource;}
		void SetMeshResource(const ResourceHandle &res);
		bool GetExpand() const;
		void SetExpand(bool value);
		bool GetCastShadow()const {return m_CastShadow;}
		void SetCastShadow(bool value);
		bool GetReceiveShadow()const {return m_ReceiveShadow;}
		void SetReceiveShadow(bool value);
		void Expand(SceneObjectPtr parent, osg::Node *node, bool load);
		void ExpandRec(SceneObjectPtr parent, osg::Node* node, bool load);
		void OnLocationLoaded(LocationLoadedEventPtr message);
		void OnMaterialMessage(ReplaceMaterialRequestPtr message);
		void OnCollisionSettings(CollisionSettingsRequestPtr message);
		void OnVisibilityMessage(GeometryVisibilityRequestPtr message);
		void OnMeshFileNameMessage(MeshFileRequestPtr message);
		void CalulateBoundingbox(osg::Node *node, const osg::Matrix& M = osg::Matrix::identity());
		void LoadMesh(const ResourceHandle &filename);
		void LoadMesh(const std::string &file_name);
		void SetImportMesh(const FilePath &file_name);
		FilePath  GetImportMesh() const;
		void SetGeometryFlagsBinder(GeometryFlagsBinder value);
		GeometryFlagsBinder GetGeometryFlagsBinder() const;

		ResourceHandle m_MeshResource;
		bool m_CastShadow;
		bool m_ReceiveShadow;
		osg::ref_ptr<osg::Node> m_MeshNode;
		AABox m_BBox;
		bool m_Initlized;
		GeometryFlags m_GeomFlags;
		bool m_Lighting;
		bool m_Expand;
		bool m_Collision;
	};

	typedef GASS_SHARED_PTR<OSGMeshComponent> OSGMeshComponentPtr;
}

