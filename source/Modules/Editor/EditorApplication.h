#pragma once

#include "Sim/GASS.h"
#include "EditorCommon.h"

namespace GASS
{
	class EditorModuleExport EditorApplication : public StaticMessageListener
	{
	public:
		EditorApplication(const FilePath &log_folder);
		~EditorApplication() override;
		void Init(const FilePath &editor_config, const FilePath &gass_configuration, const FilePath &control_configuration, void* main_win_handle,void*  render_win_handle);
		bool Update();
	protected:
		//void OnRequestSimulatiornStep(TimeStepRequestPtr message);
		//void  UnloadScene();

		std::string m_StartScene;
		bool m_Initilized;
		bool  LoadSettings(const std::string &filename);
		std::vector<std::string> m_Templates;
		std::string m_GUIState;
		std::string m_Mode;
		std::string m_ServerName;
		std::string m_ClientName;
		int m_ServerPort;
		int m_ClientPort;
		double m_UpdateFreq;
		volatile bool m_StepSimulation;
		double m_SimStepDeltaTime;
		bool m_ExternalUpdate;
		//FilePath m_LogFolder;
	};
}

