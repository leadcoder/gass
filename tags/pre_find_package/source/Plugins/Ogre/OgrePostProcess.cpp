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
#include "Core/Common.h"
#include "Plugins/Ogre/OgrePostProcess.h"
#include "Sim/GASSSceneManagerFactory.h"
#include "Sim/GASSScene.h"

#include "Sim/GASSSceneObject.h"

#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Plugins/Ogre/OgreGraphicsSceneManager.h"
#include "Plugins/Ogre/OgreGraphicsSystem.h"
#include "Plugins/Ogre/Components/OgreCameraComponent.h"
#include "Plugins/Ogre/Components/OgreLocationComponent.h"

#include "Core/System/GASSSystemFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"

#include <OgreRoot.h>
#include <OgreRenderSystem.h>
#include <OgreCompositorChain.h>
#include <OgreRenderWindow.h>
#include <OgreCompositionTechnique.h>
#include <OgreCompositorManager.h>



using namespace Ogre;

namespace GASS
{


	SSAOListener::SSAOListener(Ogre::Viewport* vp) : m_Viewport(vp)
	{

	}

	SSAOListener::~SSAOListener()
	{

	}

	/*void SSAOListener::notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
	{

	}*/

	void SSAOListener::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
	{
		if (pass_id != 42) // not SSAO, return
			return;

		// this is the camera you're using
		Ogre::Camera *cam = m_Viewport->getCamera();

		// calculate the far-top-right corner in view-space
		Ogre::Vector3 farCorner = cam->getViewMatrix(true) * cam->getWorldSpaceCorners()[4];

		// get the pass
		Ogre::Pass *pass = mat->getBestTechnique()->getPass(0);

		// get the vertex shader parameters
		Ogre::GpuProgramParametersSharedPtr params = pass->getVertexProgramParameters();
		// set the camera's far-top-right corner
		if (params->_findNamedConstantDefinition("farCorner"))
			params->setNamedConstant("farCorner", farCorner);

		// get the fragment shader parameters
		params = pass->getFragmentProgramParameters();
		// set the projection matrix we need
		static const Ogre::Matrix4 CLIP_SPACE_TO_IMAGE_SPACE(
			0.5,    0,    0,  0.5,
			0,   -0.5,    0,  0.5,
			0,      0,    1,    0,
			0,      0,    0,    1);
		if (params->_findNamedConstantDefinition("ptMat"))
			params->setNamedConstant("ptMat", CLIP_SPACE_TO_IMAGE_SPACE * cam->getProjectionMatrixWithRSDepth());
		if (params->_findNamedConstantDefinition("far"))
			params->setNamedConstant("far", cam->getFarClipDistance());
	}

	/*************************************************************************
	HeatVisionListener Methods
	*************************************************************************/
	//---------------------------------------------------------------------------
	HeatVisionListener::HeatVisionListener()
	{
		timer = new Ogre::Timer();
		start = end = curr = 0.0f;
	}
	//---------------------------------------------------------------------------
	HeatVisionListener::~HeatVisionListener()
	{
		delete timer;
	}
	//---------------------------------------------------------------------------
	void HeatVisionListener::notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
	{
		if(pass_id == 0xDEADBABE)
		{
			timer->reset();
			fpParams =
				mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
		}
	}
	//---------------------------------------------------------------------------
	void HeatVisionListener::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
	{
		if(pass_id == 0xDEADBABE)
		{
			// "random_fractions" parameter
			fpParams->setNamedConstant("random_fractions", Ogre::Vector4(Ogre::Math::RangeRandom(0.0, 1.0), Ogre::Math::RangeRandom(0, 1.0), 0, 0));

			// "depth_modulator" parameter
			float inc = ((float)timer->getMilliseconds())/1000.0f;
			if ( (fabs(curr-end) <= 0.001) ) {
				// take a new value to reach
				end = Ogre::Math::RangeRandom(0.95, 1.0);
				start = curr;
			} else {
				if (curr > end) curr -= inc;
				else curr += inc;
			}
			timer->reset();

			fpParams->setNamedConstant("depth_modulator", Ogre::Vector4(curr, 0, 0, 0));
		}
	}


	Ogre::CompositorInstance::Listener* HeatVisionLogic::createListener(Ogre::CompositorInstance* instance)
	{
		return new HeatVisionListener;
	}




