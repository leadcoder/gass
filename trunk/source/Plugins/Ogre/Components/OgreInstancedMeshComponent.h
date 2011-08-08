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
#include "Sim/Components/Graphics/Geometry/IMeshComponent.h"
#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Common.h"
#include "Core/Math/Vector.h"
#include "Core/Math/AABox.h"
#include "Core/Math/Sphere.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"
#include "Plugins/Ogre/OgreGraphicsSceneManager.h"

namespace Ogre
{
    class Entity;
    class Bone;
    class IndexData;
    class VertexData;
	class InstancedGeometry;
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

	class OgreInstancedMeshComponent : public Reflection<OgreInstancedMeshComponent,BaseSceneComponent>, public IMeshComponent , public IGeometryComponent
	{
	public:
		OgreInstancedMeshComponent (void);
		~OgreInstancedMeshComponent (void);
		static void RegisterReflection();
		virtual void OnCreate();
		virtual AABox GetBoundingBox()const;
		virtual Sphere GetBoundingSphere()const;
		virtual GeometryCategory GetGeometryCategory() const;

		virtual std::string GetFilename()const {return m_Filename;}
		virtual void GetMeshData(MeshDataPtr mesh_data);
		virtual void LoadXML(TiXmlElement *elem);
	protected:
		std::string GetRenderQueue()const {return m_RenderQueue;}
		void SetRenderQueue(const std::string &rq) {m_RenderQueue = rq;}
		bool GetCastShadow()const {return m_CastShadow;}
		void SetCastShadow(bool castShadow) {m_CastShadow = castShadow;}
		void SetRegionSize(Float size) {m_RegionSize = size;}
		Float GetRegionSize() const {return m_RegionSize;}

		void SetGlobalScale(const Vec3 &scale) {m_GlobalScale = scale;}
		Vec3 GetGlobalScale() const {return m_GlobalScale;}

		void AddVertexData(const Ogre::VertexData *vertex_data,MeshDataPtr mesh, const Ogre::Vector3 &offset);
		void AddIndexData(const Ogre::IndexData *data, const unsigned int offset,MeshDataPtr mesh);
	
		void OnLoad(LoadGFXComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		void OnMeshFileNameMessage(MeshFileMessagePtr message);
	
		void setupInstancedMaterialToEntity(Ogre::Entity*ent);
		Ogre::String buildInstancedMaterial(const Ogre::String &originalMaterialName);
	
		
		std::string m_RenderQueue;
		bool m_CastShadow;
		bool m_ReadyToLoadMesh;
		bool m_UniqueMaterialCreated;

		typedef std::vector<MeshInstance> MeshInstanceVector;
		typedef std::map<std::string, MeshInstanceVector> MeshMap;
		MeshMap m_MeshInstances;
		std::vector<Ogre::InstancedGeometry*> m_InstancedGeometries;
		OgreGraphicsSceneManagerWeakPtr m_OgreSceneManager;
		std::string m_Filename;
		Float m_RegionSize;
		Vec3 m_GlobalScale;
	};

	typedef boost::shared_ptr<OgreInstancedMeshComponent> OgreInstancedMeshComponentPtr;
}

