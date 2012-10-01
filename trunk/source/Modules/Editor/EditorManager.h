#pragma once

#include "Core/Utils/GASSSingleton.h"
#include "Core/Utils/GASSFilePath.h"
#include "Sim/Scene/GASSSceneObject.h"
#include "Sim/Scene/GASSCoreSceneMessages.h"
#include "Sim/Scene/GASSGraphicsSceneMessages.h"
#include "Sim/Systems/Messages/GASSCoreSystemMessages.h"
#include "Sim/Components/Graphics/GASSICameraComponent.h"
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

	class EditorModuleExport EditorManager : public Singleton<EditorManager> , public boost::enable_shared_from_this<EditorManager>, public IMessageListener
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

		bool IsObjectLocked(SceneObjectWeakPtr obj);
		void UnlockObject(SceneObjectWeakPtr obj);
		void LockObject(SceneObjectWeakPtr obj);

		bool IsObjectVisible(SceneObjectWeakPtr obj);
		void UnhideObject(SceneObjectWeakPtr obj);
		void HideObject(SceneObjectWeakPtr obj);
		bool IsObjectStatic(SceneObjectWeakPtr obj);
	

		void SetSceneObjectsSelectable(bool value) {m_SceneObjectsSelectable = value;}
		bool GetSceneObjectsSelectable() const {return m_SceneObjectsSelectable;}
	
		ScenePtr GetScene(){return ScenePtr(m_Scene,boost::detail::sp_nothrow_tag());}
		void SetObjectSite(SceneObjectPtr obj);
		SceneObjectPtr GetObjectSite() const;
		void MoveCameraToObject(SceneObjectPtr obj);
		CameraComponentPtr GetActiveCamera() const {return CameraComponentPtr(m_ActiveCamera,boost::detail::sp_nothrow_tag());}
		SceneObjectPtr GetActiveCameraObject() const {return SceneObjectPtr(m_ActiveCameraObject,boost::detail::sp_nothrow_tag());}
	protected:
		void OnSceneLoaded(SceneLoadedNotifyMessagePtr message);
		void OnNewScene(SceneAboutToLoadNotifyMessagePtr message);
		void OnChangeCamera(ChangeCameraMessagePtr message);
		
		CameraComponentWeakPtr m_ActiveCamera;
		SceneObjectWeakPtr m_ActiveCameraObject;
		MouseToolControllerPtr m_MouseTools;
		MessageManager* m_MessageManager;
		FilePath m_ExecutionFolder;
		FilePath m_AppDataFolder;
		FilePath m_MyDocumentsFolder;
		GUISchemaLoader* m_GUISettings;

		SceneObjectWeakPtr m_SelectedObject;
		SceneWeakPtr m_Scene;

		std::set<GASS::SceneObjectWeakPtr> m_LockedObjects;
		std::set<GASS::SceneObjectWeakPtr> m_InvisibleObjects;
		std::set<GASS::SceneObjectWeakPtr> m_StaticObjects;
		bool m_SceneObjectsSelectable;
		SceneObjectWeakPtr m_CurrentSite;
	};
}
