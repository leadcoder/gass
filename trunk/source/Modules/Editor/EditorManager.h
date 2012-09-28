#pragma once

#include "Core/Utils/GASSSingleton.h"
#include "Core/Utils/GASSFilePath.h"
#include "Sim/Scene/GASSSceneObject.h"
#include "EditorCommon.h"
#include "GUISchemaLoader.h"
#include <list>
#include <vector>
#include <set>

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

		SceneObjectPtr GetSelectedObject() const;
		void SelectSceneObject(SceneObjectPtr obj);

		bool IsObjectLocked(GASS::SceneObjectWeakPtr obj);
		void UnlockObject(SceneObjectWeakPtr obj);
		void LockObject(SceneObjectWeakPtr obj);
	protected:
	
		MouseToolControllerPtr m_MouseTools;
		MessageManager* m_MessageManager;
		FilePath m_ExecutionFolder;
		FilePath m_AppDataFolder;
		FilePath m_MyDocumentsFolder;
		GUISchemaLoader* m_GUISettings;

		SceneObjectWeakPtr m_SelectedObject;

		std::set<GASS::SceneObjectWeakPtr> m_LockedObjects;
	};
}
