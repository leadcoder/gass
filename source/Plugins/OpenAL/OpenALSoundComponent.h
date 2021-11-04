#ifndef OPEN_AL_SOUND_COMPONENT_HH
#define OPEN_AL_SOUND_COMPONENT_HH


#include "Sim/GASSCommon.h"
#include "Sim/GASSComponent.h"
#include "Sim/Messages/GASSSoundSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/GASSComponentFactory.h"
#include "Sim/Interface/GASSISoundComponent.h"
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
	class OpenALSoundComponent : public Reflection<OpenALSoundComponent,ISoundComponent>
	{
	public:
		OpenALSoundComponent(void);
		~OpenALSoundComponent(void) override;
		static void RegisterReflection();

		void OnInitialize() override;
		void OnDelete() override;
		bool GetPlay() const override;
		void SetPlay(bool value) override;
		float GetVolume() const override;
		void SetVolume(float volume) override;
		bool GetLoop() const override;
		void SetLoop(bool loop) override;
		void SetPitch(float pitch) override;
		float GetPitch() const override;

		float GetMinDistance() const override;
		void SetMinDistance(float min_dist) override;
		float GetMaxDistance() const override;
		void SetMaxDistance(float max_dist) override;

		void SetRolloff(float rolloff) override;
		float GetRolloff() const override;
		
	protected:
		bool IsPlaying() const;
		void Play();
		void Stop();

		//Helper to load sound from wave-file
		void LoadWaveSound(const std::string &filePath);
		ResourceHandle GetSoundFile() const;
		void SetSoundFile(const ResourceHandle& file);
		void SetPosition(const Vec3& pos);
		void SetVelocity(const Vec3& vel);

		//Message functions
		void OnPositionChanged(TransformationChangedEventPtr message);
		void OnPhysicsUpdate(PhysicsVelocityEventPtr message);
		
		ALuint m_Buffer{0};
		ALuint m_Source{0}; // Source for current sound, allocated when sample is to be played, 0 otherwise
		ResourceHandle m_SoundResource;
		float m_Pitch{1};
		float m_MinDistance{1};
		float m_MaxDistance{200};
		float m_Rolloff{1};
		bool m_Loop{false};
		bool m_Play{ false };
		float m_Volume{1};
	};
}
#endif
