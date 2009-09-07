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

#include "Core/MessageSystem/MessageType.h"
#include "Sim/Systems/Graphics/IGraphicsSystem.h"
#include "Sim/Systems/SimSystem.h"
#include "Sim/Scheduling/ITaskListener.h"
#include <string>
#include <osgViewer/Viewer>

namespace GASS
{
	class OSGGraphicsSystem : public Reflection<OSGGraphicsSystem,SimSystem> , public IGraphicsSystem, public ITaskListener
	{
		friend class OSGGraphicsSceneManager;
	public:
		OSGGraphicsSystem();
		virtual ~OSGGraphicsSystem();
		static void RegisterReflection();
		virtual void OnCreate();
		SystemType GetSystemType() {return "GraphicsSystem";}
		void GetMainWindowInfo(unsigned int &width, unsigned int &height, int &left, int &top);
		osgViewer::CompositeViewer*  GetViewer() {return m_Viewer ;}
		osg::Group* GetActiveData() {return m_Root;}

		//ITaskListener interface
		void Update(double delta);
		TaskGroup GetTaskGroup() const;


	protected:
		void OnCreateRenderWindow(MessagePtr message);
		bool GetCreateMainWindowOnInit() const {return m_CreateMainWindowOnInit;}
		void SetCreateMainWindowOnInit(bool value){m_CreateMainWindowOnInit = value;}

		void CreateView(osgViewer::CompositeViewer *viewer,
                 //osg::ref_ptr<osg::Group> scene,
                 osg::ref_ptr<osg::GraphicsContext> gc,
                 int x, int y, int width, int height);
		void SetActiveData(osg::Group* root);
		void OnInit(MessagePtr message);		
	private:
		osgViewer::CompositeViewer* m_Viewer;
		osg::ref_ptr<osg::GraphicsContext> m_GraphicsContext;
		osg::Group* m_Root;
		bool m_CreateMainWindowOnInit;
	};
	typedef boost::shared_ptr<OSGGraphicsSystem>  OSGGraphicsSystemPtr;
	typedef boost::weak_ptr<OSGGraphicsSystem>  OSGGraphicsSystemWeakPtr;
}
