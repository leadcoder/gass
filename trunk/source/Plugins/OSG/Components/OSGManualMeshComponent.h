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
#include "Sim/Components/Graphics/GeometryCategory.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Components/Graphics/MeshData.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"


#include <osg/Geometry>

namespace GASS
{
	class DynamicLines;
	class OSGManualMeshComponent : public Reflection<OSGManualMeshComponent ,BaseSceneComponent> , public IGeometryComponent
	{
	public:
		OSGManualMeshComponent(void);
		~OSGManualMeshComponent(void);
		static void RegisterReflection();
		virtual void OnCreate();
		virtual AABox GetBoundingBox() const;
		virtual Sphere GetBoundingSphere() const;
	
	protected:
		void OnLoad(LoadGFXComponentsMessagePtr message);
		void OnDataMessage(ManualMeshDataMessagePtr message);
		void OnClearMessage(ClearManualMeshMessagePtr message);
		void OnMaterialMessage(MaterialMessagePtr message);
		void OnCollisionSettings(CollisionSettingsMessagePtr message);
		void SetGeometryCategory(const GeometryCategory &value);
		GeometryCategory GetGeometryCategory() const;
	


		void CreateMesh(ManualMeshDataPtr data);
		void Clear();
		
		osg::ref_ptr<osg::Geometry> m_OSGGeometry;
		osg::ref_ptr<osg::Geode> m_GeoNode;

		std::vector<MeshData> m_MeshData;

		osg::ref_ptr<osg::DrawArrays> m_DrawArrays;
		osg::ref_ptr<osg::DrawElementsUInt> m_DrawElements;
		GeometryCategory m_Category;
		
	};
}