	//---------------------------------------------------------------------------

	/*************************************************************************
	HDRListener Methods
	*************************************************************************/
	//---------------------------------------------------------------------------
	HDRListener::HDRListener()
	{
	}
	//---------------------------------------------------------------------------
	HDRListener::~HDRListener()
	{
	}
	//---------------------------------------------------------------------------
	void HDRListener::notifyViewportSize(int width, int height)
	{
		mVpWidth = width;
		mVpHeight = height;
	}
	//---------------------------------------------------------------------------
	void HDRListener::notifyCompositor(Ogre::CompositorInstance* instance)
	{
		// Get some RTT dimensions for later calculations
		Ogre::CompositionTechnique::TextureDefinitionIterator defIter =
			instance->getTechnique()->getTextureDefinitionIterator();
		while (defIter.hasMoreElements())
		{
			Ogre::CompositionTechnique::TextureDefinition* def =
				defIter.getNext();
			if(def->name == "rt_bloom0")
			{
				mBloomSize = (int)def->width; // should be square
				// Calculate gaussian texture offsets & weights
				float deviation = 3.0f;
				float texelSize = 1.0f / (float)mBloomSize;

				// central sample, no offset
				mBloomTexOffsetsHorz[0][0] = 0.0f;
				mBloomTexOffsetsHorz[0][1] = 0.0f;
				mBloomTexOffsetsVert[0][0] = 0.0f;
				mBloomTexOffsetsVert[0][1] = 0.0f;
				mBloomTexWeights[0][0] = mBloomTexWeights[0][1] =
					mBloomTexWeights[0][2] = Ogre::Math::gaussianDistribution(0, 0, deviation);
				mBloomTexWeights[0][3] = 1.0f;

				// 'pre' samples
				for(int i = 1; i < 8; ++i)
				{
					mBloomTexWeights[i][0] = mBloomTexWeights[i][1] =
						mBloomTexWeights[i][2] = 1.25f * Ogre::Math::gaussianDistribution(i, 0, deviation);
					mBloomTexWeights[i][3] = 1.0f;
					mBloomTexOffsetsHorz[i][0] = i * texelSize;
					mBloomTexOffsetsHorz[i][1] = 0.0f;
					mBloomTexOffsetsVert[i][0] = 0.0f;
					mBloomTexOffsetsVert[i][1] = i * texelSize;
				}
				// 'post' samples
				for(int i = 8; i < 15; ++i)
				{
					mBloomTexWeights[i][0] = mBloomTexWeights[i][1] =
						mBloomTexWeights[i][2] = mBloomTexWeights[i - 7][0];
					mBloomTexWeights[i][3] = 1.0f;

					mBloomTexOffsetsHorz[i][0] = -mBloomTexOffsetsHorz[i - 7][0];
					mBloomTexOffsetsHorz[i][1] = 0.0f;
					mBloomTexOffsetsVert[i][0] = 0.0f;
					mBloomTexOffsetsVert[i][1] = -mBloomTexOffsetsVert[i - 7][1];
				}

			}
		}
	}
	//---------------------------------------------------------------------------
	void HDRListener::notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
	{
		// Prepare the fragment params offsets
		switch(pass_id)
		{
			//case 994: // rt_lum4
		case 993: // rt_lum3
		case 992: // rt_lum2
		case 991: // rt_lum1
		case 990: // rt_lum0
			break;
		case 800: // rt_brightpass
			break;
		case 701: // rt_bloom1
			{
				// horizontal bloom
				mat->load();
				Ogre::GpuProgramParametersSharedPtr fparams =
					mat->getBestTechnique()->getPass(0)->getFragmentProgramParameters();
				const Ogre::String& progName = mat->getBestTechnique()->getPass(0)->getFragmentProgramName();
				fparams->setNamedConstant("sampleOffsets", mBloomTexOffsetsHorz[0], 15);
				fparams->setNamedConstant("sampleWeights", mBloomTexWeights[0], 15);

				break;
			}
		case 700: // rt_bloom0
			{
				// vertical bloom
				mat->load();
				Ogre::GpuProgramParametersSharedPtr fparams =
					mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
				const Ogre::String& progName = mat->getBestTechnique()->getPass(0)->getFragmentProgramName();
				fparams->setNamedConstant("sampleOffsets", mBloomTexOffsetsVert[0], 15);
				fparams->setNamedConstant("sampleWeights", mBloomTexWeights[0], 15);

				break;
			}
		}
	}
	//---------------------------------------------------------------------------
	void HDRListener::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
	{
	}
	//---------------------------------------------------------------------------
	Ogre::CompositorInstance::Listener* HDRLogic::createListener(Ogre::CompositorInstance* instance)
	{
		HDRListener* listener = new HDRListener;
		Ogre::Viewport* vp = instance->getChain()->getViewport();
		listener->notifyViewportSize(vp->getActualWidth(), vp->getActualHeight());
		listener->notifyCompositor(instance);
		return listener;
	}
	//---------------------------------------------------------------------------

