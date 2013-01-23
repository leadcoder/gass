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

#include "Sim/Interface/GASSIGeometryComponent.h"
#include <tinyxml.h>

#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSCommon.h"
#include "OgreTerrain.h"
#include "OgreTerrainGroup.h"
#include "OgreTerrainQuadTreeNode.h"
#include "OgreTerrainMaterialGeneratorA.h"
#include "OgreTerrainPaging.h"
#include "OgreMaterialManager.h"

namespace GASS
{
	
    class GASSTerrainMaterialGenerator : public Ogre::TerrainMaterialGenerator
   {
   public:

	   GASSTerrainMaterialGenerator(const Ogre::String &material) :
         Ogre::TerrainMaterialGenerator()
      {
         mProfiles.push_back(OGRE_NEW CustomMaterialProfile(this, "CustomMaterialProfile", "Profile used custom materials",material));
         setActiveProfile("CustomMaterialProfile");
      }

      class CustomMaterialProfile : public Ogre::TerrainMaterialGenerator::Profile
      {
      public:
         CustomMaterialProfile(Ogre::TerrainMaterialGenerator* parent, const Ogre::String& name, const Ogre::String& desc, const Ogre::String& material) :
            Ogre::TerrainMaterialGenerator::Profile(parent,name,desc), m_Material(material)
         {
         }

         virtual ~CustomMaterialProfile() {}
         Ogre::MaterialPtr generate(const Ogre::Terrain* terrain)
         {
			Ogre::MaterialPtr mat = terrain->_getMaterial();
			if (mat.isNull())
			{
				Ogre::MaterialManager& matMgr = Ogre::MaterialManager::getSingleton();
				Ogre::MaterialPtr temp_mat = matMgr.getByName(m_Material);
				mat = temp_mat->clone("TerrainPageMaterial");
				Ogre::TextureUnitState* tu = mat->getTechnique(0)->getPass(0)->getTextureUnitState("normal_map");
				if(tu)
				{
					tu->setTextureName(terrain->getTerrainNormalMap()->getName());
					tu->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
				}
			}
			return mat;
         }

         Ogre::MaterialPtr generateForCompositeMap(const Ogre::Terrain* terrain)
         {
           
			Ogre::MaterialPtr mat = terrain->_getCompositeMapMaterial();
			if (mat.isNull())
			{
				Ogre::MaterialManager& matMgr = Ogre::MaterialManager::getSingleton();
				Ogre::MaterialPtr temp_mat = matMgr.getByName(m_Material);
				mat = temp_mat->clone("TerrainPageCompositeMaterial");
				Ogre::TextureUnitState* tu = mat->getTechnique(0)->getPass(0)->getTextureUnitState("normal_map");
				if(tu)
				{
					tu->setTextureName(terrain->getTerrainNormalMap()->getName());
					tu->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
				}
			}
			return mat;
         }

         Ogre::uint8 getMaxLayers(const Ogre::Terrain* terrain) const
         {
            return 1;
         }

         void updateParams(const Ogre::MaterialPtr& mat, const Ogre::Terrain* terrain) {}

         void updateParamsForCompositeMap(const Ogre::MaterialPtr& mat, const Ogre::Terrain* terrain) {}

         void requestOptions(Ogre::Terrain* terrain)
		 {
			 //terrain->_setMorphRequired(true);
			 terrain->_setNormalMapRequired(true);
			 terrain->_setLightMapRequired(false, false);
			 terrain->_setCompositeMapRequired(false);
		 }

         bool isLayerNormalMappingEnabled() const  { return false; }
         void setLayerNormalMappingEnabled(bool enabled) {}
         bool isLayerParallaxMappingEnabled() const  { return false; }
         void setLayerParallaxMappingEnabled(bool enabled) {}
         bool isLayerSpecularMappingEnabled() const  { return false; }
         void setLayerSpecularMappingEnabled(bool enabled) {}
         bool isGlobalColourMapEnabled() const  { return false; }
         void setGlobalColourMapEnabled(bool enabled) {}
         bool isLightmapEnabled() const  { return false; }
         void setLightmapEnabled(bool enabled) {}
         bool isCompositeMapEnabled() const  { return false; }
         void setCompositeMapEnabled(bool enabled) {}
		 bool isVertexCompressionSupported() const {return false;}
	  protected:
		  Ogre::String m_Material;
      };
	  
   };


}

