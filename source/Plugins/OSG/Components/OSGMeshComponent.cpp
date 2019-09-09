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

#include "Plugins/OSG/Components/OSGMeshComponent.h"
#include "Plugins/OSG/Components/OSGLocationComponent.h"
#include "Plugins/OSG/OSGGraphicsSystem.h"
#include "Plugins/OSG/OSGGeometryRecorder.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/OSGNodeMasks.h"
#include "Plugins/OSG/OSGNodeData.h"
#include "Sim/GASSResourceManager.h"

namespace GASS
{
	std::vector<std::string> OSGMeshEnumerationMetaData::GetEnumeration(BaseReflectionObjectPtr object) const
	{
		std::vector<std::string> content;
		OSGMeshComponentPtr mesh = GASS_DYNAMIC_PTR_CAST<OSGMeshComponent>(object);
		if(mesh)
		{
			content = mesh->GetAvailableMeshFiles();
		}
		return content;
	}

	OSGMeshComponent::OSGMeshComponent() : m_CastShadow (false),
		m_ReceiveShadow  (false),
		m_Initlized(false),
		m_Lighting(true),
		m_GeomFlags(GEOMETRY_FLAG_UNKNOWN),
		m_Expand(false),
		m_FlipDDS(false),
		m_Collision(true)
	{

	}

	OSGMeshComponent::~OSGMeshComponent()
	{

	}

