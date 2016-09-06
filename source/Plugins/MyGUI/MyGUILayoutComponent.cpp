/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
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


#include "MyGUILayoutComponent.h"

#include <osg/NodeVisitor>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/Geode>
#include <osg/Image>
//avoid warning spam from MYGUI
#   pragma warning (disable : 4100)
#   pragma warning (disable : 4127)
#   pragma warning (disable : 4512)
#   pragma warning (disable : 4267)
#   pragma warning (disable : 4702)

#include <MyGUI.h>
#include "MyGUIOSGSystem.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/GASSSimEngine.h"
#include "Plugins/OSG/IOSGMesh.h"
#include "MyGUIMessages.h"

namespace GASS
{
	class MyGUIOSGTexture : public osg::Texture2D
	{
	public:
		MyGUIOSGTexture(unsigned int id): m_ID(id)
		{

		}
		virtual ~MyGUIOSGTexture()
		{

		}
		virtual void apply(osg::State& state) const
		{
			glBindTexture(GL_TEXTURE_2D, m_ID);
		}
		unsigned int m_ID;
	};

	class GeodeFinder : public osg::NodeVisitor {
	public:

		GeodeFinder (const std::string &tex_name,osg::ref_ptr<MyGUIOSGTexture> rtt_tex) : m_TextureName(tex_name),
			m_RTTTex(rtt_tex),
			NodeVisitor (osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}

		void apply (osg::Node &searchNode) 
		{
			if (! strcmp (searchNode.className(), "Geode")) 
			{
				osg::Geode* geode = dynamic_cast<osg::Geode*>(&searchNode);
				m_FoundGeodes.push_back (geode);
				
				for(int i = 0; i < geode->getNumDrawables(); i++)
				{
					osg::Drawable* drawable = geode->getDrawable(i);
					if(drawable)
					{
						osg::StateSet* stateset = drawable->getOrCreateStateSet();
						osg::Texture2D* tex = dynamic_cast<osg::Texture2D*>(stateset->getTextureAttribute(0,osg::StateAttribute::TEXTURE));
						if(tex)
						{
							osg::Image* image = tex->getImage();
							if(image)
							{
								const std::string file_name = image->getFileName();
								if(file_name.find(m_TextureName) != std::string::npos)
								{
									stateset->setTextureAttributeAndModes(0,m_RTTTex,osg::StateAttribute::ON);
									//replace this texture with our own!
								}
							}
						}
					}
				}
			}
			traverse (searchNode);
		}

		osg::Geode* getFirst () {
			if (m_FoundGeodes.size() > 0)
				return m_FoundGeodes.at(0);
			else
				return NULL;
		}

		std::vector<osg::Geode*> getNodeList() {
			return m_FoundGeodes;
		}

		// List of nodes with names that match the searchForName string
		std::vector<osg::Geode*> m_FoundGeodes;
		std::string m_TextureName;
		osg::ref_ptr<MyGUIOSGTexture> m_RTTTex;
	};

	MyGUILayoutComponent::MyGUILayoutComponent(void) 
	{

	}

	MyGUILayoutComponent::~MyGUILayoutComponent(void)
	{
		
	}

	void MyGUILayoutComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("MyGUILayoutComponent",new GASS::Creator<MyGUILayoutComponent, Component>);
		RegisterProperty<std::string>("TextureName", &GASS::MyGUILayoutComponent::GetTextureName, &GASS::MyGUILayoutComponent::SetTextureName);
		RegisterProperty<std::string>("LayoutFile", &GASS::MyGUILayoutComponent::GetLayoutFile, &GASS::MyGUILayoutComponent::SetLayoutFile);
	}

	void MyGUILayoutComponent::OnInitialize()
	{
		//wait for mesh data to load!
		GetSceneObject()->RegisterForMessage(REG_TMESS(MyGUILayoutComponent::OnGeometryChanged,GeometryChangedEvent ,0));
	}

	void MyGUILayoutComponent::OnGeometryChanged(GeometryChangedEventPtr message)
	{
		MyGUIOSGSystemPtr gui = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<MyGUIOSGSystem>();
		if(gui->IsInitialized())
		{
			_SetupLayout();
		}
		else //wait for GUI
		{
			SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(MyGUILayoutComponent::OnGUILoaded, GUILoadedEvent ,0));
		}
	}

	void MyGUILayoutComponent::OnGUILoaded(GUILoadedEventPtr message)
	{
		_SetupLayout();
	}

	void MyGUILayoutComponent::_SetupLayout()
	{
		m_Layout = MyGUI::LayoutManager::getInstance().loadLayout(m_LayoutFile.c_str());
		//MyGUI::ILayer* layer = MyGUI::LayerManager::getInstance().getByName("RTT_Panel", false);
		/*if (layer != nullptr)
		{
			MyGUI::RTTLayer* rttLayer = layer->castType<MyGUI::RTTLayer>();
			MyGUI::OpenGLTexture* tex = dynamic_cast<MyGUI::OpenGLTexture*>(rttLayer->GetTexture());
			unsigned int ogl_tex_id = tex->getTextureID();
			m_OSGRTTTexture  = new MyGUIOSGTexture(ogl_tex_id);
			OSGMeshPtr node= GetSceneObject()->GetFirstComponentByClass<IOSGMesh>();
			if(node && node->GetNode())
			{
				GeodeFinder finder(m_TextureName,m_OSGRTTTexture);
				node->GetNode()->accept (finder);
			}
		}*/
		GetSceneObject()->PostEvent(LayoutLoadedEventPtr(new LayoutLoadedEvent(m_Layout)));
	}

	std::string MyGUILayoutComponent::GetTextureName() const
	{
		return m_TextureName;
	}

	void MyGUILayoutComponent::SetTextureName(const std::string &value)
	{
		m_TextureName = value;
	}

	std::string MyGUILayoutComponent::GetLayoutFile() const
	{
		return m_LayoutFile;
	}

	void MyGUILayoutComponent::SetLayoutFile(const std::string &value)
	{
		m_LayoutFile = value;
	}

	
}
