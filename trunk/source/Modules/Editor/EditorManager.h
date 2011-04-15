#pragma once

#include "Core/Utils/Singleton.h"
#include "EditorCommon.h"
#include <list>
#include <vector>


namespace GASS
{
	class MouseToolController;
	class MessageManager;

	class EditorModuleExport EditorManager : public Singleton<EditorManager>
	{
	public:
		EditorManager();
		virtual ~EditorManager(void);
		static EditorManager* GetPtr();
		static EditorManager& Get();

		void Init();
		GASS::MessageManager* GetMessageManager(void);
		MouseToolController* GetMouseToolController() {return m_MouseTools;}
		//const std::string GetWorkingDirPath(void) {return m_WorkingDirPath;}
	protected:
		//std::string m_WorkingDirPath;
		//Create tool controller
		MouseToolController* m_MouseTools;
		MessageManager* m_MessageManager;
	};
}