	/*************************************************************************
	GaussianListener Methods
	*************************************************************************/
	//---------------------------------------------------------------------------
	GaussianListener::GaussianListener()
	{
	}
	//---------------------------------------------------------------------------
	GaussianListener::~GaussianListener()
	{
	}
	//---------------------------------------------------------------------------
	void GaussianListener::notifyViewportSize(int width, int height)
	{
		mVpWidth = width;
		mVpHeight = height;
		// Calculate gaussian texture offsets & weights
		float deviation = 3.0f;
		float texelSize = 1.0f / (float)std::min(mVpWidth, mVpHeight);

		// central sample, no offset
		mBloomTexOffsetsHorz[0][0] = 0.0f;
		mBloomTexOffsetsHorz[0][1] = 0.0f;
		mBloomTexOffsetsVert[0][0] = 0.0f;
		mBloomTexOffsetsVert[0][1] = 0.0f;
		mBloomTexWeights[0][0] = mBloomTexWeights[0][1] =
			mBloomTexWeights[0][2] = Ogre::Math::gaussianDistribution(0, 0, deviation);
		mBloomTexWeights[0][3] = 1.0f;

		// 'pre' samples
		for(int i = 1; i < 8; ++i)
		{
			mBloomTexWeights[i][0] = mBloomTexWeights[i][1] =
				mBloomTexWeights[i][2] = Ogre::Math::gaussianDistribution(i, 0, deviation);
			mBloomTexWeights[i][3] = 1.0f;
			mBloomTexOffsetsHorz[i][0] = i * texelSize;
			mBloomTexOffsetsHorz[i][1] = 0.0f;
			mBloomTexOffsetsVert[i][0] = 0.0f;
			mBloomTexOffsetsVert[i][1] = i * texelSize;
		}
		// 'post' samples
		for(int i = 8; i < 15; ++i)
		{
			mBloomTexWeights[i][0] = mBloomTexWeights[i][1] =
				mBloomTexWeights[i][2] = mBloomTexWeights[i - 7][0];
			mBloomTexWeights[i][3] = 1.0f;

			mBloomTexOffsetsHorz[i][0] = -mBloomTexOffsetsHorz[i - 7][0];
			mBloomTexOffsetsHorz[i][1] = 0.0f;
			mBloomTexOffsetsVert[i][0] = 0.0f;
			mBloomTexOffsetsVert[i][1] = -mBloomTexOffsetsVert[i - 7][1];
		}
	}
	//---------------------------------------------------------------------------
	void GaussianListener::notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
	{
		// Prepare the fragment params offsets
		switch(pass_id)
		{
		case 701: // blur horz
			{
				// horizontal bloom
				mat->load();
				Ogre::GpuProgramParametersSharedPtr fparams =
					mat->getBestTechnique()->getPass(0)->getFragmentProgramParameters();
				const Ogre::String& progName = mat->getBestTechnique()->getPass(0)->getFragmentProgramName();
				fparams->setNamedConstant("sampleOffsets", mBloomTexOffsetsHorz[0], 15);
				fparams->setNamedConstant("sampleWeights", mBloomTexWeights[0], 15);

				break;
			}
		case 700: // blur vert
			{
				// vertical bloom
				mat->load();
				Ogre::GpuProgramParametersSharedPtr fparams =
					mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
				const Ogre::String& progName = mat->getBestTechnique()->getPass(0)->getFragmentProgramName();
				fparams->setNamedConstant("sampleOffsets", mBloomTexOffsetsVert[0], 15);
				fparams->setNamedConstant("sampleWeights", mBloomTexWeights[0], 15);

				break;
			}
		}
	}
	//---------------------------------------------------------------------------
	void GaussianListener::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
	{
	}

