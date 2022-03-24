#pragma once

#include "Sim/GASSCommon.h"
#include "Core/Utils/GASSSingleton.h"
#include "Core/Utils/GASSFilePath.h"
#include "Sim/GASSSystemFactory.h"

#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSGraphicsSceneMessages.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "Sim/GASSSimSystem.h"
#include "Modules/Editor/EditorCommon.h"

namespace GASS
{
	
	class MouseToolController;
	class MessageManager;
	using MouseToolControllerPtr = std::shared_ptr<MouseToolController>;

	class EditorModuleExport EditorSystem :  public Reflection<EditorSystem, SimSystem>
	{
	public:
		EditorSystem(SimSystemManagerWeakPtr manager);
		~EditorSystem(void) override;
		static  void RegisterReflection();
		void OnSystemInit() override;
		std::string GetSystemName() const override {return "EditorSystem";}
	
		bool GetLockTerrainObjects() const { return m_LockTerrainObjects; }
		std::string GetDefaultCameraTemplate() const { return m_DefaultCameraTemplate; }
		void SetShowGUI(bool value) { m_ShowGUI = value; }
		bool GetShowGUI() const { return m_ShowGUI; }
	protected:
		void OnDrawGui(DrawGUIEventPtr gui_event);
		bool m_LockTerrainObjects;
		std::string m_DefaultCameraTemplate;
		bool m_ShowGUI = false;
	};
	using EditorSystemPtr = std::shared_ptr<EditorSystem>;
}
