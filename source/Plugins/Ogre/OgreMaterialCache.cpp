#include "OgreMaterialCache.h"

namespace GASS
{
	OgreMaterialCache::MaterialCacheMap OgreMaterialCache::m_MaterialCacheMap;
	void OgreMaterialCache::Add( const std::string &name,Ogre::Entity* entity )
	{
		if(m_MaterialCacheMap.find(name) == m_MaterialCacheMap.end())
		{
			std::vector<std::string> sub_mesh_mats;
			for(unsigned int i = 0 ; i < entity->getNumSubEntities(); i++)
			{
				Ogre::SubEntity* se = entity->getSubEntity(i);
				Ogre::MaterialPtr mat = se->getMaterial();
				sub_mesh_mats.push_back(mat->getName());
			}
			m_MaterialCacheMap[name] = sub_mesh_mats;
		}
	}

	void OgreMaterialCache::Add(Ogre::ManualObject* mo )
	{
		const std::string name = mo->getName();
		if(m_MaterialCacheMap.find(name) == m_MaterialCacheMap.end())
		{
			std::vector<std::string> sub_mesh_mats;
			for(unsigned int i = 0 ; i < mo->getNumSections(); i++)
			{
				Ogre::ManualObject::ManualObjectSection* mos = mo->getSection(i);
				Ogre::MaterialPtr mat = mos->getMaterial();
				sub_mesh_mats.push_back(mat->getName());
			}
			m_MaterialCacheMap[name] = sub_mesh_mats;
		}
	}

	void OgreMaterialCache::Restore(const std::string &name,Ogre::Entity* entity )
	{
		
		if(m_MaterialCacheMap.find(name) != m_MaterialCacheMap.end())
		{
			std::vector<std::string> sub_mesh_mats = m_MaterialCacheMap[name];
			if(sub_mesh_mats .size() == entity->getNumSubEntities())
			{
				for(unsigned int i = 0 ; i < entity->getNumSubEntities(); i++)
				{
					Ogre::SubEntity* se = entity->getSubEntity(i);
					se->setMaterialName(sub_mesh_mats[i]);
				}
			}
		}
	}


	void OgreMaterialCache::Restore(Ogre::ManualObject* mo )
	{
		const std::string name = mo->getName();
		if(m_MaterialCacheMap.find(name) != m_MaterialCacheMap.end())
		{
			std::vector<std::string> sub_mesh_mats = m_MaterialCacheMap[name];
			if(sub_mesh_mats.size() == mo->getNumSections())
			{
				for(unsigned int i = 0 ; i < mo->getNumSections(); i++)
				{
					Ogre::ManualObject::ManualObjectSection* mos = mo->getSection(i);
					mos->setMaterialName(sub_mesh_mats[i]);
				}
			}
		}
	}
}