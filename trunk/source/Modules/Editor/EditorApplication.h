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
		void Init(const FilePath &working_folder, const FilePath &appdata_folder_path, const FilePath &mydocuments_folder_path, void* main_win_handle,void*  render_win_handle);
		void Update();
		GASS::ScenePtr GetScene(void) {return m_Scene;}
		void LoadScene(const std::string &scene_path);
		//void NewScene(const std::string &scene_path);
		//void SaveScene(const std::string &scene_path);
		FilePath GetWorkingFolder() const;
		std::string GetScenePath() const{ return m_ScenePath;}
		
	protected:
		void OnRequestSimulatiornStep(RequestTimeStepMessagePtr message);
		//helpers
		void  UnloadScene();
		void OnLoadScene(SceneLoadedNotifyMessagePtr message);
		
		ScenePtr m_Scene;
		std::string m_StartScene;

		bool m_Initilized;
		bool  LoadSettings(const std::string &filename);
		std::vector<std::string> m_SceneNames;
		std::vector<std::string> m_ScenePaths;
		std::vector<std::string> m_Templates;
		std::string m_GUIState;

		std::string m_ScenePath;
		std::string m_SceneName;
		bool m_NewScene;
		bool m_LoadScene;
		bool m_UseObjectID;
		bool m_UseTerrainNormalOnDrop;
		float m_RayPickDist;
		std::string m_IDSuffix;
		std::string m_IDPrefix;
		int m_NumRTCThreads;

		std::string m_Mode;
		std::string m_ServerName;
		std::string m_ClientName;
		int m_ServerPort;
		int m_ClientPort;
		FilePath m_WorkingFolder;
		double m_UpdateFreq;


		volatile bool m_StepSimulation;
		double m_SimStepDeltaTime;
	};
}

