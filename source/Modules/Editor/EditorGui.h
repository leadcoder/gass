#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"

//#include "windows.h"
//#include "tinyfiledialogs.h"
#include "Core/Utils/GASSColorRGB.h"
#include "Core/Math/GASSAABox.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSceneObjectRef.h"
#include "Sim/GASSSimEngine.h"
#include "EditorSceneManager.h"
#include "ToolSystem/MouseToolController.h"
#include "Sim/GASSSceneObjectTemplateManager.h"

#include "Sim/Interface/GASSIGraphComponent.h"
#include "Sim/Interface/GASSIGraphNodeComponent.h"
#include "Sim/Interface/GASSIGraphEdgeComponent.h"
#include "Modules/Editor/ToolSystem/GraphTool.h"


namespace GASS
{
	class EditorGui
	{
	public:
		EditorGui()
		{
		}

		void drawUi()
		{
			dockingBegin();

			if (m_ShowObjectTree)
				drawObjectTree();

			if (m_ShowProperties)
				drawProperties();
			if (m_ShowTools)
				drawTools();
			if (m_ShowTemplates)
				drawTemplates();
		}
	protected:
		bool m_ShowProperties = true;
		bool m_ShowObjectTree = true;
		bool m_ShowLog = false;
		bool m_ShowLayers = false;
		bool m_ShowTools = true;
		bool m_ShowTemplates = true;
		Scene* m_SceneSelected = nullptr;
		std::string m_DropTemplate;

		void drawMainMenu()
		{
			// Menu Bar
			ImGui::BeginMenuBar();
			//if (open)
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("New"))
					{
						m_SceneSelected = nullptr;
						auto scene = getFirstScene();
						if (scene)
							SimEngine::Get().DestroyScene(scene);
						scene = ScenePtr(SimEngine::Get().CreateScene("NewScene"));
						scene->GetFirstSceneManagerByClass<EditorSceneManager>()->SetObjectSite(scene->GetSceneryRoot());
						scene->GetFirstSceneManagerByClass<EditorSceneManager>()->CreateCamera();
					}

					if (ImGui::BeginMenu("Load Scene"))
					{
						auto scenes= GASS::SimEngine::Get().GetSavedScenes();
						for (size_t i = 0; i < scenes.size(); i++)
						{
							if (ImGui::MenuItem(scenes[i].c_str()))
							{
								m_SceneSelected = nullptr;
								auto scene = getFirstScene();
								if (scene)
									SimEngine::Get().DestroyScene(scene);
								scene = ScenePtr(SimEngine::Get().CreateScene(scenes[i]));
								scene->Load(scenes[i]);
								scene->GetFirstSceneManagerByClass<EditorSceneManager>()->SetObjectSite(scene->GetSceneryRoot());
								scene->GetFirstSceneManagerByClass<EditorSceneManager>()->CreateCamera();
								break;
							}
						}
						ImGui::EndMenu();
					}
					
					if (ImGui::MenuItem("Save", "Ctrl+S"))
					{
						auto scene = getFirstScene();
						if (scene && scene->GetName() != "")
							scene->Save(scene->GetName());
						//char const* filterPatterns[1] = { "*.earth" };
						//if (char const* fileToSave = tinyfd_saveFileDialog("Save File", "", 1, filterPatterns, nullptr))
						{

						}
					}

					if (ImGui::MenuItem("Exit"))
					{
						exit(0);
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("View"))
				{
					ImGui::MenuItem("Object Tree", NULL, &m_ShowObjectTree);
					ImGui::MenuItem("Properties", NULL, &m_ShowProperties);
					ImGui::MenuItem("Log", NULL, &m_ShowLog);
					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			//ImGuiViewportP* viewport = (ImGuiViewportP*)(void*)ImGui::GetMainViewport();
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
			float height = ImGui::GetFrameHeight();

			if (ImGui::BeginViewportSideBar("##StatusBar", viewport, ImGuiDir_Down, height, window_flags)) {
				if (ImGui::BeginMenuBar()) {
					auto scene = getFirstScene();
					if(scene)
						ImGui::Text("Loaded Scene:%s", scene->GetName().c_str());
					ImGui::EndMenuBar();
				}
				ImGui::End();
			}

			
		}

