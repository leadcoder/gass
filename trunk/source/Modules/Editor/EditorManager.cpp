#include "EditorManager.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "ToolSystem/MouseToolController.h"
namespace GASS
{
	EditorManager::EditorManager()	: m_MessageManager(new MessageManager()),
		m_GUISettings(new GUISchemaLoader)
	{
	}

	EditorManager::~EditorManager(void)
	{
		delete m_MessageManager;
		delete m_GUISettings;
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

	void EditorManager::Init(const FilePath &execution_folder,
							 const FilePath &appdata_folder,
							 const FilePath &mydocuments_folder)
	{
		m_ExecutionFolder = execution_folder;
		m_AppDataFolder = appdata_folder;
		m_MyDocumentsFolder = m_MyDocumentsFolder;
		m_MouseTools =  MouseToolControllerPtr(new MouseToolController(false));
		m_MouseTools->Init();
	}

	void EditorManager::Update(double delta_time)
	{
		EditorManager::Get().GetMessageManager()->Update(delta_time);
		EditorManager::Get().GetMouseToolController()->Update(delta_time);
	}

	MessageManager* EditorManager::GetMessageManager(void)
	{
		return m_MessageManager;
	}


	
}