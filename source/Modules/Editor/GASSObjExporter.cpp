#include "GASSObjExporter.h"
#include "Sim/GASS.h"
#include "Core/Math/GASSMath.h"
#include <fstream>
#include "Core/Utils/GASSFilesystem.h"

namespace GASS
{

	ObjExporter::ObjExporter() 
		
	{


	}
	ObjExporter::~ObjExporter()
	{

	}

	void ObjExporter::ExportFiles(const std::string &out_dir, SceneObjectPtr root_obj)
	{
		SceneObject::ComponentVector comps;
		root_obj->GetComponentsByClass<IMeshComponent>(comps, true);
		std::map<std::string, SceneObjectPtr > meshmap;
		for (size_t i = 0; i < comps.size(); i++)
		{
			SceneObjectPtr obj = comps[i]->GetSceneObject();
			MeshComponentPtr mesh = GASS_DYNAMIC_PTR_CAST<IMeshComponent>(comps[i]);
			ResourceComponentPtr res = GASS_DYNAMIC_PTR_CAST<IResourceComponent>(comps[i]);
			if (mesh && res)
			{
				std::string out_file = FileUtils::RemoveExtension(res->GetResource().Name());
				out_file = out_dir + "/" + out_file;
				meshmap[out_file] = obj;
			}
		}

		auto iter = meshmap.begin();
		while (iter != meshmap.end())
		{
			Export(iter->first, iter->second, false);
			++iter;
		}

	}

