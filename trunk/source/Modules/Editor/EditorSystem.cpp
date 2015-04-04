#include "EditorSystem.h"
#include "EditorSceneManager.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSceneManagerFactory.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSIViewport.h"
#include "Sim/Interface/GASSICameraComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "ToolSystem/MouseToolController.h"

namespace GASS
{
	EditorSystem::EditorSystem() : m_GUISettings(new GUISchemaLoader), 
		m_LockTerrainObjects(true),
		m_DefaultCameraTemplate("FreeCameraObject")
	{
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
		//Register at rtc
		SimEngine::Get().GetRuntimeController()->Register(shared_from_this(),m_TaskNodeName);
	}

}