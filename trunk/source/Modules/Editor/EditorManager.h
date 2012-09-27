#pragma once

#include "Core/Utils/GASSSingleton.h"
#include "Core/Utils/GASSFilePath.h"
#include "EditorCommon.h"
#include "GUISchemaLoader.h"
#include <list>
#include <vector>


namespace GASS
{
	
	class MouseToolController;
	class MessageManager;
	typedef boost::shared_ptr<MouseToolController> MouseToolControllerPtr;

	class EditorModuleExport EditorManager : public Singleton<EditorManager> 
	{
	public:
		EditorManager();
		virtual ~EditorManager(void);
		static EditorManager* GetPtr();
		static EditorManager& Get();

		void Init(const FilePath &execution_folder,
					const FilePath &appdata_folder,
					const FilePath &mydocuments_folder);
		void Update(double delta_time);
		GASS::MessageManager* GetMessageManager(void);
		MouseToolControllerPtr GetMouseToolController() {return m_MouseTools;}
		const FilePath GetWorkingFolder(void) {return m_ExecutionFolder;}
		GUISchemaLoader* GetGUISettings() const {return m_GUISettings;}
	protected:
	
		MouseToolControllerPtr m_MouseTools;
		MessageManager* m_MessageManager;
		FilePath m_ExecutionFolder;
		FilePath m_AppDataFolder;
		FilePath m_MyDocumentsFolder;
		GUISchemaLoader* m_GUISettings;
	};
}
