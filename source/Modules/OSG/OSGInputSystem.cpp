#include "OSGInputSystem.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSystemFactory.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSException.h"
#include "Core/Utils/GASSLogger.h"

namespace GASS
{
	float NormalizeMouseDelta(float value)
	{
		const float mouse_speed = 20;
		float ret = 0;
		if (value > mouse_speed)
			ret = 1;
		else if (value < -mouse_speed)
			ret = -1;
		else
			ret = value / mouse_speed;
		return ret;
	}

	MouseData GetMouseData(const osgGA::GUIEventAdapter& ea, const MouseData& last_state)
	{
		MouseData mouse_data;
		mouse_data.XAbs = ea.getX();
		mouse_data.YAbs = ea.getY();
		mouse_data.XRel = NormalizeMouseDelta(static_cast<float>(mouse_data.XAbs - last_state.XAbs));
		mouse_data.YRel = NormalizeMouseDelta(static_cast<float>(last_state.YAbs - mouse_data.YAbs));
		mouse_data.XAbsNorm = (1.0f + ea.getXnormalized()) / 2.0f;
		mouse_data.YAbsNorm = 1.0f - (1.0f + ea.getYnormalized()) / 2.0f;
		return mouse_data;
	}

	std::pair<bool, MouseButtonId> GetMouseButton(const osgGA::GUIEventAdapter& ea)
	{
		std::pair<bool, MouseButtonId> ret;
		if (ea.getButtonMask() & osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		{
			return { true, MBID_LEFT };
		}

		if (ea.getButtonMask() & osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
		{
			return { true,MBID_RIGHT };
		}

		if (ea.getButtonMask() & osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
		{
			return { true,MBID_MIDDLE };
		}
		return { false, MBID_LEFT };
	}

	OSGInputHandler::OSGInputHandler(OSGInputSystem* is)
		: m_IS(is)
	{
		
	}

	bool OSGInputHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& /*aa*/)
	{
		if (ea.getHandled())
			return false;
		const bool want_capture_mouse = false;
		const bool want_capture_keyboard = false;
		using osgKey = osgGA::GUIEventAdapter::KeySymbol;
		static std::map<int, int> keymap;
		if (keymap.empty())
		{
			keymap[osgKey::KEY_A] = KEY_A;
			keymap[osgKey::KEY_B] = KEY_B;
			keymap[osgKey::KEY_C] = KEY_C;
			keymap[osgKey::KEY_D] = KEY_D;
			keymap[osgKey::KEY_E] = KEY_E;
			keymap[osgKey::KEY_F] = KEY_F;
			keymap[osgKey::KEY_G] = KEY_G;
			keymap[osgKey::KEY_H] = KEY_H;
			keymap[osgKey::KEY_I] = KEY_I;
			keymap[osgKey::KEY_J] = KEY_J;
			keymap[osgKey::KEY_K] = KEY_K;
			keymap[osgKey::KEY_L] = KEY_L;
			keymap[osgKey::KEY_M] = KEY_M;
			keymap[osgKey::KEY_N] = KEY_N;
			keymap[osgKey::KEY_O] = KEY_O;
			keymap[osgKey::KEY_P] = KEY_P;
			keymap[osgKey::KEY_Q] = KEY_Q;
			keymap[osgKey::KEY_R] = KEY_R;
			keymap[osgKey::KEY_S] = KEY_S;
			keymap[osgKey::KEY_T] = KEY_T;
			keymap[osgKey::KEY_U] = KEY_U;
			keymap[osgKey::KEY_V] = KEY_V;
			keymap[osgKey::KEY_W] = KEY_W;
			keymap[osgKey::KEY_X] = KEY_X;
			keymap[osgKey::KEY_Y] = KEY_Y;
			keymap[osgKey::KEY_Z] = KEY_Z;

			keymap[osgKey::KEY_0] = KEY_0;
			keymap[osgKey::KEY_1] = KEY_1;
			keymap[osgKey::KEY_2] = KEY_2;
			keymap[osgKey::KEY_3] = KEY_3;
			keymap[osgKey::KEY_4] = KEY_4;
			keymap[osgKey::KEY_5] = KEY_5;
			keymap[osgKey::KEY_6] = KEY_6;
			keymap[osgKey::KEY_7] = KEY_7;
			keymap[osgKey::KEY_8] = KEY_8;
			keymap[osgKey::KEY_9] = KEY_9;

			keymap[osgKey::KEY_F1] = KEY_F1;
			keymap[osgKey::KEY_F2] = KEY_F2;
			keymap[osgKey::KEY_F3] = KEY_F3;
			keymap[osgKey::KEY_F4] = KEY_F4;
			keymap[osgKey::KEY_F5] = KEY_F5;
			keymap[osgKey::KEY_F6] = KEY_F6;
			keymap[osgKey::KEY_F7] = KEY_F7;
			keymap[osgKey::KEY_F8] = KEY_F8;
			keymap[osgKey::KEY_F9] = KEY_F9;
			keymap[osgKey::KEY_F10] = KEY_F10;
			keymap[osgKey::KEY_F11] = KEY_F11;
			keymap[osgKey::KEY_F12] = KEY_F12;

			keymap[osgKey::KEY_Delete] = KEY_DELETE;
			keymap[osgKey::KEY_Left] = KEY_LEFT;
			keymap[osgKey::KEY_Right] = KEY_RIGHT;
			keymap[osgKey::KEY_Up] = KEY_UP;
			keymap[osgKey::KEY_Down] = KEY_DOWN;
			
			keymap[osgKey::KEY_Shift_L] = KEY_LSHIFT;
			keymap[osgKey::KEY_Shift_R] = KEY_RSHIFT;
			keymap[osgKey::KEY_Control_L] = KEY_LCONTROL;
			keymap[osgKey::KEY_Control_R] = KEY_RCONTROL;
			keymap[osgKey::KEY_Space] = KEY_SPACE;
		}
		static MouseData last_data;
		switch (ea.getEventType())
		{
		case osgGA::GUIEventAdapter::KEYDOWN:
		case osgGA::GUIEventAdapter::KEYUP:
		{
			const bool is_key_down = ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN;
			const int c = ea.getUnmodifiedKey();
			is_key_down ? m_IS->InjectKeyPressed(keymap[c], keymap[c]) : m_IS->InjectKeyReleased(keymap[c], keymap[c]);
			return want_capture_keyboard;
		}
		case (osgGA::GUIEventAdapter::RELEASE):
		case (osgGA::GUIEventAdapter::PUSH):
		{
			const bool is_pushed = ea.getEventType() == osgGA::GUIEventAdapter::PUSH;
			const MouseData mouse_data = GetMouseData(ea, last_data);
			last_data = mouse_data;
			static std::pair<bool, MouseButtonId> button_data;
			if (is_pushed)
			{
				button_data = GetMouseButton(ea);
				if (button_data.first)
					m_IS->InjectMousePressed(mouse_data, button_data.second);
			}
			else
			{
				if (button_data.first)
					m_IS->InjectMouseReleased(mouse_data, button_data.second);
			}
			return want_capture_mouse;
		}
		case (osgGA::GUIEventAdapter::DRAG):
		case (osgGA::GUIEventAdapter::MOVE):
		{
			const MouseData mouse_data = GetMouseData(ea, last_data);
			last_data = mouse_data;
			m_IS->InjectMouseMoved(mouse_data);
			return want_capture_mouse;
		}
		case (osgGA::GUIEventAdapter::SCROLL):
		{
			//mouseWheel_ = ea.getScrollingMotion() == osgGA::GUIEventAdapter::SCROLL_UP ? 1.0 : -1.0;
			return want_capture_mouse;
		}
		default:
		{
			return false;
		}
		}
		return false;
	}

	OSGInputSystem::OSGInputSystem(SimSystemManagerWeakPtr manager) : Reflection(manager)
	{
		m_UpdateGroup = UGID_PRE_SIM;
	}

	void OSGInputSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register<OSGInputSystem>("OSGInputSystem");
		RegisterGetSet("EnableKey", &GASS::OSGInputSystem::GetEnableKey, &GASS::OSGInputSystem::SetEnableKey);
		RegisterGetSet("EnableMouse", &GASS::OSGInputSystem::GetEnableMouse, &GASS::OSGInputSystem::SetEnableMouse);
	}

	void OSGInputSystem::SetMainWindowHandle(void*)
	{
		SimEngine::Get().GetSimSystemManager()->PostMessage(SystemMessagePtr(new InputSystemLoadedEvent()));
	}

	void OSGInputSystem::AddKeyListener(IKeyListener* key_listener)
	{
		m_KeyListeners.push_back(key_listener);
	}

	void OSGInputSystem::RemoveKeyListener(IKeyListener* key_listener)
	{
		auto iter = m_KeyListeners.begin();
		while (iter != m_KeyListeners.end())
		{
			IKeyListener* kl = *iter;
			if (kl == key_listener)
			{
				iter = m_KeyListeners.erase(iter);
			}
			else ++iter;
		}
	}

	void OSGInputSystem::AddMouseListener(IMouseListener* mouse_listener)
	{
		m_MouseListeners.push_back(mouse_listener);
	}

	void OSGInputSystem::RemoveMouseListener(IMouseListener* mouse_listener)
	{
		auto iter = m_MouseListeners.begin();
		while (iter != m_MouseListeners.end())
		{
			IMouseListener* ml = *iter;
			if (ml == mouse_listener)
			{
				iter = m_MouseListeners.erase(iter);
			}
			else ++iter;
		}
	}

	void OSGInputSystem::SetEnableKey(bool value)
	{
		m_KeyActive = value;
	}

	void OSGInputSystem::SetEnableMouse(bool value)
	{
		m_MouseActive = value;
	}

	bool OSGInputSystem::GetEnableKey() const
	{
		return m_KeyActive;
	}

	bool OSGInputSystem::GetEnableMouse() const
	{
		return m_MouseActive;
	}

	void OSGInputSystem::InjectMouseMoved(const MouseData& data)
	{
		auto iter = m_MouseListeners.begin();
		while (iter != m_MouseListeners.end())
		{
			IMouseListener* ml = *iter;
			ml->MouseMoved(data);
			++iter;
		}
	}

	void OSGInputSystem::InjectMousePressed(const MouseData& data, MouseButtonId id)
	{
		auto iter = m_MouseListeners.begin();
		while (iter != m_MouseListeners.end())
		{
			IMouseListener* ml = *iter;
			ml->MousePressed(data, id);
			++iter;
		}
	}

	void OSGInputSystem::InjectMouseReleased(const MouseData& data, MouseButtonId id)
	{
		auto iter = m_MouseListeners.begin();
		while (iter != m_MouseListeners.end())
		{
			IMouseListener* ml = *iter;
			ml->MouseReleased(data, id);
			++iter;
		}
	}

	void OSGInputSystem::InjectKeyPressed(int key, unsigned int text)
	{
		auto iter = m_KeyListeners.begin();
		while (iter != m_KeyListeners.end())
		{
			IKeyListener* kl = *iter;
			kl->KeyPressed(key, text);
			++iter;
		}
	}

	void OSGInputSystem::InjectKeyReleased(int key, unsigned int text)
	{
		auto iter = m_KeyListeners.begin();
		while (iter != m_KeyListeners.end())
		{
			IKeyListener* kl = *iter;
			kl->KeyReleased(key, text);
			++iter;
		}
	}
}