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

#include "Core/Common.h"
#include "Plugins/Ogre/OgreCommon.h"
#include "Plugins/Ogre/GASSOgreRenderWindow.h"
#include "Plugins/Ogre/GASSOgreViewport.h"
#include "Plugins/Ogre/OgreGraphicsSystem.h"
#include "Plugins/Ogre/OgrePostProcess.h"
#include "Core/Utils/GASSException.h"

namespace GASS
{
	unsigned int OgreRenderWindow::GetWidth() const {return m_Window->getWidth();}
	unsigned int OgreRenderWindow::GetHeight() const {return m_Window->getHeight();}

	void* OgreRenderWindow::GetHWND() const
	{
		void* window_hnd = 0;
		m_Window->getCustomAttribute("WINDOW", &window_hnd);
		return window_hnd; 
	}

	ViewportPtr OgreRenderWindow::CreateViewport(const std::string &name, float  left, float top, float width, float height)
	{
		int num_viewports = m_Window->getNumViewports();

		//we need a camera before we can create the viewport!
		std::stringstream ss;
		ss << "DefaultViewportCamera" << num_viewports;
		std::string cam_name;
		ss >> cam_name;

		//get graphic system and create assign dummy camera to scene
		Ogre::SceneManager *sm = GetSystem()->GetBootSceneManager();
		Ogre::Camera* cam;
		if(sm->hasCamera(cam_name))
			cam = sm->getCamera(cam_name);
		else
		{
			cam = sm->createCamera(cam_name);
			cam->setPosition(Ogre::Vector3(0,0,0));
			cam->setNearClipDistance(0.02f);
			cam->setFarClipDistance(5000);
		}
		Ogre::Viewport* vp = m_Window->addViewport(cam, num_viewports, left , top, width , height);
		// Create one viewport, entire window
		//vp->setBackgroundColour(colour);
		//Alter the camera aspect ratio to match the viewport
		cam->setAspectRatio( Ogre::Real(vp->getActualWidth())/Ogre::Real(vp->getActualHeight()));
		OgreViewportPtr vp_wrapper(new OgreViewport(name,vp,this));
		vp_wrapper->Init();
		m_Viewports.push_back(vp_wrapper);
		return vp_wrapper;
	}
}