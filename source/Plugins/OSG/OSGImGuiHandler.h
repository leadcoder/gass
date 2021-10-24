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
		static void init();
	protected:
		// Put your ImGui code inside this function
		virtual void drawUi();

	private:


		void setCameraCallbacks(osg::Camera* camera);

		void newFrame(osg::RenderInfo& renderInfo);

		void render(osg::RenderInfo& renderInfo);

	private:
		struct ImGuiNewFrameCallback;
		struct ImGuiRenderCallback;

		double time_{0.0f};
		bool mousePressed_[3]{ false };
		float mouseWheel_{0.0f};
		bool initialized_{false};
	};
}
