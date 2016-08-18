#include "GASSOgreMeshExporter.h"
#include "Sim/GASS.h"
#include "Plugins/Ogre/GASSOgreConvert.h"
#include "Plugins/Ogre/GASSOgreGraphicsSceneManager.h"
#include <OgreMeshManager.h>
#include <OgreEntity.h>
#include <OgreSceneManager.h>
#include <OgreTextureUnitState.h>
#include <OgreMaterialManager.h>
#include <OgreManualObject.h>
#include <OgreMeshSerializer.h>

namespace GASS
{
	OgreMeshExporter::OgreMeshExporter() : m_CopyTextures(true),
		m_FlipDDSTexCoords(true)
	{
	}

	OgreMeshExporter::~OgreMeshExporter()
	{

	}

	void OgreMeshExporter::Add(Ogre::ManualObject* manual_object, GraphicsMeshPtr mesh)
	{
		for(size_t i = 0; i < mesh->SubMeshVector.size() ; i++)
		{
			GraphicsSubMeshPtr sub_mesh =   mesh->SubMeshVector[i];
			Ogre::RenderOperation::OperationType op = OgreConvert::ToOgre(sub_mesh->Type);
			//use material name or create new material?
			manual_object->begin(sub_mesh->MaterialName, op);

			size_t num_pos = sub_mesh->PositionVector.size();

			bool has_normals = false;
			bool has_tangents = false;
			bool has_colors = false;
			bool has_tex_coords = false;

			if(sub_mesh->NormalVector.size() ==  num_pos)
				has_normals = true;
			if(sub_mesh->TangentVector.size() ==  num_pos)
				has_tangents = true;
			if(sub_mesh->ColorVector.size() ==  num_pos)
				has_colors = true;
			if(sub_mesh->TexCoordsVector.size() > 0 &&  sub_mesh->TexCoordsVector[0].size() ==  num_pos)
				has_tex_coords = true;

			for(size_t j = 0; j < sub_mesh->PositionVector.size() ; j++)
			{
				manual_object->position(OgreConvert::ToOgre(sub_mesh->PositionVector[j]));
				if(has_normals)
					manual_object->normal(OgreConvert::ToOgre(sub_mesh->NormalVector[j]));
				if(has_tangents)
					manual_object->tangent(OgreConvert::ToOgre(sub_mesh->TangentVector[j]));
				if(has_colors)
					manual_object->colour(OgreConvert::ToOgre(sub_mesh->ColorVector[j]));
				if(has_tex_coords)
				{
					Vec4 tex_coord = sub_mesh->TexCoordsVector[0].at(j);
					manual_object->textureCoord(static_cast<Ogre::Real>(tex_coord.x), static_cast<Ogre::Real>(tex_coord.y));
				}
			}
			for(size_t j = 0; j < sub_mesh->IndexVector.size();j++)
			{
				manual_object->index(sub_mesh->IndexVector[j]);
			}
			manual_object->end();
		}
	}

	void OgreMeshExporter::SaveToMesh(Ogre::ManualObject* manual_object, const std::string &file_name)
	{
		Ogre::MeshPtr mesh = manual_object->convertToMesh("ConvertedTempMesh");
		Ogre::MeshSerializer serializer;
		// Write Mesh to file
		serializer.exportMesh(mesh.getPointer(),file_name);
	}

