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
#   pragma warning (disable : 4541)
#include "Sim/GASS.h"
#include "Plugins/OSG/OSGConvert.h"

namespace osg
{
	class Geode;
}
namespace GASS
{
	class OSGMeshEnumerationMetaData : public EnumerationPropertyMetaData
	{
	public:
		OSGMeshEnumerationMetaData(const std::string &annotation, PropertyFlags flags): EnumerationPropertyMetaData(annotation,flags,false){}
		virtual std::vector<std::string> GetEnumeration(BaseReflectionObjectPtr object) const;
	private:
	};
	typedef SPTR<OSGMeshEnumerationMetaData> OSGMeshEnumerationMetaDataPtr;

	class OSGMeshComponent : public Reflection<OSGMeshComponent,BaseSceneComponent> , public IMeshComponent , public IGeometryComponent, public IResourceComponent 
	{
	public:
		OSGMeshComponent (void);
		~OSGMeshComponent (void);
		static void RegisterReflection();
		virtual void OnInitialize();

		//IGeometryComponent
		virtual AABox GetBoundingBox() const;
		virtual Sphere GetBoundingSphere() const;
		virtual GeometryFlags GetGeometryFlags() const;
		virtual void SetGeometryFlags(GeometryFlags flags);

		//IResourceComponent
		ResourceHandle GetResource() const {return m_MeshResource;}

		//IMeshComponent
		virtual GraphicsMesh GetMeshData() const;

		//set external mesh
		void SetMeshNode(osg::ref_ptr<osg::Node> mesh);
		osg::ref_ptr<osg::Node> GetMeshNode() const {return m_MeshNode ;}

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
		void OnCollisionSettings(CollisionSettingsMessagePtr message);
		void OnVisibilityMessage(VisibilityRequestPtr message);
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
	};

	typedef SPTR<OSGMeshComponent> OSGMeshComponentPtr;
}

