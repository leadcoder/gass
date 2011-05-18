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

namespace Ogre
{
    class Entity;
    class Bone;
    class IndexData;
    class VertexData;
}

namespace GASS
{



	class GASSPluginExport OgreMeshComponent : public Reflection<OgreMeshComponent,BaseSceneComponent>, public IMeshComponent , public IGeometryComponent, public boost::enable_shared_from_this<OgreMeshComponent>
	{
	public:
		OgreMeshComponent (void);
		~OgreMeshComponent (void);
		static void RegisterReflection();
		virtual void OnCreate();
		virtual AABox GetBoundingBox()const;
		virtual Sphere GetBoundingSphere()const;
		virtual GeometryCategory GetGeometryCategory() const;
		virtual std::string GetFilename()const {return m_Filename;}
		virtual void GetMeshData(MeshDataPtr mesh_data);
		Ogre::Entity*  GetOgreEntity(){return m_OgreEntity;}
	protected:
		std::string GetRenderQueue()const {return m_RenderQueue;}
		void SetRenderQueue(const std::string &rq);
		void SetFilename(const std::string &filename);
		bool GetCastShadow()const {return m_CastShadow;}
		void SetCastShadow(bool castShadow);

		void AddVertexData(const Ogre::VertexData *vertex_data,MeshDataPtr mesh);
		void AddIndexData(Ogre::IndexData *data, const unsigned int offset,MeshDataPtr mesh);
		void OnLoad(LoadGFXComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		void OnMeshFileNameMessage(MeshFileMessagePtr message);
		void OnTexCoordMessage(TextureCoordinateMessagePtr message);
		void OnMaterialMessage(MaterialMessagePtr message);
		void OnBoneTransformationMessage(BoneTransformationMessagePtr message);

		void SetTexCoordSpeed(const Vec2 &speed);


		Ogre::Bone* GetClosestBone(const Vec3 &pos);
		bool HasSkeleton() const;

		Ogre::Entity* m_OgreEntity;
		std::string m_RenderQueue;
		std::string m_Filename;
		bool m_CastShadow;
		bool m_ReadyToLoadMesh;
		bool m_UniqueMaterialCreated;
	};

	typedef boost::shared_ptr<OgreMeshComponent> OgreMeshComponentPtr;
}

