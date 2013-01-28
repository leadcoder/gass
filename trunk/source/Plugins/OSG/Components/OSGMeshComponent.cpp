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
		m_Initlized(false),
		m_Lighting(true),
		m_GeomFlags(GEOMETRY_FLAG_UNKOWN),
		m_Expand(false)
	{

	}

	OSGMeshComponent::~OSGMeshComponent()
	{

	}

	void OSGMeshComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("MeshComponent",new GASS::Creator<OSGMeshComponent, IComponent>);
		RegisterProperty<ResourceHandle>("Filename", &GetMeshResource, &SetMeshResource);
		RegisterProperty<bool>("CastShadow", &GetCastShadow, &SetCastShadow);
		RegisterProperty<bool>("ReceiveShadow", &GetReceiveShadow, &SetReceiveShadow);
		RegisterProperty<bool>("Lighting", &GetLighting, &SetLighting);
		RegisterProperty<bool>("Expand", &GetExpand, &SetExpand);
		RegisterEnumProperty<GeometryFlagsBinder>("GeometryFlags", &GetGeometryFlagsBinder, &SetGeometryFlagsBinder);
	}

	void OSGMeshComponent::SetGeometryFlagsBinder(GeometryFlagsBinder value)
	{
		SetGeometryFlags(value.GetValue());
	}

	GeometryFlagsBinder OSGMeshComponent::GetGeometryFlagsBinder() const
	{
		return GeometryFlagsBinder(GetGeometryFlags());
	}

	void OSGMeshComponent::SetGeometryFlags(GeometryFlags value)
	{
		m_GeomFlags = value;
		if(m_MeshNode.valid())
		{
			OSGConvert::Get().SetOSGNodeMask(value,m_MeshNode.get());
		}
	}

	GeometryFlags OSGMeshComponent::GetGeometryFlags() const
	{
		return m_GeomFlags;
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

	void OSGMeshComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGMeshComponent::OnLocationLoaded,LocationLoadedMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGMeshComponent::OnMaterialMessage,MaterialMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGMeshComponent::OnCollisionSettings,CollisionSettingsMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGMeshComponent::OnVisibilityMessage,VisibilityMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGMeshComponent::OnMeshFileNameMessage,MeshFileMessage,0));
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


	void OSGMeshComponent::OnLocationLoaded(LocationLoadedMessagePtr message)
	{
		LoadMesh(m_MeshResource.Name());
		if(m_MeshNode.get())
			CalulateBoundingbox(m_MeshNode.get());
		m_Initlized = true;
	}


	void OSGMeshComponent::SetMeshResource(const ResourceHandle &res)
	{
		m_MeshResource = res;
		if(m_Initlized) //not loaded
		{
			LoadMesh(m_MeshResource.Name());
			if(m_MeshNode.get())
				CalulateBoundingbox(m_MeshNode.get());
		}
	}

	void OSGMeshComponent::OnVisibilityMessage(VisibilityMessagePtr message)
	{
		bool visibility = message->GetValue();
		if(visibility)
		{
			//restore flags
			SetCastShadow(m_CastShadow);
			SetReceiveShadow(m_ReceiveShadow);
			SetGeometryFlags(m_GeomFlags);
		}
		else
		{
			//m_MeshNode->setNodeMask(~NM_VISIBLE & m_MeshNode->getNodeMask());
			m_MeshNode->setNodeMask(0);

		}
	}

	void OSGMeshComponent::LoadMesh(const ResourceHandle &filename)
	{
		if(!filename.Valid()) //not loaded
			return;

		SPTR<OSGLocationComponent> lc = GetSceneObject()->GetFirstComponentByClass<OSGLocationComponent>();
		if(!lc)
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed loading " + GetSceneObject()->GetName()  + ", not possible to use mesh components without location compoent","OSGMeshComponent::SetFilename");
		}

		
		//check if extension exist?
		FilePath file_path = filename.GetResource().Path();
		const std::string extension =  file_path.GetExtension();

		std::string  mod_file_path = file_path.GetFullPath();
		//hack to convert ogre meshes
		if(extension == "mesh") //this is ogre model, try to load 3ds instead
		{
			mod_file_path = Misc::Replace(mod_file_path,".mesh",".3ds");
		}

		if(m_MeshNode.valid())
		{
			lc->GetOSGNode()->removeChild(m_MeshNode.get());
			m_MeshNode.release();
		}

		//osgDB::Registry::instance()->getOptions()->setDatabasePath(path);
		//osgUtil::Optimizer optimizer;
		//optimizer.optimize(loadedModel.get());
		m_MeshNode = (osg::Group*) osgDB::readNodeFile((mod_file_path));

		if( ! m_MeshNode)
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to load mesh: " + mod_file_path,"OSGMeshComponent::SetFilename");
		}

		osgUtil::Optimizer optimizer;
		optimizer.optimize(m_MeshNode.get());

		OSGNodeData* data = new OSGNodeData(shared_from_this());
		m_MeshNode->setUserData(data);

		SetLighting(m_Lighting);
		SetCastShadow(m_CastShadow);
		SetReceiveShadow(m_ReceiveShadow);
		GetSceneObject()->PostMessage(MessagePtr(new GeometryChangedMessage(DYNAMIC_PTR_CAST<IGeometryComponent>(shared_from_this()))));

		//expand children
		if(m_Expand)
			Expand(GetSceneObject(),m_MeshNode.get(),m_Initlized);
	
		lc->GetOSGNode()->addChild(m_MeshNode.get());
		//update mask!
		SetGeometryFlags(m_GeomFlags);
	}


	bool OSGMeshComponent::GetExpand() const
	{
		return m_Expand;
	}

	void OSGMeshComponent::SetExpand(bool value)
	{
		m_Expand = value;
	}

	void OSGMeshComponent::Expand(SceneObjectPtr parent, osg::Node* node, bool load) 
	{
		if(node->asTransform())
		{
			//add top transform or replace current location?
			//move top transform to location!
			//node->asTransform()->asMatrixTransform()->getMatrix().getTrans();
			//SPTR<OSGLocationComponent> lc = GetSceneObject()->GetFirstComponentByClass<OSGLocationComponent>();
			//GetOSGNode()->setPosition();
			//osg::Matrix mat = node->asTransform()->asMatrixTransform()->getMatrix();
			//HACK: reset top transform to get zoom function to work on nodes that dont have location!
			osg::Matrix mat;
			mat.setTrans(osg::Vec3(0,0,0));
			node->asTransform()->asMatrixTransform()->setMatrix(mat);
		}

		if(node->asGroup())
		{
			for(unsigned int i = 0 ; i < node->asGroup()->getNumChildren(); i++)
			{
				osg::Node* child_node = node->asGroup()->getChild(i);
				ExpandRec(parent,child_node, load);
			}
		}
	}


	void OSGMeshComponent::ExpandRec(SceneObjectPtr parent, osg::Node* node, bool load) 
	{
		const std::string template_name = "OSGExpandedMeshObject";
		SceneObjectPtr so;
		try
		{
			so = DYNAMIC_PTR_CAST<SceneObject> (SimEngine::Get().GetSceneObjectTemplateManager()->CreateFromTemplate(template_name));
		}
		catch(...)
		{
			so = SceneObjectPtr(new SceneObject());
		}
		
		if (so) 
		{
			so->SetName(node->getName());
			so->SetID(node->getName());
			osg::Geode* geode = dynamic_cast<osg::Geode*> (node);
			if(geode)
			{
				
				//osg::PositionAttitudeTransform* trans = dynamic_cast<osg::PositionAttitudeTransform*> (child_node);
				/*if(child_node->asTransform())
				{
				osg::Vec3d pos = child_node->asTransform()->asMatrixTransform()->getMatrix().getTrans();
				LogManager::getSingleton().stream() << "found trans:" << pos.x() << " " << pos.y() << " " << pos.z();	

				}*/
				SPTR<OSGMeshComponent> mesh_comp(new OSGMeshComponent());
				OSGNodeData* node_data = new OSGNodeData(mesh_comp);
				node->setUserData(node_data);
				mesh_comp->SetMeshNode(node);
				so->AddComponent(mesh_comp);
				//load this object
			}
			
			parent->AddChildSceneObject(so,load);
			//epxand recursive
			if(node->asGroup())
			{
				for(unsigned int i = 0 ; i < node->asGroup()->getNumChildren(); i++)
				{
					osg::Node* child_node = node->asGroup()->getChild(i);
					Expand(so,child_node, load);
				}
			}
		}


		//create gass scene object
		/*if(node->asGroup())
		{
			for(unsigned int i = 0 ; i < node->asGroup()->getNumChildren(); i++)
			{
				osg::Node* child_node = node->asGroup()->getChild(i);
				//LogManager::getSingleton().stream() << "Try to expand OSG node:" << child_node->getName();	

				//use template if present!
				const std::string template_name = "OSGExpandedMeshObject";
				SceneObjectPtr so;
				try
				{
					so = DYNAMIC_PTR_CAST<SceneObject> (SimEngine::Get().GetSceneObjectTemplateManager()->CreateFromTemplate(template_name));
				}
				catch(std::exception& e)
				{
					so = SceneObjectPtr(new SceneObject());
				}
				if(so)
				{
					so->SetName(child_node->getName());
					so->SetID(child_node->getName());

					if (child_node->asTransform()) 
					{
						LogManager::getSingleton().stream() << "found child Transform";	
						//Expand(parent,child_node, load);
					}
					else if (child_node->asGroup())
					{
						LogManager::getSingleton().stream() << "found child  gorup";	
						//Expand(parent,child_node, load);
					} 
					//else 
					{
						osg::Geode* geode = dynamic_cast<osg::Geode*> (child_node);
						if (geode) 
						{

							//osg::PositionAttitudeTransform* trans = dynamic_cast<osg::PositionAttitudeTransform*> (child_node);
							/*if(child_node->asTransform())
							{
							osg::Vec3d pos = child_node->asTransform()->asMatrixTransform()->getMatrix().getTrans();
							LogManager::getSingleton().stream() << "found trans:" << pos.x() << " " << pos.y() << " " << pos.z();	

							}*/


				/*			SPTR<OSGMeshComponent> mesh_comp(new OSGMeshComponent());
							OSGNodeData* node_data = new OSGNodeData(mesh_comp);
							child_node->setUserData(node_data);
							mesh_comp->SetMeshNode(child_node);
							so->AddComponent(mesh_comp);
							//load this object
							parent->AddChildSceneObject(so,load);
							//epxand recursive
							Expand(so,child_node, load);
						}
					}
				}
			}
		}
		else if (node->asTransform()) 
		{
			LogManager::getSingleton().stream() << "found Transform";	
		}
		else
		{
			//use template if present!
			const std::string template_name = "OSGExpandedMeshObject";
			SceneObjectPtr so;
			try
			{
				so = DYNAMIC_PTR_CAST<SceneObject> (SimEngine::Get().GetSceneObjectTemplateManager()->CreateFromTemplate(template_name));
			}
			catch(std::exception& e)
			{
				so = SceneObjectPtr(new SceneObject());
			}
			so->SetName(node->getName());
			so->SetID(node->getName());
			if(so)
			{

				osg::Geode* geode = dynamic_cast<osg::Geode*> (node);
				if (geode) 
				{
					LogManager::getSingleton().stream() << "found Geode";	

					SPTR<OSGMeshComponent> mesh_comp(new OSGMeshComponent());
					OSGNodeData* node_data = new OSGNodeData(mesh_comp);
					node->setUserData(node_data);
					mesh_comp->SetMeshNode(node);
					so->AddComponent(mesh_comp);
					//load this object
					parent->AddChildSceneObject(so,load);
				}
			}
		}*/

	}

	void OSGMeshComponent::SetMeshNode(osg::ref_ptr<osg::Node> mesh)
	{
		m_MeshNode =mesh;
		CalulateBoundingbox(mesh.get());
	}


	void OSGMeshComponent::OnMeshFileNameMessage(MeshFileMessagePtr message)
	{
		ResourceHandle res(message->GetFileName());
		SetMeshResource(res);
	}

	AABox OSGMeshComponent::GetBoundingBox() const
	{
		return m_BBox;
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
				//SPTR<OSGLocationComponent> lc = GetSceneObject()->GetFirstComponentByClass<OSGLocationComponent>();
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
				OSGConvert::Get().SetOSGNodeMask(m_GeomFlags,m_MeshNode.get());
			else
				OSGConvert::Get().SetOSGNodeMask(GEOMETRY_FLAG_TRANSPARENT_OBJECT,m_MeshNode.get());
		}
	}
}
