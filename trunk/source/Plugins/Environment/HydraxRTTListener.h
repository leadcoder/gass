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
#ifndef HYDRAX_RTT_LISTENER_H
#define HYDRAX_RTT_LISTENER_H

#include "Sim/Components/GASSBaseSceneComponent.h"
#include "Sim/Scenario/Scene/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Components/Graphics/Geometry/GASSIGeometryComponent.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include <OgreRenderTargetListener.h>
#include <OgreRenderTargetListener.h>
#include "Hydrax/Hydrax.h"
#include "Hydrax/Noise/Perlin/Perlin.h"
#include "Hydrax/Modules/ProjectedGrid/ProjectedGrid.h"
#include "SkyX.h"

namespace GASS
{
	class HydraxRttListener : public Hydrax::RttManager::RttListener
	{
	private:
		Hydrax::Hydrax *mHydrax;
		// SkyX object
		SkyX::SkyX *mSkyX;
	public:
		HydraxRttListener(SkyX::SkyX *skyx,Hydrax::Hydrax *hydrax): Hydrax::RttManager::RttListener(), mSkyX(skyx) , mHydrax(hydrax) 
		{

		}
		void preRenderTargetUpdate(const Hydrax::RttManager::RttType& Rtt)
		{
			// If needed in any case...
			bool underwater = mHydrax->_isCurrentFrameUnderwater();

			switch (Rtt)
			{
			case Hydrax::RttManager::RTT_REFLECTION:
				{
					// No stars in the reflection map
					mSkyX->setStarfieldEnabled(false);
				}
				break;

			case Hydrax::RttManager::RTT_REFRACTION:
				{
				}
				break;

			case Hydrax::RttManager::RTT_DEPTH: case Hydrax::RttManager::RTT_DEPTH_REFLECTION:
				{
					// Hide SkyX components in depth maps
					mSkyX->getMeshManager()->getEntity()->setVisible(false);
					mSkyX->getMoonManager()->getMoonBillboard()->setVisible(false);
				}
				break;
			}
		}

		void postRenderTargetUpdate(const Hydrax::RttManager::RttType& Rtt)
		{
			bool underwater = mHydrax->_isCurrentFrameUnderwater();

			switch (Rtt)
			{
			case Hydrax::RttManager::RTT_REFLECTION:
				{
					mSkyX->setStarfieldEnabled(true);
				}
				break;

			case Hydrax::RttManager::RTT_REFRACTION:
				{
				}
				break;

			case Hydrax::RttManager::RTT_DEPTH: case Hydrax::RttManager::RTT_DEPTH_REFLECTION:
				{
					mSkyX->getMeshManager()->getEntity()->setVisible(true);
					mSkyX->getMoonManager()->getMoonBillboard()->setVisible(true);
				}
				break;
			}
		}
	};
}

#endif