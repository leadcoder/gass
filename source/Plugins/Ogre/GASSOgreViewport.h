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

#include "Sim/Interface/GASSIViewport.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "Plugins/Ogre/GASSOgreRenderWindow.h"
#include "Plugins/Ogre/OgrePostProcess.h"
#include <string>

namespace Ogre
{
	class Viewport;
}

namespace GASS
{
	class OgreViewport : public IViewport, public GASS_ENABLE_SHARED_FROM_THIS<OgreViewport>, public IMessageListener
	{
		friend class OgreRenderWindow;
	public:
		OgreViewport(const std::string &name,Ogre::Viewport* vp, OgreRenderWindow* window);
		virtual ~OgreViewport();
		virtual CameraComponentPtr GetCamera() const;
		virtual void SetCamera(CameraComponentPtr camera);
		virtual std::string GetName() const {return m_Name;}
	private:
		void Init();
		void OnChangeCamera(ChangeCameraRequestPtr message);
		Ogre::Viewport* m_OgreViewport;
		std::string m_Name;
		OgreRenderWindow* m_Window;
		CameraComponentWeakPtr m_Camera;
		OgrePostProcesGASS_SHARED_PTR m_PostProcess;
	};
	typedef GASS_SHARED_PTR<OgreViewport> OgreViewportPtr;
}