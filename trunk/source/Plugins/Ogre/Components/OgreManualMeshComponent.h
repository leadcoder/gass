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


#include "Sim/GASSCommon.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSIMeshComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include <OgreManualObject.h>

namespace Ogre
{
	class ManualObject;
}

namespace GASS
{
	
	class OgreManualMeshComponent : public Reflection<OgreManualMeshComponent , BaseSceneComponent> , public IMeshComponent, public IGeometryComponent
	{
	public:
		OgreManualMeshComponent(void);
		~OgreManualMeshComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual AABox GetBoundingBox() const;
		virtual Sphere GetBoundingSphere() const;
		virtual GeometryFlags GetGeometryFlags() const;
		virtual void SetGeometryFlags(GeometryFlags flags);
		Ogre::ManualObject* GetManualObject() const {return m_MeshObject;}

		//IMeshComponent
		virtual GraphicsMesh GetMeshData() const;
	protected:
		void SetCastShadow(bool castShadow);
		bool GetCastShadow() const {return m_CastShadows;}

		void OnLocationLoaded(LocationLoadedMessagePtr message);
		void OnDataMessage(ManualMeshDataMessagePtr message);
		void OnClearMessage(ClearManualMeshMessagePtr message);
		void OnReplaceMaterial(ReplaceMaterialMessagePtr message);
		void OnTextureMessage(TextureMessagePtr message);
		void OnResetMaterial(ResetMaterialMessagePtr message);
		void OnVisibilityMessage(GeometryVisibilityMessagePtr message);

		void CreateMesh(ManualMeshDataPtr data);
		void CreateMesh(GraphicsMeshPtr data);
		void Clear();
		
		Ogre::ManualObject* m_MeshObject;
		std::vector<GraphicsMesh> m_MeshData;
		bool m_CastShadows;
		GeometryFlags m_GeomFlags;
	};
}

