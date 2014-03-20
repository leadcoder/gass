#ifndef H_MYGUIDRAWABLE
#define H_MYGUIDRAWABLE

#include <MyGUI.h>
#include <MyGUI_OpenGLPlatform.h>
#include <osg/Camera>
#include <osg/Drawable>
#include <osgGA/GUIEventHandler>
#include <queue>
namespace GASS
{
	class MyGUISystem;
}
class MYGUIOSGDrawable;

class MYGUIOSGEventHandler : public osgGA::GUIEventHandler
{
public:
	MYGUIOSGEventHandler( osg::Camera* c, MYGUIOSGDrawable* m ) : _camera(c), _manager(m) {}
	virtual bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );
protected:
	osg::observer_ptr<osg::Camera> _camera;
	MYGUIOSGDrawable* _manager;
};

class MYGUIOSGDrawable : public osg::Drawable, public MyGUI::OpenGLImageLoader
{
public:
	MYGUIOSGDrawable() {};
	MYGUIOSGDrawable(GASS::MyGUISystem* system);
	MYGUIOSGDrawable( const MYGUIOSGDrawable& copy, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY );
	META_Object( osg, MYGUIOSGDrawable )

		void pushEvent( const osgGA::GUIEventAdapter* ea )
	{ m_EventsToHandle.push( ea ); }

	// image loader methods
	virtual void* loadImage( int& width, int& height, MyGUI::PixelFormat& format, const std::string& filename );
	virtual void saveImage( int width, int height, MyGUI::PixelFormat format, void* texture, const std::string& filename );

	// drawable methods
	virtual void drawImplementation( osg::RenderInfo& renderInfo ) const;
	virtual void releaseGLObjects( osg::State* state=0 ) const;

protected:
	virtual ~MYGUIOSGDrawable() {}
	virtual void updateEvents() const;

	MyGUI::MouseButton convertMouseButton( int button ) const;
	MyGUI::KeyCode convertKeyCode( int key ) const;

	std::queue< osg::ref_ptr<const osgGA::GUIEventAdapter> > m_EventsToHandle;
	MyGUI::OpenGLPlatform* m_OpenGLPlatform;
	unsigned int m_ActiveContextID;
	bool m_Initialized;
	GASS::MyGUISystem* m_GUISystem;
};

#endif