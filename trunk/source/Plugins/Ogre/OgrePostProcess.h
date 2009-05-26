/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                 *
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

#include "Sim/Systems/Graphics/IGraphicsSystem.h"
#include "Sim/Scenario/Scene/BaseSceneManager.h"
#include "Core/MessageSystem/MessageType.h"
#include <string>
#include <OgreCompositorInstance.h>
#include <OgreGpuProgram.h>
#include <OgreViewport.h>
#include <OgreCamera.h>


namespace GASS
{

	//---------------------------------------------------------------------------

	//CompositorInstance::Listener
	class SSAOListener: public Ogre::CompositorInstance::Listener
    {
    public:
        SSAOListener(Ogre::Viewport* vp);
        virtual ~SSAOListener();
        //virtual void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
        virtual void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
    protected:
		Ogre::Viewport* m_Viewport;
    };

    class HeatVisionListener: public Ogre::CompositorInstance::Listener
    {
    public:
        HeatVisionListener();
        virtual ~HeatVisionListener();
        virtual void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
        virtual void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
    protected:
        Ogre::GpuProgramParametersSharedPtr fpParams;
        float start, end, curr;
        Ogre::Timer *timer;
    };
	//---------------------------------------------------------------------------
	class HDRListener: public Ogre::CompositorInstance::Listener
	{
	protected:
		int mVpWidth, mVpHeight;
		int mBloomSize;
		// Array params - have to pack in groups of 4 since this is how Cg generates them
		// also prevents dependent texture read problems if ops don't require swizzle
		float mBloomTexWeights[15][4];
		float mBloomTexOffsetsHorz[15][4];
		float mBloomTexOffsetsVert[15][4];
	public:
		HDRListener();
		virtual ~HDRListener();
		void notifyViewportSize(int width, int height);
		void notifyCompositor(Ogre::CompositorInstance* instance);
		virtual void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
		virtual void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
	};
	//---------------------------------------------------------------------------
	class GaussianListener: public Ogre::CompositorInstance::Listener
	{
	protected:
		int mVpWidth, mVpHeight;
		// Array params - have to pack in groups of 4 since this is how Cg generates them
		// also prevents dependent texture read problems if ops don't require swizzle
		float mBloomTexWeights[15][4];
		float mBloomTexOffsetsHorz[15][4];
		float mBloomTexOffsetsVert[15][4];
	public:
		GaussianListener();
		virtual ~GaussianListener();
		void notifyViewportSize(int width, int height);
		virtual void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
		virtual void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
	};
	//---------------------------------------------------------------------------

	class IComponent;
	class OgreGraphicsSystem;
	class OgrePostProcess
	{
	public:
		OgrePostProcess(Ogre::Viewport* vp);
		virtual ~OgrePostProcess();
		void EnableCompositor(const std::string &name);
		void DisableCompositor(const std::string &name);
		void RegisterCompositors(Ogre::Viewport* vp);
		void Update(Ogre::Camera* vp);
		void SetActiveCompositors(const std::vector<std::string> &active_vec){ m_ActiveVec =active_vec;}
	private:
		Ogre::Viewport* m_Viewport;
		HeatVisionListener* m_HVListener;
		HDRListener* m_HDRListener;
		SSAOListener* m_SSAOListener;
		GaussianListener* m_GaussianListener;
		std::vector<std::string> m_ActiveVec;
	};

	typedef boost::shared_ptr<OgrePostProcess> OgrePostProcessPtr;
}
