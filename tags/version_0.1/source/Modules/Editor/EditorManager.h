#pragma once

#include "Core/Utils/Singleton.h"
#include "Core/Utils/FilePath.h"
#include "EditorCommon.h"
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
		GASS::MessageManager* GetMessageManager(void);
		MouseToolControllerPtr GetMouseToolController() {return m_MouseTools;}
		const FilePath GetWorkingFolder(void) {return m_ExecutionFolder;}
	protected:
		//std::string m_WorkingDirPath;
		//Create tool controller
		MouseToolControllerPtr m_MouseTools;
		MessageManager* m_MessageManager;
		FilePath m_ExecutionFolder;
		FilePath m_AppDataFolder;
		FilePath m_MyDocumentsFolder;
	};
}
