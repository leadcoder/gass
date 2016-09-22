#include "GASSObjExporter.h"
#include "Sim/GASS.h"
#include "Core/Math/GASSMath.h"
#include <fstream>
#include "Core/Utils/GASSFilesystem.h"

namespace GASS
{

	ObjExporter::ObjExporter() :
		m_CopyTextures(true),
		m_FlipDDSTexCoords(true),
		m_WrapTexCoords(true)
	{


	}
	ObjExporter::~ObjExporter()
	{

	}

	void ObjExporter::ExportFiles(const std::string &out_dir, SceneObjectPtr root_obj)
	{
		ComponentContainer::ComponentVector comps;
		root_obj->GetComponentsByClass<IMeshComponent>(comps,true);
		std::map<std::string, SceneObjectPtr > meshmap;
		for(size_t i = 0;  i <  comps.size(); i++)
		{
			SceneObjectPtr obj = (GASS_DYNAMIC_PTR_CAST<BaseSceneComponent>(comps[i]))->GetSceneObject();
			MeshComponentPtr mesh =  GASS_DYNAMIC_PTR_CAST<IMeshComponent>(comps[i]);
			ResourceComponentPtr res =  GASS_DYNAMIC_PTR_CAST<IResourceComponent>(comps[i]);
			if(mesh && res)
			{
				std::string out_file = FileUtils::RemoveExtension(res->GetResource().Name());
				out_file = out_dir + "/"+ out_file;
				meshmap[out_file] = obj;
			}
		}

		std::map<std::string, SceneObjectPtr >::iterator iter = meshmap.begin();
		while(iter != meshmap.end())
		{
			Export(iter->first, iter->second,false);
			++iter;
		}

	}

