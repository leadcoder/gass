#ifndef OPENAL_SOUND_SYSTEM_H
#define OPENAL_SOUND_SYSTEM_H
#include "Sim/GASSCommon.h"
#include "Sim/GASSSimSystem.h"
#include "Core/Math/GASSQuaternion.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"
#ifdef WIN32
	#include "al.h"
	#include "alc.h"
#else //diffrent location
	#include "al.h"
	#include "alc.h"
#endif

// Be very careful with these two parameters
// It is very dependant on the audio hardware your
// user is using. It you get too large, it may work
// on one persons system but not on another.
// TODO Write a fct testing the hardware !
#define MAX_AUDIO_BUFFERS   64
#define MAX_AUDIO_SOURCES   16
#define GASS_TO_OAL_VEC(v) { static_cast<ALfloat>(v.x), static_cast<ALfloat>(v.y), static_cast<ALfloat>(v.z) }

namespace GASS
{
	class Scene;
	class SceneObject;
	using SceneObjectWeakPtr = std::weak_ptr<SceneObject>;
	using SceneWeakPtr = std::weak_ptr<Scene>;


	class OpenALSoundSystem  :  public Reflection<OpenALSoundSystem, SimSystem>
	{
	public:
		using SoundMap = std::map<std::string, ALuint>;
		using SourceMap = std::map<std::string, ALuint>;

		static void RegisterReflection();
		OpenALSoundSystem(SimSystemManagerWeakPtr manager);
		~OpenALSoundSystem() override;
		void OnSystemInit() override;
	
		// OpenAL-specific functions
		void LoadWaveSound(const std::string &filePath,ALuint &buffer);
		/**
			Check or OpenAL error, will cast exception if error present
			@param what_class Provide class and function to exception
		*/
		static void CheckAlError(const std::string &what_class);
		//bool CheckAlError( void );
		//bool CheckAlError( std::string pMsg );
		std::string GetSystemName() const override {return "OpenALSoundSystem";}
	protected:
		std::string ListAvailableDevices( void );
		void OnCameraChanged(CameraChangedEventPtr message);
		void OnCameraMoved(TransformationChangedEventPtr message);
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
