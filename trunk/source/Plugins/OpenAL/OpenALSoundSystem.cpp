#include "OpenALSoundSystem.h"
#include "Core/Utils/Log.h"
#include "Core/System/SystemFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Math/Matrix.h"
#include "Sim/Systems/Resource/IResourceSystem.h"
#include "Sim/Scenario/Scenario.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Scenario/Scene/SceneObject.h"


#include "Sim/Systems/SimSystemManager.h"
#include "Sim/SimEngine.h"


namespace GASS
{
	OpenALSoundSystem::OpenALSoundSystem()
	{
		m_Context = NULL;
		m_Device = NULL;
		m_IsInitialised = false;

	}

	OpenALSoundSystem::~OpenALSoundSystem()
	{

	}

	void OpenALSoundSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("OpenALSoundSystem",new GASS::Creator<OpenALSoundSystem, ISystem>);
	}

	void OpenALSoundSystem::OnCreate()
	{
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OpenALSoundSystem::OnInit,InitMessage,0));
		//catch camera change messages to update openal listener
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OpenALSoundSystem::OnSceneLoaded,ScenarioSceneAboutToLoadNotifyMessage,0));
	}

	void OpenALSoundSystem::OnSceneLoaded(ScenarioSceneAboutToLoadNotifyMessagePtr message)
	{
		m_Scene = message->GetScenarioScene();
		if(message->GetScenarioScene())
			message->GetScenarioScene()->RegisterForMessage(REG_TMESS(OpenALSoundSystem::OnChangeCamera,CameraChangedNotifyMessage,0));
	}

	void OpenALSoundSystem::OnChangeCamera(CameraChangedNotifyMessagePtr message)
	{
		SceneObjectPtr cam_obj = message->GetCamera();
		SceneObjectPtr current_cam_obj(m_CurrentCamera,boost::detail::sp_nothrow_tag());
		if(current_cam_obj)
		{
			current_cam_obj->UnregisterForMessage(typeid(TransformationNotifyMessage), TYPED_MESSAGE_FUNC(OpenALSoundSystem::OnCameraMoved,TransformationNotifyMessage));
		}
		cam_obj->RegisterForMessage(REG_TMESS(OpenALSoundSystem::OnCameraMoved,TransformationNotifyMessage,0));
		m_CurrentCamera = cam_obj;
	}

	void OpenALSoundSystem::OnCameraMoved(TransformationNotifyMessagePtr message)
	{
		Vec3 pos = message->GetPosition();
		Quaternion rot = message->GetRotation();
		Vec3 vel = Vec3(0,0,0);
		UpdateListener(pos,rot,vel);
	}

	void OpenALSoundSystem::OnInit(MessagePtr message)
	{
		if ( m_IsInitialised ) return;

		// Open an audio device
		m_Device = alcOpenDevice( NULL ); // TODO ((ALubyte*) "DirectSound3D");
		// mSoundDevice = alcOpenDevice( "DirectSound3D" );

		// Check for errors
		if ( !m_Device )
		{
			Log::Error("SoundManager::init error : No sound device.");
			return;
		}

		m_Context = alcCreateContext( m_Device, NULL );
		//   if ( CheckAlError() || !mSoundContext ) // TODO seems not to work! why ?
		if ( !m_Context )
		{
			Log::Error( "OpenALSoundSystem::Init error : No sound context.");
			return;
		}

		// Make the context current and active
		alcMakeContextCurrent( m_Context );
		if ( CheckAlError( "Init()" ) )
		{
			Log::Error( "SoundManager::init error : could not make sound context current and active.");
			return;
		}

		// Check for EAX 2.0 support and
		// Retrieves function entry addresses to API ARB extensions, in this case,
		// for the EAX extension. See Appendix 1 (Extensions) of
		// http://www.openal.org/openal_webstf/specs/OpenAL1-1Spec_html/al11spec7.html
		//
		// TODO EAX fct not used anywhere in the code ... !!!
		/*	isEAXPresent = alIsExtensionPresent( "EAX2.0" );
		if ( isEAXPresent )
		{
		printf( "EAX 2.0 Extension available\n" );

		#ifdef _USEEAX
		eaxSet = (EAXSet) alGetProcAddress( "EAXSet" );
		if ( eaxSet == NULL )
		isEAXPresent = false;

		eaxGet = (EAXGet) alGetProcAddress( "EAXGet" );
		if ( eaxGet == NULL )
		isEAXPresent = false;

		if ( !isEAXPresent )
		CheckAlError( "Failed to get the EAX extension functions adresses.\n" );
		#else
		isEAXPresent = false;
		printf( "... but not used.\n" );
		#endif // _USEEAX

		}
		*/

		// Gain
		alListenerf( AL_GAIN, 1.0 );

		// Initialise Doppler
		alDopplerFactor( 1.0 ); // 1.2 = exaggerate the pitch shift by 20%
		alDopplerVelocity( 343.0f ); // m/s this may need to be scaled at some point
		alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);

		// Ok
		m_IsInitialised = true;

		Log::Print("GASSSoundOpenOpenAL initialised.");
		return;
	}


	bool OpenALSoundSystem::CheckAlError( void )
	{
		ALenum errCode;
		if ( ( errCode = alGetError() ) != AL_NO_ERROR )
		{
			std::string err = "ERROR SoundManager:: ";
			err += (char*) alGetString( errCode );

			Log::Error( "%s", err.c_str());
			return true;
		}
		return false;
	}

	void OpenALSoundSystem::UpdateListener(const Vec3 &pos, const Quaternion &rot, const Vec3 &vel)
	{

		// Position of the listener.
		ALfloat ListenerPos[] = { pos.x, pos.y, pos.z};
		// Velocity of the listener.
		ALfloat ListenerVel[] = { vel.x,vel.y,vel.z};
		// Orientation of the listener. (first 3 elements are "dir", second 3 are "up")

		Mat4 rot_mat;
		rot_mat.Identity();
		rot.ToRotationMatrix(rot_mat);
		Vec3 dir = -rot_mat.GetViewDirVector();
		Vec3 up = rot_mat.GetUpVector();

		ALfloat ListenerOri[] = { dir.x, dir.y, dir.z,  up.x, up.y, up.z};

		alListenerfv(AL_POSITION,    ListenerPos);
		alListenerfv(AL_VELOCITY,    ListenerVel);
		alListenerfv(AL_ORIENTATION, ListenerOri);
	}

	bool OpenALSoundSystem::CheckAlError( std::string pMsg )
	{
		ALenum error = 0;

		if ( (error = alGetError()) == AL_NO_ERROR )
			return false;

		switch ( error )
		{
		case AL_INVALID_NAME:
			Log::Error("ERROR SoundManager::%s Invalid Name", pMsg.c_str());
			break;
		case AL_INVALID_ENUM:
			Log::Error("ERROR SoundManager::%s Invalid Enum", pMsg.c_str());
			break;
		case AL_INVALID_VALUE:
			Log::Error("ERROR SoundManager::%s Invalid Value", pMsg.c_str());
			break;
		case AL_INVALID_OPERATION:
			Log::Error("ERROR SoundManager::%s Invalid Operation", pMsg.c_str());
			break;
		case AL_OUT_OF_MEMORY:
			Log::Error("ERROR SoundManager::%s Out Of Memory", pMsg.c_str());
			break;
		default:
			Log::Error("ERROR SoundManager::%s Unknown error (%i) case in testALError()", pMsg.c_str(), error);
			break;
		};

		return true;
	}


	bool OpenALSoundSystem::LoadWaveSound(const std::string &filePath,ALuint &buffer,ALsizei &freq, ALenum &format)
	{
		ALsizei		size;		//the bit depth
		ALboolean	loop;
		ALvoid*		data=NULL;
		std::string full_path = "";

		IResourceSystem* rs = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<IResourceSystem>().get();
		SoundMap::iterator pos;
		pos = m_BufferMap.find(filePath);
		if (pos != m_BufferMap.end())
		{
			buffer = m_BufferMap[filePath];
		}
		else
		{
			if (m_BufferMap.size() >= MAX_AUDIO_BUFFERS)
			{
				Log::Warning("OpenALSoundComponent::LoadWaveSound() - All sound buffers are in use!");
				return false;
			}
			if(rs->GetFullPath(filePath,full_path))
			{
				// We have more buffers free, generate a new one
				alGenBuffers(1, &buffer);

				alGetError();   // Clear Error Code

				alutLoadWAVFile( (ALbyte*)full_path.c_str(), &format, &data, &size, &freq, &loop);

				if ( CheckAlError("loadWAV::alutLoadWAVFile: ") )
					return false;

				// Copy the new WAV data into the buffer
				alBufferData(buffer, format, data, size, freq);
				/*
				AL_INVALID_VALUE :
				The size parameter is not valid for the format
				specified, the buffer is in use, or the data is a
				NULL pointer.
				*/
				if (CheckAlError("loadWAV::alBufferData: ") )
					return false;

				// Unload the WAV file
				alutUnloadWAV(format, data, size, freq);
				if (CheckAlError("loadWAV::alutUnloadWAV: ") )
					return false;
				if(buffer) 
					m_BufferMap[filePath] = buffer;

				//m_BaseFreq = freq;
			}
			else
			{
				return false;
			}
		}
		return true;
	}

	std::string OpenALSoundSystem::ListAvailableDevices( void )
	{
		std::string str = "Sound Devices available : ";

		if ( alcIsExtensionPresent( NULL, "ALC_ENUMERATION_EXT" ) == AL_TRUE )
		{
			str = "List of Devices : ";
			str += (char*) alcGetString( NULL, ALC_DEVICE_SPECIFIER );
			str += "\n";
		}
		else
			str += " ... eunmeration error.\n";

		return str;
	}

} 