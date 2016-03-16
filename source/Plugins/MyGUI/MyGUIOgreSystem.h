#ifndef MYGUI_OGRE_SYSTEM
#define MYGUI_OGRE_SYSTEM

#include "Sim/GASS.h"
#include "MyGuiSystem.h"
#include <osg/ref_ptr>

namespace GASS
{
	class MyGUIOgreSystem : public Reflection<MyGUIOgreSystem, MyGUISystem>
	{
	public:
		MyGUIOgreSystem();
		virtual ~MyGUIOgreSystem();
		static void RegisterReflection();
		virtual void Init();
		virtual std::string GetSystemName() const {return "MyGUIOgreSystem";}
	protected:
		void OnInputSystemLoaded(InputSystemLoadedEventPtr message);
	/*	bool MouseMoved(const MouseData &data);
		bool MousePressed(const MouseData &data, MouseButtonId id );
		bool MouseReleased(const MouseData &data, MouseButtonId id );
		bool KeyPressed(int key, unsigned int text);
		bool KeyReleased( int key, unsigned int text);*/
	};
	typedef GASS_SHARED_PTR<MyGUIOgreSystem> MyGUIOgreSystemPtr;
}

#endif