#ifndef OPENAL_SOUND_SYSTEM_H
#define OPENAL_SOUND_SYSTEM_H

#include "Sim/GASSSimSystem.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSQuaternion.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Messages/GASSSoundSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "Sim/Messages/GASSGraphicsSceneMessages.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"
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
	class Scene;
	class SceneObject;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;
	typedef boost::weak_ptr<Scene> SceneWeakPtr;


	class OpenALSoundSystem  :  public Reflection<OpenALSoundSystem, SimSystem>
	{
	public:
		typedef std::map<std::string,ALuint> SoundMap;
		typedef std::map<std::string,ALuint> SourceMap;

		OpenALSoundSystem();
		virtual ~OpenALSoundSystem();
		static void RegisterReflection();
		virtual void Init();

		// OpenAL-specific functions
		void LoadWaveSound(const std::string &filePath,ALuint &buffer,ALsizei &freq, ALenum &format);
		bool CheckAlError( void );
		bool CheckAlError( std::string pMsg );
		std::string GetSystemName() const {return "OpenALSoundSystem";}
	protected:
		/** See http://www.openal.org/windows_enumeration.html for installing other
		*   devices. You should at least have "Generic Hardware".
		*/
		std::string ListAvailableDevices( void );

		void OnInit(MessagePtr message);
		void OnSceneLoaded(SceneAboutToLoadNotifyMessagePtr message);
		void OnChangeCamera(CameraChangedEventPtr message);
		void OnCameraMoved(TransformationNotifyMessagePtr message);
		void UpdateListener(const Vec3 &pos, const Quaternion &rot, const Vec3 &vel);

		ALCdevice *m_Device;
		ALCcontext *m_Context;
		bool m_IsInitialised;

		SoundMap m_BufferMap;
		SourceMap m_SourceMap;
		SceneWeakPtr m_Scene;
		SceneObjectWeakPtr m_CurrentCamera;
	};

}

#endif
