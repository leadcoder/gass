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
#include "Sim/Interface/GASSIMeshComponent.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSIResourceComponent.h"

#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSResource.h"
#include "Sim/GASSCommon.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSAABox.h"
#include "Core/Math/GASSSphere.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"

namespace Ogre
{
    class Entity;
    class Bone;
    class IndexData;
    class VertexData;
}

namespace GASS
{



	class GASSPluginExport OgreMeshComponent : public Reflection<OgreMeshComponent,BaseSceneComponent>, public IMeshComponent , public IGeometryComponent, public IResourceComponent 
	{
	public:
		OgreMeshComponent (void);
		~OgreMeshComponent (void);
		static void RegisterReflection();
		virtual void OnInitialize();
		//IGeometryComponent
		virtual AABox GetBoundingBox()const;
		virtual Sphere GetBoundingSphere()const;
		virtual GeometryFlags GetGeometryFlags() const;
		virtual void SetGeometryFlags(GeometryFlags flags);

		//IResourceComponent
		Resource GetResource() const {return m_MeshResource;}

		//IMeshComponent
		virtual void GetMeshData(MeshDataPtr mesh_data) const;

		Ogre::Entity*  GetOgreEntity(){return m_OgreEntity;}
	protected:
		std::string GetRenderQueue()const {return m_RenderQueue;}
		void SetRenderQueue(const std::string &rq);
		Resource GetMeshResource() const {return m_MeshResource;}
		void SetMeshResource(const Resource &res);
		bool GetCastShadow()const {return m_CastShadow;}
		void SetCastShadow(bool castShadow);

		void AddVertexData(const Ogre::VertexData *vertex_data,MeshDataPtr mesh) const;
		void AddIndexData(Ogre::IndexData *data, const unsigned int offset,MeshDataPtr mesh) const;
		void OnLocationLoaded(LocationLoadedMessagePtr message);
		void OnDelete();
		void OnMeshFileNameMessage(MeshFileMessagePtr message);
		void OnTexCoordMessage(TextureCoordinateMessagePtr message);
		void OnMaterialMessage(MaterialMessagePtr message);
		void OnBoneTransformationMessage(BoneTransformationMessagePtr message);
		void SetTexCoordSpeed(const Vec2 &speed);

		Ogre::Bone* GetClosestBone(const Vec3 &pos);
		bool HasSkeleton() const;

		Ogre::Entity* m_OgreEntity;
		std::string m_RenderQueue;
		Resource m_MeshResource;
		bool m_CastShadow;
		bool m_ReadyToLoadMesh;
		bool m_UniqueMaterialCreated;
		GeometryFlags m_GeomFlags;
	};

	typedef boost::shared_ptr<OgreMeshComponent> OgreMeshComponentPtr;
}

