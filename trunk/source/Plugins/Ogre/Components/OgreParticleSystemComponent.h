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
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSCommon.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSAABox.h"
#include "Core/Math/GASSSphere.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"


class Ogre::ParticleSystem;

namespace GASS
{
	class OgreParticleSystemComponent : public Reflection<OgreParticleSystemComponent,BaseSceneComponent> , public IGeometryComponent
	{
	public:
		OgreParticleSystemComponent (void);
		~OgreParticleSystemComponent (void);
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual AABox GetBoundingBox()const;
		virtual Sphere GetBoundingSphere()const;
		virtual GeometryFlags GetGeometryFlags() const;
		virtual void SetGeometryFlags(GeometryFlags flags);
	//	virtual void GetMeshData(MeshDataPtr mesh_data);
	protected:
		void SetParticleTemplate(const std::string &part_temp) {m_ParticleTemplate = part_temp;}
		std::string GetParticleTemplate()const {return m_ParticleTemplate;}
		float GetTimeToLive()const {return m_TimeToLive;}
		void SetTimeToLive(float value) {m_TimeToLive = value;}
		bool GetCastShadow()const {return m_CastShadow;}
		void SetCastShadow(bool castShadow) {m_CastShadow = castShadow;}
		void OnLocationLoaded(LocationLoadedMessagePtr message);
		void OnParameterMessage(ParticleSystemParameterMessagePtr message);

		std::string m_RenderQueue;
		std::string m_ParticleTemplate;
		bool m_CastShadow;
		float m_TimeToLive;
		Ogre::ParticleSystem* m_ParticleSystem;
		GeometryFlags m_GeomFlags;
	};

	typedef boost::shared_ptr<OgreParticleSystemComponent> OgreParticleSystemComponentPtr;
}

