#include "EditorSystem.h"
#include "EditorSceneManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSceneManagerFactory.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "EditorGui.h"

namespace GASS
{
	EditorSystem::EditorSystem(SimSystemManagerWeakPtr manager) : Reflection(manager),
		m_LockTerrainObjects(true),
		m_DefaultCameraTemplate("FreeCameraObject")
	{
		m_UpdateGroup = UGID_PRE_SIM;
	}

	EditorSystem::~EditorSystem(void)
	{
	}

	void EditorSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register<EditorSystem>("EditorSystem");
		SceneManagerFactory::GetPtr()->Register<EditorSceneManager>("EditorSceneManager");
		RegisterMember("LockTerrainObjects", &EditorSystem::m_LockTerrainObjects);
		RegisterMember("DefaultCameraTemplate", &EditorSystem::m_DefaultCameraTemplate);
		RegisterGetSet("ShowGUI", &EditorSystem::GetShowGUI, &EditorSystem::SetShowGUI);
	}

	void EditorSystem::OnSystemInit()
	{
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(EditorSystem::OnDrawGui, DrawGUIEvent, 0));
	}

	void EditorSystem::OnDrawGui(DrawGUIEventPtr gui_event)
	{
		static EditorGui editor;
		if (m_ShowGUI)
		{
			ImGuiContext* context = static_cast<ImGuiContext*>(gui_event->m_Context);
			ImGui::SetCurrentContext(context);
			editor.drawUi();
		}
	}
}