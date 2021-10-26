#include "OSGImGuiHandler.h"
#include <iostream>
#include <osg/Camera>
#include <osgUtil/GLObjectsVisitor>
#include <osgUtil/SceneView>
#include <osgUtil/UpdateVisitor>
#include <osgViewer/ViewerEventHandlers>
#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"

namespace GASS
{
	OSGImGuiHandler::GlewInitOperation::GlewInitOperation()
		: osg::Operation("GlewInitCallback", false)
	{
	}

	void OSGImGuiHandler::GlewInitOperation::operator()(osg::Object* object)
	{
		auto* context = dynamic_cast<osg::GraphicsContext*>(object);
		if (!context)
			return;

		if (glewInit() != GLEW_OK)
		{
			std::cout << "glewInit() failed\n";
		}
		OSGImGuiHandler::Init();
	}

	struct OSGImGuiHandler::ImGuiNewFrameCallback : public osg::Camera::DrawCallback
	{
		ImGuiNewFrameCallback(OSGImGuiHandler& handler)
			: m_Handler(handler)
		{
		}

		void operator()(osg::RenderInfo& renderInfo) const override
		{
			m_Handler.NewFrame(renderInfo);
		}

	private:
		OSGImGuiHandler& m_Handler;
	};

	struct OSGImGuiHandler::ImGuiRenderCallback : public osg::Camera::DrawCallback
	{
		ImGuiRenderCallback(OSGImGuiHandler& handler)
			: m_Handler(handler)
		{
		}

		void operator()(osg::RenderInfo& renderInfo) const override
		{
			m_Handler.Render(renderInfo);
		}

	private:
		OSGImGuiHandler& m_Handler;
	};

	OSGImGuiHandler::OSGImGuiHandler()
		 
	{

		ImGuiIO& io = ImGui::GetIO();
		(void)io;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		// init();
	}

	/**
	 * Imporant Note: Dear ImGui expects the control Keys indices not to be
	 * greater thant 511. It actually uses an array of 512 elements. However,
	 * OSG has indices greater than that. So here I do a conversion for special
	 * keys between ImGui and OSG.
	 */
	static int ConvertFromOSGKey(int key)
	{
		using KEY = osgGA::GUIEventAdapter::KeySymbol;

		switch (key)
		{
		case KEY::KEY_Tab:
			return ImGuiKey_Tab;
		case KEY::KEY_Left:
			return ImGuiKey_LeftArrow;
		case KEY::KEY_Right:
			return ImGuiKey_RightArrow;
		case KEY::KEY_Up:
			return ImGuiKey_UpArrow;
		case KEY::KEY_Down:
			return ImGuiKey_DownArrow;
		case KEY::KEY_Page_Up:
			return ImGuiKey_PageUp;
		case KEY::KEY_Page_Down:
			return ImGuiKey_PageDown;
		case KEY::KEY_Home:
			return ImGuiKey_Home;
		case KEY::KEY_End:
			return ImGuiKey_End;
		case KEY::KEY_Delete:
			return ImGuiKey_Delete;
		case KEY::KEY_BackSpace:
			return ImGuiKey_Backspace;
		case KEY::KEY_Return:
			return ImGuiKey_Enter;
		case KEY::KEY_Escape:
			return ImGuiKey_Escape;
		default: // Not found
			return -1;
		}
	}

	void OSGImGuiHandler::Init()
	{
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		io.MouseDoubleClickTime = 0.3;
		io.MouseDoubleClickMaxDist = 6;
		io.WantCaptureKeyboard = true;
		io.WantCaptureMouse = true;
		// Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
		io.KeyMap[ImGuiKey_Tab] = ImGuiKey_Tab;
		io.KeyMap[ImGuiKey_LeftArrow] = ImGuiKey_LeftArrow;
		io.KeyMap[ImGuiKey_RightArrow] = ImGuiKey_RightArrow;
		io.KeyMap[ImGuiKey_UpArrow] = ImGuiKey_UpArrow;
		io.KeyMap[ImGuiKey_DownArrow] = ImGuiKey_DownArrow;
		io.KeyMap[ImGuiKey_PageUp] = ImGuiKey_PageUp;
		io.KeyMap[ImGuiKey_PageDown] = ImGuiKey_PageDown;
		io.KeyMap[ImGuiKey_Home] = ImGuiKey_Home;
		io.KeyMap[ImGuiKey_End] = ImGuiKey_End;
		io.KeyMap[ImGuiKey_Delete] = ImGuiKey_Delete;
		io.KeyMap[ImGuiKey_Backspace] = ImGuiKey_Backspace;
		io.KeyMap[ImGuiKey_Enter] = ImGuiKey_Enter;
		io.KeyMap[ImGuiKey_Escape] = ImGuiKey_Escape;
		io.KeyMap[ImGuiKey_A] = osgGA::GUIEventAdapter::KeySymbol::KEY_A;
		io.KeyMap[ImGuiKey_C] = osgGA::GUIEventAdapter::KeySymbol::KEY_C;
		io.KeyMap[ImGuiKey_V] = osgGA::GUIEventAdapter::KeySymbol::KEY_V;
		io.KeyMap[ImGuiKey_X] = osgGA::GUIEventAdapter::KeySymbol::KEY_X;
		io.KeyMap[ImGuiKey_Y] = osgGA::GUIEventAdapter::KeySymbol::KEY_Y;
		io.KeyMap[ImGuiKey_Z] = osgGA::GUIEventAdapter::KeySymbol::KEY_Z;

		ImGui_ImplOpenGL3_Init();

		//io.RenderDrawListsFn = ImGui_ImplOpenGL3_RenderDrawData;
	}

