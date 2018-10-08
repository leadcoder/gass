#include "EditorSystem.h"
#include "EditorSceneManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSceneManagerFactory.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"

namespace GASS
{
	EditorSystem::EditorSystem() : m_GUISettings(new GUISchemaLoader), 
		m_LockTerrainObjects(true),
		m_DefaultCameraTemplate("FreeCameraObject")
	{
		m_UpdateGroup = UGID_PRE_SIM;
	}

	EditorSystem::~EditorSystem(void)
	{
		delete m_GUISettings;
	}

	void EditorSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("EditorSystem",new GASS::Creator<EditorSystem, SimSystem>);
		SceneManagerFactory::GetPtr()->Register("EditorSceneManager",new GASS::Creator<EditorSceneManager, ISceneManager>);
		RegisterProperty<bool>("LockTerrainObjects",&EditorSystem::GetLockTerrainObjects, &EditorSystem::SetLockTerrainObjects);
		RegisterProperty<std::string>("DefaultCameraTemplate",&EditorSystem::GetDefaultCameraTemplate, &EditorSystem::SetDefaultCameraTemplate);
	}

	void EditorSystem::Init()
	{
	}

}