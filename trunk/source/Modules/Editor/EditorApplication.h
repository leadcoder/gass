#pragma once

#include <math.h>
#include <string>
#include "Sim/GASS.h"
#include "EditorCommon.h"

namespace GASS
{
	class EditorModuleExport EditorApplication : public StaticMessageListener
	{
	public:
		EditorApplication();
		~EditorApplication();
		void Init(const FilePath &working_folder, const FilePath &appdata_folder_path, const FilePath &mydocuments_folder_path, const std::string &render_system, void* main_win_handle,void*  render_win_handle);
		void Update();
		FilePath GetWorkingFolder() const;
	protected:
		void OnRequestSimulatiornStep(TimeStepRequestPtr message);
		//helpers
		void  UnloadScene();
//		void OnLoadScene(PostSceneLoadEventPtr message);
		
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
		FilePath m_WorkingFolder;
		double m_UpdateFreq;

		volatile bool m_StepSimulation;
		double m_SimStepDeltaTime;
		bool m_ExternalUpdate;
	};
}