		void dockingBegin()
		{
			// ImGui code goes here...
			//ImGui::ShowDemoWindow();
			//return;

			ImGuiWindowFlags window_dock_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_dock_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_dock_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			window_dock_flags |= ImGuiWindowFlags_NoBackground;

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("DockSpace Main", nullptr, window_dock_flags);
			ImGui::PopStyleVar();
			ImGui::PopStyleVar(2);

			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
				ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
			}
			drawMainMenu();
			ImGui::End();
		}


		void drawScene(Scene* scene)
		{
			bool node_open = ImGui::TreeNode(scene->GetName().c_str());
			
			if (ImGui::IsItemClicked())
			{
				auto editor = scene->GetFirstSceneManagerByClass<EditorSceneManager>();
				editor->SetSelectedObjects({ });
				m_SceneSelected = scene;
			}

			if (node_open)
			{
				drawSceneObject(scene->GetRootSceneObject());
				ImGui::TreePop();
			}
		}

		void drawSceneObject(SceneObjectPtr so)
		{
			auto editor = so->GetScene()->GetFirstSceneManagerByClass<EditorSceneManager>();
			SceneObjectPtr selected = editor->GetFirstSelectedObject();

			ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
			if (so == selected)
				base_flags |= ImGuiTreeNodeFlags_Selected;

			bool is_leaf = true;
			if (so->GetNumChildren() > 0)
			{
				is_leaf = false;
			}
			base_flags |= is_leaf ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_None;
			bool node_open = ImGui::TreeNodeEx(so.get(), base_flags, so->GetName().c_str());

			if (ImGui::IsItemClicked())
			{
				editor->SetSelectedObjects({ so });
			}

			if (m_DropTemplate != "" && ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE"))
				{
					auto new_obj = SimEngine::Get().CreateObjectFromTemplate(m_DropTemplate);
					so->AddChildSceneObject(new_obj, true);
					m_DropTemplate = "";
				}
				ImGui::EndDragDropTarget();
			}

			bool node_deleted = false;

			if (ImGui::BeginPopupContextItem())
			{

				if (ImGui::MenuItem("Delete"))
				{
					so->GetParentSceneObject()->RemoveChildSceneObject(so);
					node_deleted = true;
				}

				if (ImGui::MenuItem("Set as parent for new objects"))
				{
					editor->SetObjectSite(so);
				}
				
				if (so->GetFirstComponentByClass<IGraphComponent>().get())
				{
					if (ImGui::MenuItem("Add Node..."))
					{
						OnAddGraphNode(so);
					}

					if (ImGui::MenuItem("Insert Node..."))
					{
						OnInsertGraphNode(so);
					}
				}

				if (so->GetFirstComponentByClass<IGraphNodeComponent>().get())
				{
					if (ImGui::MenuItem("Add Node..."))
					{
						OnAddGraphNode(so);
					}
				}

				

				ImGui::EndPopup();

			}

			if (node_open)
			{
				if (!node_deleted)
				{
					for (int i = 0; i < so->GetNumChildren(); i++)
					{
						drawSceneObject(std::static_pointer_cast<SceneObject>(so->GetChild(i)));
					}
				}
				ImGui::TreePop();
			}
		}

		void drawObjectTree()
		{
			auto scene = getFirstScene();
			if (!scene)
				return;
			ImGuiWindowFlags window_flags = 0;
			if (!ImGui::Begin("Object tree", &m_ShowObjectTree, window_flags))
			{
				// Early out if the window is collapsed, as an optimization.
				ImGui::End();
				return;
			}
			drawScene(scene.get());
			ImGui::End();
		}

