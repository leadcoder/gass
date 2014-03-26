#ifndef MYGUI_OSG_SYSTEM
#define MYGUI_OSG_SYSTEM

#include "Sim/GASS.h"
#include "MyGUISystem.h"
#include <osg/ref_ptr>
namespace MyGUI
{
	class Gui;
	class OpenGLPlatform;
}
namespace osg
{
 class Camera;
}
namespace osgViewer
{
	class View;
}


class MYGUIOSGPlatformProxy;

namespace GASS
{
	class MyGUIOSGSystem : public Reflection<MyGUIOSGSystem, MyGUISystem>
	{
	public:
		MyGUIOSGSystem();
		virtual ~MyGUIOSGSystem();
		static void RegisterReflection();
		virtual void Init();
		virtual std::string GetSystemName() const {return "MyGUISystem";}
		MyGUI::OpenGLPlatform* InitializeOpenGLPlatform();
		void ShutdownOSG(){};
	protected:
		void _SetupOSG(osgViewer::View* view);
		void OnGraphicsSystemLoaded(GraphicsSystemLoadedEventPtr message);
		void OnCameraChanged(CameraChangedEventPtr message);
		osg::ref_ptr<MYGUIOSGPlatformProxy>  m_OSGManager;
		osg::ref_ptr<osg::Camera> m_HUDCamera;
	};
	typedef SPTR<MyGUIOSGSystem> MyGUIOSGSystemPtr;
}

#endif