	void OSGImGuiHandler::SetCameraCallbacks(osg::Camera* camera)
	{
		camera->setPreDrawCallback(new ImGuiNewFrameCallback(*this));
		camera->setPostDrawCallback(new ImGuiRenderCallback(*this));
	}

	void OSGImGuiHandler::NewFrame(osg::RenderInfo& renderInfo)
	{
		ImGui_ImplOpenGL3_NewFrame();

		ImGuiIO& io = ImGui::GetIO();

		osg::Viewport* viewport = renderInfo.getCurrentCamera()->getViewport();
		io.DisplaySize = ImVec2(viewport->width(), viewport->height());

		double current_time = renderInfo.getView()->getFrameStamp()->getSimulationTime();
		io.DeltaTime = current_time - m_Time + 0.0000001;
		m_Time = current_time;

		for (int i = 0; i < 3; i++)
		{
			io.MouseDown[i] = m_MousePressed[i];
		}

		io.MouseWheel = m_MouseWheel;
		m_MouseWheel = 0.0f;

		ImGui::NewFrame();
	}

	void OSGImGuiHandler::Render(osg::RenderInfo&)
	{
		DrawUi();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	bool OSGImGuiHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		if (!m_Initialized)
		{
			auto view = aa.asView();
			if (view)
			{
				SetCameraCallbacks(view->getCamera());
				m_Initialized = true;
			}
		}

		ImGuiIO& io = ImGui::GetIO();
		const bool want_capture_mouse = io.WantCaptureMouse;
		const bool want_capture_keyboard = io.WantCaptureKeyboard;

		switch (ea.getEventType())
		{
		case osgGA::GUIEventAdapter::KEYDOWN:
		case osgGA::GUIEventAdapter::KEYUP:
		{
			const bool is_key_down = ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN;
			const int c = ea.getKey();
			const int special_key = ConvertFromOSGKey(c);
			if (special_key > 0)
			{
				assert((special_key >= 0 && special_key < 512) && "ImGui KeysMap is an array of 512");

				io.KeysDown[special_key] = is_key_down;

				io.KeyCtrl = ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_CTRL;
				io.KeyShift = ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_SHIFT;
				io.KeyAlt = ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_ALT;
				io.KeySuper = ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_SUPER;
			}
			else if (is_key_down && c > 0 && c < 0xFF)
			{
				io.AddInputCharacter((unsigned short)c);
			}
			return want_capture_keyboard;
		}
		case (osgGA::GUIEventAdapter::RELEASE):
		case (osgGA::GUIEventAdapter::DOUBLECLICK):
		case (osgGA::GUIEventAdapter::PUSH):
		{
			io.MousePos = ImVec2(ea.getX(), io.DisplaySize.y - ea.getY());
			m_MousePressed[0] = ea.getButtonMask() & osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON;
			m_MousePressed[1] = ea.getButtonMask() & osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON;
			m_MousePressed[2] = ea.getButtonMask() & osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON;
			return want_capture_mouse;
		}
		case (osgGA::GUIEventAdapter::DRAG):
		case (osgGA::GUIEventAdapter::MOVE):
		{
			io.MousePos = ImVec2(ea.getX(), io.DisplaySize.y - ea.getY());
			return want_capture_mouse;
		}
		case (osgGA::GUIEventAdapter::SCROLL):
		{
			m_MouseWheel = ea.getScrollingMotion() == osgGA::GUIEventAdapter::SCROLL_UP ? 1.0 : -1.0;
			return want_capture_mouse;
		}
		default:
		{
			return false;
		}
		}

		return false;
	}

	void OSGImGuiHandler::DrawUi()
	{
		SimEngine::Get().GetSimSystemManager()->SendImmediate(std::make_shared<DrawGUIEvent>(ImGui::GetCurrentContext()));
	}
}