	void ObjExporter::Export(const std::string &out_file, SceneObjectPtr root_obj, bool recursive) const
	{
		SceneObject::ComponentVector comps;
		root_obj->GetComponentsByClass<IMeshComponent>(comps, recursive);

		LocationComponentPtr root_lc = root_obj->GetFirstComponentByClass<ILocationComponent>();
		Vec3 offset(0, 0, 0);
		if (root_lc)
		{
			offset = -root_lc->GetWorldPosition();
		}

		if (comps.size() > 0)
		{
			std::vector<GraphicsMesh> mesh_data_vec;
			for (size_t i = 0; i < comps.size(); i++)
			{
				SceneObjectPtr obj = comps[i]->GetSceneObject();
				MeshComponentPtr mesh = GASS_DYNAMIC_PTR_CAST<IMeshComponent>(comps[i]);
				GraphicsMesh  mesh_data = mesh->GetMeshData();

				//Transform to world coordinates!
				GeometryComponentPtr geom = obj->GetFirstComponentByClass<IGeometryComponent>();
				LocationComponentPtr lc = obj->GetFirstComponentByClass<ILocationComponent>();
				if (lc && lc != root_lc)
				{
					Mat4 trans_mat = Mat4::CreateTransformationSRT(lc->GetScale(), lc->GetWorldRotation(), lc->GetWorldPosition() + offset);
					mesh_data.Transform(trans_mat);
				}
				mesh_data_vec.push_back(mesh_data);
			}

			//ManualMeshData final_mesh_data;
			std::vector<Vec3> position_vector;
			std::vector<Vec3> normal_vector;
			std::vector<Vec4> tex_coord_vector;
			std::vector<unsigned int > face_vector;
			std::map<std::string, PhongMaterialConfig> materials;

			PhongMaterialConfig def_mat;
			materials["DefaultMat"] = def_mat;

			for (size_t i = 0; i < mesh_data_vec.size(); i++)
			{
				for (size_t h = 0; h < mesh_data_vec[i].SubMeshVector.size(); h++)
				{
					GraphicsSubMeshPtr sub_mesh = mesh_data_vec[i].SubMeshVector[h];

					if (sub_mesh->Type == TRIANGLE_LIST)
					{

						if (sub_mesh->MaterialName != "" && sub_mesh->MaterialConfig)
							materials[sub_mesh->MaterialName] = *((PhongMaterialConfig*)sub_mesh->MaterialConfig.get());
						else
						{
							sub_mesh->MaterialName = "DefaultMat";
							*(sub_mesh->MaterialConfig) = def_mat;
						}
						for (size_t j = 0; j < sub_mesh->PositionVector.size(); j++)
						{
							position_vector.push_back(sub_mesh->PositionVector[j]);
						}

						//generate normals if not present
						if (sub_mesh->NormalVector.size() == 0)
						{
							sub_mesh->NormalVector.resize(sub_mesh->PositionVector.size());
							for (size_t j = 0; j < sub_mesh->IndexVector.size(); j += 3)
							{
								Vec3 p1 = sub_mesh->PositionVector[sub_mesh->IndexVector[j]];
								Vec3 p2 = sub_mesh->PositionVector[sub_mesh->IndexVector[j + 1]];
								Vec3 p3 = sub_mesh->PositionVector[sub_mesh->IndexVector[j + 2]];
								Vec3 v1 = p1 - p3;
								Vec3 v2 = p2 - p3;
								Vec3 norm = Vec3::Cross(v1, v2);
								norm.Normalize();
								sub_mesh->NormalVector[sub_mesh->IndexVector[j]] = norm;
								sub_mesh->NormalVector[sub_mesh->IndexVector[j + 1]] = norm;
								sub_mesh->NormalVector[sub_mesh->IndexVector[j + 2]] = norm;
							}
						}

						for (size_t j = 0; j < sub_mesh->NormalVector.size(); j++)
						{
							normal_vector.push_back(sub_mesh->NormalVector[j]);
						}

						bool dds_tex = false;
						/*if (sub_mesh->Material.Textures.size() > 0)
						{
							std::string ext = StringUtils::ToLower(FileUtils::GetExtension(sub_mesh->Material.Textures[0]));
							if (ext == "dds")
								dds_tex = true;
						}*/

						if (sub_mesh->TexCoordsVector.size() > 0 && sub_mesh->TexCoordsVector[0].size() > 0)
						{
							for (size_t j = 0; j < sub_mesh->TexCoordsVector[0].size(); j++)
							{
								Vec4 tc = sub_mesh->TexCoordsVector[0].at(j);
								tc.y = -tc.y;
								if (m_FlipDDSTexCoords && dds_tex)
								{
									tc.y = -tc.y;
								}

								if (m_WrapTexCoords)
								{
									//tc.x = fmod(tc.x, 1.0);
									//tc.y = fmod(tc.y, 1.0);
								}
								tex_coord_vector.push_back(tc);
							}
						}
						else
						{
							for (size_t j = 0; j < sub_mesh->PositionVector.size(); j++)
							{
								tex_coord_vector.emplace_back(1, 0, 0, 0);
							}
						}
					}
				}
			}

			//std::stringstream ss;
			std::ofstream obj_file_ptr;
			obj_file_ptr.open((out_file + ".obj").c_str());

			//std::string mat_file = out_file

			obj_file_ptr << "\n";
			obj_file_ptr << "mtllib " << FileUtils::GetFilename(out_file) << ".mtl" << "\n";
			obj_file_ptr << "\n";

			obj_file_ptr << "#num verts" << position_vector.size() << "\n";
			//Serialize mesh data to obj format!
			for (size_t i = 0; i < position_vector.size(); i++)
				obj_file_ptr << "v " << position_vector[i] << "\n";
			obj_file_ptr << "\n";
			for (size_t i = 0; i < tex_coord_vector.size(); i++)
				obj_file_ptr << "vt " << tex_coord_vector[i].x << " " << tex_coord_vector[i].y << "\n";
			obj_file_ptr << "\n";
			for (size_t i = 0; i < normal_vector.size(); i++)
				obj_file_ptr << "vn " << normal_vector[i] << "\n";
			obj_file_ptr << "\n";

			int sub_mesh_index = 0;
			unsigned int base_index = 1;//VertexVector.size();
			for (size_t i = 0; i < mesh_data_vec.size(); i++)
			{
				for (size_t h = 0; h < mesh_data_vec[i].SubMeshVector.size(); h++)
				{

					GraphicsSubMeshPtr sub_mesh = mesh_data_vec[i].SubMeshVector[h];
					if (sub_mesh->Type == TRIANGLE_LIST)
					{
						obj_file_ptr << "g " << "submesh" << sub_mesh_index << "\n";
						sub_mesh_index++;

						obj_file_ptr << "usemtl " << sub_mesh->MaterialName << "\n";


						obj_file_ptr << "#num faces" << (sub_mesh->IndexVector.size() / 3) << "\n";


						for (size_t j = 0; j < sub_mesh->IndexVector.size(); j += 3)
						{
							unsigned int v_index_1 = sub_mesh->IndexVector[j] + base_index;
							unsigned int v_index_2 = sub_mesh->IndexVector[j + 1] + base_index;
							unsigned int v_index_3 = sub_mesh->IndexVector[j + 2] + base_index;
							obj_file_ptr << "f " << v_index_1 << "/" << v_index_1 << "/" << v_index_1 << " "
								<< v_index_2 << "/" << v_index_2 << "/" << v_index_2 << " "
								<< v_index_3 << "/" << v_index_3 << "/" << v_index_3 << "\n";
						}
						base_index += static_cast<int>(sub_mesh->PositionVector.size());
					}
				}
			}

			obj_file_ptr << "\n";
			try
			{
				//obj_file_ptr << ss.str().c_str();
				obj_file_ptr.close();
			}
			catch (std::exception &e)
			{
				GASS_LOG(LERROR) << "Failed to write obj data to file:" << e.what();
			}



			std::ofstream mtl_file_ptr;
			mtl_file_ptr.open((out_file + ".mtl").c_str());

			auto iter = materials.begin();
			while (iter != materials.end())
			{
				PhongMaterialConfig mat = iter->second;
				mtl_file_ptr << "\n";
				mtl_file_ptr << "newmtl " << iter->first << "\n";
				mtl_file_ptr << "Kd " << mat.Diffuse << "\n";
				mtl_file_ptr << "Ka " << mat.Ambient << "\n";
				mtl_file_ptr << "illum " << "1" << "\n";
				//if (mat.Textures.size() > 0 && mat.Textures[0] != "")
				//	mtl_file_ptr << "map_Kd " << mat.Textures[0] << "\n";
				mtl_file_ptr << "\n";
				++iter;
			}
			mtl_file_ptr.close();

			//copy textures
			if (m_CopyTextures)
			{
				iter = materials.begin();
				while (iter != materials.end())
				{
					PhongMaterialConfig mat = iter->second;
					/*if (mat.Textures.size() > 0)
					{
						std::string texture_name = mat.Textures[0];
						ResourceHandle res(texture_name);
						FileResourcePtr file_res;
						try
						{
							file_res = res.GetResource();
						}
						catch (...)
						{
							GASS_LOG(LWARNING) << "Failed to find texture:" << texture_name;
						}
						if (file_res)
						{
							std::string full_path = StringUtils::Replace(file_res->Path().GetFullPath(), "\\", "/");
							std::string out_path = FileUtils::RemoveFilename(out_file);
							out_path = out_path + texture_name;
							FileUtils::CopyFile(full_path, out_path);
						
						}
					}*/
					++iter;
				}
			}
		}
	}
}