		template<typename TYPE>
		GASS::TypedProperty<TYPE>* AsTypedProp(GASS::IProperty* prop)
		{
			if (*prop->GetTypeID() == typeid(TYPE))
			{
				return dynamic_cast<GASS::TypedProperty<TYPE>*>(prop);
			}
			return nullptr;
		}

		ScenePtr getFirstScene()
		{
			ScenePtr scene;
			SimEngine::SceneIterator iter = SimEngine::Get().GetScenes();
			if (iter.hasMoreElements())
			{
				scene = iter.getNext();
			}
			return scene;
		}

		void drawProp(BaseReflectionObject* obj, IProperty* prop)
		{
			const std::string prop_name = prop->GetName();

			if (prop->GetFlags() & GASS::PF_VISIBLE)
			{
				if (prop->HasMetaData())
				{
					GASS::PropertyMetaDataPtr meta_data = prop->GetMetaData();
					const bool editable = (prop->GetFlags() & GASS::PF_EDITABLE);
					const std::string documentation = prop->GetDescription();
					if (GASS_DYNAMIC_PTR_CAST<GASS::FilePathPropertyMetaData>(meta_data))
					{
						//item = m_VariantManager->addProperty(QtVariantPropertyManager::enumTypeId(), prop_name.c_str());
						GASS::FilePathPropertyMetaDataPtr file_path_data = GASS_DYNAMIC_PTR_CAST<GASS::FilePathPropertyMetaData>(meta_data);
						std::vector<std::string> exts = file_path_data->GetExtensions();
						GASS::FilePathPropertyMetaData::FilePathEditType type = file_path_data->GetType();

						std::string filter;
						for (size_t i = 0; i < exts.size(); i++)
						{
							if (i != 0)
								filter += " ";
							filter += "*.";
							filter += exts[i];
						}

						std::string filename = prop->GetValueAsString(obj);
						filename = GASS::StringUtils::Replace(filename, "/", "\\");

						//m_VariantManager->setAttribute(item,QLatin1String("filter"),QVariant("*.png *.jpg"));
						switch (type)
						{
						case GASS::FilePathPropertyMetaData::IMPORT_FILE:
							//item = m_VariantManager->addProperty(filePathTypeId(), prop_name.c_str());
							//item->setValue(filename.c_str());
							//item->setAttribute(QLatin1String("filter"), QVariant(filter.c_str()));
							break;
						case GASS::FilePathPropertyMetaData::EXPORT_FILE:
							//item = m_VariantManager->addProperty(newFileTypeId(), prop_name.c_str());
							//item->setValue(filename.c_str());
							//item->setAttribute(QLatin1String("filter"), QVariant(filter.c_str()));
							break;
						case GASS::FilePathPropertyMetaData::PATH_SELECTION:
							break;
						}
					}
				}
				else //if (!item)
				{
					const bool editable = (prop->GetFlags() & GASS::PF_EDITABLE);
					const bool multi = (prop->GetFlags() & GASS::PF_MULTI_OPTIONS);
					if (prop->HasOptions())
					{
						std::vector<std::string> options = prop->GetStringOptions();
						std::vector<std::string> option_names = options;

						if (prop->HasObjectOptions())
						{
							std::vector<std::string> object_options = prop->GetStringOptionsByObject(obj);
							options.insert(options.end(), object_options.begin(), object_options.end());
							if (auto sor_prop = AsTypedProp<SceneObjectRef>(prop))
							{
								std::vector<SceneObjectRef> sor_options = sor_prop->GetOptionsByObject(obj);
								for (auto ref : sor_options)
								{
									option_names.push_back(ref->GetName());
								}
							}
							else if (auto sor_vec_prop = AsTypedProp<std::vector<SceneObjectRef> >(prop))
							{
								std::vector<SceneObjectRef> sor_options = sor_vec_prop->GetOptionsByObject(obj);
								for (auto ref : sor_options)
								{
									option_names.push_back(ref->GetName());
								}
							}
							else
								option_names.insert(option_names.end(), object_options.begin(), object_options.end());
						}

						if (multi)
						{
							const bool open = ImGui::TreeNode(prop_name.c_str());
							if (open)
							{
								const std::string prop_value = prop->GetValueAsString(obj);
								bool clicked = false;
								std::string values;
								for (auto& option : options)
								{
									bool value = prop_value.find(option) != std::string::npos;
									if (ImGui::Checkbox(option.c_str(), &value))
									{
										clicked = true;
									}
									if (value)
										values += " " + option;
								}
								if (clicked)
									prop->SetValueByString(obj, values);
								ImGui::TreePop();
							}
						}
						else
						{
							const std::string prop_value = prop->GetValueAsString(obj);
							int item_current = -1;
							std::string combo_options;
							for (size_t i = 0; i < options.size(); i++)
							{
								combo_options = combo_options + options[i];
								combo_options.append(1, '\0');
								if (prop_value == options[i])
									item_current = (int)i;
							}
							combo_options.append(1, '\0');
							if (ImGui::Combo(prop_name.c_str(), &item_current, combo_options.c_str()))
							{
								prop->SetValueByString(obj, options[item_current]);
							}
						}
					}
					else
					{
						float drag_speed = 0.1f;
						if (*prop->GetTypeID() == typeid(bool))
						{
							bool value = false;
							obj->GetPropertyValue(prop, value);
							if (ImGui::Checkbox(prop_name.c_str(), &value))
							{
								obj->SetPropertyValue(prop, value);
							}
						}
						else if (*prop->GetTypeID() == typeid(float))
						{
							float value = 0;
							obj->GetPropertyValue(prop, value);
							if (ImGui::DragFloat(prop_name.c_str(), &value, drag_speed))
							{
								obj->SetPropertyValue(prop, value);
							}
						}
						else if (*prop->GetTypeID() == typeid(double))
						{
							double value = 0;
							obj->GetPropertyValue(prop, value);
							if (ImGui::DragScalarN(prop_name.c_str(), ImGuiDataType_Double , &value,1, drag_speed))
							{
								obj->SetPropertyValue(prop, value);
							}
						}
						else if (*prop->GetTypeID() == typeid(Vec2d))
						{
							Vec2d value;
							obj->GetPropertyValue(prop, value);
							if (ImGui::DragScalarN(prop_name.c_str(), ImGuiDataType_Double, (void*)&value.x, 2, drag_speed))
							{
								obj->SetPropertyValue(prop, value);
							}
						}
						else if (*prop->GetTypeID() == typeid(Vec3d))
						{
							Vec3d value;
							obj->GetPropertyValue(prop, value);
							if (ImGui::DragScalarN(prop_name.c_str(), ImGuiDataType_Double, (void*)&value.x,3, drag_speed))
							{
								obj->SetPropertyValue(prop, value);
							}
						}
						else if (*prop->GetTypeID() == typeid(Vec4d))
						{
							Vec4d value;
							obj->GetPropertyValue(prop, value);
							if (ImGui::DragScalarN(prop_name.c_str(), ImGuiDataType_Double, (void*)&value.x, 4, drag_speed))
							{
								obj->SetPropertyValue(prop, value);
							}
						}
						else if (*prop->GetTypeID() == typeid(Vec3f))
						{
							Vec3f value;
							obj->GetPropertyValue(prop, value);
							if (ImGui::DragScalarN(prop_name.c_str(), ImGuiDataType_Float, (void*)&value.x, 2, drag_speed))
							{
								obj->SetPropertyValue(prop, value);
							}
						}
						else if (*prop->GetTypeID() == typeid(Vec3f))
						{
							Vec3f value;
							obj->GetPropertyValue(prop, value);
							if (ImGui::DragScalarN(prop_name.c_str(), ImGuiDataType_Float, (void*)&value.x, 3, drag_speed))
							{
								obj->SetPropertyValue(prop, value);
							}
						}
						else if (*prop->GetTypeID() == typeid(Vec4f))
						{
							Vec4f value;
							obj->GetPropertyValue(prop, value);
							if (ImGui::DragScalarN(prop_name.c_str(), ImGuiDataType_Float, (void*)&value.x, 4, drag_speed))
							{
								obj->SetPropertyValue(prop, value);
							}
						}
						else if (*prop->GetTypeID() == typeid(Quaterniond))
						{
							Quaterniond value;
							obj->GetPropertyValue(prop, value);
							if (ImGui::DragScalarN(prop_name.c_str(), ImGuiDataType_Float, (void*)&value.x, 4, drag_speed))
							{
								obj->SetPropertyValue(prop, value);
							}
						}
						else if (*prop->GetTypeID() == typeid(Quaternionf))
						{
							Quaternionf value;
							obj->GetPropertyValue(prop, value);
							if (ImGui::DragScalarN(prop_name.c_str(), ImGuiDataType_Float, (void*)&value.x, 4, drag_speed))
							{
								obj->SetPropertyValue(prop, value);
							}
						}
						else if (*prop->GetTypeID() == typeid(EulerRotation))
						{
							EulerRotation value;
							obj->GetPropertyValue(prop, value);
							if (ImGui::DragScalarN(prop_name.c_str(), ImGuiDataType_Double, (void*)&value.Heading, 3, 0.1f))
							{
								obj->SetPropertyValue(prop, value);
							}
						}
						else if (*prop->GetTypeID() == typeid(ColorRGB))
						{
							ColorRGB color;
							obj->GetPropertyValue(prop, color);

							float col3[3] = { (float)color.r, (float)color.g, (float)color.b };
							if (ImGui::ColorEdit3(prop_name.c_str(), col3))
							{
								obj->SetPropertyValue(prop, ColorRGB(double(col3[0]), double(col3[1]), double(col3[2])));
							}
						}
						else if (*prop->GetTypeID() == typeid(ColorRGBA))
						{
							ColorRGBA color;
							obj->GetPropertyValue(prop, color);

							float col4[4] = { static_cast<float>(color.r), static_cast<float>(color.g), static_cast<float>(color.b), static_cast<float>(color.a) };
							if (ImGui::ColorEdit4(prop_name.c_str(), col4))
							{
								obj->SetPropertyValue(prop, ColorRGBA(col4[0], col4[1], col4[2], col4[3]));
							}
						}
						else if (*prop->GetTypeID() == typeid(AABoxd))
						{
							AABox bb;
							obj->GetPropertyValue(prop, bb);
							std::string min_name = prop_name + " Min";
							if (ImGui::DragScalarN(min_name.c_str(), ImGuiDataType_Double, (void*)&bb.Min, 3, 0.1f))
							{
								obj->SetPropertyValue(prop, bb);
							}
							std::string max_name = prop_name + " Max";
							if (ImGui::DragScalarN(max_name.c_str(), ImGuiDataType_Double, (void*)&bb.Max, 3, 0.1f))
							{
								obj->SetPropertyValue(prop, bb);
							}
						}
						else if (*prop->GetTypeID() == typeid(GASS::FilePath))
						{
							//std::string filename = prop_value;
							//filename = GASS::StringUtils::Replace(filename, "/", "\\");
							if (ImGui::Button(prop_name.c_str()))
							{

							}
						}
						else
						{
							//const std::string prop_name = prop->GetName();
							std::string prop_value = prop->GetValueAsString(obj);
							if (ImGui::InputText(prop_name.c_str(), &prop_value))
							{
								
							}
							if(ImGui::IsItemDeactivated())
								prop->SetValueByString(obj, prop_value);

						}

					}
				}
			}
		}

