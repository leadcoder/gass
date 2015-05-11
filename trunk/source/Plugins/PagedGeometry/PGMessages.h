/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSQuaternion.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"

namespace GASS
{
	class GrassLoaderComponentLoaded : public SceneObjectEventMessage
	{
		public:
		GrassLoaderComponentLoaded (SenderID sender_id = -1, double delay= 0) :  SceneObjectEventMessage(sender_id , delay)
		{
		}
	};
	typedef SPTR<GrassLoaderComponentLoaded> GrassLoaderComponentLoadedPtr;

	
	class GrassPaintMessage : public SceneObjectRequestMessage
	{
	public:
		GrassPaintMessage (const Vec3 &pos, float brush_size, float brush_inner_size, float intensity = 1, float noise = 0, SenderID sender_id = -1, double delay= 0) :  SceneObjectRequestMessage(sender_id , delay), 
			m_Position(pos), 
			m_BrushSize(brush_size),
			m_BrushInnerSize(brush_inner_size),
			m_Intensity(intensity),
			m_Noise(noise)
		{

		}
		float GetBrushSize() const {return m_BrushSize;}
		float GetBrushInnerSize() const {return m_BrushInnerSize;}
		float GetIntensity() const {return m_Intensity;}
		Vec3 GetPosition() const {return m_Position;}
		float GetNoise() const {return m_Noise;}
	private:
		Vec3 m_Position;
		float m_BrushSize;
		float m_BrushInnerSize;
		float m_Intensity;
		float m_Noise;
	};
	typedef SPTR<GrassPaintMessage> GrassPaintMessagePtr;
}
