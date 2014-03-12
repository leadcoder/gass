#include "OpenALSoundComponent.h"
#include "OpenALSoundSystem.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Utils/GASSException.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Framework/Framework.h"

namespace GASS
{
	OpenALSoundComponent::OpenALSoundComponent(): m_Buffer(0),
		m_Source(0),
		m_Volume(1),
		m_Pitch(1),
		m_Loop(false),
		m_MinDistance(1), //meters
		m_MaxDistance(200),//meters
		m_Rolloff(1)
	{
		

	}

	OpenALSoundComponent::~OpenALSoundComponent(void)
	{
		
	}

	void OpenALSoundComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("OpenALSoundComponent",new Creator<OpenALSoundComponent, IComponent>);
		RegisterProperty<float>("MinDistance", &OpenALSoundComponent::GetMinDistance, &OpenALSoundComponent::SetMinDistance);
		RegisterProperty<float>("MaxDistance", &OpenALSoundComponent::GetMaxDistance, &OpenALSoundComponent::SetMaxDistance);
		RegisterProperty<float>("RolloffFactor", &OpenALSoundComponent::GetRolloff, &OpenALSoundComponent::SetRolloff);
		RegisterProperty<float>("Volume", &OpenALSoundComponent::GetVolume, &OpenALSoundComponent::SetVolume);
		RegisterProperty<bool>("Loop", &OpenALSoundComponent::GetLoop, &OpenALSoundComponent::SetLoop);
		RegisterProperty<ResourceHandle>("SoundFile", &OpenALSoundComponent::GetSoundFile, &OpenALSoundComponent::SetSoundFile);
	}

	void OpenALSoundComponent::OnInitialize()
	{
		
		GetSceneObject()->RegisterForMessage(REG_TMESS(OpenALSoundComponent::OnPositionChanged, TransformationChangedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OpenALSoundComponent::OnPhysicsUpdate,PhysicsVelocityEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OpenALSoundComponent::OnParameterMessage,SoundParameterRequest,0));

		LoadWaveSound(m_SoundResource.GetResource()->Path().GetFullPath());//, 0);
		//sound loaded, update sound settings
		SetLoop(m_Loop);
		SetMaxDistance(m_MaxDistance);
		SetMinDistance(m_MinDistance);
		SetRolloff(m_Rolloff);
		//Play();
	
	}

	void OpenALSoundComponent::OnDelete()
	{
		if(m_Source)
			alDeleteSources(1, &m_Source);
	}

	void OpenALSoundComponent::OnPositionChanged(TransformationChangedEventPtr message)
	{
		Vec3 pos = message->GetPosition();
		SetPosition(pos);
	}

	void OpenALSoundComponent::OnPhysicsUpdate(PhysicsVelocityEventPtr message)
	{
		Vec3 vel = message->GetLinearVelocity();
		SetVelocity(vel);
	}

	void OpenALSoundComponent::OnParameterMessage(SoundParameterRequestPtr message)
	{
		//LogManager::getSingleton().stream() << "WARNING:OpenALSoundComponent::OnParameterMessage");
		SoundParameterRequest::SoundParameterType type = message->GetParameter();
		switch(type)
		{
		case SoundParameterRequest::PLAY:
			{
				//LogManager::getSingleton().stream() << "WARNING:OpenALSoundComponent::Play() - play!");
				Play();
			}
			break;
		case SoundParameterRequest::STOP:
			{
				Stop();
			}
			break;
		case SoundParameterRequest::PITCH:
			{
				float value = message->GetValue();
				//float pitch = GetPitch();
				SetPitch(value);
			}
			break;
		case SoundParameterRequest::VOLUME:
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
		return m_Volume;
	}

	void OpenALSoundComponent::SetVolume(float volume)
	{
		if(volume <  0)
		{
			LogManager::getSingleton().stream() << "WARNING:Invalid volume " << volume;
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
			LogManager::getSingleton().stream() << "WARNING:Invalid pitch value:" << pitch;
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

	ResourceHandle OpenALSoundComponent::GetSoundFile() const
	{
		return m_SoundResource;
	}

	void OpenALSoundComponent::SetSoundFile(const ResourceHandle &file)
	{
		m_SoundResource = file;
	}

	


	void OpenALSoundComponent::Play()
	{
		//already playing?
		if(IsPlaying())
			return;

		if (m_Source == 0)
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Play called without m_Source set","OpenALSoundComponent::Play()");
			//LogManager::getSingleton().stream() << "WARNING:OpenALSoundComponent::Play() called without m_Source set";
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
		ALint iVal;
		alGetError();

		if (m_Source == 0)
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"IsPlaying called without m_Source set","OpenALSoundComponent::IsPlaying()");
		}

		alGetSourcei(m_Source, AL_SOURCE_STATE, &iVal);
		OpenALSoundSystem::CheckAlError("OpenALSoundComponent::IsPlaying");
		return (iVal == AL_PLAYING);
	}

	void OpenALSoundComponent::LoadWaveSound(const std::string &filePath)
	{
		OpenALSoundSystem* ss = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<OpenALSoundSystem>().get();
		if(!ss)
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"OpenALSoundSystem is missing, you must add OpenALSoundSystem to SimSystemManager before loading OpenALSoundComponents","OpenALSoundComponent::LoadWaveSound");
		}
		ALenum      format;// = (sc->GetStereo() == 0  ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16	);     //for the buffer format
		ALsizei		freq;		//for the frequency of the buffer
		//ALboolean	loop;// = (sc->GetLoop() == 0 ? AL_FALSE : AL_TRUE);         //looped
		ss->LoadWaveSound(filePath,m_Buffer,freq,format);

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
