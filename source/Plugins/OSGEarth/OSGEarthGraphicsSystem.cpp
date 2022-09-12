#include "OSGEarthGraphicsSystem.h"
#include "OSGEarthCommonIncludes.h"

namespace GASS
{
	OSGEarthGraphicsSystem::OSGEarthGraphicsSystem(SimSystemManagerWeakPtr manager) : Reflection(manager)
	{
		m_UpdateGroup = UGID_PRE_SIM;
	}

	OSGEarthGraphicsSystem::~OSGEarthGraphicsSystem()
	{

	}

	void OSGEarthGraphicsSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register<OSGEarthGraphicsSystem>("OSGEarthGraphicsSystem");
		RegisterMember("UseLogHandler", &GASS::OSGEarthGraphicsSystem::m_UseLogHandler);
	}

	class OENotifyHandler : public osg::NotifyHandler
	{
	public:
		void notify(osg::NotifySeverity severity, const char* message) override
		{
			if (message == nullptr)
				return;
			if (*message == '\0')
				return;
			switch (severity)
			{
			case osg::NotifySeverity::ALWAYS:
				GASS_LOG(LINFO) << " OE-ALWAYS:" << message;
				break;
			case osg::NotifySeverity::FATAL:
				GASS_LOG(LERROR) << " OE-FATAL:" << message;
				break;
			case osg::NotifySeverity::WARN:
				GASS_LOG(LWARNING) << " OE-WARN:" << message;
				break;
			case osg::NotifySeverity::NOTICE:
				GASS_LOG(LINFO) << " OE-NOTICE:" << message;
				break;
			case osg::NotifySeverity::INFO:
				GASS_LOG(LINFO) << " OE-INFO:" << message;
				break;
			case osg::NotifySeverity::DEBUG_INFO:
				GASS_LOG(LINFO) << " OE-DEBUG_INFO:" << message;
				break;
			case osg::NotifySeverity::DEBUG_FP:
				GASS_LOG(LINFO) << " OE-DEBUG_FP:" << message;
				break;
			}
		}
	};

	void OSGEarthGraphicsSystem::OnSystemInit()
	{
		if(m_UseLogHandler)
			osgEarth::setNotifyHandler(new OENotifyHandler());
		osgEarth::initialize();
	}
}
