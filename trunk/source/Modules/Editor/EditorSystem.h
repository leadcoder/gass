#pragma once

#include "Core/Utils/GASSSingleton.h"
#include "Core/Utils/GASSFilePath.h"
#include "Core/MessageSystem/GASSStaticMessageListener.h"
#include "Core/System/GASSSystemFactory.h"

#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "Sim/Messages/GASSGraphicsSceneMessages.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"
#include "Sim/GASSSimSystem.h"
#include "Sim/Interface/GASSICameraComponent.h"

#include "EditorCommon.h"
#include "GUISchemaLoader.h"
#include <list>
#include <vector>
#include <set>

namespace GASS
{
	
	class MouseToolController;
	class MessageManager;
	typedef SPTR<MouseToolController> MouseToolControllerPtr;

	class EditorModuleExport EditorSystem :  public Reflection<EditorSystem, SimSystem>
	{
	public:
		EditorSystem();
		virtual ~EditorSystem(void);
		static  void RegisterReflection();
		virtual void Init();
		virtual std::string GetSystemName() const {return "EditorSystem";}

		void SetPaths(const FilePath &execution_folder,
		const FilePath &appdata_folder,
		const FilePath &mydocuments_folder);
		
		const FilePath GetWorkingFolder(void) {return m_ExecutionFolder;}
		GUISchemaLoader* GetGUISettings() const {return m_GUISettings;}
	protected:
		FilePath m_ExecutionFolder;
		FilePath m_AppDataFolder;
		FilePath m_MyDocumentsFolder;
		GUISchemaLoader* m_GUISettings;
		
	};
	typedef SPTR<EditorSystem> EditorSystemPtr;
}
