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
		EditorSystem();
		virtual ~EditorSystem(void);
		static  void RegisterReflection();
		virtual void Init();
		virtual std::string GetSystemName() const {return "EditorSystem";}
		GUISchemaLoader* GetGUISettings() const {return m_GUISettings;}
		ADD_PROPERTY(bool,LockTerrainObjects)
		ADD_PROPERTY(std::string,DefaultCameraTemplate)
	protected:
		GUISchemaLoader* m_GUISettings;
	};
	typedef GASS_SHARED_PTR<EditorSystem> EditorSystemPtr;
}
