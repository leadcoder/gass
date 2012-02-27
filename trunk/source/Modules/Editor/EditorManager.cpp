#include "EditorManager.h"
#include "Core/Utils/Log.h"
#include "Core/MessageSystem/MessageManager.h"
#include "ToolSystem/MouseToolController.h"
namespace GASS
{
	EditorManager::EditorManager()	: m_MessageManager(new MessageManager())
	{
	}

	EditorManager::~EditorManager(void)
	{
		delete m_MessageManager;
	}

	template<> EditorManager* Singleton<EditorManager>::m_Instance = 0;
	EditorManager* EditorManager::GetPtr(void)
	{
		assert(m_Instance);
		return m_Instance;
	}

	EditorManager& EditorManager::Get(void)
	{
		assert(m_Instance);
		return *m_Instance;
	}

	void EditorManager::Init(const FilePath &working_folder)
	{
		m_WorkingFolder = working_folder;
		m_MouseTools =  MouseToolControllerPtr(new MouseToolController(false));
		m_MouseTools->Init();
	}

	MessageManager* EditorManager::GetMessageManager(void)
	{
		return m_MessageManager;
	}


	
}