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
#include "Plugins/Ogre/OgreCommon.h"

#include "Sim/Interface/GASSIMeshComponent.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSICollisionComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSCommon.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSAABox.h"
#include "Core/Math/GASSSphere.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Plugins/Ogre/OgreGraphicsSceneManager.h"

namespace Ogre
{
	class Entity;
	class Bone;
	class IndexData;
	class VertexData;
}

namespace GASS
{
	struct MeshInstance
	{
		std::string m_Filename;
		Vec3 m_Position;
		Quaternion m_Rotation;
		Vec3 m_Scale;
	};

	class OgreStaticMeshComponent : public Reflection<OgreStaticMeshComponent,BaseSceneComponent>, public IMeshComponent , public IGeometryComponent
	{
	public:
		OgreStaticMeshComponent (void);
		~OgreStaticMeshComponent (void);
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();

		//IGeometryComponent
		virtual AABox GetBoundingBox() const;
		virtual Sphere GetBoundingSphere() const;
		virtual GeometryFlags GetGeometryFlags() const;
		virtual void SetGeometryFlags(GeometryFlags flags);
		virtual bool GetCollision() const;
		virtual void SetCollision(bool value);

		//IMeshComponent
		virtual ResourceHandle GetMeshResource()const {return m_MeshResource;}
		virtual GraphicsMesh  GetMeshData() const;
		virtual void LoadXML(tinyxml2::XMLElement *elem);
	protected:
		void OnMeshFileNameMessage(MeshFileRequestPtr message);

		std::string GetRenderQueue()const {return m_RenderQueue;}
		void SetRenderQueue(const std::string &rq) {m_RenderQueue = rq;}
		bool GetCastShadow()const {return m_CastShadow;}
		void SetCastShadow(bool castShadow) {m_CastShadow = castShadow;}
		void SetRegionSize(Float size) {m_RegionSize = size;}
		Float GetRegionSize() const {return m_RegionSize;}
		void AddVertexData(const Ogre::VertexData *vertex_data,GraphicsMeshPtr mesh, const Ogre::Vector3 &offset) const;
		void AddIndexData(const Ogre::IndexData *data, const unsigned int offset,GraphicsMeshPtr mesh) const;

		std::string m_RenderQueue;
		bool m_CastShadow;
		bool m_ReadyToLoadMesh;
		bool m_UniqueMaterialCreated;
		typedef std::vector<MeshInstance> MeshInstanceVector;
		typedef std::map<std::string, MeshInstanceVector> MeshMap;
		MeshMap m_MeshInstances;
		Ogre::StaticGeometry *m_StaticGeometry;
		OgreGraphicsSceneManagerWeakPtr m_OgreSceneManager;
		ResourceHandle m_MeshResource;
		Float m_RegionSize;
		GeometryFlags m_GeomFlags;
		CollisionComponentPtr m_Collision;
	};
	typedef GASS_SHARED_PTR<OgreStaticMeshComponent> OgreStaticMeshComponentPtr;
}