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

namespace GASS
{
	enum MapLayerType
	{
		MLT_IMAGE,
		MLT_ELEVATION,
		MLT_MODEL
	};

	class IMapLayer
	{
	public :
		virtual std::string GetName() const = 0;
		virtual void SetName(const std::string& name) = 0;
		virtual bool GetVisible() const = 0;
		virtual void SetVisible(bool value) = 0;
		virtual bool GetEnabled() const = 0;
		virtual void SetEnabled(bool value) = 0;
		virtual MapLayerType GetType() const = 0;
		virtual int GetUID() const = 0;
	};

	typedef std::unique_ptr<IMapLayer> MapLayerPtr;
	typedef std::vector<MapLayerPtr> MapLayers;

	class IMapComponent
	{
	public:
		virtual ~IMapComponent(){}
		virtual std::vector<std::string> GetViewpointNames() const = 0;
		virtual void SetViewpointByName(const std::string &viewpoint_name) = 0;
		virtual const MapLayers& GetMapLayers() const= 0;
		virtual double GetTimeOfDay() const = 0;
		virtual void SetTimeOfDay(double time) = 0;
		virtual float GetMinimumAmbient() const = 0;
		virtual void SetMinimumAmbient(float value) = 0;
		virtual void SetEarthFile(const ResourceHandle &earth_file) = 0;
		virtual ResourceHandle GetEarthFile() const = 0;
	};

	typedef GASS_SHARED_PTR<IMapComponent> MapComponentPtr;
	typedef GASS_WEAK_PTR<IMapComponent> MapComponentWeakPtr;
}