	void ObjExporter::Export(const std::string &out_file, SceneObjectPtr root_obj,bool recursive) const
	{
		ComponentContainer::ComponentVector comps;
		root_obj->GetComponentsByClass<IMeshComponent>(comps,recursive);

		LocationComponentPtr root_lc = root_obj->GetFirstComponentByClass<ILocationComponent>();
		Vec3 offset(0,0,0);
		if(root_lc)
		{
			offset = -root_lc->GetWorldPosition();
		}

		if(comps.size()>0)
		{
			std::vector<GraphicsMesh> mesh_data_vec;
			for(size_t i = 0;  i <  comps.size(); i++)
			{
				SceneObjectPtr obj = (GASS_DYNAMIC_PTR_CAST<BaseSceneComponent>(comps[i]))->GetSceneObject();
				MeshComponentPtr mesh =  GASS_DYNAMIC_PTR_CAST<IMeshComponent>(comps[i]);
				GraphicsMesh  mesh_data = mesh->GetMeshData();

				//Transform to world coordinates!
				GeometryComponentPtr geom = obj->GetFirstComponentByClass<IGeometryComponent>();
				LocationComponentPtr lc = obj->GetFirstComponentByClass<ILocationComponent>();
				if(lc && lc != root_lc)
				{
					Mat4 trans_mat(lc->GetWorldPosition() + offset, lc->GetWorldRotation(), lc->GetScale());
					mesh_data.Transform(trans_mat);
				}
				mesh_data_vec.push_back(mesh_data);
			}

			//ManualMeshData final_mesh_data;
			std::vector<Vec3> PositionVector;
			std::vector<Vec3> NormalVector;
			std::vector<Vec4> TexCoordVector;
			std::vector<unsigned int > FaceVector;
			std::map<std::string,GraphicsMaterial> Materials;

			GraphicsMaterial def_mat;
			Materials["DefaultMat"] = def_mat;

			for(size_t i = 0; i < mesh_data_vec.size() ; i++)
			{
				for(size_t h = 0; h < mesh_data_vec[i].SubMeshVector.size() ; h++)
				{
					GraphicsSubMeshPtr sub_mesh =   mesh_data_vec[i].SubMeshVector[h];

					if(sub_mesh->Type == TRIANGLE_LIST)
					{

						if(sub_mesh->MaterialName != "")
							Materials[sub_mesh->MaterialName] = sub_mesh->Material;
						else
						{
							sub_mesh->MaterialName = "DefaultMat";
							sub_mesh->Material = def_mat;
						}
						for(size_t j = 0 ; j < sub_mesh->PositionVector.size(); j++)
						{
							PositionVector.push_back(sub_mesh->PositionVector[j]);
						}

						//generate normals if not present
						if(sub_mesh->NormalVector.size() == 0)
						{
							sub_mesh->NormalVector.resize(sub_mesh->PositionVector.size());
							for(size_t j = 0 ; j < sub_mesh->IndexVector.size(); j += 3)
							{
								Vec3 p1 = sub_mesh->PositionVector[sub_mesh->IndexVector[j]];
								Vec3 p2 = sub_mesh->PositionVector[sub_mesh->IndexVector[j+1]];
								Vec3 p3 = sub_mesh->PositionVector[sub_mesh->IndexVector[j+2]];
								Vec3 v1 = p1 - p3;
								Vec3 v2 = p2 - p3;
								Vec3 norm = Math::Cross(v1,v2);
								norm.FastNormalize();
								sub_mesh->NormalVector[sub_mesh->IndexVector[j]] = norm;
								sub_mesh->NormalVector[sub_mesh->IndexVector[j+1]] = norm;
								sub_mesh->NormalVector[sub_mesh->IndexVector[j+2]] = norm;
							}
						}

						for(size_t j = 0 ; j < sub_mesh->NormalVector.size(); j++)
						{
							NormalVector.push_back(sub_mesh->NormalVector[j]);
						}

						bool dds_tex = false;
						if(sub_mesh->Material.Textures.size() > 0)
						{
							std::string ext = StringUtils::ToLower(FileUtils::GetExtension(sub_mesh->Material.Textures[0]));
							if(ext == "dds")
								dds_tex = true;
						}

						if(sub_mesh->TexCoordsVector.size() > 0 && sub_mesh->TexCoordsVector[0].size() > 0)
						{
							for(size_t j = 0 ; j < sub_mesh->TexCoordsVector[0].size(); j++)
							{
								Vec4 tc = sub_mesh->TexCoordsVector[0].at(j);
								tc.y = -tc.y;
								if(m_FlipDDSTexCoords && dds_tex)
								{
									tc.y = -tc.y;
								}

								if(m_WrapTexCoords)
								{
									//tc.x = fmod(tc.x, 1.0);
									//tc.y = fmod(tc.y, 1.0);
								}
								TexCoordVector.push_back(tc);
							}
						}
						else
						{
							for(size_t j = 0 ; j < sub_mesh->PositionVector.size(); j++)
							{
								TexCoordVector.push_back(Vec4(1,0,0,0));
							}
						}
					}
				}
			}

			//std::stringstream ss;
			std::ofstream file_ptr;
			file_ptr.open((out_file + ".obj").c_str());

			//std::string mat_file = out_file

			file_ptr << "\n";
			file_ptr << "mtllib " << FileUtils::GetFilename(out_file) << ".mtl" << "\n";
			file_ptr << "\n";

			file_ptr << "#num verts" << PositionVector.size() << "\n";
			//Serialize mesh data to obj format!
			for(size_t i = 0; i < PositionVector.size() ; i++)
				file_ptr << "v " << PositionVector[i] << "\n";
			file_ptr << "\n";
			for(size_t i = 0; i < TexCoordVector.size() ; i++)
				file_ptr << "vt " << TexCoordVector[i].x  << " " << TexCoordVector[i].y << "\n";
			file_ptr << "\n";
			for(size_t i = 0; i < NormalVector.size() ; i++)
				file_ptr << "vn " << NormalVector[i] << "\n";
			file_ptr << "\n";

			int sub_mesh_index = 0;
			unsigned int base_index = 1;//VertexVector.size();
			for(size_t i = 0; i < mesh_data_vec.size() ; i++)
			{
				for(size_t h = 0; h < mesh_data_vec[i].SubMeshVector.size() ; h++)
				{

						GraphicsSubMeshPtr sub_mesh =   mesh_data_vec[i].SubMeshVector[h];
						if(sub_mesh->Type == TRIANGLE_LIST)
						{
						file_ptr << "g " << "submesh" << sub_mesh_index << "\n";
						sub_mesh_index++;

						file_ptr << "usemtl " << sub_mesh->MaterialName << "\n";


						file_ptr << "#num faces" << (sub_mesh->IndexVector.size()/3) << "\n";


						for(size_t j = 0 ; j < sub_mesh->IndexVector.size(); j += 3)
						{
							unsigned int v_index_1 = sub_mesh->IndexVector[j] + base_index;
							unsigned int v_index_2 = sub_mesh->IndexVector[j+1] + base_index;
							unsigned int v_index_3 = sub_mesh->IndexVector[j+2] + base_index;
							file_ptr << "f " << v_index_1 << "/" << v_index_1 << "/" << v_index_1 << " "
								<< v_index_2 << "/" << v_index_2 << "/" << v_index_2 << " "
								<< v_index_3 << "/" << v_index_3 << "/" << v_index_3 << "\n";
						}
						base_index += static_cast<int>(sub_mesh->PositionVector.size());
					}
				}
			}

			file_ptr << "\n";
			try
			{
				//file_ptr << ss.str().c_str();
				file_ptr.close();
			}
			catch (std::exception &e)
			{
				LogManager::getSingleton().stream() << "Failed to write obj data to file:" << e.what();
			}

			{

				std::ofstream file_ptr;
				file_ptr.open((out_file + ".mtl").c_str());

				std::map<std::string,GraphicsMaterial>::iterator iter = Materials.begin();
				while(iter != Materials.end())
				{
					GraphicsMaterial mat = iter->second;
					file_ptr << "\n";
					file_ptr << "newmtl " << iter->first << "\n";
					file_ptr << "Kd " << mat.Diffuse << "\n";
					file_ptr << "Ka " << mat.Ambient << "\n";
					file_ptr << "illum " << "1" << "\n";
					if(mat.Textures.size() > 0 && mat.Textures[0] !="")
						file_ptr << "map_Kd " << mat.Textures[0] << "\n";
					file_ptr << "\n";
					++iter;
				}
				file_ptr.close();

				//copy textures
				if(m_CopyTextures)
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
						++iter;
					}
				}
			}
		}
	}
}









