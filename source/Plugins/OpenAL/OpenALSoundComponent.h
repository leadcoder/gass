#ifndef OPEN_AL_SOUND_COMPONENT_HH
#define OPEN_AL_SOUND_COMPONENT_HH


#include "Sim/GASSCommon.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSSoundSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/Math/GASSVector.h"
#ifdef WIN32
	#include "al.h"
	//#include "alc.h"
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
		~OpenALSoundComponent(void) override;
		static void RegisterReflection();

		void OnInitialize() override;
		void OnDelete() override;
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
		bool GetLoop() const;
		void SetLoop(bool loop);
		void SetPitch(float pitch);
		float GetPitch() const;
		void SetRolloff(float rolloff);
		float GetRolloff() const;
		ResourceHandle GetSoundFile() const;
		void SetSoundFile(const ResourceHandle &file);

		void SetPosition(const Vec3 &pos);
		void SetVelocity(const Vec3 &vel);

		//Message functions
		void OnPositionChanged(TransformationChangedEventPtr message);
		void OnPhysicsUpdate(PhysicsVelocityEventPtr message);
		void OnParameterMessage(SoundParameterRequestPtr message);
		
		ALuint m_Buffer;
		ALuint m_Source; // Source for current sound, allocated when sample is to be played, 0 otherwise
		ResourceHandle m_SoundResource;
		float m_Pitch;
		//int m_Frequency;
		float m_MinDistance;
		float m_MaxDistance;
		float m_Rolloff;
		bool m_Loop;
		float m_Volume;
	};
}
#endif
