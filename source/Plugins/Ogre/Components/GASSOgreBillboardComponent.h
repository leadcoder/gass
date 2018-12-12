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
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Core/Math/GASSAABox.h"
#include "Core/Math/GASSSphere.h"
#include "Plugins/Ogre/GASSOgreMaterial.h"

namespace Ogre
{
    class BillboardSet;
    class Billboard;
}

namespace GASS
{
	class OgreBillboardComponent : public Reflection<OgreBillboardComponent,BaseSceneComponent> , public IGeometryComponent
	{
	public:
		OgreBillboardComponent (void);
		~OgreBillboardComponent (void) override;
		static void RegisterReflection();
		void OnInitialize() override;
		AABox GetBoundingBox()const override;
		Sphere GetBoundingSphere()const override;
		GeometryFlags GetGeometryFlags() const override;
		void SetGeometryFlags(GeometryFlags flags) override;
		bool GetCollision() const override;
		void SetCollision(bool value) override;

		Ogre::BillboardSet* GetBillboardSet() const {return m_BillboardSet;}
	protected:
		void SetGeometryFlagsBinder(GeometryFlagsBinder value);
		GeometryFlagsBinder GetGeometryFlagsBinder() const;
		void OnGeometryScale(GeometryScaleRequestPtr message);
		void OnVisibilityMessage(GeometryVisibilityRequestPtr message);

		float GetWidth() const;
		void SetWidth(float width);
		float GetHeight() const;
		void SetHeight(float height);
		std::string GetRenderQueue()const {return m_RenderQueue;}
		void SetRenderQueue(const std::string &rq) {m_RenderQueue = rq;}
		void SetMaterial(const OgreMaterial &mat) {m_Material = mat;}
		OgreMaterial GetMaterial()const {return m_Material;}
		bool GetCastShadow()const {return m_CastShadow;}
		void SetCastShadow(bool castShadow) {m_CastShadow = castShadow;}
		void OnLocationLoaded(LocationLoadedEventPtr message);
		void OnSetColorMessage(BillboardColorRequestPtr message);
		
		std::string m_RenderQueue;
		OgreMaterial m_Material;
		bool m_CastShadow;
		Ogre::BillboardSet* m_BillboardSet;
		Ogre::Billboard* m_Billboard;
		float m_Width;
		float m_Height;
		GeometryFlags m_GeomFlags;
		CollisionComponentPtr m_Collision;
	};
	typedef GASS_SHARED_PTR<OgreBillboardComponent> OgreBillboardComponentPtr;
}

