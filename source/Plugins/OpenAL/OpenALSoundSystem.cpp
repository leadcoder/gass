#include "OpenALSoundSystem.h"
#include "Sim/GASSSystemFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSMatrix.h"
#include "Core/Utils/GASSException.h"
#include "Sim/Interface/GASSIViewport.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSComponent.h"
#include "Sim/Interface/GASSICameraComponent.h"
#include "Core/Utils/GASSLogger.h"

#ifdef WIN32
	#include "Framework/Framework.h"
#else
	#include <alut.h>
#endif

namespace GASS
{
	OpenALSoundSystem::OpenALSoundSystem(SimSystemManagerWeakPtr manager) : Reflection(manager) , m_Context(nullptr),
		m_Device(nullptr),
		m_IsInitialised(false)
	{
		m_UpdateGroup = UGID_SIM;
	}

	OpenALSoundSystem::~OpenALSoundSystem()
	{
#ifdef WIN32
		ALFWShutdown();
#endif
		ALCcontext *p_context;
		ALCdevice *p_device;
		p_context = alcGetCurrentContext();
		p_device = alcGetContextsDevice(p_context);
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(p_context);
		alcCloseDevice(p_device);
	}

	void OpenALSoundSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register<OpenALSoundSystem>("OpenALSoundSystem");
	}

	void OpenALSoundSystem::OnSystemInit()
	{
		if ( m_IsInitialised)
			return;

		//catch camera change messages to update openal listener
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OpenALSoundSystem::OnCameraChanged,CameraChangedEvent,0));

		// Open an audio device
		m_Device = alcOpenDevice( nullptr ); // TODO ((ALubyte*) "DirectSound3D");
		// mSoundDevice = alcOpenDevice( "DirectSound3D" );

		// Check for errors
		if ( !m_Device )
		{
			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"No sound device", "SoundManager::Init");
		}

		m_Context = alcCreateContext( m_Device, nullptr );
		//   if ( CheckAlError() || !mSoundContext ) // TODO seems not to work! why ?

		if ( !m_Context )
		{
			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"No sound context", "SoundManager::Init");
		}

		// Make the context current and active
		alcMakeContextCurrent( m_Context );
		CheckAlError("OpenALSoundSystem::OnSystemInit()");

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

#ifdef WIN32
		ALFWInit();
#else
		alutInitWithoutContext(NULL,NULL);
#endif
		// Ok
		m_IsInitialised = true;
		GASS_LOG(LINFO) << "GASSSoundOpenOpenAL initialized";
	}

	void OpenALSoundSystem::OnCameraChanged(CameraChangedEventPtr message)
	{
		CameraComponentPtr camera = message->GetViewport()->GetCamera();
		auto cam_obj = camera->GetSceneObject();

		SceneObjectPtr current_cam_obj = m_CurrentCamera.lock();
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
		ALenum err_code;
		if ( ( err_code = alGetError() ) != AL_NO_ERROR )
		{
			std::string error = (char*) alGetString( err_code );
			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"OpenAL error:" + error, what_class);
		}
	}
	
	void OpenALSoundSystem::UpdateListener(const Vec3 &pos, const Quaternion &rot, const Vec3 &vel)
	{
		// Position of the listener.
		ALfloat listener_pos[] = GASS_TO_OAL_VEC(pos);
		// Velocity of the listener.
		ALfloat listener_vel[] = GASS_TO_OAL_VEC(vel);
		// Orientation of the listener. (first 3 elements are "dir", second 3 are "up")

		Mat4 rot_mat(rot);
		Vec3 dir = -rot_mat.GetZAxis();
		Vec3 up = rot_mat.GetYAxis();

		ALfloat listener_ori[] = { static_cast<ALfloat>(dir.x), static_cast<ALfloat>(dir.y), static_cast<ALfloat>(dir.z),  
								  static_cast<ALfloat>(up.x),  static_cast<ALfloat>(up.y),  static_cast<ALfloat>(up.z)};

		alListenerfv(AL_POSITION,    listener_pos);
		alListenerfv(AL_VELOCITY,    listener_vel);
		alListenerfv(AL_ORIENTATION, listener_ori);
	}

	void OpenALSoundSystem::LoadWaveSound(const std::string &filePath,ALuint &buffer)
	{
		//ALvoid*		data=NULL;
	
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
#ifdef WIN32
			// We have more buffers free, generate a new one
			alGenBuffers(1, &buffer);
			CheckAlError("OpenALSoundSystem::LoadWaveSound()");
			ALboolean ret = ALFWLoadWaveToBuffer(filePath.c_str(),buffer);
			if(ret == AL_FALSE)
			{
				GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"Failed to load wave file:" + filePath ,"OpenALSoundComponent::LoadWaveSound()");
			}
#else
			buffer = alutCreateBufferFromFile(filePath.c_str());
#endif

			CheckAlError("OpenALSoundSystem::LoadWaveSound()");

			m_BufferMap[filePath] = buffer;
		}
		
	}

	std::string OpenALSoundSystem::ListAvailableDevices( void )
	{
		std::string str = "Sound Devices available : ";

		if ( alcIsExtensionPresent( nullptr, "ALC_ENUMERATION_EXT" ) == AL_TRUE )
		{
			str = "List of Devices : ";
			str += (char*) alcGetString( nullptr, ALC_DEVICE_SPECIFIER );
			str += "\n";
		}
		else
			str += " ... eunmeration error.\n";

		return str;
	}

} 
