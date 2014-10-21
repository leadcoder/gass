// CoreTest.cpp : Defines the entry point for the console application.
//

#ifndef GASS_OBJ_EXPORTER_H
#define GASS_OBJ_EXPORTER_H

#include "Sim/GASSSceneObject.h"
#include "EditorCommon.h"

namespace GASS
{

	class EditorModuleExport ObjExporter
	{
	public:
		ObjExporter();
		virtual ~ObjExporter();
		void ExportFiles(const std::string &out_dir, SceneObjectPtr root_obj);
		void Export(const std::string &out_file, SceneObjectPtr root_obj, bool recursive = true);
		void SetFlipDDSTexCoords(bool value) {m_FlipDDSTexCoords = value;}
		bool GetFlipDDSTexCoords() const {return m_FlipDDSTexCoords;}
		void SetCopyTextures(bool value) {m_CopyTextures = value;}
		bool GetCopyTextures() const {return m_CopyTextures;}
	private:
		bool m_CopyTextures;
		bool m_FlipDDSTexCoords;
	};
}
#endif