		void drawProperties()
		{
			ScenePtr scene = getFirstScene();
			if (!scene)
				return;
			ImGuiWindowFlags window_flags = 0;
			if (!ImGui::Begin("Properties", &m_ShowProperties, window_flags))
			{
				// Early out if the window is collapsed, as an optimization.
				ImGui::End();
				return;
			}

			auto* so = scene->GetFirstSceneManagerByClass<EditorSceneManager>()->GetFirstSelectedObject().get();
			
			if (so)
			{
				m_SceneSelected = nullptr;
				auto props = so->GetProperties();
				for (auto prop : props)
				{
					drawProp(so, prop);
				}

				auto comp_iter = so->GetComponents();
				while (comp_iter.hasMoreElements())
				{
					GASS::ComponentPtr comp = GASS_STATIC_PTR_CAST<GASS::Component>(comp_iter.getNext());
					std::string class_name = comp->GetRTTI()->GetClassName();
					if (comp->HasMetaData() && comp->GetMetaData()->GetFlags() & GASS::OF_VISIBLE) //we have settings!
					{
						ImGui::SetNextItemOpen(true, ImGuiCond_Once);
						const bool open = ImGui::TreeNode(class_name.c_str());
						if (open)
						{
							auto comp_props = comp->GetProperties();
							for (auto comp_prop : comp_props)
							{
								drawProp(comp.get(), comp_prop);
							}
							ImGui::TreePop();
						}
					}
				}
			}
			else if (m_SceneSelected)
			{
				std::string scene_name = m_SceneSelected->GetName();
				if (ImGui::InputText("Name", &scene_name))
				{
					m_SceneSelected->SetName(scene_name);
				}
				auto props = m_SceneSelected->GetProperties();
				for (auto prop : props)
				{
					drawProp(m_SceneSelected, prop);
				}
				auto sms = m_SceneSelected->GetSceneManagers();
				for (auto sm : sms)
				{
					GASS::BaseReflectionObjectPtr obj = GASS_DYNAMIC_PTR_CAST<GASS::BaseReflectionObject>(sm);
					auto sm_props = obj->GetProperties();
					for (auto prop : sm_props)
					{
						drawProp(obj.get(), prop);
					}
				}
			}


			ImGui::End();
		}

