#include "EditorSystem.h"
#include "EditorSceneManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSceneManagerFactory.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"

namespace GASS
{
	EditorSystem::EditorSystem(SimSystemManagerWeakPtr manager) : Reflection(manager),
		m_GUISettings(new GUISchemaLoader), 
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
		SystemFactory::GetPtr()->Register<EditorSystem>("EditorSystem");
		SceneManagerFactory::GetPtr()->Register<EditorSceneManager>("EditorSceneManager");
		RegisterMember("LockTerrainObjects", &EditorSystem::m_LockTerrainObjects);
		RegisterMember("DefaultCameraTemplate", &EditorSystem::m_DefaultCameraTemplate);
	}

	void EditorSystem::OnSystemInit()
	{

	}
}