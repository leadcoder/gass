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
#include "Sim/Components/Graphics/Geometry/IMeshComponent.h"
#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Common.h"
#include "Core/Math/Vector.h"
#include "Core/Math/AABox.h"
#include "Core/Math/Sphere.h"
#include "Core/MessageSystem/Message.h"


class Ogre::ParticleSystem;

namespace GASS
{
	class OgreParticleSystemComponent : public Reflection<OgreParticleSystemComponent,BaseSceneComponent> , public IGeometryComponent
	{
	public:
		OgreParticleSystemComponent (void);
		~OgreParticleSystemComponent (void);
		static void RegisterReflection();
		virtual void OnCreate();
		virtual AABox GetBoundingBox()const;
		virtual Sphere GetBoundingSphere()const;
		virtual void GetMeshData(MeshDataPtr mesh_data);
	protected:
		void SetParticleTemplate(const std::string &part_temp) {m_ParticleTemplate = part_temp;}
		std::string GetParticleTemplate()const {return m_ParticleTemplate;}
		bool GetCastShadow()const {return m_CastShadow;}
		void SetCastShadow(bool castShadow) {m_CastShadow = castShadow;}
		void OnLoad(MessagePtr message);
		void OnParameterMessage(MessagePtr message);

		std::string m_RenderQueue;
		std::string m_ParticleTemplate;
		bool m_CastShadow;
		Ogre::ParticleSystem* m_ParticleSystem;
	};

	typedef boost::shared_ptr<OgreParticleSystemComponent> OgreParticleSystemComponentPtr;
}

