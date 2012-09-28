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
		m_LockedObjects.clear();
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

	SceneObjectPtr EditorManager::GetSelectedObject() const
	{
		return SceneObjectPtr(m_SelectedObject,boost::detail::sp_nothrow_tag());
	}

	void EditorManager::SelectSceneObject(SceneObjectPtr obj)
	{
		if(obj != GetSelectedObject())
		{
			m_SelectedObject = obj;
			//notify listeners
			int from_id = (int) this;
			MessagePtr selection_msg(new ObjectSelectedMessage(obj,from_id));
			GetMessageManager()->PostMessage(selection_msg);
		}
	}

	void EditorManager::UnlockObject(SceneObjectWeakPtr obj)
	{
		std::set<GASS::SceneObjectWeakPtr>::iterator iter = m_LockedObjects.find(obj);
		if(m_LockedObjects.end() != iter)
		{
			m_LockedObjects.erase(iter);
			EditorManager::GetPtr()->GetMessageManager()->PostMessage(MessagePtr(new ObjectLockMessage(SceneObjectPtr(obj),false)));
		}
	}

	void EditorManager::LockObject(SceneObjectWeakPtr obj)
	{
		std::set<GASS::SceneObjectWeakPtr>::iterator iter = m_LockedObjects.find(obj);
		if(m_LockedObjects.end() == iter)
		{
			m_LockedObjects.insert(obj);
			EditorManager::GetPtr()->GetMessageManager()->PostMessage(MessagePtr(new ObjectLockMessage(SceneObjectPtr(obj),true)));
		}
	}

	bool EditorManager::IsObjectLocked(SceneObjectWeakPtr obj)
	{
		if(m_LockedObjects.end() != m_LockedObjects.find(obj))
		{
			return true;
		}
		return false;
	}
}