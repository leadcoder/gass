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
#endif

#include <osg/Camera>
#include <osg/ref_ptr>
#include <osg/Node>
#include <osg/Quat>
#include <osg/Geode>
#include <osg/Billboard>
#include <osg/Projection>
#include <osg/Referenced>
#include <osg/LineWidth>
#include <osg/Point>
#include <osg/TriangleFunctor>
#include <osg/NodeVisitor>
#include <osg/Fog>
#include <osg/PositionAttitudeTransform>
#include <osg/Projection>
#include <osg/MatrixTransform>
#include <osg/Transform>
#include <osg/Light>
#include <osg/LightSource>
#include <osg/Geometry>
#include <osg/CullSettings>
#include <osg/Depth>
#include <osg/StateAttribute>
#include <osg/Material>
#include <osg/BlendFunc>
#include <osg/CullFace>
#include <osg/Billboard>
#include <osg/Texture2D>
#include <osg/AlphaFunc>
#include <osg/LineWidth>
#include <osg/TextureCubeMap>
#include <osg/TexEnv>
#include <osg/TexMat>
#include <osg/TexGen>
#include <osg/TexEnvCombine>
#include <osg/ShapeDrawable>


#include <osgViewer/View>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgGA/TerrainManipulator>
#include <osgGA/CameraManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/GUIEventHandler>
#include <osgGA/TrackballManipulator>
#include <osgGA/NodeTrackerManipulator>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgShadow/ShadowTechnique>
#include <osgShadow/ShadowedScene>
#include <osgShadow/ShadowVolume>
#include <osgShadow/ShadowTexture>
#include <osgShadow/ShadowMap>
#include <osgShadow/SoftShadowMap>
#include <osgShadow/ParallelSplitShadowMap>
#include <osgShadow/LightSpacePerspectiveShadowMap>
#include <osgShadow/StandardShadowMap>
#include <osgShadow/ViewDependentShadowMap>

#include <osgText/Text>

#include <osgUtil/LineSegmentIntersector>
#include <osgUtil/IntersectionVisitor>
#include <osgUtil/IntersectionVisitor>
#include <osgUtil/Optimizer>
#include <osgUtil/CullVisitor>

#include <osgSim/LineOfSight>
#include <osgSim/HeightAboveTerrain>
#include <osgSim/ElevationSlice>
#include <osgSim/MultiSwitch>


#if defined(WIN32) && !defined(__CYGWIN__)
#include <osgViewer/api/Win32/GraphicsWindowWin32>
typedef HWND WindowHandle;
typedef osgViewer::GraphicsWindowWin32::WindowData WindowData;

#elif defined(__APPLE__) // Assume using Carbon on Mac.
#include <osgViewer/api/Carbon/GraphicsWindowCarbon>
typedef WindowRef WindowHandle;
typedef osgViewer::GraphicsWindowCarbon::WindowData WindowData;

#else // all other unix
#include <osgViewer/api/X11/GraphicsWindowX11>
typedef Window WindowHandle;
typedef osgViewer::GraphicsWindowX11::WindowData WindowData;
#endif

#ifdef _MSC_VER
#pragma warning (pop)
#endif


