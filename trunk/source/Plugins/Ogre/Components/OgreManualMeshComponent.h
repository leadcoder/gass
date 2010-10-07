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


#include "Sim/Common.h"
#include "Sim/Components/Graphics/Geometry/ILineComponent.h"
#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Components/Graphics/MeshData.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"

namespace Ogre
{
	class ManualObject;
}

namespace GASS
{
	class DynamicLines;
	class OgreManualMeshComponent : public Reflection<OgreManualMeshComponent ,BaseSceneComponent> , public IGeometryComponent, public boost::enable_shared_from_this<OgreManualMeshComponent>
	{
	public:
		OgreManualMeshComponent(void);
		~OgreManualMeshComponent(void);
		static void RegisterReflection();
		virtual void OnCreate();
		virtual AABox GetBoundingBox() const;
		virtual Sphere GetBoundingSphere() const;
	
	protected:
		void OnLoad(LoadGFXComponentsMessagePtr message);
		void OnDataMessage(ManualMeshDataMessagePtr message);
		void OnClearMessage(ClearManualMeshMessagePtr message);
		void OnMaterialMessage(MaterialMessagePtr message);

		void CreateMesh(ManualMeshDataPtr data);
		void Clear();
		Ogre::ManualObject* m_MeshObject;
		std::vector<MeshData> m_MeshData;
		bool m_UniqueMaterialCreated;
	};
}

