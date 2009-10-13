#include "OpenALSoundComponent.h"
#include "OpenALSoundSystem.h"
#include "Core/Utils/Log.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"


namespace GASS
{
	
	OpenALSoundComponent::OpenALSoundComponent()
	{
		m_Buffer = 0;
		m_Source = 0;
		m_Volume = 1;
		m_Pitch = 1;
		m_Loop = false;
		m_Stereo = 0;
		m_MinDistance = 1; //meter
		m_MaxDistance = 200;//meter
		m_Rolloff = 1;

	}

	OpenALSoundComponent::~OpenALSoundComponent(void)
	{
		if(m_Source)
			alDeleteSources(1, &m_Source);
	}

	ALvoid OpenALSoundComponent::DisplayALError(ALchar *szText, ALint errorcode)
	{
		Log::Warning("%s%s",szText,alGetString(errorcode));
		return;
	}
	
	void OpenALSoundComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("OpenALSoundComponent",new Creator<OpenALSoundComponent, IComponent>);
		RegisterProperty<float>("MinDistance", &OpenALSoundComponent::GetMinDistance, &OpenALSoundComponent::SetMinDistance);
		RegisterProperty<float>("MaxDistance", &OpenALSoundComponent::GetMaxDistance, &OpenALSoundComponent::SetMaxDistance);
		RegisterProperty<float>("RolloffFactor", &OpenALSoundComponent::GetRolloff, &OpenALSoundComponent::SetRolloff);
		RegisterProperty<float>("Volume", &OpenALSoundComponent::GetVolume, &OpenALSoundComponent::SetVolume);
		//RegisterProperty<bool>("Stereo", &OpenALSoundComponent::GetStereo, &OpenALSoundComponent::SetStereo);
		RegisterProperty<bool>("Loop", &OpenALSoundComponent::GetLoop, &OpenALSoundComponent::SetLoop);
		RegisterProperty<std::string>("SoundFile", &OpenALSoundComponent::GetSoundFile, &OpenALSoundComponent::SetSoundFile);
	}

	void OpenALSoundComponent::OnCreate()
	{
		REGISTER_OBJECT_MESSAGE_CLASS(OpenALSoundComponent::OnLoad,LoadGFXComponentsMessage,1);
		REGISTER_OBJECT_MESSAGE_CLASS(OpenALSoundComponent::OnPositionChanged, TransformationNotifyMessage,0);
		REGISTER_OBJECT_MESSAGE_CLASS(OpenALSoundComponent::OnPhysicsUpdate,VelocityNotifyMessage,0);
		REGISTER_OBJECT_MESSAGE_CLASS(OpenALSoundComponent::OnParameterMessage,SoundParameterMessage,0);
	}

	void OpenALSoundComponent::OnPositionChanged(TransformationNotifyMessagePtr message)
	{
		Vec3 pos = message->GetPosition();
		SetPosition(pos);
	}

	void OpenALSoundComponent::OnPhysicsUpdate(VelocityNotifyMessagePtr message)
	{
		Vec3 vel = message->GetLinearVelocity();
		SetVelocity(vel);
	}

	void OpenALSoundComponent::OnParameterMessage(SoundParameterMessagePtr message)
	{
		//Log::Warning("OpenALSoundComponent::OnParameterMessage");
		SoundParameterMessage::SoundParameterType type = message->GetParameter();
		switch(type)
		{
		case SoundParameterMessage::PLAY:
			{
				//Log::Warning("OpenALSoundComponent::Play() - play!");
				Play();
			}
			break;
		case SoundParameterMessage::STOP:
			{
				Stop();
			}
			break;
		case SoundParameterMessage::PITCH:
			{
				float value = message->GetValue();
				//float pitch = GetPitch();
				SetPitch(value);
			}
			break;
		case SoundParameterMessage::VOLUME:
			{
				float value = message->GetValue();
				//float pitch = GetPitch();
				SetVolume(value);
			}
			break;
		}
	}

	float OpenALSoundComponent::GetMinDistance() const
	{
		return m_MinDistance;
	}

	void OpenALSoundComponent::SetMinDistance(float min_dist) 
	{
		m_MinDistance = min_dist;

		if (m_Source) 
		{
			alSourcef(m_Source,AL_REFERENCE_DISTANCE,min_dist); 
		}
	}

	float OpenALSoundComponent::GetRolloff() const
	{
		return m_Rolloff;
	}

	void OpenALSoundComponent::SetRolloff(float rolloff) 
	{
		m_Rolloff = rolloff;

		if (m_Source) 
		{
			alSourcef(m_Source,AL_ROLLOFF_FACTOR,rolloff); 
		}
	}

	float OpenALSoundComponent::GetMaxDistance() const
	{
		return m_MaxDistance;
	}

	void OpenALSoundComponent::SetMaxDistance(float max_dist) 
	{
		m_MaxDistance = max_dist;

		if (m_Source) 
		{
			alSourcef(m_Source,AL_MAX_DISTANCE,max_dist); 
		}
	}

	float OpenALSoundComponent::GetVolume() const
	{
		return m_MinDistance;
	}

	void OpenALSoundComponent::SetVolume(float volume) 
	{
		if(volume <  0)
		{
			Log::Warning("Invalid volume %.3f",volume);
			return;
		}
		m_Volume = volume;
		if (m_Source) 
		{
			alSourcef(m_Source,AL_GAIN,m_Volume); 
		}
	}

	void OpenALSoundComponent::SetPitch(float pitch) 
	{
		if(pitch <=  0)
		{
			Log::Warning("Invalid pitch %.3f",pitch);
			return;
		}
		m_Pitch = pitch;
		if (m_Source) 
		{
			alSourcef(m_Source,AL_PITCH,pitch);
		}
	}

	float OpenALSoundComponent::GetPitch() const
	{
		return m_Pitch;
	}
		
	bool OpenALSoundComponent::GetStereo() const
	{
		return m_Stereo;
	}

	void OpenALSoundComponent::SetStereo(bool stereo) 
	{
		m_Stereo = stereo;
	}

	
	bool OpenALSoundComponent::GetLoop() const
	{
		return m_Loop;
	}

	void OpenALSoundComponent::SetLoop(bool loop) 
	{
		m_Loop = loop;

		if(m_Source)
		{
			if(m_Loop)
			{
				alSourcei(m_Source, AL_LOOPING, 1);
			}
			else
				alSourcei(m_Source, AL_LOOPING, 0);
		}

	}

	std::string OpenALSoundComponent::GetSoundFile() const
	{
		return m_Filename;
	}

	void OpenALSoundComponent::SetSoundFile(const std::string &file) 
	{
		m_Filename = file;
	}

	void OpenALSoundComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{
		bool wasSoundLoaded  = LoadWaveSound(m_Filename);//, 0);
		//sound loaded, update sound settings
		SetLoop(m_Loop); 
		SetMaxDistance(m_MaxDistance); 
		SetMinDistance(m_MinDistance); 
		SetRolloff(m_Rolloff); 
		//Play();
	}

	void OpenALSoundComponent::Update(float delta)
	{
		if(!m_Source) return;
	}

	void OpenALSoundComponent::Play()
	{
		//already playing?
		if(IsPlaying())
			return;

		if (m_Source == 0)
		{
			Log::Warning("OpenALSoundComponent::Play() called without m_Source set");
			return;
		}

		alSourcePlay(m_Source);
	}

	void OpenALSoundComponent::SetPosition(const Vec3 &pos)
	{
		ALfloat sourcePos[] = { pos.x, pos.y, pos.z};
		if(m_Source)
			alSourcefv(m_Source, AL_POSITION, sourcePos);
	}

	void OpenALSoundComponent::SetVelocity(const Vec3 &vel)
	{
		ALfloat sourceVel[] = { vel.x, vel.y, vel.z};
		if(m_Source)
			alSourcefv(m_Source, AL_VELOCITY, sourceVel);

	}

	
	bool OpenALSoundComponent::IsPlaying()
	{
		ALint	error;
		ALint iVal;
		alGetError();

		if (m_Source == 0)
		{
			Log::Error("OpenALSoundComponent::SetFrequency() called without m_Source set");
		}

		alGetSourcei(m_Source, AL_SOURCE_STATE, &iVal);
		if ((error = alGetError()) != AL_NO_ERROR)
			DisplayALError((ALbyte *) "OpenALSoundComponent::IsPlaying(): : ", error);

		return (iVal == AL_PLAYING);
	}

	bool OpenALSoundComponent::LoadWaveSound(const std::string &filePath)
	{
		OpenALSoundSystem* ss = SimEngine::GetPtr()->GetSystemManager()->GetFirstSystem<OpenALSoundSystem>().get();
		if(!ss)
		{
			Log::Error("OpenALSoundSystem is missing, you must add OpenALSoundSystem to SimSystemManager before loading OpenALSoundComponents");
			return false;
		}
		ALenum      format;// = (sc->GetStereo() == 0  ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16	);     //for the buffer format
		ALsizei		freq;		//for the frequency of the buffer
		//ALboolean	loop;// = (sc->GetLoop() == 0 ? AL_FALSE : AL_TRUE);         //looped

		if(ss->LoadWaveSound(filePath,m_Buffer,freq,format))
		{
			alGenSources( 1, &m_Source);
			//if ( sound_man->CheckAlError( "init::alGenSources :") )
			//	return false;
			alSourcei( m_Source, AL_BUFFER, m_Buffer );
			//if ( sound_man->CheckAlError( "init::alGenSources :") )
			//	return false;
			alSourcei( m_Source, AL_LOOPING, m_Loop);
			//if( CheckAlError( "loadSource()::alSourcei" ) )
			//	return false;
		}

		else
		{
			Log::Warning("OpenALSoundComponent::LoadWaveSound() - Failed to load sound buffer!");
			return false;
		}
		return true;
	}

	void OpenALSoundComponent::StopLooping()
	{
		if (m_Source == 0) return;
			alSourcei( m_Source, AL_LOOPING, 0);
	}

	void OpenALSoundComponent::Stop()
	{
		if (m_Source == 0) return;
		alSourceStop(m_Source);
	}
} 
