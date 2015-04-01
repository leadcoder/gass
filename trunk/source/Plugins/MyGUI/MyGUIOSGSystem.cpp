/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                 *
*                                                                           *
* Copyright (c) 2008-2009 GASS team. See Contributors.txt for details.      *
*                                                                           *
* GASS is free software: you can redistribute it and/or modify              *
* it under the terms of the GNU Lesser General Public License as published  *
* by the Free Software Foundation, either version 3 of the License, or      *
* (at your option) any later version.                                       *
*                                                                           *
* GASS is distributed in the hope that it will be useful,                   *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU Lesser General Public License for more details.                       *
*                                                                           *
* You should have received a copy of the GNU Lesser General Public License  *
* along with GASS. If not, see <http://www.gnu.org/licenses/>.              *
*****************************************************************************/

#include "MyGUIOSGSystem.h"
#include <stdio.h>
#include "Sim/Messages/GASSScriptSystemMessages.h"
#include "MyGUIOSG.h"
#include <MyGUI.h>
#include "MyGUI_LastHeader.h"

#include <osg/Texture2D>
#include <osg/Texture>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgGA/StateSetManipulator>
#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/Viewer>
#include <osgViewer/View>
#include <osgViewer/CompositeViewer>
#include <osg/GraphicsContext>

#include "Plugins/OSG/IOSGGraphicsSceneManager.h"
#include "Plugins/OSG/IOSGGraphicsSystem.h"
#include "Plugins/OSG/IOSGCamera.h"
#include "Sim/Interface/GASSIViewport.h"

#include "MyGUI_RTTLayer.h"
#include "MyGUI_OpenGLTexture.h"

namespace GASS
{
	class GUITexture : public osg::Texture2D
	{
	public:
		GUITexture(unsigned int id): m_ID(id)
		{

		}
		virtual ~GUITexture()
		{

		}
		virtual void apply(osg::State& state) const
		{
			glBindTexture(GL_TEXTURE_2D, m_ID);
		}
		unsigned int m_ID;
	};
	osg::Image* mimage = NULL;
	osgViewer::View* mview;
	GUITexture* my_texture = NULL;
	unsigned int my_tex_id = 0;

