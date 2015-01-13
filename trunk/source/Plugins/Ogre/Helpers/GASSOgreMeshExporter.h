#pragma once

#include "Sim/GASSSceneObject.h"
#include "Sim/GASSGraphicsMesh.h"

namespace Ogre
{
	class ManualObject;
}

namespace GASS
{

	class OgreMeshExporter
	{
	public:
		OgreMeshExporter();
		virtual ~OgreMeshExporter();
		void Export(const std::string &out_file, SceneObjectPtr root_obj);
		void SetFlipDDSTexCoords(bool value) {m_FlipDDSTexCoords = value;}
		bool GetFlipDDSTexCoords() const {return m_FlipDDSTexCoords;}

		void SetCopyTextures(bool value) {m_CopyTextures = value;}
		bool GetCopyTextures() const {return m_CopyTextures;}
	private:
		void Add(Ogre::ManualObject* manual_object, GraphicsMeshPtr mesh);
		void SaveToMesh(Ogre::ManualObject* manual_object,const std::string &file_name);

		bool m_CopyTextures;
		bool m_FlipDDSTexCoords;
	};
}