		void drawTools()
		{
			ScenePtr scene = getFirstScene();
			if (!scene)
				return;
			ImGuiWindowFlags window_flags = 0;
			if (!ImGui::Begin("Tools", &m_ShowTools, window_flags))
			{
				// Early out if the window is collapsed, as an optimization.
				ImGui::End();
				return;
			}

			auto* esm= scene->GetFirstSceneManagerByClass<EditorSceneManager>().get();
			std::string tool_name;
			if (auto* active_tool = esm->GetMouseToolController()->GetActiveTool())
			{
				tool_name =  active_tool->GetName();
			}
			std::vector<std::string> tools = { TID_SELECT ,TID_MOVE,TID_ROTATE,TID_GOTO_POS,TID_BOX };

			int e = -1;
			for (size_t i = 0; i < tools.size(); i++)
			{
				if (tools[i] == tool_name)
					e = int(i);
				if (ImGui::RadioButton(tools[i].c_str(), &e,int(i)))
					esm->GetMouseToolController()->SelectTool(tools[i]);
			}
			
			ImGui::End();
		}

		void drawTemplates()
		{
			ScenePtr scene = getFirstScene();
			if (!scene)
				return;
			ImGuiWindowFlags window_flags = 0;
			if (!ImGui::Begin("Templates", &m_ShowTemplates, window_flags))
			{
				// Early out if the window is collapsed, as an optimization.
				ImGui::End();
				return;
			}

			ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_Leaf;// | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
			//if (so == selected)
			//base_flags |= ImGuiTreeNodeFlags_Selected;

			

			auto templates = SimEngine::Get().GetSceneObjectTemplateManager()->GetTemplateNames();
			for (auto temp : templates)
			{
				GASS::SceneObjectTemplatePtr sot = GASS_DYNAMIC_PTR_CAST<GASS::SceneObjectTemplate>(GASS::SimEngine::Get().GetSceneObjectTemplateManager()->GetTemplate(temp));
				if (sot->GetInstantiable())
				{
					bool node_open = ImGui::TreeNodeEx(sot.get(), base_flags, temp.c_str());

					

					if (ImGui::BeginDragDropSource())
					{
						m_DropTemplate = temp;
						//uintptr_t addr = reinterpret_cast<uintptr_t>(node);
						ImGui::SetDragDropPayload("_TREENODE", nullptr, 0);
						ImGui::Text("Drop in scene");
						ImGui::EndDragDropSource();
					}
					
					if (node_open)
						ImGui::TreePop();
					
				}
			}
			ImGui::End();

			static bool dragging = false;
			if (!ImGui::IsMouseDragging(0) && dragging)
			{
				if (m_DropTemplate != "")
				{
					auto* esm = scene->GetFirstSceneManagerByClass<EditorSceneManager>().get();
					ImVec2 screen_pos = ImGui::GetIO().MousePos;
					ImVec2 whole_content_size = ImGui::GetIO().DisplaySize;
					float x = float(screen_pos.x) / float(whole_content_size.x);
					float y = float(screen_pos.y) / float(whole_content_size.y);
					esm->GetMouseToolController()->CreateSceneObject(m_DropTemplate,Vec2(x, y));
					m_DropTemplate = "";
					//auto so = SimEngine::Get().CreateObjectFromTemplate(m_DropTemplate);
				}
				dragging = false;
			}
			if (ImGui::IsMouseDragging(0))
			{
				dragging = true;
			}
		}



