// CoreTest.cpp : Defines the entry point for the console application.
//

#ifndef SIM_APPLICATION_H
#define SIM_APPLICATION_H


#include "Sim/GASS.h"
#include "Plugins/Game/GameMessages.h"
#include "tinyxml.h"


#include <stdio.h>
#include <iostream>
#include <fstream>

class SimApplication : public GASS::IMessageListener, public boost::enable_shared_from_this<SimApplication> 
{
protected:
	std::string m_SceneName;
	GASS::SimEngine* m_Engine;
	GASS::SceneWeakPtr m_Scene;
	GASS::Timer* m_Timer;
	double m_UpdateFreq;
public:
	SimApplication() :
	  m_SceneName("new_terrain"),
		  m_Timer(new GASS::Timer()),
		  m_UpdateFreq(60)

	  {


	  }
	  virtual ~SimApplication()
	  {
		  delete m_Engine;
	  }

	  virtual bool Init()
	  {
		  m_Engine = new GASS::SimEngine();
		  m_Engine->Init(GASS::FilePath("GASSSimDemo.xml"));

		  GASS::GraphicsSystemPtr gfx_sys = m_Engine->GetSimSystemManager()->GetFirstSystemByClass<GASS::IGraphicsSystem>();

		  GASS::RenderWindowPtr win = gfx_sys->CreateRenderWindow("MainWindow",800,600);
		  win->CreateViewport("MainViewport", 0, 0, 1, 1);

		  GASS::InputSystemPtr input_system = GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<GASS::IInputSystem>();
		  input_system->SetMainWindowHandle(win->GetHWND());

		  GASS::LogManager::getSingleton().stream() << "SimApplication::Init -- Start Loading Scene:" <<  m_SceneName;
		  m_Scene = m_Engine->CreateScene("NewScene");
		  //m_Scene = GASS::SimEngine::Get().CreateScene("NewScene");
		  GASS::ScenePtr scene = GASS::ScenePtr(m_Scene);
		  scene->Load(m_SceneName);
		  //m_Scene->Load(m_SceneName);
		  GASS::LogManager::getSingleton().stream() << "SimApplication::Init -- Scene Loaded:" << m_SceneName;
		  //create free camera and set start pos
		  GASS::SceneObjectPtr free_obj = scene->LoadObjectFromTemplate("FreeCameraObject",scene->GetRootSceneObject());
		  GASS::MessagePtr pos_msg(new GASS::PositionMessage(scene->GetStartPos()));
		  if(free_obj)
		  {
			  free_obj->SendImmediate(pos_msg);

			  GASS::SystemMessagePtr camera_msg(new GASS::ChangeCameraRequest(free_obj->GetFirstComponentByClass<GASS::ICameraComponent>()));
			  m_Engine->GetSimSystemManager()->PostMessage(camera_msg);
		  }
		  using namespace GASS;
		  SceneObjectPtr root_obj = scene->GetSceneryRoot();
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
						  for(int j = 0 ; j < sub_mesh->FaceVector.size(); j += 3)
						  {
							  Vec3 p1 = sub_mesh->PositionVector[sub_mesh->FaceVector[j]];
							  Vec3 p2 = sub_mesh->PositionVector[sub_mesh->FaceVector[j+1]];
							  Vec3 p3 = sub_mesh->PositionVector[sub_mesh->FaceVector[j+2]];
							  Vec3 v1 = p1 - p3;
							  Vec3 v2 = p2 - p3;
							  Vec3 norm = Math::Cross(v1,v2);
							  norm.FastNormalize();
							  sub_mesh->NormalVector[sub_mesh->FaceVector[j]] = norm;
							  sub_mesh->NormalVector[sub_mesh->FaceVector[j+1]] = norm;
							  sub_mesh->NormalVector[sub_mesh->FaceVector[j+2]] = norm;
						  }
					  }

					  for(int j = 0 ; j < sub_mesh->NormalVector.size(); j++)
					  {
						  NormalVector.push_back(sub_mesh->NormalVector[j]);
					  }

					  bool flip_tex = false;
					  if(sub_mesh->Material.Textures.size() > 0)
					  {
						  std::string ext = StringUtils::ToLower(FileUtils::GetExtension(sub_mesh->Material.Textures[0]));
						   if(ext == "dds") 
							   flip_tex = true;
					  }

