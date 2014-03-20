#ifndef MYGUI_SYSTEM
#define MYGUI_SYSTEM

#include "Sim/GASS.h"
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

class MYGUIOSGDrawable;

namespace GASS
{
	class MyGUISystem : public Reflection<MyGUISystem, SimSystem>, public IMouseListener, public IKeyListener
	{
	public:
		MyGUISystem();
		virtual ~MyGUISystem();
		static void RegisterReflection();
		virtual void Update(double delta_time);
		virtual void Init();
		virtual std::string GetSystemName() const {return "MyGUISystem";}
		MyGUI::OpenGLPlatform* InitializeOpenGLPlatform();
		void ShutdownOSG(){};
	protected:
		void _InitlizeGUI();
		void _SetupOSG();
		void OnPostSceneCreate(PostSceneCreateEventPtr message);
		void OnInputSystemLoaded(InputSystemLoadedEventPtr message);
		void OnCameraChanged(CameraChangedEventPtr message);
		void OnShutdown(MessagePtr message);
		void OnLoadGUIScript(GUIScriptRequestPtr message);
		bool MouseMoved(const MouseData &data);
		bool MousePressed(const MouseData &data, MouseButtonId id );
		bool MouseReleased(const MouseData &data, MouseButtonId id );
		bool KeyPressed(int key, unsigned int text);
		bool KeyReleased( int key, unsigned int text);

		MyGUI::Gui* mGUI;

		osg::ref_ptr<MYGUIOSGDrawable>  m_OSGManager;
		osg::ref_ptr<osg::Camera> m_HUDCamera;
	};
	typedef SPTR<MyGUISystem> MyGUISystemPtr;
}

#endif