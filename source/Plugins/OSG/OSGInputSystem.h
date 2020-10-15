#pragma once

#include "Sim/GASSCommon.h"
#include "Sim/Interface/GASSIInputSystem.h"
#include "Sim/GASSSimSystem.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include <osgViewer/ViewerEventHandlers>

namespace GASS
{
	class OSGInputSystem;
	class OSGInputHandler : public osgGA::GUIEventHandler
	{
	public:
		OSGInputHandler(OSGInputSystem* is);
		bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override;
	protected:
	private:
		OSGInputSystem* m_IS = nullptr;
	};

	class OSGInputSystem : public Reflection<OSGInputSystem, SimSystem>, public IInputSystem, public IProxyInputSystem
	{
	public:
		static void RegisterReflection();
		OSGInputSystem(SimSystemManagerWeakPtr manager);
		void OnSystemInit() override {};
		void OnSystemUpdate(double) override {};
		std::string GetSystemName() const override { return "OSGInputSystem"; }

		//IInputSystem
		void AddKeyListener(IKeyListener* key_listener) override;
		void RemoveKeyListener(IKeyListener* key_listener) override;
		void AddMouseListener(IMouseListener* mouse_listener) override;
		void RemoveMouseListener(IMouseListener* mouse_listener) override;
		void AddGameControllerListener(IGameControllerListener*) override {};
		void RemoveGameControllerListener(IGameControllerListener*) override {};
		void ClipInputWindow(int /*left*/, int /*top*/, int /*right*/, int /*bottom*/) override {};
		void SetEnableKey(bool value) override;
		void SetEnableJoystick(bool /*value*/) override {}
		void SetEnableMouse(bool value) override;
		bool GetEnableKey() const override;
		bool GetEnableJoystick() const override { return false; }
		bool GetEnableMouse() const override;
		void SetMainWindowHandle(void* main_win_handle) override;

		//IProxyInputSystem
		void InjectMouseMoved(const MouseData& data) override;
		void InjectMousePressed(const MouseData& data, MouseButtonId id) override;
		void InjectMouseReleased(const MouseData& data, MouseButtonId id) override;
		void InjectKeyPressed(int key, unsigned int text) override;
		void InjectKeyReleased(int key, unsigned int text) override;

	private:
		std::vector<IKeyListener*> m_KeyListeners;
		std::vector<IMouseListener*> m_MouseListeners;
		bool m_KeyActive = true;
		bool m_MouseActive = true;
	};
}