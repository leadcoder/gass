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

#include <boost/bind.hpp>

#include "Core/Math/Quaternion.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSException.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Components/Graphics/GeometryCategory.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/Resource/IResourceSystem.h"
//#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include "Plugins/OSG/OSGGraphicsSystem.h"
#include "Plugins/OSG/Components/OSGLocationComponent.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/Components/OSGMeshComponent.h"
#include "Plugins/OSG/OSGNodeMasks.h"
#include "Plugins/OSG/OSGNodeData.h"

#include <osgDB/ReadFile> 
#include <osgUtil/Optimizer>
#include <osg/MatrixTransform>
#include <osgShadow/ShadowTechnique>
#include <osg/Material>
#include <osg/BlendFunc>



namespace GASS
{

	OSGMeshComponent::OSGMeshComponent() : m_CastShadow (false),
		m_ReceiveShadow  (false),
		m_ReadyToLoadMesh(false),
		m_Lighting(true),
		m_Category(GT_REGULAR)
	{
		
	}


	OSGMeshComponent::~OSGMeshComponent()
	{

	}

	void OSGMeshComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("MeshComponent",new GASS::Creator<OSGMeshComponent, IComponent>);
		RegisterProperty<std::string>("Filename", &GetFilename, &SetFilename);
		RegisterProperty<bool>("CastShadow", &GetCastShadow, &SetCastShadow);
		RegisterProperty<bool>("ReceiveShadow", &GetReceiveShadow, &SetReceiveShadow);
		RegisterProperty<bool>("Lighting", &GetLighting, &SetLighting);
		RegisterProperty<GeometryCategory>("GeometryCategory", &GetGeometryCategory, &SetGeometryCategory);
	}

	void OSGMeshComponent::SetGeometryCategory(const GeometryCategory &value)
	{
		m_Category = value;
		if(m_MeshNode.valid())
		{
			OSGGraphicsSceneManager::UpdateNodeMask(m_MeshNode.get(),value);
		}
	}

	GeometryCategory OSGMeshComponent::GetGeometryCategory() const
	{
		return m_Category;
	}

	void OSGMeshComponent::SetCastShadow(bool value)
	{
		m_CastShadow = value;
		if(m_CastShadow && m_MeshNode.valid())
			m_MeshNode->setNodeMask(NM_CAST_SHADOWS | m_MeshNode->getNodeMask());
		else if(m_MeshNode.valid())
		{
			m_MeshNode->setNodeMask(~NM_CAST_SHADOWS & m_MeshNode->getNodeMask());
		}
	}

	void OSGMeshComponent::SetReceiveShadow(bool value)
	{
		m_ReceiveShadow = value;
		if(m_ReceiveShadow && m_MeshNode.valid())
			m_MeshNode->setNodeMask(NM_RECEIVE_SHADOWS | m_MeshNode->getNodeMask());
		else if(m_MeshNode.valid())
		{
			m_MeshNode->setNodeMask(~NM_RECEIVE_SHADOWS & m_MeshNode->getNodeMask());
		}
	}

	void OSGMeshComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGMeshComponent::OnLoad,LoadGFXComponentsMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGMeshComponent::OnMaterialMessage,MaterialMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGMeshComponent::OnCollisionSettings,CollisionSettingsMessage ,0));
	}


	void OSGMeshComponent::OnMaterialMessage(MaterialMessagePtr message)
	{
		Vec4 diffuse = message->GetDiffuse();
		Vec3 ambient = message->GetAmbient();
		Vec3 specular = message->GetSpecular();
		Vec3 si = message->GetSelfIllumination();

		osg::ref_ptr<osg::Material> mat (new osg::Material);
		//Specifying the yellow colour of the object
		if( diffuse.x >= 0)
			mat->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(diffuse.x,diffuse.y,diffuse.z,diffuse.w));
		if( ambient.x >= 0)
			mat->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(ambient.x,ambient.y,ambient.z,1));
		if( specular.x >= 0)
			mat->setSpecular(osg::Material::FRONT_AND_BACK,osg::Vec4(specular.x,specular.y,specular.z,1));
		if( message->GetShininess() >= 0)
			mat->setShininess(osg::Material::FRONT_AND_BACK,message->GetShininess());
		if( si.x >= 0)
			mat->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(si.x,si.y,si.z,1));
		//Attaching the newly defined state set object to the node state set
		osg::ref_ptr<osg::StateSet> nodess (m_MeshNode->getOrCreateStateSet());
		nodess->setAttribute(mat.get());

		
		nodess->setAttributeAndModes( mat.get() , osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
        // Turn on blending
		if(diffuse.w < 1.0)
		{
			osg::ref_ptr<osg::BlendFunc> bf (new   osg::BlendFunc(osg::BlendFunc::SRC_ALPHA,  osg::BlendFunc::ONE_MINUS_SRC_ALPHA ));
			nodess->setAttributeAndModes(bf);
		}
	}


	void OSGMeshComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{
		//OSGGraphicsSceneManager* osg_sm = boost::any_cast<OSGGraphicsSceneManager*>(message->GetData("GraphicsSceneManager"));
		//assert(osg_sm);
		//TODO: get resource manager and get full path
	
		/*for(int i = 0; i < m_MeshNode->getNumChildren(); i++)
		{
			lc->GetOSGNode()->addChild(getChild(i));
		}*/
		m_ReadyToLoadMesh = true;
		SetFilename(m_Filename);
		if(m_MeshNode.get())
			CalulateBoundingbox(m_MeshNode.get());

		//std::cout << "Min:" << m_BBox.m_Min << " Max:" << m_BBox.m_Max << "\n";
	}

	

	void OSGMeshComponent::SetFilename(const std::string &filename)
	{
		m_Filename = filename;
		if(!m_ReadyToLoadMesh) //not loaded
			return;

		boost::shared_ptr<OSGLocationComponent> lc = GetSceneObject()->GetFirstComponentByClass<OSGLocationComponent>();
		if(!lc)
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed loading " + GetSceneObject()->GetName()  + ", not possible to use mesh components without location compoent","OSGMeshComponent::SetFilename");
		}
		
		std::string full_path;
		ResourceSystemPtr rs = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<IResourceSystem>();
		//check if extension exist?
		std::string extesion =  Misc::GetExtension(m_Filename);
		if(extesion == "mesh") //this is ogre model, try to load 3ds instead
		{
			m_Filename = Misc::Replace(m_Filename,".mesh",".3ds");
		}
		
		if(m_MeshNode.valid())
		{
			lc->GetOSGNode()->removeChild(m_MeshNode.get());
			m_MeshNode.release();
		}

		if(rs->GetFullPath(m_Filename,full_path))
		{
			std::string path = Misc::RemoveFilename(full_path);
			//osgDB::Registry::instance()->getOptions()->setDatabasePath(path);
			//osgUtil::Optimizer optimizer;
			//optimizer.optimize(loadedModel.get());
			m_MeshNode = (osg::Group*) osgDB::readNodeFile((full_path));
			
			if( ! m_MeshNode)
			{
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to load mesh: " + full_path,"OSGMeshComponent::SetFilename");
			}

			osgUtil::Optimizer optimizer;
		    optimizer.optimize(m_MeshNode.get());

			OSGNodeData* data = new OSGNodeData(shared_from_this());
			m_MeshNode->setUserData((osg::Referenced*)data);
			
			SetLighting(m_Lighting);
			SetCastShadow(m_CastShadow);
			SetReceiveShadow(m_ReceiveShadow);
			GetSceneObject()->PostMessage(MessagePtr(new GeometryChangedMessage(boost::shared_dynamic_cast<IGeometryComponent>(shared_from_this()))));
		}
		else 
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to find mesh: " + full_path,"OSGMeshComponent::SetFilename");

	
		lc->GetOSGNode()->addChild(m_MeshNode.get());
	}

	AABox OSGMeshComponent::GetBoundingBox() const
	{

		/*	if(m_MeshNode.get())
		{
		CalulateBoundingbox(m_MeshNode.get());
		}*/
		return m_BBox;//Convert::ToGASS(m_OgreEntity->getBoundingBox());

	}

	bool OSGMeshComponent::GetLighting() const
	{
		return m_Lighting;
	}

	void OSGMeshComponent::SetLighting(bool value)
	{
		m_Lighting = value;
		if(m_MeshNode.valid())
		{
			osg::ref_ptr<osg::StateSet> nodess = m_MeshNode->getOrCreateStateSet();
			if(value) nodess->setMode(GL_LIGHTING,osg::StateAttribute::ON);
			else nodess->setMode(GL_LIGHTING,osg::StateAttribute::OVERRIDE|osg::StateAttribute::OFF);
		}
	}


	Sphere OSGMeshComponent::GetBoundingSphere() const
	{
		Sphere sphere;
		sphere.m_Pos = Vec3(0,0,0);
		

		if(m_MeshNode.get())
		{
			osg::BoundingSphere bsphere = m_MeshNode->getBound();
			sphere.m_Radius = bsphere._radius;
		}
		//sphere.m_Radius = m_OgreEntity->getBoundingRadius();
		return sphere;
	}



	void OSGMeshComponent::CalulateBoundingbox(osg::Node* node, const osg::Matrix& M) 
	{
		if (!node)
			return;

		if (node->asTransform() && node->asTransform()->asMatrixTransform()) 
		{
			for (unsigned int i = 0; i < node->asGroup()->getNumChildren(); i++)
				CalulateBoundingbox(node->asGroup()->getChild(i),node->asTransform()->asMatrixTransform()->getMatrix() * M);
			return;
		} 
		else if (node->asGroup())
		{
			for (unsigned int i = 0; i < node->asGroup()->getNumChildren(); i++)
				CalulateBoundingbox(node->asGroup()->getChild(i),M);
			return;
		} 
		else 
		{
			osg::Geode* geode = dynamic_cast<osg::Geode*> (node);
			if (geode) 
			{
				//boost::shared_ptr<OSGLocationComponent> lc = GetSceneObject()->GetFirstComponentByClass<OSGLocationComponent>();
				//lc->GetOSGNode()->addChild(geode);
		
				osg::BoundingBox bbox = geode->getBoundingBox();

				/*osg::Matrix trans;
				trans.set(osg::Quat(Math::Deg2Rad(-90),osg::Vec3(1,0,0),
					Math::Deg2Rad(180),osg::Vec3(0,1,0),
					Math::Deg2Rad(0),osg::Vec3(0,0,1)));*/
				//osg::Vec3 t_max = bbox._max*trans;
				//osg::Vec3 t_min = bbox._min*trans;

				osg::Vec3d p1 = bbox._max*M;
				osg::Vec3d p2 = bbox._min*M;

				AABox box;
				box.Union(OSGConvert::Get().ToGASS(p1));
				box.Union(OSGConvert::Get().ToGASS(p2));
				m_BBox.Union(box);
				//shape.M = osg::Matrix::translate(bbox.center()) * M;
				//shape.p1 = max3(bbox.xMax()-bbox.xMin(), bbox.yMax()-bbox.yMin(), shape.p3 = bbox.zMax()-bbox.zMin())/2.0;
				// Traverse drawables
				for (unsigned int i = 0; i < geode->getNumDrawables(); i++) 
				{
					osg::BoundingBox bbox = geode->getDrawable(i)->getBound();
					AABox box;

					osg::Vec3d p1 = bbox._max*M;
					osg::Vec3d p2 = bbox._min*M;

					box.Union(OSGConvert::Get().ToGASS(p1));
					box.Union(OSGConvert::Get().ToGASS(p2));
					m_BBox.Union(box);
				}
			}
		}
	}

	void OSGMeshComponent::GetMeshData(MeshDataPtr mesh_data) const
	{
		mesh_data->NumVertex = 0;
		mesh_data->VertexVector = NULL;
		mesh_data->NumFaces = 0;
		mesh_data->FaceVector = NULL;

		if(!m_MeshNode.valid())
		{
			LogManager::getSingleton().stream() << "WARNING:You have to load mesh before trying to fetch mesh data. Mesh name: " << GetName();
			return;
		}
		DrawableVisitor<TriangleRecorder> mv;	
		m_MeshNode->accept(mv);

		mesh_data->NumVertex = mv.mFunctor.mVertices.size();
		mesh_data->VertexVector = new Vec3[mv.mFunctor.mVertices.size()];
		mesh_data->NumFaces = mv.mFunctor.mTriangles.size();
		mesh_data->FaceVector = new unsigned int[mv.mFunctor.mTriangles.size()*3];

		if( !mv.mFunctor.mVertices.empty() ) 
		{
			memcpy(  mesh_data->VertexVector,
				&mv.mFunctor.mVertices[0],
				mv.mFunctor.mVertices.size()*sizeof(StridedVertex) );
		}

		if( !mv.mFunctor.mTriangles.empty() )
		{
			memcpy(  mesh_data->FaceVector,
				&mv.mFunctor.mTriangles[0],
				mv.mFunctor.mTriangles.size()*sizeof(StridedTriangle) );
		}
	}

	void OSGMeshComponent::OnCollisionSettings(CollisionSettingsMessagePtr message)
	{
		if(m_MeshNode.valid())
		{
			if(message->EnableCollision())
				SetGeometryCategory(m_Category);
			else
				OSGGraphicsSceneManager::UpdateNodeMask(m_MeshNode.get(),GeometryCategory(GT_UNKNOWN));
		}
	}
}