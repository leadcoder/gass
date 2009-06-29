#ifndef OPEN_AL_SOUND_COMPONENT_HH
#define OPEN_AL_SOUND_COMPONENT_HH


#include "Sim/Common.h"
#include "Sim/Components/Graphics/ILightComponent.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/IComponent.h"
#include "Core/MessageSystem/Message.h"
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

		virtual void Update(float delta);
		virtual void Play();
		virtual void Stop();
		virtual void StopLooping();

		//virtual void ScaleVolume(float scaleVal);
		//virtual void ScaleFrequency(float scaleVal);
		virtual bool IsPlaying();
		bool LoadWaveSound(const std::string &filePath);//, int mode);
	protected:
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

		void SetPosition(const Vec3 &pos);
		void SetVelocity(const Vec3 &vel);

		void OnPositionChanged(MessagePtr message);
		void OnPhysicsUpdate(MessagePtr message);
	
	
		void SetSoundFile(const std::string &file);
	

		void OnLoad(MessagePtr message);

		ALvoid DisplayALError(ALchar *szText, ALint errorcode);
		/*ALboolean LoadWave(char *szWaveFile, ALuint BufferID);
		ALvoid getWAVData(const ALbyte *file,ALenum *format,ALvoid **data,ALsizei *size,ALsizei *freq, ALboolean *loop);
		ALvoid unloadWAVData(ALvoid *data);*/
		//bool loadWAV( std::string filename, ALuint pDestAudioBuffer );

		void ImplementRelease();
		void ImplementStart();
		void testSound( const char* wavFile );	 // for debug

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
		int m_Priority; 
		float m_Volume;

		int m_3D;
	
	/*	float m_StartTime;
		float m_StopTime;
	
		int m_Trigger;
		std::string m_TriggerName;
		std::string m_StopString;
		int m_RandomPlay;
		Vec3 m_RelativePosition; 
		float m_EffectVolume;
		float m_BaseFreq;
		int m_DopplerOff;
		bool m_Start;
		bool m_Release;
		
		float m_RandPitchVal1;
		float m_RandPitchVal2;
		bool m_FirstRelease;*/
	};

} // namespace HiFi
#endif //OpenALSoundComponent_HH
