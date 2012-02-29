#ifndef OPENAL_SOUND_SYSTEM_H
#define OPENAL_SOUND_SYSTEM_H

#include "Sim/Systems/SimSystem.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Math/Quaternion.h"
#include "Sim/Scenario/Scene/Messages/CoreSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/SoundSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/GraphicsSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/CoreScenarioSceneMessages.h"
#include "Sim/Scenario/Scene/Messages/GraphicsScenarioSceneMessages.h"
#include "Sim/Systems/Messages/CoreSystemMessages.h"
#ifdef WIN32
#include "al.h"
#include "alc.h"
//#include "alut.h"
#else 
#include "AL/al.h"
#include "AL/alc.h"
//#include "AL/alut.h"
#endif
#include <string>

// Be very careful with these two parameters
// It is very dependant on the audio hardware your
// user is using. It you get too large, it may work
// on one persons system but not on another.
// TODO Write a fct testing the hardware !
#define MAX_AUDIO_BUFFERS   64
#define MAX_AUDIO_SOURCES   16


namespace GASS
{
	class Scenario;
	class SceneObject;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;
	typedef boost::weak_ptr<Scenario> ScenarioWeakPtr;


	class OpenALSoundSystem  :  public Reflection<OpenALSoundSystem, SimSystem>
	{
	public:
		typedef std::map<std::string,ALuint> SoundMap;
		typedef std::map<std::string,ALuint> SourceMap;

		OpenALSoundSystem();
		virtual ~OpenALSoundSystem();
		static void RegisterReflection();
		void OnCreate();

		// OpenAL-specific functions
		bool LoadWaveSound(const std::string &filePath,ALuint &buffer,ALsizei &freq, ALenum &format);
		bool CheckAlError( void );
		bool CheckAlError( std::string pMsg );
		SystemType GetSystemType() const {return "SoundSystem";}
	protected:
		/** See http://www.openal.org/windows_enumeration.html for installing other
		*   devices. You should at least have "Generic Hardware".
		*/
		std::string ListAvailableDevices( void );

		void OnInit(MessagePtr message);
		void OnSceneLoaded(ScenarioAboutToLoadNotifyMessagePtr message);
		void OnChangeCamera(CameraChangedNotifyMessagePtr message);
		void OnCameraMoved(TransformationNotifyMessagePtr message);
		void UpdateListener(const Vec3 &pos, const Quaternion &rot, const Vec3 &vel);

		ALCdevice *m_Device;
		ALCcontext *m_Context;
		bool m_IsInitialised;

		SoundMap m_BufferMap;
		SourceMap m_SourceMap;
		ScenarioWeakPtr m_Scenario;
		SceneObjectWeakPtr m_CurrentCamera;
	};

}

#endif
