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


#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4100)
#pragma warning (disable : 4541)
#pragma warning (disable : 4127)
#pragma warning (disable : 4245)
#pragma warning (disable : 4244)
#pragma warning (disable : 4267)
#endif

#include <osgEarth/PhongLightingEffect>
#include <osgEarth/MapNode>
#include <osgEarth/MapModelChange>
#include <osgEarth/ImageLayer>
#include <osgEarth/ModelLayer>
#include <osgEarth/GeoTransform>
#include <osgEarthUtil/Sky>
#include <osgEarthUtil/ExampleResources>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/LatLongFormatter>
#include <osgEarthUtil/MGRSFormatter>
#include <osgEarthUtil/MouseCoordsTool>
#include <osgEarthUtil/AutoClipPlaneHandler>
#include <osgEarthUtil/Controls>
#include <osgEarthUtil/Fog>
#include <osgEarthUtil/Controls>
#include <osgEarthUtil/ViewFitter>

#include <osgEarthAnnotation/AnnotationData>
#include <osgEarthAnnotation/AnnotationRegistry>
#include <osgEarthAnnotation/PlaceNode>

#ifdef _MSC_VER
#pragma warning (pop)
#endif