	osg::Node* createFilterWall(osg::BoundingBox& bb,const std::string& filename, unsigned int tex_id)
	{
		osg::Group* group = new osg::Group;

		// left hand side of bounding box.
		osg::Vec3 top_left(bb.xMin(),bb.yMin(),bb.zMax());
		osg::Vec3 bottom_left(bb.xMin(),bb.yMin(),bb.zMin());
		osg::Vec3 bottom_right(bb.xMin(),bb.yMax(),bb.zMin());
		osg::Vec3 top_right(bb.xMin(),bb.yMax(),bb.zMax());
		osg::Vec3 center(bb.xMin(),(bb.yMin()+bb.yMax())*0.5f,(bb.zMin()+bb.zMax())*0.5f);
		float height = bb.zMax()-bb.zMin();

		// create the geometry for the wall.
		osg::Geometry* geom = new osg::Geometry;

	/*osg::Node* createBase(const osg::Vec3& center,float radius)
	{



		int numTilesX = 10;
		int numTilesY = 10;

		float width = 2*radius;
		float height = 2*radius;

		osg::Vec3 v000(center - osg::Vec3(width*0.5f,height*0.5f,0.0f));
		osg::Vec3 dx(osg::Vec3(width/((float)numTilesX),0.0,0.0f));
		osg::Vec3 dy(osg::Vec3(0.0f,height/((float)numTilesY),0.0f));

		// fill in vertices for grid, note numTilesX+1 * numTilesY+1...
		osg::Vec3Array* coords = new osg::Vec3Array;
		int iy;
		for(iy=0;iy<=numTilesY;++iy)
		{
			for(int ix=0;ix<=numTilesX;++ix)
			{
				coords->push_back(v000+dx*(float)ix+dy*(float)iy);
			}
		}

		//Just two colours - black and white.
		osg::Vec4Array* colors = new osg::Vec4Array;
		colors->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.0f)); // white
		colors->push_back(osg::Vec4(0.0f,0.0f,0.0f,1.0f)); // black
		int numColors=colors->size();


		int numIndicesPerRow=numTilesX+1;
		osg::UByteArray* coordIndices = new osg::UByteArray; // assumes we are using less than 256 points...
		osg::UByteArray* colorIndices = new osg::UByteArray;
		for(iy=0;iy<numTilesY;++iy)
		{
			for(int ix=0;ix<numTilesX;++ix)
			{
				// four vertices per quad.
				coordIndices->push_back(ix    +(iy+1)*numIndicesPerRow);
				coordIndices->push_back(ix    +iy*numIndicesPerRow);
				coordIndices->push_back((ix+1)+iy*numIndicesPerRow);
				coordIndices->push_back((ix+1)+(iy+1)*numIndicesPerRow);

				// one color per quad
				colorIndices->push_back((ix+iy)%numColors);
			}
		}


		// set up a single normal
		osg::Vec3Array* normals = new osg::Vec3Array;
		normals->push_back(osg::Vec3(0.0f,0.0f,1.0f));


		osg::Geometry* geom = new osg::Geometry;
		geom->setVertexArray(coords);
		geom->setVertexIndices(coordIndices);

		geom->setColorArray(colors);
		geom->setColorIndices(colorIndices);
		geom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);

		geom->setNormalArray(normals);
		geom->setNormalBinding(osg::Geometry::BIND_OVERALL);

		geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,coordIndices->size()));

		osg::Geode* geode = new osg::Geode;
		geode->addDrawable(geom);

		return geode;
	}*/


		osg::Vec3Array* vertices = new osg::Vec3Array(4);
		(*vertices)[0] = top_left;
		(*vertices)[1] = bottom_left;
		(*vertices)[2] = bottom_right;
		(*vertices)[3] = top_right;
		geom->setVertexArray(vertices);

		osg::Vec2Array* texcoords = new osg::Vec2Array(4);
		(*texcoords)[0].set(0.0f,1.0f);
		(*texcoords)[1].set(0.0f,0.0f);
		(*texcoords)[2].set(1.0f,0.0f);
		(*texcoords)[3].set(1.0f,1.0f);
		geom->setTexCoordArray(0,texcoords);

		osg::Vec3Array* normals = new osg::Vec3Array(1);
		(*normals)[0].set(1.0f,0.0f,0.0f);
		geom->setNormalArray(normals);//, osg::Geometry::BIND_OVERALL);
		geom->setNormalBinding( osg::Geometry::BIND_OVERALL );

		osg::Vec4Array* colors = new osg::Vec4Array(1);
		(*colors)[0].set(1.0f,1.0f,1.0f,1.0f);
		geom->setColorArray(colors);
		geom->setColorBinding( osg::Geometry::BIND_OVERALL );

		geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS,0,4));

		osg::Geode* geom_geode = new osg::Geode;
		geom_geode->addDrawable(geom);
		group->addChild(geom_geode);


		// set up the texture state.
		my_texture  = new GUITexture(tex_id);
		my_texture->setDataVariance(osg::Object::DYNAMIC); // protect from being optimized away as static state.

		//glBindTexture(GL_TEXTURE_2D, tex_id); 
		mimage = osgDB::readImageFile(filename);
		//mimage->readImageFromCurrentTexture(tex_id,true);
		my_texture->setImage(mimage);
		//my_texture->setImage(osgDB::readImageFile(filename));

		osg::StateSet* stateset = geom->getOrCreateStateSet();
		stateset->setTextureAttributeAndModes(0,my_texture,osg::StateAttribute::ON);
		
		// create the text label.

		/*osgText::Text* text = new osgText::Text;
		text->setDataVariance(osg::Object::DYNAMIC);

		text->setFont("fonts/arial.ttf");
		text->setPosition(center);
		text->setCharacterSize(height*0.03f);
		text->setAlignment(osgText::Text::CENTER_CENTER);
		text->setAxisAlignment(osgText::Text::YZ_PLANE);

		osg::Geode* text_geode = new osg::Geode;
		text_geode->addDrawable(text);

		osg::StateSet* text_stateset = text_geode->getOrCreateStateSet();
		text_stateset->setAttributeAndModes(new osg::PolygonOffset(-1.0f,-1.0f),osg::StateAttribute::ON);

		group->addChild(text_geode);

		// set the update callback to cycle through the various min and mag filter modes.
		group->setUpdateCallback(new FilterCallback(texture,text));*/

		return group;

	}
