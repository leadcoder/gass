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
#include "Sim/GASSCommon.h"
#include "Plugins/Ogre/GASSOgreCommon.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSICollisionComponent.h"
#include "Sim/Interface/GASSIManualMeshComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/GASSGeometryFlags.h"

namespace Ogre
{
	class ManualObject;
}

namespace GASS
{
	class OgreManualMeshComponent : public Reflection<OgreManualMeshComponent , BaseSceneComponent> , public IManualMeshComponent, public IGeometryComponent
	{
	public:
		OgreManualMeshComponent(void);
		~OgreManualMeshComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();

		//IGeometryComponent
		virtual AABox GetBoundingBox() const;
		virtual Sphere GetBoundingSphere() const;
		virtual GeometryFlags GetGeometryFlags() const;
		virtual void SetGeometryFlags(GeometryFlags flags);
		virtual bool GetCollision() const;
		virtual void SetCollision(bool value);
		
		//IManaulMeshComponent
		virtual GraphicsMesh GetMeshData() const;
		virtual void SetMeshData(const GraphicsMesh& mesh);

		Ogre::ManualObject* GetManualObject() const {return m_MeshObject;}
	protected:
		ADD_PROPERTY(GeometryFlagsBinder,GeometryFlagsBinder);
		void SetCastShadow(bool castShadow);
		bool GetCastShadow() const {return m_CastShadows;}

		void OnLocationLoaded(LocationLoadedEventPtr message);
		void OnDataMessage(ManualMeshDataRequestPtr message);
		void OnClearMessage(ClearManualMeshRequestPtr message);
		void OnReplaceMaterial(ReplaceMaterialRequestPtr message);
		void OnResetMaterial(ResetMaterialRequestPtr message);
		void OnVisibilityMessage(GeometryVisibilityRequestPtr message);
		void Clear();

		Ogre::ManualObject* m_MeshObject;
		std::vector<GraphicsMesh> m_MeshData;
		bool m_CastShadows;
		CollisionComponentPtr m_Collision;
	};
}