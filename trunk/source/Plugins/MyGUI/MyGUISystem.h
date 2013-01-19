#ifndef MYGUI_SYSTEM
#define MYGUI_SYSTEM

#include "Sim/GASS.h"
namespace MyGUI
{
	class Gui;
}

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
		virtual void OnCreate(SystemManagerPtr owner);
		virtual std::string GetSystemName() const {return "MyGUISystem";}
	protected:
		void OnInputSystemLoaded(InputSystemLoadedEventPtr message);
		void OnShutdown(MessagePtr message);
		void OnLoadGUIScript(GUIScriptRequestPtr message);
		bool MouseMoved(const MouseData &data);
		bool MousePressed(const MouseData &data, MouseButtonId id );
		bool MouseReleased(const MouseData &data, MouseButtonId id );
		bool KeyPressed(int key, unsigned int text);
		bool KeyReleased( int key, unsigned int text);

		MyGUI::Gui* mGUI;
	};
	typedef SPTR<MyGUISystem> MyGUISystemPtr;
}

#endif