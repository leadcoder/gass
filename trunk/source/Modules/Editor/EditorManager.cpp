#include "EditorManager.h"
#include "Core/Utils/Log.h"
#include "Core/MessageSystem/MessageManager.h"
#include "ToolSystem/MouseToolController.h"
namespace GASS
{
	EditorManager::EditorManager()	: m_MouseTools (NULL), m_MessageManager(NULL)
	{
	}

	EditorManager::~EditorManager(void)
	{
		delete m_MouseTools;
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

	void EditorManager::Init()
	{
		m_MessageManager = new MessageManager();
		m_MouseTools=  new MouseToolController(false);
	}

	MessageManager* EditorManager::GetMessageManager(void)
	{
		return m_MessageManager;
	}
}