		void OnAddGraphNode(SceneObjectPtr obj)
		{
			if (obj)
			{
				GraphComponentPtr graph = obj->GetFirstComponentByClass<IGraphComponent>();
				if (graph)
				{
					EditorSceneManagerPtr sm = obj->GetScene()->GetFirstSceneManagerByClass<EditorSceneManager>();
					sm->GetMouseToolController()->SelectTool(TID_GRAPH);
					GraphTool* tool = static_cast<GraphTool*> (sm->GetMouseToolController()->GetTool(TID_GRAPH));
					tool->SetMode(GASS::GTM_ADD);
					tool->SetParentObject(obj);
					tool->SetConnetionObject(SceneObjectPtr());
					tool->SetNodeTemplateName(graph->GetNodeTemplate());
					tool->SetEdgeTemplateName(graph->GetEdgeTemplate());
				}

				GraphNodeComponentPtr node = obj->GetFirstComponentByClass<IGraphNodeComponent>();
				if (node)
				{
					GraphComponentPtr graph = obj->GetParentSceneObject()->GetFirstComponentByClass<IGraphComponent>();
					if (graph)
					{
						EditorSceneManagerPtr sm = obj->GetScene()->GetFirstSceneManagerByClass<EditorSceneManager>();
						sm->GetMouseToolController()->SelectTool(TID_GRAPH);
						GraphTool* tool = static_cast<GraphTool*> (sm->GetMouseToolController()->GetTool(TID_GRAPH));
						tool->SetParentObject(obj->GetParentSceneObject());
						tool->SetMode(GTM_ADD);
						tool->SetConnetionObject(obj);
						tool->SetNodeTemplateName(graph->GetNodeTemplate());
						tool->SetEdgeTemplateName(graph->GetEdgeTemplate());
					}
				}
			}
		}

		void OnInsertGraphNode(SceneObjectPtr obj)
		{
			if (obj)
			{
				GraphComponentPtr graph = obj->GetFirstComponentByClass<IGraphComponent>();
				if (graph)
				{
					EditorSceneManagerPtr sm = obj->GetScene()->GetFirstSceneManagerByClass<EditorSceneManager>();
					sm->GetMouseToolController()->SelectTool(TID_GRAPH);
					GraphTool* tool = static_cast<GraphTool*> (sm->GetMouseToolController()->GetTool(TID_GRAPH));
					tool->SetMode(GTM_INSERT);
					tool->SetParentObject(obj);
					tool->SetNodeTemplateName(graph->GetNodeTemplate());
					tool->SetEdgeTemplateName(graph->GetEdgeTemplate());
				}
			}
		}

	};
}