/*

	osg::Node* createBase(const osg::Vec3& center,float radius)
	{



		int numTilesX = 10;
		int numTilesY = 10;

		float width = 2*radius;
		float height = 2*radius;

		osg::Vec3 v000(center - osg::Vec3(width*0.5f,height*0.5f,0.0f));
		osg::Vec3 dx(osg::Vec3(width/((float)numTilesX),0.0,0.0f));
		osg::Vec3 dy(osg::Vec3(0.0f,height/((float)numTilesY),0.0f));

		// fill in vertices for grid, note numTilesX+1 * numTilesY+1...
		osg::Vec3Array* coords = new osg::Vec3Array;
		int iy;
		for(iy=0;iy<=numTilesY;++iy)
		{
			for(int ix=0;ix<=numTilesX;++ix)
			{
				coords->push_back(v000+dx*(float)ix+dy*(float)iy);
			}
		}

		//Just two colours - black and white.
		osg::Vec4Array* colors = new osg::Vec4Array;
		colors->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.0f)); // white
		colors->push_back(osg::Vec4(0.0f,0.0f,0.0f,1.0f)); // black
		int numColors=colors->size();


		int numIndicesPerRow=numTilesX+1;
		osg::UByteArray* coordIndices = new osg::UByteArray; // assumes we are using less than 256 points...
		osg::UByteArray* colorIndices = new osg::UByteArray;
		for(iy=0;iy<numTilesY;++iy)
		{
			for(int ix=0;ix<numTilesX;++ix)
			{
				// four vertices per quad.
				coordIndices->push_back(ix    +(iy+1)*numIndicesPerRow);
				coordIndices->push_back(ix    +iy*numIndicesPerRow);
				coordIndices->push_back((ix+1)+iy*numIndicesPerRow);
				coordIndices->push_back((ix+1)+(iy+1)*numIndicesPerRow);

				// one color per quad
				colorIndices->push_back((ix+iy)%numColors);
			}
		}


		// set up a single normal
		osg::Vec3Array* normals = new osg::Vec3Array;
		normals->push_back(osg::Vec3(0.0f,0.0f,1.0f));


		osg::Geometry* geom = new osg::Geometry;
		geom->setVertexArray(coords);
		geom->setVertexIndices(coordIndices);

		geom->setColorArray(colors);
		geom->setColorIndices(colorIndices);
		geom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);

		geom->setNormalArray(normals);
		geom->setNormalBinding(osg::Geometry::BIND_OVERALL);

		geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,coordIndices->size()));

		osg::Geode* geode = new osg::Geode;
		geode->addDrawable(geom);

		return geode;
	}*/

	MyGUIOSGSystem::MyGUIOSGSystem() 
	{
		m_OSGManager = new MYGUIOSGPlatformProxy(this);
	}

	MyGUIOSGSystem::~MyGUIOSGSystem()
	{
		//delete m_OSGManager;
	}

	void MyGUIOSGSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("MyGUIOSGSystem",new GASS::Creator<MyGUIOSGSystem, ISystem>);
	}

	void MyGUIOSGSystem::Init()
	{
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(MyGUIOSGSystem::OnCameraChanged,CameraChangedEvent, 0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(MyGUIOSGSystem::OnGraphicsSystemLoaded,GraphicsSystemLoadedEvent ,0));
		MyGUISystem::Init();
	}

	MyGUI::OpenGLPlatform* MyGUIOSGSystem::InitializeOpenGLPlatform()
	{
		MyGUI::OpenGLPlatform* gl_platform = new MyGUI::OpenGLPlatform;
		gl_platform->initialise( m_OSGManager.get() );
		//reflect MYGUI resource group to platform
		ResourceManagerPtr rm = SimEngine::Get().GetResourceManager();
		
		ResourceGroupPtr group = rm->GetFirstResourceGroupByName("MyGUI");
		ResourceLocationVector locations = group->GetResourceLocations();
		for(int i = 0; i < locations.size(); i++)
		{
			const std::string path = locations[i]->GetPath().GetFullPath();
			gl_platform->getDataManagerPtr()->addResourceLocation(path, true);
		}

		/*gl_platform->getDataManagerPtr()->addResourceLocation("C:/dev/construction-sim/trunk/data/gfx/GUI/MyGUI/Common/Demos", true);
		gl_platform->getDataManagerPtr()->addResourceLocation("C:/dev/construction-sim/trunk/data/gfx/GUI/MyGUI/Common/base", true);
		gl_platform->getDataManagerPtr()->addResourceLocation("C:/dev/construction-sim/trunk/data/gfx/GUI/MyGUI/Common/Themes", true);
		gl_platform->getDataManagerPtr()->addResourceLocation("C:/dev/construction-sim/trunk/data/gfx/GUI/MyGUI/MyGUI_Media", true);
		gl_platform->getDataManagerPtr()->addResourceLocation("C:/dev/construction-sim/trunk/data/gfx/GUI/MyGUI/GASS", true);
		*/

		
		InitGUI();


		/*MyGUI::FactoryManager::getInstance().registerFactory<MyGUI::RTTLayer>("Layer");
		MyGUI::ResourceManager::getInstance().load("RTTResources.xml");
		demo::MonitorPanel* mMonitorPanel = new demo::MonitorPanel();
		MyGUI::ILayer* layer = MyGUI::LayerManager::getInstance().getByName("RTT_Monitor", false);
		if (layer != nullptr)
		{
			MyGUI::RTTLayer* rttLayer = layer->castType<MyGUI::RTTLayer>();
			MyGUI::OpenGLTexture* tex = dynamic_cast<MyGUI::OpenGLTexture*>(rttLayer->GetTexture());
			unsigned int id = tex->getTextureID();

			osg::Vec3 center(0.0f,0.0f,40.0f);
			float radius = 100.0f;
			//osg::Group* root = new osg::Group;
			float baseHeight = center.z()-radius*0.5;
			//view->getSceneData()->asGroup()->addChild(baseModel);
			osg::BoundingBox bb(0.0f,0.0f,0.0f,10.0f,10.0f,10.0f);
			mview->getSceneData()->asGroup()->addChild(createFilterWall(bb,"C:/dev/foi/MSI-projects/GASSData/trunk/scenarios/osg/demo_scenario/gfx/textures/skybox/blue_sb_north.jpg",id));
			//mimage->dirty();
			my_tex_id = id;
		}*/

		return gl_platform;
	}

	void MyGUIOSGSystem::_SetupOSG(osgViewer::View* view)
	{
		m_HUDCamera = new osg::Camera;
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		geode->setCullingActive( false );
		geode->addDrawable( m_OSGManager.get() );
		geode->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
		geode->getOrCreateStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
		//geode->getOrCreateStateSet()->setAttribute(new osg::Program, osg::StateAttribute::PROTECTED | osg::StateAttribute::OVERRIDE); 
		//geode->setNodeMask(0x00000001);
		
		m_HUDCamera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
		m_HUDCamera->setRenderOrder( osg::Camera::POST_RENDER );
		m_HUDCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
		m_HUDCamera->setAllowEventFocus( false );
		m_HUDCamera->setProjectionMatrix( osg::Matrix::ortho2D(0.0, 1.0, 0.0, 1.0) );
		m_HUDCamera->addChild( geode.get() );
		
		IOSGGraphicsSystemPtr osg_sys = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IOSGGraphicsSystem>();
		osgViewer::CompositeViewer* viewer = osg_sys->GetViewer();
		osgViewer::Viewer::Windows windows;
		viewer->getWindows(windows);

		if (windows.empty()) 
			return;
		// set up cameras to render on the first window available.
		m_HUDCamera->setGraphicsContext(windows[0]);
		m_HUDCamera->setViewport(0,0, windows[0]->getTraits()->width, windows[0]->getTraits()->height);
		osgViewer::View* hudView = new osgViewer::View;
		hudView->setCamera(m_HUDCamera);
		viewer->addView(hudView);
		view->addEventHandler(new MYGUIOSGEventHandler(m_HUDCamera.get(), m_OSGManager.get()) );
		osgViewer::GraphicsWindow* gw = dynamic_cast<osgViewer::GraphicsWindow*>( windows[0] );
		if ( gw )
		{
			// Send window size for MyGUI to initialize
			int x, y, w, h; gw->getWindowRectangle( x, y, w, h );
			view->getEventQueue()->windowResize( x, y, w, h );
		}
	}

	void MyGUIOSGSystem::OnGraphicsSystemLoaded(GraphicsSystemLoadedEventPtr message)
	{
		//_SetupOSG();
	}

	void MyGUIOSGSystem::OnCameraChanged(CameraChangedEventPtr message)
	{
		CameraComponentPtr camera_comp = message->GetViewport()->GetCamera();
		OSGCameraPtr osg_cam = DYNAMIC_PTR_CAST<IOSGCamera>(camera_comp);
		osgViewer::View* view = dynamic_cast<osgViewer::View*>(osg_cam->GetOSGCamera()->getView());

		if(!m_HUDCamera.valid()) //create hud camera
		{
			_SetupOSG(view);
		}
		mview = view; 
		//Add render to texture plane
	}

	void MyGUIOSGSystem::Update(double delta_time)
	{
		if(mimage)
		{
			//glBindTexture(GL_TEXTURE_2D, my_tex_id);
			//mimage->readImageFromCurrentTexture(my_tex_id,true);
			//mimage->dirty();
			//my_texture->setImage(mimage);
			osg::GraphicsContext * gc = mview->getCamera()->getGraphicsContext();
			unsigned int contextID = gc->getState()->getContextID();
			osg::Texture::TextureObject* to = my_texture->getTextureObject(contextID);
			
			unsigned int id;
			if(to)
				id = to->id();
			int hej;
			hej=0;

		}
	}
}
