#include "OpenALSoundSystem.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/System/GASSSystemFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSMatrix.h"
#include "Core/Utils/GASSException.h"

#include "Sim/Interface/GASSICameraComponent.h"
#include "Sim/Interface/GASSIViewport.h"

#include "Sim/GASSScene.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSBaseSceneComponent.h"

#include "Framework/Framework.h"


namespace GASS
{
	OpenALSoundSystem::OpenALSoundSystem() : m_Context(NULL),
		m_Device(NULL),
		m_IsInitialised(false)
	{

	}

	OpenALSoundSystem::~OpenALSoundSystem()
	{
		ALFWShutdown();
		ALCcontext *pContext;
		ALCdevice *pDevice;
		pContext = alcGetCurrentContext();
		pDevice = alcGetContextsDevice(pContext);
		alcMakeContextCurrent(NULL);
		alcDestroyContext(pContext);
		alcCloseDevice(pDevice);
	}

	void OpenALSoundSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("OpenALSoundSystem",new GASS::Creator<OpenALSoundSystem, ISystem>);
	}

	void OpenALSoundSystem::Init()
	{
		if ( m_IsInitialised)
			return;

		SimEngine::Get().GetRuntimeController()->Register(shared_from_this(),m_TaskNodeName);
		//catch camera change messages to update openal listener
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OpenALSoundSystem::OnSceneLoaded,PreSceneCreateEvent,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OpenALSoundSystem::OnCameraChanged,CameraChangedEvent,0));

		// Open an audio device
		m_Device = alcOpenDevice( NULL ); // TODO ((ALubyte*) "DirectSound3D");
		// mSoundDevice = alcOpenDevice( "DirectSound3D" );

		// Check for errors
		if ( !m_Device )
		{
			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"No sound device", "SoundManager::Init");
		}

		m_Context = alcCreateContext( m_Device, NULL );
		//   if ( CheckAlError() || !mSoundContext ) // TODO seems not to work! why ?

		if ( !m_Context )
		{
			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"No sound context", "SoundManager::Init");
		}

		// Make the context current and active
		alcMakeContextCurrent( m_Context );
		CheckAlError("OpenALSoundSystem::Init()");

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


		ALFWInit();
		// Ok
		m_IsInitialised = true;

		LogManager::getSingleton().stream() << "GASSSoundOpenOpenAL initialised";
	}



	void OpenALSoundSystem::OnSceneLoaded(PreSceneCreateEventPtr message)
	{
		
	}

	void OpenALSoundSystem::OnCameraChanged(CameraChangedEventPtr message)
	{
		CameraComponentPtr camera = message->GetViewport()->GetCamera();
		SceneObjectPtr cam_obj = DYNAMIC_PTR_CAST<BaseSceneComponent>(camera)->GetSceneObject();

		SceneObjectPtr current_cam_obj(m_CurrentCamera,NO_THROW);
		if(current_cam_obj)
		{
			current_cam_obj->UnregisterForMessage(typeid(TransformationChangedEvent), TYPED_MESSAGE_FUNC(OpenALSoundSystem::OnCameraMoved,TransformationChangedEvent));
		}
		cam_obj->RegisterForMessage(REG_TMESS(OpenALSoundSystem::OnCameraMoved,TransformationChangedEvent,0));
		m_CurrentCamera = cam_obj;
	}

	void OpenALSoundSystem::OnCameraMoved(TransformationChangedEventPtr message)
	{
		Vec3 pos = message->GetPosition();
		Quaternion rot = message->GetRotation();
		Vec3 vel = Vec3(0,0,0);
		UpdateListener(pos,rot,vel);
	}

	void OpenALSoundSystem::CheckAlError(const std::string &what_class)
	{
		ALenum errCode;
		if ( ( errCode = alGetError() ) != AL_NO_ERROR )
		{
			std::string error = (char*) alGetString( errCode );
			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"OpenAL error:" + error, what_class);
		}
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

	void OpenALSoundSystem::LoadWaveSound(const std::string &filePath,ALuint &buffer,ALsizei &freq, ALenum &format)
	{
		ALvoid*		data=NULL;
	
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
				GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"All sound buffers are in use","OpenALSoundComponent::LoadWaveSound()");
			}
			// We have more buffers free, generate a new one
			alGenBuffers(1, &buffer);
			CheckAlError("OpenALSoundSystem::LoadWaveSound()");

			//ALFWLoadWaveToBuffer (ALbyte*)full_path.c_str(), &format, &data, &size, &freq, &loop);
			ALboolean ret = ALFWLoadWaveToBuffer( (ALbyte*)filePath.c_str(),buffer);
			if(ret == AL_FALSE)
			{
				GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"Failed to load wave file:" + filePath ,"OpenALSoundComponent::LoadWaveSound()");
			}
			CheckAlError("OpenALSoundSystem::LoadWaveSound()");

			m_BufferMap[filePath] = buffer;
		}
		
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