	void OSGMeshComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<OSGMeshComponent>("MeshComponent");
		ADD_DEPENDENCY("OSGLocationComponent")
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("MeshComponent", OF_VISIBLE)));

		RegisterProperty<ResourceHandle>("Filename", &OSGMeshComponent::GetMeshResource, &OSGMeshComponent::SetMeshResource,
			OSGMeshEnumerationMetaDataPtr(new OSGMeshEnumerationMetaData("Mesh File",PF_VISIBLE)));
		RegisterMember("EnumerationResourceGroup", &OSGMeshComponent::m_EnumerationResourceGroup,PF_VISIBLE,"EnumerationResourceGroup");
		RegisterGetSet("CastShadow", &OSGMeshComponent::GetCastShadow, &OSGMeshComponent::SetCastShadow,PF_VISIBLE | PF_EDITABLE,"Should this mesh cast shadows or not");
		RegisterGetSet("ReceiveShadow", &OSGMeshComponent::GetReceiveShadow, &OSGMeshComponent::SetReceiveShadow,PF_VISIBLE | PF_EDITABLE,"Should this mesh receive shadows or not");
		RegisterGetSet("Lighting", &OSGMeshComponent::GetLighting, &OSGMeshComponent::SetLighting,PF_VISIBLE | PF_EDITABLE,"Enable Light for this mesh");
		RegisterGetSet("Expand", &OSGMeshComponent::GetExpand, &OSGMeshComponent::SetExpand,PF_VISIBLE,"Expand mesh child nodes");
		RegisterGetSet("GeometryFlags", &OSGMeshComponent::GetGeometryFlagsBinder, &OSGMeshComponent::SetGeometryFlagsBinder, PF_VISIBLE | PF_EDITABLE | PF_MULTI_OPTIONS, "Geometry Flags");
		RegisterMember("FlipDDS", &GASS::OSGMeshComponent::m_FlipDDS, PF_VISIBLE | PF_EDITABLE,"Flip DDS textures for this model? (need reload)");

		std::vector<std::string> ext;
		ext.push_back("3ds");
		ext.push_back("flt");
		ext.push_back("obj");
		ext.push_back("*");

		RegisterProperty<FilePath>("ImportMesh", &OSGMeshComponent::GetImportMesh, &OSGMeshComponent::SetImportMesh,
			FilePathPropertyMetaDataPtr(new FilePathPropertyMetaData("Import new mesh",PF_VISIBLE | PF_EDITABLE, FilePathPropertyMetaData::IMPORT_FILE,ext)));
	}

	void OSGMeshComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGMeshComponent::OnLocationLoaded,LocationLoadedEvent,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGMeshComponent::OnMaterialMessage,ReplaceMaterialRequest,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGMeshComponent::OnCollisionSettings,CollisionSettingsRequest ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGMeshComponent::OnVisibilityMessage,GeometryVisibilityRequest ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGMeshComponent::OnMeshFileNameMessage,MeshFileRequest,0));
	}

	std::vector<std::string> OSGMeshComponent::GetAvailableMeshFiles() const
	{
		std::vector<std::string> content;
		ResourceManagerPtr rm = GASS::SimEngine::Get().GetResourceManager();
		ResourceGroupVector groups = rm->GetResourceGroups();
		std::vector<std::string> values;
		for(size_t i = 0; i < groups.size();i++)
		{
			ResourceGroupPtr group = groups[i];
			if(m_EnumerationResourceGroup == "" || group->GetName() == m_EnumerationResourceGroup)
			{
				ResourceVector res_vec;
				group->GetResourcesByType(res_vec,"MESH");
				for(size_t j = 0; j < res_vec.size();j++)
				{
					content.push_back(res_vec[j]->Name());
				}
			}
		}
		return content;
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
			OSGConvert::SetOSGNodeMask(value,m_MeshNode.get());
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

	void OSGMeshComponent::OnMaterialMessage(ReplaceMaterialRequestPtr message)
	{

		//TODO:implement this...
	}


	void OSGMeshComponent::OnLocationLoaded(LocationLoadedEventPtr message)
	{
		LoadMesh(m_MeshResource);
		m_Initlized = true;
	}

	void OSGMeshComponent::SetMeshResource(const ResourceHandle &res)
	{
		m_MeshResource = res;
		if(m_Initlized)
		{
			LoadMesh(m_MeshResource);
		}
	}

	void OSGMeshComponent::OnVisibilityMessage(GeometryVisibilityRequestPtr message)
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

			m_MeshNode->setNodeMask(0);
		}
	}

	FilePath OSGMeshComponent::GetImportMesh() const
	{
		return FilePath();
	}

	void OSGMeshComponent::SetImportMesh(const FilePath &path)
	{
		if(path.GetFullPath() !="")
			LoadMesh(path.GetFullPath());
	}

	void OSGMeshComponent::LoadMesh(const ResourceHandle &filename)
	{
		if(!filename.Valid())
			return;

		FilePath temp_file_path(filename.Name());
		const std::string extension =  temp_file_path.GetExtension();
		std::string  mod_file_name = filename.Name();

		//hack to support .mesh format
		if(extension == "mesh") //this is ogre model, try to load 3ds instead
		{
			ResourceManagerPtr rm = SimEngine::Get().GetResourceManager();
			std::string file_name_3ds = StringUtils::Replace(mod_file_name,".mesh",".3ds");
			std::string file_name_obj = StringUtils::Replace(mod_file_name,".mesh",".obj");
			std::string file_name_flt = StringUtils::Replace(mod_file_name,".mesh",".flt");
			if(rm->HasResource(file_name_3ds))
				mod_file_name = file_name_3ds;
			else if(rm->HasResource(file_name_obj))
				mod_file_name = file_name_obj;
			else if(rm->HasResource(file_name_flt))
				mod_file_name = file_name_flt;
		}

		ResourceHandle new_res(mod_file_name);
		FilePath file_path = new_res.GetResource()->Path();
		LoadMesh(file_path.GetFullPath());
	}

	void OSGMeshComponent::LoadMesh(const std::string &file_name)
	{
		GASS_SHARED_PTR<OSGLocationComponent> lc = GetSceneObject()->GetFirstComponentByClass<OSGLocationComponent>();
		if(!lc)
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed loading " + GetSceneObject()->GetName()  + ", not possible to use mesh components without location compoent","OSGMeshComponent::SetFilename");
		}

		if(m_MeshNode.valid())
		{
			lc->GetOSGNode()->removeChild(m_MeshNode.get());
			m_MeshNode.release();
		}

		const osg::ref_ptr<osgDB::ReaderWriter::Options> options = new osgDB::ReaderWriter::Options();

		if(m_FlipDDS)
		{
			options->setOptionString("dds_flip");
		}

		options->setBuildKdTreesHint(osgDB::ReaderWriter::Options::BUILD_KDTREES);

		m_MeshNode = (osg::Group*) osgDB::readNodeFile(file_name,options);

		if( ! m_MeshNode)
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to load mesh: " + file_name,"OSGMeshComponent::SetFilename");
		}

		osgUtil::Optimizer optimizer;
		optimizer.optimize(m_MeshNode.get(), osgUtil::Optimizer::DEFAULT_OPTIMIZATIONS & ~osgUtil::Optimizer::OPTIMIZE_TEXTURE_SETTINGS);

		OSGNodeData* data = new OSGNodeData(shared_from_this());
		m_MeshNode->setUserData(data);

		SetLighting(m_Lighting);
		SetCastShadow(m_CastShadow);
		SetReceiveShadow(m_ReceiveShadow);

		m_BBox = AABox();
		if(m_MeshNode.get())
			CalulateBoundingbox(m_MeshNode.get());

		GetSceneObject()->PostEvent(GeometryChangedEventPtr(new GeometryChangedEvent(GASS_DYNAMIC_PTR_CAST<IGeometryComponent>(shared_from_this()))));

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
			//GASS_SHARED_PTR<OSGLocationComponent> lc = GetSceneObject()->GetFirstComponentByClass<OSGLocationComponent>();
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
			so = GASS_DYNAMIC_PTR_CAST<SceneObject> (SimEngine::Get().GetSceneObjectTemplateManager()->CreateFromTemplate(template_name));
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
				GASS_SHARED_PTR<OSGMeshComponent> mesh_comp(new OSGMeshComponent());
				OSGNodeData* node_data = new OSGNodeData(mesh_comp);
				node->setUserData(node_data);
				mesh_comp->SetMeshNode(node);
				so->AddComponent(mesh_comp);
			}

			parent->AddChildSceneObject(so,load);
			//expand recursive
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
				//GASS_LOG(LINFO) << "Try to expand OSG node:" << child_node->getName();

				//use template if present!
				const std::string template_name = "OSGExpandedMeshObject";
				SceneObjectPtr so;
				try
				{
					so = GASS_DYNAMIC_PTR_CAST<SceneObject> (SimEngine::Get().GetSceneObjectTemplateManager()->CreateFromTemplate(template_name));
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
						GASS_LOG(LINFO) << "found child Transform";
						//Expand(parent,child_node, load);
					}
					else if (child_node->asGroup())
					{
						GASS_LOG(LINFO) << "found child  gorup";
						//Expand(parent,child_node, load);
					}
					//else
					{
						osg::Geode* geode = dynamic_cast<osg::Geode*> (child_node);
						if (geode)
						{

							//osg::PositionAttitudeTransform* trans = dynamic_cast<osg::PositionAttitudeTransform*> (child_node);
							//if(child_node->asTransform())
							//{
							//osg::Vec3d pos = child_node->asTransform()->asMatrixTransform()->getMatrix().getTrans();
							//GASS_LOG(LINFO) << "found trans:" << pos.x() << " " << pos.y() << " " << pos.z();

							//}


							GASS_SHARED_PTR<OSGMeshComponent> mesh_comp(new OSGMeshComponent());
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
			GASS_LOG(LINFO) << "found Transform";
		}
		else
		{
			//use template if present!
			const std::string template_name = "OSGExpandedMeshObject";
			SceneObjectPtr so;
			try
			{
				so = GASS_DYNAMIC_PTR_CAST<SceneObject> (SimEngine::Get().GetSceneObjectTemplateManager()->CreateFromTemplate(template_name));
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
					GASS_LOG(LINFO) << "found Geode";

					GASS_SHARED_PTR<OSGMeshComponent> mesh_comp(new OSGMeshComponent());
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

	void OSGMeshComponent::OnMeshFileNameMessage(MeshFileRequestPtr message)
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

		osg::Geode* geode = dynamic_cast<osg::Geode*> (node);
		if (geode)
		{
			osg::BoundingBox bbox = geode->getBoundingBox();

			osg::Vec3d p1 = bbox._max*M;
			osg::Vec3d p2 = bbox._min*M;

			AABox box;
			box.Union(OSGConvert::ToGASS(p1));
			box.Union(OSGConvert::ToGASS(p2));
			m_BBox.Union(box);
			// Traverse drawables
			for (unsigned int i = 0; i < geode->getNumDrawables(); i++)
			{
#ifdef OSG_VERSION_GREATER_OR_EQUAL
#if(OSG_VERSION_GREATER_OR_EQUAL(3,3,2))
				osg::BoundingBox osg_bbox = geode->getDrawable(i)->getBoundingBox();
#else
				osg::BoundingBox osg_bbox = geode->getDrawable(i)->getBound();
#endif
#else
				osg::BoundingBox osg_bbox = geode->getDrawable(i)->getBound();
#endif

				AABox temp_box;

				osg::Vec3d tp1 = osg_bbox._max*M;
				osg::Vec3d tp2 = osg_bbox._min*M;

				temp_box.Union(OSGConvert::ToGASS(tp1));
				temp_box.Union(OSGConvert::ToGASS(tp2));
				m_BBox.Union(box);
			}
		}

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
	}

	GraphicsMesh OSGMeshComponent::GetMeshData() const
	{
		GraphicsMesh  mesh_data;
		if(!m_MeshNode.valid())
		{
			GASS_LOG(LWARNING) << "You have to load mesh before trying to fetch mesh data. Mesh name: " << GetName();
			return mesh_data;
		}
		DrawableVisitor<TriangleRecorder> mv;
		m_MeshNode->accept(mv);

		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());

		//TODO: get materials and sub meshes!!

		sub_mesh_data->Type = TRIANGLE_LIST;
		mesh_data.SubMeshVector.push_back(sub_mesh_data);

		sub_mesh_data->PositionVector = mv.mFunctor.mVertices;
		sub_mesh_data->IndexVector = mv.mFunctor.mTriangles;

		return mesh_data;
		//TODO: support materials
	}

	void OSGMeshComponent::OnCollisionSettings(CollisionSettingsRequestPtr message)
	{
		SetCollision(message->EnableCollision());
	}

	void OSGMeshComponent::SetCollision(bool value)
	{
		if(m_MeshNode.valid() && m_MeshNode->getNodeMask())
		{
			if(value)
				OSGConvert::SetOSGNodeMask(m_GeomFlags,m_MeshNode.get());
			else
			{
				OSGConvert::SetOSGNodeMask(GEOMETRY_FLAG_TRANSPARENT_OBJECT,m_MeshNode.get());
			}
		}
		m_Collision = value;
	}

	bool OSGMeshComponent::GetCollision() const
	{
		return m_Collision;
	}

}