	Ogre::CompositorInstance::Listener* GaussianBlurLogic::createListener(Ogre::CompositorInstance* instance)
	{
		GaussianListener* listener = new GaussianListener;
		Ogre::Viewport* vp = instance->getChain()->getViewport();
		listener->notifyViewportSize(vp->getActualWidth(), vp->getActualHeight());
		return listener;
	}



	OgrePostProcess::OgrePostProcess(Ogre::Viewport* vp)
		:m_Viewport(vp)
	{

		//m_HVListener = new HeatVisionListener();
		//m_HDRListener = new HDRListener();
		//m_GaussianListener = new GaussianListener();
		m_SSAOListener = new SSAOListener(vp);
		static bool firstTime = true;
		if (firstTime)
		{
			Ogre::CompositorManager& compMgr = Ogre::CompositorManager::getSingleton();
			compMgr.registerCompositorLogic("GaussianBlur", new GaussianBlurLogic);
			compMgr.registerCompositorLogic("HDR", new HDRLogic);
			compMgr.registerCompositorLogic("HeatVision", new HeatVisionLogic);
			firstTime = false;
		}
		
		RegisterCompositors(vp);
	}

	OgrePostProcess::~OgrePostProcess(void)
	{
		delete m_SSAOListener;
		Ogre::CompositorManager& compMgr = Ogre::CompositorManager::getSingleton();
		compMgr.removeCompositorChain(m_Viewport);
		
	}

	void OgrePostProcess::Update(OgreCameraComponentPtr camera)
	{
		for(int i = 0 ; i < m_ActiveVec.size(); i++ )
		{
			DisableCompositor(m_ActiveVec[i]);
		}
		//if(camera->GetPostFilters().size() > 0)
		m_ActiveVec = camera->GetPostFilters();

		for(int i = 0 ; i < m_ActiveVec.size(); i++ )
		{
			DisableCompositor(m_ActiveVec[i]);
			EnableCompositor(m_ActiveVec[i]);
		}
	}

	void OgrePostProcess::EnableCompositor(const std::string &name)
	{
		Ogre::CompositorManager::getSingleton().setCompositorEnabled(m_Viewport,name,true);
	}

	void OgrePostProcess::DisableCompositor(const std::string &name)
	{
		Ogre::CompositorManager::getSingleton().setCompositorEnabled(m_Viewport,name,false);
	}

	void OgrePostProcess::RegisterCompositors(Ogre::Viewport* vp)
	{
		Ogre::CompositorManager::ResourceMapIterator resourceIterator =
			Ogre::CompositorManager::getSingleton().getResourceIterator();

		while (resourceIterator.hasMoreElements())
		{
			Ogre::ResourcePtr resource = resourceIterator.getNext();
			const Ogre::String& compositorName = resource->getName();

			if (compositorName == "Ogre/Scene")
				continue;

			int addPosition = -1;
			if (compositorName == "HDR")
			{
				// HDR must be first in the chain
				addPosition = 0;
			}
			// Don't add the deferred shading compositors, thats a different demo.
			if (Ogre::StringUtil::startsWith(compositorName, "DeferredShading", false))
				continue;

			
			Ogre::CompositorManager::getSingleton().addCompositor(vp, compositorName, addPosition);
			Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, compositorName, false);
			//Ogre::CompositorInstance *instance = Ogre::CompositorManager::getSingleton().addCompositor(vp, compositorName, addPosition);
			//Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, compositorName, false);
			//special handling for Heat Vision which uses a listener
			/*if(instance && (compositorName == "HeatVision"))
				instance->addListener(m_HVListener);
			else if(instance && (compositorName == "HDR"))
			{
				instance->addListener(m_HDRListener);
				m_HDRListener->notifyViewportSize(vp->getActualWidth(), vp->getActualHeight());
				m_HDRListener->notifyCompositor(instance);
			}
			else if(instance && (compositorName == "GaussianBlur"))
			{
				instance->addListener(m_GaussianListener);
				m_GaussianListener->notifyViewportSize(vp->getActualWidth(), vp->getActualHeight());
			}
			else if(instance && (compositorName == "ssao"))
			{
				instance->addListener(m_SSAOListener);

				//m_SSAOListener->notifyViewportSize(vp->getActualWidth(), vp->getActualHeight());
			}*/
		}
	}
}




