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
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/GASSCommon.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSAABox.h"
#include "Core/Math/GASSSphere.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Plugins/Ogre/OgreMaterial.h"

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
		~OgreBillboardComponent (void);
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual AABox GetBoundingBox()const;
		virtual Sphere GetBoundingSphere()const;
		virtual GeometryFlags GetGeometryFlags() const;
		virtual void SetGeometryFlags(GeometryFlags flags);
		Ogre::BillboardSet* GetBillboardSet() const {return m_BillboardSet;}
	protected:
		void OnGeometryScale(GeometryScaleMessagePtr message);
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
		void OnLocationLoaded(LocationLoadedMessagePtr message);
		void OnMaterialMessage(MaterialMessagePtr message);
		std::string m_RenderQueue;
		OgreMaterial m_Material;
		bool m_CastShadow;

		Ogre::BillboardSet* m_BillboardSet;
		Ogre::Billboard* m_Billboard;
		float m_Width;
		float m_Height;
		GeometryFlags m_GeomFlags;
	};

	typedef boost::shared_ptr<OgreBillboardComponent> OgreBillboardComponentPtr;

}

