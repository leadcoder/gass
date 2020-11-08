#pragma once
#include "imgui.h"
#include "imgui_stdlib.h"
//#include "windows.h"
//#include "tinyfiledialogs.h"
#include "Core/Utils/GASSColorRGB.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSceneObjectRef.h"
#include "Sim/GASSSimEngine.h"
#include "EditorSceneManager.h"
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
		}
	protected:
		bool m_ShowProperties = true;
		bool m_ShowObjectTree = true;
		bool m_ShowLog = false;
		bool m_ShowLayers = false;
		bool m_ShowNetwork = false;
		bool m_ShowSearch = true;

		SceneObjectPtr m_Selected;

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

					}

					if (ImGui::MenuItem("Open", "Ctrl+O"))
					{

						char const* filterPatterns[1] = { "*.earth" };

						//if (char const* fileToOpen = tinyfd_openFileDialog("Open File", "", 1, filterPatterns, NULL, 0))
						{
						}
					}
					if (ImGui::MenuItem("Save", "Ctrl+S"))
					{
						char const* filterPatterns[1] = { "*.earth" };
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
		}

		void dockingBegin()
		{
			// ImGui code goes here...
			//ImGui::ShowDemoWindow();
			//return;

			ImGuiWindowFlags window_dock_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->GetWorkPos());
			ImGui::SetNextWindowSize(viewport->GetWorkSize());
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
			drawSceneObject(scene->GetRootSceneObject());
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
				m_Selected = so;
			}

			bool node_deleted = false;

			if (ImGui::BeginPopupContextItem())
			{

				if (ImGui::MenuItem("Delete"))
				{
					so->GetParentSceneObject()->RemoveChildSceneObject(so);
					//m_Editor->deleteNode(node);
					node_deleted = true;
				}

				if (ImGui::MenuItem("Add Object"))
				{
					//m_Editor->createTestObjects();
				}

				if (ImGui::MenuItem("Add Feature"))
				{
					//m_Editor->addFeature();
				}
				if (!node_deleted)
				{
					/*if (auto fn = dynamic_cast<FeatureNode*>(node))
					{
						if (ImGui::MenuItem("Add Points..."))
						{
							auto addp = new AddPointHandler(fn);
							m_Editor->getViewer()->addEventHandler(addp);
						}

						if (ImGui::MenuItem("Edit"))
						{
							auto fe = new FeatureEditor(fn);
							m_Editor->getRoot()->addChild(fe);
						}
					}*/
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
			ImGuiWindowFlags window_flags = 0;
			if (!ImGui::Begin("Object tree", &m_ShowObjectTree, window_flags))
			{
				// Early out if the window is collapsed, as an optimization.
				ImGui::End();
				return;
			}

			SimEngine::SceneIterator iter = SimEngine::Get().GetScenes();
			while (iter.hasMoreElements())
			{
				ScenePtr scene = iter.getNext();
				drawScene(scene.get());
			}

			ImGui::End();
		}

		bool DragDouble(const char* label, double* v, float v_speed = 1.0f, double v_min = 0.0f, double v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0)
		{
			return ImGui::DragScalar(label, ImGuiDataType_Double, v, v_speed, &v_min, &v_max, format, flags);
		}

		bool SliderDouble(const char* label, double* v, double v_min = 0.0, double v_max = 0.0, const char* format = "%.3f", ImGuiSliderFlags flags = 0)
		{
			return ImGui::SliderScalar(label, ImGuiDataType_Double, v, &v_min, &v_max, format, flags);
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

							float col3[3] = { (float)color.r, (float)color.r, (float)color.b };
							if (ImGui::ColorEdit3(prop_name.c_str(), col3))
							{
								obj->SetPropertyValue(prop, ColorRGB(col3[0], col3[1], col3[2]));
							}
						}
						else if (*prop->GetTypeID() == typeid(ColorRGBA))
						{
							ColorRGBA color;
							obj->GetPropertyValue(prop, color);

							float col4[4] = { static_cast<float>(color.r), static_cast<float>(color.r), static_cast<float>(color.b), static_cast<float>(color.a) };
							if (ImGui::ColorEdit4(prop_name.c_str(), col4))
							{
								obj->SetPropertyValue(prop, ColorRGBA(col4[0], col4[1], col4[2], col4[3]));
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
								prop->SetValueByString(obj, prop_value);
							}
						}

					}
				}
			}
		}

		void drawProperties()
		{
			ImGuiWindowFlags window_flags = 0;
			if (!ImGui::Begin("Properties", &m_ShowProperties, window_flags))
			{
				// Early out if the window is collapsed, as an optimization.
				ImGui::End();
				return;
			}

			SimEngine::SceneIterator iter = SimEngine::Get().GetScenes();
			SceneObject* so = nullptr;
			if(iter.hasMoreElements())
			{
				ScenePtr scene = iter.getNext();
				auto esm = scene->GetFirstSceneManagerByClass<EditorSceneManager>();
				so = esm->GetFirstSelectedObject().get();
			}
			
			
			if (so)
			{
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

			ImGui::End();
		}
	};
}