	void OgreMeshExporter::Export(const std::string &out_file, SceneObjectPtr root_obj)
	{
		ComponentContainer::ComponentVector comps;
		root_obj->GetComponentsByClass<IMeshComponent>(comps,true);

		if(comps.size()>0)
		{
			LocationComponentPtr root_lc = root_obj->GetFirstComponentByClass<ILocationComponent>();
			Vec3 offset(0,0,0); 
			if(root_lc)
			{
				offset = -root_lc->GetWorldPosition();
			}
			std::map<std::string, std::vector<GraphicsSubMeshPtr> > mesh_data_map;
			for(size_t i = 0;  i <  comps.size(); i++)
			{
				SceneObjectPtr obj = (GASS_DYNAMIC_PTR_CAST<BaseSceneComponent>(comps[i]))->GetSceneObject();
				MeshComponentPtr mesh =  GASS_DYNAMIC_PTR_CAST<IMeshComponent>(comps[i]);
				GraphicsMesh  mesh_data = mesh->GetMeshData();

				//Transform to world coordinates!
				GeometryComponentPtr geom = obj->GetFirstComponentByClass<IGeometryComponent>();
				LocationComponentPtr lc = obj->GetFirstComponentByClass<ILocationComponent>();
				if(lc) //ignore root transformation?
				{
					Mat4 trans_mat(lc->GetWorldPosition() + offset, lc->GetWorldRotation(), lc->GetScale());
					mesh_data.Transform(trans_mat);
				}

				//add all sub meshes that match the type we want to save
				for(size_t j = 0; j < mesh_data.SubMeshVector.size(); j++)
				{
					if(mesh_data.SubMeshVector[j]->Type == TRIANGLE_LIST && 
						mesh_data.SubMeshVector[j]->IndexVector.size() > 0 &&
						mesh_data.SubMeshVector[j]->PositionVector.size() > 0)
						mesh_data_map[mesh_data.SubMeshVector[j]->MaterialName].push_back(mesh_data.SubMeshVector[j]);
				}
			}

			GraphicsMeshPtr new_mesh(new GraphicsMesh());
			std::map<std::string, std::vector<GraphicsSubMeshPtr> >::iterator iter = mesh_data_map.begin();
			while(iter != mesh_data_map.end())
			{
				GraphicsSubMeshPtr new_sm(new GraphicsSubMesh);
				new_mesh->SubMeshVector.push_back(new_sm);
				new_sm->MaterialName = iter->first;
				new_sm->Type = TRIANGLE_LIST;
				unsigned int base_index = 0;
				bool has_color = true;
				bool has_normal = true;
				bool has_tangent = true;
				bool has_tex_coords = true;

				for(size_t i = 0; i < iter->second.size(); i++)
				{
					GraphicsSubMeshPtr sm = iter->second.at(i);
					if(sm->ColorVector.size() == 0)
						has_color = false;
					if(sm->NormalVector.size() == 0)
						has_normal = false;
					if(sm->TangentVector.size() == 0)
						has_tangent = false;
					if(sm->TexCoordsVector.size() == 0 || sm->TexCoordsVector.size() > 0 && sm->TexCoordsVector[0].size() == 0)
						has_tex_coords = false;
				}



				for(size_t i = 0; i < iter->second.size(); i++)
				{
					GraphicsSubMeshPtr sm = iter->second.at(i);
					for(size_t j = 0; j < sm->PositionVector.size(); j++)
						new_sm->PositionVector.push_back(sm->PositionVector[j]);
				}

				if(has_color)
				{
					for(size_t i = 0; i < iter->second.size(); i++)
					{
						GraphicsSubMeshPtr sm = iter->second.at(i);
						for(size_t j = 0; j < sm->ColorVector.size(); j++)
							new_sm->ColorVector.push_back(sm->ColorVector[j]);
					}
				}

				if(has_normal)
				{
					for(size_t i = 0; i < iter->second.size(); i++)
					{
						GraphicsSubMeshPtr sm = iter->second.at(i);
						for(size_t j = 0; j < sm->NormalVector.size(); j++)
							new_sm->NormalVector.push_back(sm->NormalVector[j]);
					}
				}

				if(has_tangent)
				{
					for(size_t i = 0; i < iter->second.size(); i++)
					{
						GraphicsSubMeshPtr sm = iter->second.at(i);
						for(size_t j = 0; j < sm->TangentVector.size(); j++)
							new_sm->TangentVector.push_back(sm->TangentVector[j]);
					}
				}

				if(has_tex_coords)
				{
					std::vector<Vec4> tv;
					for(size_t i = 0; i < iter->second.size(); i++)
					{
						GraphicsSubMeshPtr sm = iter->second.at(i);
						for(size_t j = 0; j < sm->TexCoordsVector[0].size(); j++)
							tv.push_back(sm->TexCoordsVector[0].at(j));
					}
					new_sm->TexCoordsVector.push_back(tv);
				}

				for(size_t i = 0; i < iter->second.size(); i++)
				{
					GraphicsSubMeshPtr sm = iter->second.at(i);
					for(size_t j = 0; j < sm->IndexVector.size(); j++)
						new_sm->IndexVector.push_back(sm->IndexVector[j] + base_index);
					base_index += static_cast<int>(sm->PositionVector.size());
				}
				iter++;
				//create new submesh and copy all data
			}
			//create manual object and save to file!
			OgreGraphicsSceneManagerPtr ogsm =  root_obj->GetScene()->GetFirstSceneManagerByClass<OgreGraphicsSceneManager>();
			assert(ogsm);
			Ogre::SceneManager* sm = ogsm->GetOgreSceneManager();

			Ogre::ManualObject* mo = sm->createManualObject("TempManObj");
			Add(mo, new_mesh);
			SaveToMesh(mo,out_file);
			sm->destroyManualObject(mo);

			//copy textures and generate materials
			/*if(m_CopyTextures)
			{
			iter = Materials.begin();
			while(iter != Materials.end())
			{
			GraphicsMaterial mat = iter->second;
			if(mat.Textures.size() > 0)
			{
			std::string texture_name =  mat.Textures[0];
			GASS::ResourceHandle res(texture_name);
			GASS::FileResourcePtr file_res;
			try
			{
			file_res = res.GetResource();
			}
			catch(...)
			{
			LogManager::getSingleton().stream() << "WARNING: Failed to find texture:" << texture_name;
			}
			if(file_res)
			{
			std::string full_path = StringUtils::Replace(file_res->Path().GetFullPath(),"\\","/");
			std::string out_path = FileUtils::RemoveFilename(out_file);
			out_path = out_path + texture_name;

			try
			{
			GASS_FILESYSTEM::copy_file(GASS_FILESYSTEM::path(full_path),GASS_FILESYSTEM::path(out_path),GASS_FILESYSTEM::copy_option::overwrite_if_exists);
			} 
			catch (const GASS_FILESYSTEM::filesystem_error& e)
			{
			//std::cerr << "Error: " << e.what() << std::endl;
			LogManager::getSingleton().stream() << "WARNING: Failed copy texture during export:" << e.what();
			}
			}
			}
			iter++;
			}
			}
			}*/
		}
	}
}









