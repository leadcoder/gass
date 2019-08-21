#pragma once

#include "Sim/GASSCommon.h"
#include "Core/Utils/GASSSingleton.h"
#include "Core/Utils/GASSFilePath.h"
#include "Sim/GASSSystemFactory.h"

#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSGraphicsSceneMessages.h"
#include "Sim/GASSSimSystem.h"
#include "GUISchemaLoader.h"

namespace GASS
{
	
	class MouseToolController;
	class MessageManager;
	typedef GASS_SHARED_PTR<MouseToolController> MouseToolControllerPtr;

	class EditorModuleExport EditorSystem :  public Reflection<EditorSystem, SimSystem>
	{
	public:
		EditorSystem(SimSystemManagerWeakPtr manager);
		~EditorSystem(void) override;
		static  void RegisterReflection();
		void OnSystemInit() override;
		std::string GetSystemName() const override {return "EditorSystem";}
		GUISchemaLoader* GetGUISettings() const {return m_GUISettings;}

		bool GetLockTerrainObjects() const { return m_LockTerrainObjects; }
		std::string GetDefaultCameraTemplate() const { return m_DefaultCameraTemplate; }
	protected:
		bool m_LockTerrainObjects;
		std::string m_DefaultCameraTemplate;
		GUISchemaLoader* m_GUISettings;
	};
	typedef GASS_SHARED_PTR<EditorSystem> EditorSystemPtr;
}
