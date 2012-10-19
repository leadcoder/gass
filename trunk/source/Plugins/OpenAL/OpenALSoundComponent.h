#ifndef OPEN_AL_SOUND_COMPONENT_HH
#define OPEN_AL_SOUND_COMPONENT_HH


#include "Sim/GASSCommon.h"
#include "Sim/Interface/GASSILightComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Messages/GASSSoundSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSVector.h"
#ifdef WIN32
#include "al.h"
#include "alc.h"
#else 
#include "AL/al.h"
#include "AL/alc.h"
#endif

namespace GASS
{
	class OpenALSoundComponent : public Reflection<OpenALSoundComponent,BaseSceneComponent>
	{
	public:
		OpenALSoundComponent(void);
		virtual ~OpenALSoundComponent(void);
		static void RegisterReflection();

		virtual void OnInitialize();
		virtual void OnDelete();
		//create sound interface for this?
		virtual void Play();
		virtual void Stop();
		virtual void StopLooping();
		virtual bool IsPlaying();
	protected:

		//Helper to load sound from wave-file
		void LoadWaveSound(const std::string &filePath);

		float GetMinDistance() const;
		void SetMinDistance(float min_dist);
		float GetMaxDistance() const;
		void SetMaxDistance(float max_dist);
		float GetVolume() const;
		void SetVolume(float volume);
		bool GetStereo() const;
		void SetStereo(bool stereo);
		bool GetLoop() const;
		void SetLoop(bool loop);
		void SetPitch(float pitch);
		float GetPitch() const;
		void SetRolloff(float rolloff);
		float GetRolloff() const;
		std::string GetSoundFile() const;
		void SetSoundFile(const std::string &file);

		void SetPosition(const Vec3 &pos);
		void SetVelocity(const Vec3 &vel);

		//Message functions
		void OnPositionChanged(TransformationNotifyMessagePtr message);
		void OnPhysicsUpdate(VelocityNotifyMessagePtr message);
		void OnParameterMessage(SoundParameterMessagePtr message);

		ALvoid DisplayALError(ALchar *szText, ALint errorcode);

		//void ImplementRelease();
		//void ImplementStart();
		//void testSound( const char* wavFile );	 // for debug

		//
		ALuint		m_Buffer;
		// Source for current sound, allocated when sample is to be played, 0 otherwise
		ALuint m_Source;

		std::string m_Filename;
		float m_Pitch;
		int m_Frequency;

		float m_MinDistance;
		float m_MaxDistance;
		float m_Rolloff;
		int m_Stereo;
		bool m_Loop;
		//int m_Priority;
		float m_Volume;
	};
}
#endif
