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
#include "Core/MessageSystem/GASSIMessage.h"

namespace GASS
{
	class SimSceneManager;
	class ISceneManager;
	class IGeometryComponent;
	typedef GASS_SHARED_PTR<ISceneManager> SceneManagerPtr;
	typedef GASS_SHARED_PTR<IGeometryComponent> GeometryComponentPtr;

	class SoundParameterRequest : public SceneObjectRequestMessage
	{
	public:
		enum SoundParameterType
		{
			PLAY,
			STOP,
			PAUSE,
			PITCH,
			LOOP,
			VOLUME,
		};
	public:
		SoundParameterRequest(SoundParameterType parameter, float value, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_Value(value), m_Parameter(parameter)
		  {

		  }
		  float GetValue()const {return m_Value;}
		  SoundParameterType GetParameter()const {return m_Parameter;}
	private:
		SoundParameterType m_Parameter;
		float m_Value;
	};
	typedef GASS_SHARED_PTR<SoundParameterRequest> SoundParameterRequestPtr;
}
