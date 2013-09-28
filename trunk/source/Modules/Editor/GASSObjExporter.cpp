#include "GASSObjExporter.h"
#include "Sim/GASS.h"

namespace GASS
{

	ObjExporter::ObjExporter() :
		m_CopyTextures(true),
		m_FlipDDSTexCoords(true)
	{


	}
	ObjExporter::~ObjExporter()
	{

	}

	void ObjExporter::Export(const std::string &out_file, SceneObjectPtr root_obj)
	{
		IComponentContainer::ComponentVector comps;
		root_obj->GetComponentsByClass<IMeshComponent>(comps,true);

		if(comps.size()>0)
		{
			std::vector<MeshData> mesh_data_vec;
			for(int i = 0;  i <  comps.size(); i++)
			{
				SceneObjectPtr obj = (DYNAMIC_PTR_CAST<BaseSceneComponent>(comps[i]))->GetSceneObject();
				MeshComponentPtr mesh =  DYNAMIC_PTR_CAST<IMeshComponent>(comps[i]);
				MeshData  mesh_data = mesh->GetMeshData();

				//Transform to world coordinates!
				GeometryComponentPtr geom = obj->GetFirstComponentByClass<IGeometryComponent>();
				LocationComponentPtr lc = obj->GetFirstComponentByClass<ILocationComponent>();
				if(lc)
				{
					Vec3 world_pos = lc->GetWorldPosition();
					Vec3 scale = lc->GetScale();
					Quaternion world_rot = lc->GetWorldRotation();
					Mat4 trans_mat;
					trans_mat.Identity();
					trans_mat.SetTransformation(world_pos,world_rot,scale);
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
					SubMeshDataPtr sub_mesh =   mesh_data_vec[i].SubMeshVector[h];
					//unsigned int base_index = PositionVector.size();

					if(sub_mesh->MaterialName != "")
						Materials[sub_mesh->MaterialName] = sub_mesh->Material;
					else 
					{
						sub_mesh->MaterialName = "DefaultMat";
						sub_mesh->Material = def_mat;
					}
					for(int j = 0 ; j < sub_mesh->PositionVector.size(); j++)
					{
						PositionVector.push_back(sub_mesh->PositionVector[j]);
					}

					//generate normals if not present
					if(sub_mesh->NormalVector.size() == 0)
					{
						sub_mesh->NormalVector.resize(sub_mesh->PositionVector.size());
						for(int j = 0 ; j < sub_mesh->IndexVector.size(); j += 3)
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

					for(int j = 0 ; j < sub_mesh->NormalVector.size(); j++)
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
						if(!dds_tex && m_FlipDDSTexCoords)
						{
							for(int j = 0 ; j < sub_mesh->TexCoordsVector[0].size(); j++)
							{
								Vec4 tc = sub_mesh->TexCoordsVector[0].at(j);
								tc.y = -tc.y;
								TexCoordVector.push_back(tc);
							}
						}
						else
						{
							for(int j = 0 ; j < sub_mesh->TexCoordsVector[0].size(); j++)
							{
								TexCoordVector.push_back(sub_mesh->TexCoordsVector[0].at(j));
							}
						}
					}
					else
					{
						for(int j = 0 ; j < sub_mesh->PositionVector.size(); j++)
						{
							TexCoordVector.push_back(Vec4(1,0,0,0));
						}
					}

					/* for(int j = 0 ; j < sub_mesh->IndexVector.size(); j++)
					{
					FaceVector.push_back(sub_mesh->IndexVector[j]+base_index);
					}*/



					//serlize
					//delete[] mesh_data_vec[i]->FaceVector;
					//delete[] mesh_data_vec[i]->VertexVector;
					//delete mesh_data_vec[i];
					//mesh_data_vec[i] = NULL;
				}
			}

			std::stringstream ss;
			std::ofstream file_ptr;   
			file_ptr.open(out_file + ".obj");      

			//std::string mat_file = out_file

			ss << "\n";
			ss << "mtllib " << FileUtils::GetFilename(out_file) << ".mtl" << "\n";
			ss << "\n";

			ss << "#num verts" << PositionVector.size() << "\n";
			//Serialize mesh data to obj format!
			for(size_t i = 0; i < PositionVector.size() ; i++)
				ss << "v " << PositionVector[i] << "\n";
			ss << "\n";
			for(size_t i = 0; i < TexCoordVector.size() ; i++)
				ss << "vt " << TexCoordVector[i].x  << " " << TexCoordVector[i].y << "\n";
			ss << "\n";
			for(size_t i = 0; i < NormalVector.size() ; i++)
				ss << "vn " << NormalVector[i] << "\n";
			ss << "\n";

			int sub_mesh_index = 0;
			unsigned int base_index = 1;//VertexVector.size();
			for(size_t i = 0; i < mesh_data_vec.size() ; i++)
			{
				for(size_t h = 0; h < mesh_data_vec[i].SubMeshVector.size() ; h++)
				{
					SubMeshDataPtr sub_mesh =   mesh_data_vec[i].SubMeshVector[h];
					ss << "g " << "submesh" << sub_mesh_index << "\n";
					sub_mesh_index++;

					ss << "usemtl " << sub_mesh->MaterialName << "\n";


					ss << "#num faces" << (sub_mesh->IndexVector.size()/3) << "\n";


					for(int j = 0 ; j < sub_mesh->IndexVector.size(); j += 3)
					{
						unsigned int v_index_1 = sub_mesh->IndexVector[j] + base_index;
						unsigned int v_index_2 = sub_mesh->IndexVector[j+1] + base_index;
						unsigned int v_index_3 = sub_mesh->IndexVector[j+2] + base_index;
						ss << "f " << v_index_1 << "/" << v_index_1 << "/" << v_index_1 << " "
							<< v_index_2 << "/" << v_index_2 << "/" << v_index_2 << " "
							<< v_index_3 << "/" << v_index_3 << "/" << v_index_3 << "\n";
					}
					base_index += sub_mesh->PositionVector.size();
				}
			}

			/*ss << "g " << "root" << "\n";

			ss << "usemtl " << "ExportMat1" << "\n";

			ss << "#num faces" << (FaceVector.size()/3) << "\n";


			for(size_t i = 0; i < FaceVector.size() ; i += 3)
			ss << "f " << FaceVector[i]+1 << "/" << FaceVector[i]+1 << "/" << i/3+1 << " "
			<< FaceVector[i+1]+1 << "/" << FaceVector[i+1]+1 << "/" << i/3+1 << " "
			<< FaceVector[i+2]+1 << "/" << FaceVector[i+2]+1 << "/" << i/3+1 << "\n";
			*/
			ss << "\n";
			file_ptr << ss.str().c_str();     
			file_ptr.close(); 
			{
				std::stringstream ss;
				std::ofstream file_ptr;   
				file_ptr.open(out_file + ".mtl");

				std::map<std::string,GraphicsMaterial>::iterator iter = Materials.begin();
				while(iter != Materials.end())
				{
					GraphicsMaterial mat = iter->second;
					ss << "\n";
					ss << "newmtl " << iter->first << "\n";
					ss << "Kd " << mat.GetDiffuse() << "\n";
					ss << "Ka " << mat.GetAmbient() << "\n";
					ss << "illum " << "1" << "\n";
					if(mat.Textures.size() > 0)
						ss << "map_Kd " << mat.Textures[0] << "\n";
					ss << "\n";
					iter++;
				}
				file_ptr << ss.str().c_str();     
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
									boost::filesystem::copy_file(boost::filesystem::path(full_path),boost::filesystem::path(out_path),boost::filesystem::copy_option::overwrite_if_exists);
								} 
								catch (const boost::filesystem::filesystem_error& e)
								{
									//std::cerr << "Error: " << e.what() << std::endl;
									LogManager::getSingleton().stream() << "WARNING: Failed copy texture during export:" << e.what();
								}
							}
						}
						iter++;
					}
				}
			}
		}
	}
}









