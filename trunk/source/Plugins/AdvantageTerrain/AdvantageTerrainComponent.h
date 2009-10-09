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
#include "Sim/Components/Graphics/Geometry/ITerrainComponent.h"
#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Common.h"
#include "Core/MessageSystem/AnyMessage.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"
#include "Sim/Scenario/Scene/ScenarioSceneMessages.h"
#include <OgreRenderTargetListener.h>

namespace Ogre
{
	class AVTerrainSceneManager;
}

namespace GASS
{
	class AdvantageTerrainComponent : public Reflection<AdvantageTerrainComponent,BaseSceneComponent>, public ITerrainComponent,public IGeometryComponent, Ogre::RenderTargetListener
	{
	public:
		AdvantageTerrainComponent();
		~AdvantageTerrainComponent();
		static void RegisterReflection();
		virtual void OnCreate();

		std::string GetFilename() const {return m_Filename;}
		void SetFilename(const std::string &filename){m_Filename = filename;}

		virtual void Shutdown();
		//virtual void Update();
		//ITerrainComponent
		virtual float GetHeight(float x, float z);
		virtual bool CheckOnTerrain(float x, float z);
		virtual float GetSizeZ();
		virtual float GetSizeX();
		virtual unsigned int GetSamplesX();
		virtual unsigned int GetSamplesZ();
		virtual void GetHeightAndNormal(float x, float z, float &height,Vec3 &normal);
		virtual void GetBounds(Vec3 &min,Vec3 &max);
		virtual void GetMeshData(MeshDataPtr mesh_data);
		void preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt);

		//IGeometryComponent
		virtual AABox GetBoundingBox()const;
		virtual Sphere GetBoundingSphere()const;
	protected:
		void AdvantageTerrainComponent::OnChangeCamera(CameraChangedNotifyMessagePtr message);
		void OnLoad(LoadGFXComponentsMessagePtr message);
		Ogre::AVTerrainSceneManager* mAVTerrainSceneMgr;
		Vec3 m_WorldMin;
		Vec3 m_WorldMax;
		unsigned int m_SamplesX;
		unsigned int m_SamplesZ;
		float m_Offset;
		std::string m_Filename;
	};
}
