#pragma once
#include "GL/glew.h"
#include <osgViewer/ViewerEventHandlers>

namespace osg {
	class Camera;
}

namespace GASS
{
	class OSGImGuiHandler : public osgGA::GUIEventHandler
	{
	public:
		class GlewInitOperation : public osg::Operation
		{
		public:
			GlewInitOperation();
			void operator()(osg::Object* object) override;
		};

		OSGImGuiHandler();

		bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override;
		static void Init();
	protected:
		// Put your ImGui code inside this function
		virtual void DrawUi();

	private:


		void SetCameraCallbacks(osg::Camera* camera);

		void NewFrame(osg::RenderInfo& renderInfo);

		void Render(osg::RenderInfo& renderInfo);

	private:
		struct ImGuiNewFrameCallback;
		struct ImGuiRenderCallback;

		double m_Time{0.0f};
		bool m_MousePressed[3]{ false };
		float m_MouseWheel{0.0f};
		bool m_Initialized{false};
	};
}
