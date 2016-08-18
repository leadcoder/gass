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

#include "Sim/Interface/GASSIGraphicsSystem.h"
#include "Sim/GASSBaseSceneManager.h"
#include "Core/MessageSystem/GASSMessageType.h"
//#include "Plugins/Ogre/Components/OgreCameraComponent.h"

namespace GASS
{
	class OgreCameraComponent;
	typedef GASS_SHARED_PTR<OgreCameraComponent> OgreCameraComponentPtr;
	//The simple types of compositor logics will all do the same thing -
	//Attach a listener to the created compositor
	class ListenerFactoryLogic : public Ogre::CompositorLogic
	{
	public:
		/** @copydoc CompositorLogic::compositorInstanceCreated */
		virtual void compositorInstanceCreated(Ogre::CompositorInstance* newInstance) 
		{
			Ogre::CompositorInstance::Listener* listener = createListener(newInstance);
			newInstance->addListener(listener);
			mListeners[newInstance] = listener;
		}

		/** @copydoc CompositorLogic::compositorInstanceDestroyed */
		virtual void compositorInstanceDestroyed(Ogre::CompositorInstance* destroyedInstance)
		{
			delete mListeners[destroyedInstance];
			mListeners.erase(destroyedInstance);
		}
	protected:
		//This is the method that implementations will need to override
		virtual Ogre::CompositorInstance::Listener* createListener(Ogre::CompositorInstance* instance) = 0;
	private:
		typedef std::map<Ogre::CompositorInstance*, Ogre::CompositorInstance::Listener*> ListenerMap;
		ListenerMap mListeners;
	};

	//The compositor logic for the heat vision compositor
	class HeatVisionLogic : public ListenerFactoryLogic
	{
	protected:
		/** @copydoc ListenerFactoryLogic::createListener */
		virtual Ogre::CompositorInstance::Listener* createListener(Ogre::CompositorInstance* instance);
	};

	//The compositor logic for the hdr compositor
	class HDRLogic : public ListenerFactoryLogic
	{
	protected:
		/** @copydoc ListenerFactoryLogic::createListener */
		virtual Ogre::CompositorInstance::Listener* createListener(Ogre::CompositorInstance* instance);
	};

	//The compositor logic for the gaussian blur compositor
	class GaussianBlurLogic : public ListenerFactoryLogic
	{
	protected:
		/** @copydoc ListenerFactoryLogic::createListener */
		virtual Ogre::CompositorInstance::Listener* createListener(Ogre::CompositorInstance* instance);
	};


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

	class Component;
	class OgreGraphicsSystem;
	class OgrePostProcess
	{
	public:
		OgrePostProcess(Ogre::Viewport* vp);
		virtual ~OgrePostProcess();
		void EnableCompositor(const std::string &name);
		void DisableCompositor(const std::string &name);
		void RegisterCompositors(Ogre::Viewport* vp);
		void Update(OgreCameraComponentPtr camera);
		void SetActiveCompositors(const std::vector<std::string> &active_vec){ m_ActiveVec =active_vec;}
	private:
		Ogre::Viewport* m_Viewport;
		HeatVisionListener* m_HVListener;
		HDRListener* m_HDRListener;
		SSAOListener* m_SSAOListener;
		GaussianListener* m_GaussianListener;
		std::vector<std::string> m_ActiveVec;
	};
	typedef GASS_SHARED_PTR<OgrePostProcess> OgrePostProcesGASS_SHARED_PTR;
}