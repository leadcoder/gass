#ifndef OPEN_AL_SOUND_COMPONENT_HH
#define OPEN_AL_SOUND_COMPONENT_HH


#include "Sim/Common.h"
#include "Sim/Components/Graphics/ILightComponent.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/IComponent.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Math/Vector.h"
#include "al.h"
#include "alc.h"
#include "alut.h"

namespace GASS
{
	class OpenALSoundComponent : public Reflection<OpenALSoundComponent,BaseSceneComponent>
	{
	public:
		OpenALSoundComponent(void);
		virtual ~OpenALSoundComponent(void);

		static void RegisterReflection();
		virtual void OnCreate();

		//create sound interface for this?
		virtual void Update(float delta);
		virtual void Play();
		virtual void Stop();
		virtual void StopLooping();
		virtual bool IsPlaying();

		
	protected:

		//Helper to load sound from wave-file
		bool LoadWaveSound(const std::string &filePath);

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
		void OnLoad(LoadGFXComponentsMessagePtr message);
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