					  if(sub_mesh->TexCoordsVector.size() > 0 && sub_mesh->TexCoordsVector[0].size() > 0)
					  {
						  for(int j = 0 ; j < sub_mesh->TexCoordsVector[0].size(); j++)
						  {
							  if(flip_tex)
								TexCoordVector.push_back(sub_mesh->TexCoordsVector[0].at(j));
							  else
							  {
								 Vec4 tc = sub_mesh->TexCoordsVector[0].at(j);
								 tc.y = -tc.y;
								 TexCoordVector.push_back(tc);
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

					  /* for(int j = 0 ; j < sub_mesh->FaceVector.size(); j++)
					  {
					  FaceVector.push_back(sub_mesh->FaceVector[j]+base_index);
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
			  file_ptr.open("test.obj");      

			  ss << "\n";
			  ss << "mtllib " << "test.mtl" << "\n";
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
					  

					  ss << "#num faces" << (sub_mesh->FaceVector.size()/3) << "\n";


					  for(int j = 0 ; j < sub_mesh->FaceVector.size(); j += 3)
					  {
						  unsigned int v_index_1 = sub_mesh->FaceVector[j] + base_index;
						  unsigned int v_index_2 = sub_mesh->FaceVector[j+1] + base_index;
						  unsigned int v_index_3 = sub_mesh->FaceVector[j+2] + base_index;
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
				  file_ptr.open("test.mtl");

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
				  iter = Materials.begin();
				  while(iter != Materials.end())
				  {
					  GraphicsMaterial mat = iter->second;
					  if(mat.Textures.size() > 0)
					  {
						  std::string texture_name =  mat.Textures[0];
						  GASS::ResourceHandle res(texture_name);
						  std::string full_path = StringUtils::Replace(res.GetResource()->Path().GetFullPath(),"\\","/");
						  //std::string base_path = "C:/dev/gassbuild/data";
						  //full_path = StringUtils::Replace(full_path,"../../data",base_path);
						  try
						  {
							  boost::filesystem::copy_file(boost::filesystem::path(full_path),boost::filesystem::path("./" + texture_name),boost::filesystem::copy_option::overwrite_if_exists);
						  } 
						  catch (const boost::filesystem::filesystem_error& e)
						  {
							  std::cerr << "Error: " << e.what() << std::endl;
						  }
					  }
					  iter++;
				  }
			  }
		  }


		  /*		  GASS::SceneObjectPtr object  = scene->LoadObjectFromTemplate("JimTank",scene->GetRootSceneObject());
		  GASS::Vec3 pos = scene->GetStartPos();
		  GASS::MessagePtr pos_msg2(new GASS::WorldPositionMessage(pos));
		  if(object)
		  object->SendImmediate(pos_msg2);*/

		  /*for(int i = 0; i <  m_Objects.size();i++)
		  {
		  GASS::SceneObjectPtr object  = scene->LoadObjectFromTemplate(m_Objects[i],scene->GetRootSceneObject());
		  GASS::Vec3 pos = scene->GetStartPos();
		  pos.z -= 10*(i+1);

		  GASS::MessagePtr pos_msg(new GASS::WorldPositionMessage(pos));
		  if(object)
		  object->SendImmediate(pos_msg);
		  }*/
		  m_Engine->GetSimSystemManager()->PostMessage(GASS::SystemMessagePtr(new GASS::GUIScriptRequest("GUI.xml")));
		  return true;
	  }


	  /*virtual bool LoadConfig(const std::string filename)
	  {
	  if(filename =="") return false;
	  TiXmlDocument *xmlDoc = new TiXmlDocument(filename.c_str());
	  if (!xmlDoc->LoadFile())
	  {
	  // Fatal error, cannot load
	  GASS::LogManager::getSingleton().stream() << "WARNING: SimApplication::LoadConfig() - Couldn't load xmlfile: " << filename;
	  return 0;
	  }
	  TiXmlElement *app_settings = xmlDoc->FirstChildElement("SimApplication");

	  if(app_settings)
	  {
	  m_ControlSettings = app_settings->Attribute("ControlSettings");
	  m_SceneName = app_settings->Attribute("Scene");
	  GASS::FilePath full_path(m_SceneName);
	  m_SceneName = full_path.GetFullPath();
	  TiXmlElement *object_elem = app_settings->FirstChildElement("Objects");
	  if(object_elem)
	  {
	  object_elem = object_elem->FirstChildElement("Object");
	  while(object_elem)
	  {
	  std::string name = object_elem->Attribute("Name");
	  m_Objects.push_back(name);
	  object_elem = object_elem->NextSiblingElement();
	  }
	  }
	  }
	  return true;
	  //TiXmlElement *sys_config =  app_settings->FirstChildElement("SystemConfig();
	  }*/

	  virtual bool Update()
	  {
		  m_Engine->Update();
		  return true;
	  }

};
#endif




