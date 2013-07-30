/****************************************************************************
* This file is part of GASS.                                                *
* See http://sourceforge.net/projects/gass/                                 *
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



#include "Core/Common.h"

#   pragma warning (disable : 4541)
#include <osgDB/ReadFile>
#include <osgSim/MultiSwitch>
#include <osg/MatrixTransform>
#include <osg/CullSettings>
#include <osg/PositionAttitudeTransform>
#include <osgGA/TrackballManipulator>
#include <osgViewer/CompositeViewer>
#include <osgGA/StateSetManipulator>
#include <osgShadow/ShadowedScene>
#include <osgShadow/ShadowVolume>
#include <osgShadow/ShadowTexture>
#include <osgShadow/ShadowMap>
#include <osgShadow/SoftShadowMap>
#include <osgShadow/ParallelSplitShadowMap>
#include <osgShadow/LightSpacePerspectiveShadowMap>
#include <osgShadow/StandardShadowMap>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Depth>
#include <osg/Point>
#include <osg/StateAttribute>
#include <osg/Material>
#include <osg/BlendFunc>
#include <osg/CullFace>

#if defined(WIN32) && !defined(__CYGWIN__) 
#include <osgViewer/api/Win32/GraphicsWindowWin32>
typedef HWND WindowHandle; 
typedef osgViewer::GraphicsWindowWin32::WindowData WindowData; 

#elif defined(__APPLE__) // Assume using Carbon on Mac. 
#include <osgViewer/api/Carbon/GraphicsWindowCarbon> 
typedef WindowRef WindowHandle; 
typedef osgViewer::GraphicsWindowCarbon::WindowData WindowData; 

#else // all other unix 
#include <osgViewer/api/X11/GraphicsWindowX11> 
typedef Window WindowHandle; 
typedef osgViewer::GraphicsWindowX11::WindowData WindowData; 
#endif 

#include "Plugins/OSG/OSGGraphicsSystem.h"
#include "Plugins/OSG/OSGRenderWindow.h"
#include "Plugins/OSG/Utils/TextBox.h"
#include "Plugins/OSG/Components/OSGCameraComponent.h"
#include "Plugins/OSG/Components/OSGCameraManipulatorComponent.h"
#include "Plugins/OSG/IOSGCameraManipulator.h"
#include "Sim/GASS.h"

#include <osgDB/ReadFile> 
#include "tinyxml.h"


namespace GASS
{

	OSGGraphicsSystem::OSGGraphicsSystem(void) : m_ShadowSettingsFile("GASS.xml"), 
		m_DebugTextBox(new TextBox()),
		m_Viewer(NULL),
		m_FlipDSS(false)
	{

	}

	OSGGraphicsSystem::~OSGGraphicsSystem(void)
	{
		if(m_Viewer)
		{
			osgDB::Registry::instance()->closeAllLibraries();
			m_Viewer->setDone(true);
			Sleep(1000);
			m_Viewer->stopThreading();
			Sleep(1000);
		}
		delete m_Viewer;
	}

	void OSGGraphicsSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("OSGGraphicsSystem",new GASS::Creator<OSGGraphicsSystem, ISystem>);
		//RegisterProperty<bool>("CreateMainWindowOnInit", &GASS::OSGGraphicsSystem::GetCreateMainWindowOnInit, &GASS::OSGGraphicsSystem::SetCreateMainWindowOnInit);
		RegisterProperty<std::string>("ShadowSettingsFile", &GASS::OSGGraphicsSystem::GetShadowSettingsFile, &GASS::OSGGraphicsSystem::SetShadowSettingsFile);
		RegisterProperty<bool>("FlipDSS", &GASS::OSGGraphicsSystem::GetFlipDSS, &GASS::OSGGraphicsSystem::SetFlipDSS);
	}

	void OSGGraphicsSystem::LoadXML(TiXmlElement *elem)
	{
		TiXmlElement *prop_elem = elem->FirstChildElement();
		while(prop_elem)
		{
			std::string prop_name = prop_elem->Value();
			if(prop_name == "ShadowSettings")
			{
				LoadShadowSettings(prop_elem);		
			}
			else
			{
				std::string prop_val = prop_elem->FirstAttribute()->Value();
				SetPropertyByString(prop_name,prop_val);
			}
			prop_elem  = prop_elem->NextSiblingElement();
		}
	}


	
	void OSGGraphicsSystem::Init()
	{
		SimEngine::Get().GetRuntimeController()->Register(shared_from_this(),m_TaskNodeName);

		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OSGGraphicsSystem::OnViewportMovedOrResized,ViewportMovedOrResizedEvent,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OSGGraphicsSystem::OnDebugPrint,DebugPrintRequest,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OSGGraphicsSystem::OnInitializeTextBox,CreateTextBoxRequest ,0));

		m_Viewer = new osgViewer::CompositeViewer();
		m_Viewer->setThreadingModel( osgViewer::Viewer::SingleThreaded);
		m_Viewer->setKeyEventSetsDone(0);

		std::string full_path;

		ResourceManagerPtr rm = SimEngine::Get().GetResourceManager();
		FileResourcePtr font_res = rm->GetFirstResourceByName("arial.ttf");
		m_DebugTextBox->setPosition(osg::Vec3d(0, 5, 0));
		m_DebugTextBox->setFont(font_res->Path().GetFullPath());
		m_DebugTextBox->setTextSize(12);


		/*if(m_CreateMainWindowOnInit)
		{
		osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
		if (!wsi) 
		{
		GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No WindowSystemInterface available, cannot create windows","OSGGraphicsSystem::OnInit");
		//osg::notify(osg::NOTICE)<<"Error, no WindowSystemInterface available, cannot create windows."<<std::endl;
		return;
		}

		CreateRenderWindow("MainWindow",800, 600,0);
		CreateViewport("MainViewport","MainWindow", 0, 0, 800, 600);
		}*/


		//Load shadow settings
		if(m_ShadowSettingsFile != "")
		{
			TiXmlDocument *xmlDoc = new TiXmlDocument(m_ShadowSettingsFile.c_str());
			if (!xmlDoc->LoadFile())
			{
				//Fatal error, cannot load
				LogManager::getSingleton().stream() << "WARNING: OSGGraphicsSystem::OnInit - Couldn't load shadow settings from: " <<  m_ShadowSettingsFile;
			}
			else
			{
				//TiXmlElement *ss= xmlDoc->FirstChildElement("ShadowSettings");
				TiXmlElement *ss= xmlDoc->FirstChildElement("Systems");
				if(ss)
				{
					ss = ss->FirstChildElement("OSGGraphicsSystem");
					if(ss)
						ss = ss->FirstChildElement("ShadowSettings");
				}
				LoadShadowSettings(ss);
			}
		}
		//osgDB::ReaderWriter::Options* options = new osgDB::ReaderWriter::Options; 

		osgDB::ReaderWriter::Options* opt = osgDB::Registry::instance()->getOptions(); 
		if (opt == NULL) 
		{ 
			opt = new osgDB::ReaderWriter::Options(); 
		} 

		if(m_FlipDSS)
		{
			const std::string options = opt->getOptionString();
			opt->setOptionString("dds_flip"); 
			osgDB::Registry::instance()->setOptions(opt); 
		}

		//opt->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_ALL); 
		//osgDB::Registry::instance()->setOptions(opt); 
		GetSimSystemManager()->SendImmediate(SystemMessagePtr(new GraphicsSystemLoadedEvent()));
	}


	RenderWindowVector OSGGraphicsSystem::GetRenderWindows() const
	{
		RenderWindowVector ret;
		for(size_t i = 0; i < m_Windows.size();i++)
		{
			ret.push_back(m_Windows[i]);
		}
		return ret;
	}

	void OSGGraphicsSystem::OnDebugPrint(DebugPrintRequestPtr message)
	{
		std::string debug_text = message->GetText();
		m_DebugTextBox->setText(m_DebugTextBox->getText() + "\n" + debug_text);
	}

	/*void OSGGraphicsSystem::SetActiveData(osg::Group* root)
	{
		osgViewer::ViewerBase::Views views;
		m_Viewer->getViews(views);
		root->addChild(&m_DebugTextBox->getGroup());
		//set same scene in all viewports for the moment
		for(int i = 0; i < views.size(); i++)
		{
			views[i]->setSceneData(root);
		}
		//m_Viewer->realize();
	}*/


	RenderWindowPtr OSGGraphicsSystem::GetMainRenderWindow() const
	{
		RenderWindowPtr main_win;
		if(m_Windows.size() > 0)
			main_win =  m_Windows[0];
		return main_win;
	}


	RenderWindowPtr OSGGraphicsSystem::CreateRenderWindow(const std::string &name, int width, int height, void* external_handle)
	{

		osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
		if (!wsi)
	    {
			
			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"Failed to get WindowingSystemInterface", "OSGGraphicsSystem::CreateRenderWindow");
	    }

		osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();

		
		

		osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits(ds);

		traits->readDISPLAY();

		traits->screenNum = 0;
		traits->x = 50;
		traits->y = 50;
		traits->width = width;
		traits->height = height;
		traits->doubleBuffer = true;
		traits->sharedContext = 0;
		if(m_Windows.size() > 0)
		{
			traits->sharedContext = m_Windows[0]->GetOSGWindow();
		}

		if(external_handle) //external window
		{
			osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData((HWND)external_handle);
			traits->windowDecoration = false;
			traits->setInheritedWindowPixelFormat = true;
			traits->inheritedWindowData = windata;
		}
		else 
		{
			traits->windowDecoration = true;
			traits->windowName = name;
		}

		osg::ref_ptr<osg::GraphicsContext> graphics_context = osg::GraphicsContext::createGraphicsContext(traits.get());
		if (graphics_context.valid())
		{
			//osg::notify(osg::INFO)<<"  GraphicsWindow has been created successfully."<<std::endl;
			//need to ensure that the window is cleared make sure that the complete window is set the correct colour
			//rather than just the parts of the window that are under the camera's viewports
			graphics_context->setClearColor(osg::Vec4f(0.0f,0.0f,1.0f,1.0f));
			graphics_context->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			osgViewer::GraphicsWindow* gw = dynamic_cast<osgViewer::GraphicsWindow*>(graphics_context.get());
			gw->getEventQueue()->getCurrentEventState()->setWindowRectangle(0, 0, width, height );

		}
		else
		{
			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"Failed to create createGraphicsContext for:" + name, "OSGGraphicsSystem::CreateRenderWindow");
		}

		OSGRenderWindowPtr win(new  OSGRenderWindow(this,graphics_context));
		m_Windows.push_back(win);
		return win;

		//if(m_Windows.size() == 1) //first window created?
		/*{
		if(win_handle == 0) //internal window
		{
		#if defined(WIN32) && !defined(__CYGWIN__) 	
		osgViewer::GraphicsWindowWin32* win32_window = (osgViewer::GraphicsWindowWin32*)(graphics_context.get());
		win_handle = (void*) win32_window->getHWND();
		#endif
		}
		SystemMessagePtr window_msg(new RenderWindowCreatedEvent(win_handle));
		GetSimSystemManager()->SendImmediate(window_msg);
		}*/
	}

	/*void OSGGraphicsSystem::CreateViewport(const std::string &name, const std::string &render_window, float  left, float top, float width, float height)
	{
	if(m_Windows.find(render_window) != m_Windows.end())
	{
	//
	osgViewer::View* view = new osgViewer::View;
	view->setName(name);

	int win_w = m_Windows[render_window]->getTraits()->width;
	int win_h = m_Windows[render_window]->getTraits()->height;

	int p_left = win_w*left;
	int p_top = win_h*top;

	int p_width = win_w*width;
	int p_height = win_h*height;

	m_Viewer->addView(view);
	view->getCamera()->setViewport(new osg::Viewport(p_left, p_top, p_width,p_height));
	view->getCamera()->setComputeNearFarMode(osgUtil::CullVisitor::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
	view->setLightingMode(osg::View::SKY_LIGHT); 
	view->getDatabasePager()->setDoPreCompile( true );
	view->getDatabasePager()->setTargetMaximumNumberOfPageLOD(100);
	// add some stock OSG handlers:

	osgViewer::StatsHandler* stats = new osgViewer::StatsHandler();
	stats->setKeyEventTogglesOnScreenStats('y');
	stats->setKeyEventPrintsOutStats(0);

	view->addEventHandler(stats);

	view->addEventHandler(new osgViewer::WindowSizeHandler());
	view->addEventHandler(new osgViewer::ThreadingHandler());
	view->addEventHandler(new osgViewer::LODScaleHandler());

	osgGA::StateSetManipulator* ssm =  new osgGA::StateSetManipulator(view->getCamera()->getOrCreateStateSet());
	ssm->setKeyEventCyclePolygonMode('p');
	ssm->setKeyEventToggleTexturing('o');
	view->addEventHandler(ssm);
	//    view->addEventHandler(new osgViewer::HelpHandler(arguments.getApplicationUsage()));
	view->getCamera()->setGraphicsContext(m_Windows[render_window]);
	view->getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
	}
	}*/

	void OSGGraphicsSystem::ChangeCamera(const std::string &viewport, OSGCameraComponentPtr cam_comp)
	{
		osgViewer::ViewerBase::Views views;
		GetViewer()->getViews(views);

		for(int i = 0; i < views.size(); i++)
		{
			if(views[i]->getName() == viewport || viewport == "ALL")
			{
				cam_comp->SetOSGCamera( views[i]->getCamera());
				//set manipulator
				OSGCameraManipulatorPtr man = cam_comp->GetSceneObject()->GetFirstComponentByClass<IOSGCameraManipulator>();
				if(man)
					views[i]->setCameraManipulator(man->GetManipulator());
				else 
					views[i]->setCameraManipulator(NULL);
			}
		}
	}


	/*void OSGGraphicsSystem::GetMainWindowInfo(unsigned int &width, unsigned int &height, int &left, int &top) const
	{
	if(m_Windows.size() > 0)
	{
	const osg::GraphicsContext::Traits* traits = m_Windows.begin()->second->getTraits();
	width = traits->width;
	height = traits->height;
	left = traits->x;
	top = traits->y;
	}
	}*/


	void OSGGraphicsSystem::OnViewportMovedOrResized(ViewportMovedOrResizedEventPtr message)
	{

		osgViewer::ViewerBase::Views views;

		m_Viewer->getViews(views);
		//set same size in all viewports for the moment
		for(int i = 0; i < views.size(); i++)
		{
			if(views[i]->getName() == message->GetViewportName())
			{
				views[i]->getCamera()->setViewport(message->GetPositionX(),message->GetPositionY(),message->GetWidth(), message->GetHeight());
			}
		}
	}

	void OSGGraphicsSystem::Update(double delta_time)
	{
		static int tick = 0;

		if(m_Viewer->done())
		{
			return;
		}
		//m_Viewer->setRunMaxFrameRate(100);
		m_Viewer->frame(delta_time);
		m_DebugTextBox->setText("");
		//update listeners
		SimSystem::Update(delta_time);
	}


	void OSGGraphicsSystem::OnInitializeTextBox(CreateTextBoxRequestPtr message)
	{
		if(m_TextBoxes.end() == m_TextBoxes.find(message->m_BoxID))
		{
			TextBox* text_box = new TextBox();

			std::string full_path;
			ResourceManagerPtr rm = SimEngine::Get().GetResourceManager();
			FileResourcePtr font_res = rm->GetFirstResourceByName("arial.ttf");

			text_box->setPosition(osg::Vec3d(0, 0, 0));
			text_box->setFont(font_res->Path().GetFullPath());
			text_box->setTextSize(10);

			m_Viewer->getView(0)->getSceneData()->asGroup()->addChild(&text_box->getGroup());

			m_TextBoxes[message->m_BoxID] = text_box;
		}

		TextBox* text_box = m_TextBoxes[message->m_BoxID];

		text_box->setText(message->m_Text);
		text_box->setPosition(osg::Vec3d(message->m_PosX, message->m_PosY, 0));
		Vec4 color = message->m_Color;
		text_box->setColor(osg::Vec4(color.x,color.y,color.z,color.w));
	}

	void OSGGraphicsSystem::LoadShadowSettings(TiXmlElement *shadow_elem)
	{
		//Load shadow settings
		//TiXmlElement *shadow_elem= elem->FirstChildElement("ShadowSettings");

		if(shadow_elem)
		{
			std::string type = shadow_elem->Attribute("type");

			if(type == "ShadowVolume")
			{
				//GetViewer()->setThreadingModel(osgViewer::Viewer::SingleThreaded);
				// hint to tell viewer to request stencil buffer when setting up windows
				osg::DisplaySettings::instance()->setMinimumNumStencilBits(8);
				osg::ref_ptr<osgShadow::ShadowVolume> sv = new osgShadow::ShadowVolume;
				sv->setDynamicShadowVolumes(true);
				//sv->setDrawMode(osgShadow::ShadowVolumeGeometry::STENCIL_TWO_SIDED);
				//sv->setDrawMode(osgShadow::ShadowVolumeGeometry::STENCIL_TWO_PASS);
				m_ShadowTechnique  = sv;
			}
			else if(type == "ShadowTexture")
			{
				osg::ref_ptr<osgShadow::ShadowTexture> st = new osgShadow::ShadowTexture;
				m_ShadowTechnique  = st;
			}
			else if(type == "ShadowMap")
			{
				osg::ref_ptr<osgShadow::ShadowMap> sm = new osgShadow::ShadowMap;
				m_ShadowTechnique = sm;
			}
			else if(type == "StandardShadowMap")
			{
				osg::ref_ptr<osgShadow::StandardShadowMap> ssm = new osgShadow::StandardShadowMap;
				m_ShadowTechnique = ssm;
			}
			else if(type == "ParallelSplitShadowMap")
			{
				// pssm isn't yet thread safe
				//GetViewer()->setThreadingModel(osgViewer::Viewer::SingleThreaded);
				int mapcount = 4;
				int mapres = 1024;
				float maxFarPlane = 200;
				float minNearDistanceForSplits = 10;
				float moveVCamBehindRCamFactor = 1;

				shadow_elem->QueryFloatAttribute("MaxFarDistance",&maxFarPlane);
				shadow_elem->QueryIntAttribute("TextureSize",&mapres);
				shadow_elem->QueryIntAttribute("NumCount",&mapcount);
				shadow_elem->QueryFloatAttribute("MinNearDistanceForSplits",&minNearDistanceForSplits);
				shadow_elem->QueryFloatAttribute("MoveVCamBehindRCamFactor",&moveVCamBehindRCamFactor);

				osg::ref_ptr<osgShadow::ParallelSplitShadowMap> pssm = new osgShadow::ParallelSplitShadowMap(NULL,mapcount);

				pssm->setTextureResolution(mapres);
				pssm->setMaxFarDistance(maxFarPlane);
				pssm->setMoveVCamBehindRCamFactor(moveVCamBehindRCamFactor);
				pssm->setMinNearDistanceForSplits(minNearDistanceForSplits);

				double polyoffsetfactor = pssm->getPolygonOffset().x();
				double polyoffsetunit   = pssm->getPolygonOffset().y();

				shadow_elem->QueryDoubleAttribute("PolyOffsetFactor",&polyoffsetfactor);
				shadow_elem->QueryDoubleAttribute("PolyOffsetUnit",&polyoffsetunit);

				pssm->setPolygonOffset(osg::Vec2(polyoffsetfactor ,polyoffsetunit )); 


				m_ShadowTechnique = pssm;
			}
			else if(type == "LightSpacePerspectiveShadowMap")
			{
				osg::ref_ptr<osgShadow::MinimalShadowMap> sm = NULL;

				std::string sub_type = shadow_elem->Attribute("SubType");
				if(sub_type == "Draw")
					sm = new osgShadow::LightSpacePerspectiveShadowMapDB;
				else if(sub_type == "Cull")
					sm = new osgShadow::LightSpacePerspectiveShadowMapCB;
				else if(sub_type == "Frustum")
					sm = new osgShadow::LightSpacePerspectiveShadowMapVB;
				else
					sm = new osgShadow::LightSpacePerspectiveShadowMapDB;



				if( sm.valid() ) 
				{


					float minLightMargin = 20.f;
					float maxFarPlane = 500;
					int texSize = 1024;
					int baseTexUnit = 0;
					int shadowTexUnit = 1;

					shadow_elem->QueryFloatAttribute("MinLightMargin",&minLightMargin);
					shadow_elem->QueryFloatAttribute("MaxFarPlane",&maxFarPlane);
					shadow_elem->QueryIntAttribute("TextureSize",&texSize);
					shadow_elem->QueryIntAttribute("BaseTextureUnit",&baseTexUnit);
					shadow_elem->QueryIntAttribute("ShadowTextureUnit",&shadowTexUnit);

					sm->setMinLightMargin( minLightMargin );
					sm->setMaxFarPlane( maxFarPlane );
					sm->setTextureSize( osg::Vec2s( texSize, texSize ) );
					sm->setShadowTextureCoordIndex( shadowTexUnit );
					sm->setShadowTextureUnit( shadowTexUnit );
					sm->setBaseTextureCoordIndex( baseTexUnit );
					sm->setBaseTextureUnit( baseTexUnit );

					sm->setMainVertexShader( NULL ); 
					sm->setShadowVertexShader(NULL);


					osg::Shader* mainFragmentShader = new osg::Shader( osg::Shader::FRAGMENT,
						" // following expressions are auto modified - do not change them:       \n"
						" // gl_TexCoord[0]  0 - can be subsituted with other index              \n"
						"                                                                        \n"
						"float DynamicShadow( );                                                 \n"
						"                                                                        \n"
						"uniform sampler2D baseTexture;                                          \n"
						"                                                                        \n"
						"void main(void)                                                         \n"
						"{                                                                       \n"
						"  vec4 colorAmbientEmissive = gl_FrontLightModelProduct.sceneColor;     \n"
						"  // Add ambient from Light of index = 0                                \n"
						"  colorAmbientEmissive += gl_FrontLightProduct[0].ambient;              \n"
						"  vec4 color = texture2D( baseTexture, gl_TexCoord[0].xy );             \n"
						"  color *= mix( colorAmbientEmissive, gl_Color, DynamicShadow() );      \n"
						//"  const float LOG2E = 1.442692;	// = 1/log(2)                        \n"
						//"  float fog = exp2(-gl_Fog.density * abs(gl_FogFragCoord) * LOG2E);     \n"
						//"  fog = clamp(fog, 0.0, 1.0);                                            \n"
						"  float fog = clamp((gl_Fog.end - abs(gl_FogFragCoord))*gl_Fog.scale, 0.0,1.0);\n"
						"  color.rgb = mix( gl_Fog.color.rgb, color.rgb, fog );                  \n"
						"  gl_FragColor = color;                                                 \n"
						"} \n" );  

					sm->setMainFragmentShader(mainFragmentShader);

					/*osg::Shader* shadowFragmentShader = new osg::Shader( osg::Shader::FRAGMENT,
					" // following expressions are auto modified - do not change them:      \n"
					" // gl_TexCoord[1]  1 - can be subsituted with other index             \n"
					"                                                                       \n"
					"uniform sampler2DShadow shadowTexture;                                 \n"
					"                                                                       \n"
					"float DynamicShadow( )                                                 \n"
					"{                                                                      \n"
					"    return shadow2DProj( shadowTexture, gl_TexCoord[1] ).r;            \n"
					"} \n" );*/


					m_ShadowTechnique = sm;



				} 
			}
		}
	}


	void OSGGraphicsSystem::UpdateStateSet(osg::ref_ptr<osg::StateSet> state_set, const GraphicsMaterial &material)
	{
		ColorRGBA diffuse = material.GetDiffuse();
		ColorRGB ambient = material.GetAmbient();
		ColorRGB specular = material.GetSpecular();
		ColorRGB si = material.GetSelfIllumination();

		osg::ref_ptr<osg::Material> mat (new osg::Material);
		if( diffuse.r >= 0)
			mat->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(diffuse.r,diffuse.g,diffuse.b,diffuse.a));
		if( ambient.r >= 0)
			mat->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(ambient.r,ambient.g,ambient.b,1));
		if( specular.r >= 0)
			mat->setSpecular(osg::Material::FRONT_AND_BACK,osg::Vec4(specular.r,specular.g,specular.b,1));
		if( material.GetShininess() >= 0)
			mat->setShininess(osg::Material::FRONT_AND_BACK,material.GetShininess());
		if( si.r >= 0)
			mat->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(si.r,si.g,si.b,1));
		
		state_set->setAttribute(mat.get());
		if(material.GetDepthTest())
			state_set->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
		else
			state_set->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
		
		state_set->setAttributeAndModes( mat.get() , osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
        // Turn on blending
		if(diffuse.a < 1.0)
		{
			osg::ref_ptr<osg::BlendFunc> bf (new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA,  osg::BlendFunc::ONE_MINUS_SRC_ALPHA ));
			state_set->setAttributeAndModes(bf);

			// Enable blending, select transparent bin.
			state_set->setMode( GL_BLEND, osg::StateAttribute::ON );
			state_set->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

			// Enable depth test so that an opaque polygon will occlude a transparent one behind it.
			state_set->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );

			// Conversely, disable writing to depth buffer so that
			// a transparent polygon will allow polygons behind it to shine through.
			// OSG renders transparent polygons after opaque ones.
			osg::ref_ptr<osg::Depth> depth (new osg::Depth);
			depth->setWriteMask( false );
			state_set->setAttributeAndModes( depth, osg::StateAttribute::ON );
		}
		else //restore blending
		{

		}
	}
}









