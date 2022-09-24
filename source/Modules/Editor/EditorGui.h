#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"

//#include "windows.h"
#include "tinyfiledialogs.h"
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
#include "Sim/Interface/GASSIWaypointListComponent.h"
#include "Sim/Interface/GASSIMeshComponent.h"
#include "Sim/Interface/GASSIImGuiComponent.h"
#include "Modules/Editor/ToolSystem/GraphTool.h"
#include "Modules/Editor/ToolSystem/CreateTool.h"
#include "IconsFontAwesome5.h"

namespace GASS
{
	class EditorGui
	{
	public:
		EditorGui()
		{
		}

		void DrawUi()
		{
			DockingBegin();
			DrawMainMenu();
			ToolbarUI();
			if (m_ShowObjectTree)
				DrawObjectTree();

			if (m_ShowProperties)
				DrawProperties();
			if (m_ShowTemplates)
				DrawTemplates();

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
		ImGuiID m_DockSpaceId = 0;


		bool ToolbarButton(std::string name, const char* font_icon, bool is_active, const char* tooltip)
		{
			const ImVec4 col_active = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
			const ImVec4 bg_color = is_active ? col_active : ImGui::GetStyle().Colors[ImGuiCol_Text];


			auto frame_padding = ImGui::GetStyle().FramePadding;
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_Text, bg_color);
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, ImGui::GetStyle().FramePadding.y));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, frame_padding);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

			bool ret = false;
			if (ImGui::Button(font_icon ? font_icon : name.c_str())) {
				ret = true;
			}
			ImGui::PopStyleColor(4);
			ImGui::PopStyleVar(3);
			if (ImGui::IsItemHovered()) {
				ImGui::BeginTooltip();
				ImGui::TextUnformatted(tooltip);
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			return ret;
		}

		void ToolbarUI()
		{

			ScenePtr scene = GetFirstScene();
			if (!scene)
				return;
			auto* esm = scene->GetFirstSceneManagerByClass<EditorSceneManager>().get();
			std::string active_tool_name;
			if (auto* active_tool = esm->GetMouseToolController()->GetActiveTool())
			{
				active_tool_name = active_tool->GetName();
			}


			//int menuBarHeight = 10;
			float toolbarSize = 40;
			ImGuiViewport* viewport = ImGui::GetMainViewport();

			auto centralNode = ImGui::DockBuilderGetCentralNode(m_DockSpaceId);
			if (!centralNode)
				return;
			ImGui::SetNextWindowPos(centralNode->Pos);
			ImGui::SetNextWindowSize(ImVec2(centralNode->Size.x, toolbarSize));
			ImGui::SetNextWindowViewport(viewport->ID);

			ImGuiWindowFlags window_flags = 0
				| ImGuiWindowFlags_NoDocking
				| ImGuiWindowFlags_NoTitleBar
				| ImGuiWindowFlags_NoResize
				| ImGuiWindowFlags_NoMove
				| ImGuiWindowFlags_NoScrollbar
				| ImGuiWindowFlags_NoSavedSettings
				;
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
			ImGui::Begin("TOOLBAR", NULL, window_flags);
			//ImGui::SetWindowFontScale(1);

			struct ToolInfo
			{
				ToolInfo(std::string name, char* icon, std::string tt = "") : Name(name), Icon(icon), ToolTip(tt)
				{
				}
				std::string Name;
				char* Icon = nullptr;
				std::string ToolTip;
			};

			std::vector<ToolInfo> tools = { {TID_SELECT,ICON_FA_MOUSE_POINTER, "Select Tool"},
				{TID_MOVE, ICON_FA_ARROWS_ALT,"Move Tool"},
				{TID_ROTATE,ICON_FA_REDO_ALT,"Rotate Tool"},
				{TID_GOTO_POS,nullptr,"Go To Position Tool"},
				{TID_BOX , nullptr, "Box tool"} };

			for (size_t i = 0; i < tools.size(); i++)
			{
				auto& ti = tools[i];
				bool active = ti.Name == active_tool_name;
				if (ToolbarButton(ti.Name, ti.Icon, active, ti.ToolTip.c_str()))
					esm->GetMouseToolController()->SelectTool(ti.Name);
			}
			ImGui::PopStyleVar();
			ImGui::End();
		}

		void DrawMainMenu()
		{
			// Menu Bar
			ImGui::BeginMainMenuBar();
			//if (open)
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("New"))
					{
						auto scene = GetFirstScene();
						if (scene)
						{
							scene->New();
							scene->GetFirstSceneManagerByClass<EditorSceneManager>()->SetObjectSite(scene->GetSceneryRoot());
							scene->GetFirstSceneManagerByClass<EditorSceneManager>()->CreateCamera();
						}
						//m_SceneSelected = nullptr;
						//auto scene = GetFirstScene();
						//if (scene)
						//	SimEngine::Get().DestroyScene(scene);
						//scene = ScenePtr(SimEngine::Get().CreateScene("NewScene"));
						//scene->GetFirstSceneManagerByClass<EditorSceneManager>()->SetObjectSite(scene->GetSceneryRoot());
						//scene->GetFirstSceneManagerByClass<EditorSceneManager>()->CreateCamera();
					}

					if (ImGui::MenuItem("Load Scene..."))
					{
						char const* filterPatterns[1] = { "*.scene" };
						if (char const* fileToSave = tinyfd_openFileDialog("Load Scene", "", 1, filterPatterns, nullptr,0))
						{
							m_SceneSelected = nullptr;
							auto scene = GetFirstScene();
							//if (scene)
							//	SimEngine::Get().DestroyScene(scene);
							//scene = ScenePtr(SimEngine::Get().CreateScene("NewScene"));
							scene->Load(FilePath(fileToSave));
							scene->GetFirstSceneManagerByClass<EditorSceneManager>()->SetObjectSite(scene->GetSceneryRoot());
							scene->GetFirstSceneManagerByClass<EditorSceneManager>()->CreateCamera();
						}
					}

					auto scene = GetFirstScene();
					if (scene && scene->GetSceneFile().Exist())
					{
						if (ImGui::MenuItem("Save", "Ctrl+S"))
						{
							scene->Save(scene->GetSceneFile());
						}
					}

					if (ImGui::MenuItem("Save As...", "Ctrl+S"))
					{
						if (scene)
						{
							char const* filterPatterns[1] = { "*.scene" };
							if (char const* fileToSave = tinyfd_saveFileDialog("Save Scene", "", 1, filterPatterns, nullptr))
							{
								scene->Save(FilePath(fileToSave));
							}
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
					ImGui::MenuItem("Object Tree", nullptr, &m_ShowObjectTree);
					ImGui::MenuItem("Properties", nullptr, &m_ShowProperties);
					ImGui::MenuItem("Log", nullptr, &m_ShowLog);
					ImGui::EndMenu();
				}
			}
			ImGui::EndMainMenuBar();

			//ImGuiViewportP* viewport = (ImGuiViewportP*)(void*)ImGui::GetMainViewport();
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
			float height = ImGui::GetFrameHeight();

			if (ImGui::BeginViewportSideBar("##StatusBar", viewport, ImGuiDir_Down, height, window_flags)) {
				if (ImGui::BeginMenuBar()) {
					auto scene = GetFirstScene();
					if (scene)
						ImGui::Text("Loaded Scene:%s", scene->GetName().c_str());
					ImGui::EndMenuBar();
				}
				ImGui::End();
			}


		}

		void DockingBegin()
		{
			// ImGui code goes here...
			//ImGui::ShowDemoWindow();
			//return;
			constexpr ImGuiDockNodeFlags dockspace_flags =
				ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_PassthruCentralNode;
			m_DockSpaceId = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockspace_flags);

#if 0
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
			DrawMainMenu();
			ImGui::End();
#endif
		}


		void DrawScene(Scene* scene)
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
				DrawSceneObject(scene->GetRootSceneObject());
				ImGui::TreePop();
			}
		}

		void DrawSceneObject(SceneObjectPtr so)
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

				if (ImGui::MenuItem("Make parent for dnd in scene"))
				{
					editor->SetObjectSite(so);
				}

				if (ImGui::MenuItem("Save..."))
				{
					char const* filter_patterns[2] = { "*.xml" };
					char const* save_file_name = tinyfd_saveFileDialog(
						"Save SceneObject",
						"SceneObject.xml",
						1,
						filter_patterns,
						nullptr);
					if (save_file_name)
					{
						so->SaveToFile(save_file_name);
					}
				}
				if (ImGui::MenuItem("Load..."))
				{
					char const* filter_patterns[2] = { "*.xml" };
					char const* load_file_name = tinyfd_openFileDialog(
						"Load SceneObject",
						"",
						1,
						filter_patterns,
						nullptr,
						0);
					if (load_file_name)
					{
						auto child = SceneObject::LoadFromXML(load_file_name);
						so->AddChildSceneObject(child, true);
					}
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
				if (so->GetFirstComponentByClass<IWaypointListComponent>().get())
				{
					if (ImGui::MenuItem("Add Waypoint..."))
					{
						OnAddWaypoint(so);
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
						DrawSceneObject(so->GetChild(i));
					}
				}
				ImGui::TreePop();
			}
		}

		void DrawObjectTree()
		{
			auto scene = GetFirstScene();
			if (!scene)
				return;
			ImGuiWindowFlags window_flags = 0;
			if (!ImGui::Begin("Object tree", &m_ShowObjectTree, window_flags))
			{
				// Early out if the window is collapsed, as an optimization.
				ImGui::End();
				return;
			}

			DrawScene(scene.get());
			ImGui::End();
		}

		template<typename TYPE>
		TypedProperty<TYPE>* AsTypedProp(IProperty* prop)
		{
			if (*prop->GetTypeID() == typeid(TYPE))
			{
				return dynamic_cast<TypedProperty<TYPE>*>(prop);
			}
			return nullptr;
		}

		ScenePtr GetFirstScene()
		{
			ScenePtr scene;
			SimEngine::SceneIterator iter = SimEngine::Get().GetScenes();
			if (iter.hasMoreElements())
			{
				scene = iter.getNext();
			}
			return scene;
		}

		template <typename FUNC>
		void DrawPropInGrid(const std::string &prop_name, FUNC func)
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			ImGui::Text(prop_name.c_str());
			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(-FLT_MIN);
			func();
			ImGui::NextColumn();
		}


		void DrawPropInGrid(BaseReflectionObject* obj, IProperty* prop)
		{
			if (prop->GetFlags() & PF_VISIBLE)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();
				const std::string prop_name = prop->GetName();
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
				ImGui::TreeNodeEx(prop_name.c_str(), flags);
				//ImGui::Text(prop_name.c_str());
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(-FLT_MIN);
				DrawProp(obj, prop);
				ImGui::NextColumn();
			}
		}

		void DrawProp(BaseReflectionObject* obj, IProperty* prop)
		{
			const std::string prop_name = "##" + prop->GetName();

			if (prop->GetFlags() & PF_VISIBLE)
			{
				if (prop->HasMetaData())
				{
					PropertyMetaDataPtr meta_data = prop->GetMetaData();
					const bool editable = (prop->GetFlags() & PF_EDITABLE);
					const std::string documentation = prop->GetDescription();
					if (GASS_DYNAMIC_PTR_CAST<FilePathPropertyMetaData>(meta_data))
					{
						//item = m_VariantManager->addProperty(QtVariantPropertyManager::enumTypeId(), prop_name.c_str());
						FilePathPropertyMetaDataPtr file_path_data = GASS_DYNAMIC_PTR_CAST<FilePathPropertyMetaData>(meta_data);
						std::vector<std::string> exts = file_path_data->GetExtensions();
						FilePathPropertyMetaData::FilePathEditType type = file_path_data->GetType();

						std::string filename = prop->GetValueAsString(obj);
						filename = StringUtils::Replace(filename, "/", "\\");

						//m_VariantManager->setAttribute(item,QLatin1String("filter"),QVariant("*.png *.jpg"));
						switch (type)
						{
						case FilePathPropertyMetaData::IMPORT_FILE:
							if (ImGui::Button(prop_name.c_str()))
							{
								std::vector<char const*> filterPatterns;
								for (size_t i = 0; i < exts.size(); i++)
								{
									filterPatterns.push_back(exts[i].c_str());
								}
								char const* const* const filterptr = filterPatterns.empty() ? nullptr : &filterPatterns[0];
								if (char const* fileToLoad = tinyfd_openFileDialog("Import File", "", filterPatterns.size(), filterptr, nullptr, 0))
								{
									prop->SetValueByString(obj, std::string(fileToLoad));
								}
							}
							break;
						case FilePathPropertyMetaData::EXPORT_FILE:
							//item = m_VariantManager->addProperty(newFileTypeId(), prop_name.c_str());
							//item->setValue(filename.c_str());
							//item->setAttribute(QLatin1String("filter"), QVariant(filter.c_str()));
							break;
						case FilePathPropertyMetaData::PATH_SELECTION:
							break;
						}
					}
				}
				else //if (!item)
				{
					const bool editable = (prop->GetFlags() & PF_EDITABLE);
					const bool multi = (prop->GetFlags() & PF_MULTI_OPTIONS);
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
							if (ImGui::DragScalarN(prop_name.c_str(), ImGuiDataType_Double, &value, 1, drag_speed))
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
							if (ImGui::DragScalarN(prop_name.c_str(), ImGuiDataType_Double, (void*)&value.x, 3, drag_speed))
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
						else if (*prop->GetTypeID() == typeid(FilePath))
						{
							//std::string filename = prop_value;
							//filename = StringUtils::Replace(filename, "/", "\\");
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
							if (ImGui::IsItemDeactivated())
								prop->SetValueByString(obj, prop_value);

						}

					}
				}
			}
		}

		void DrawProperties()
		{
			ScenePtr scene = GetFirstScene();
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
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
			if (ImGui::BeginTable("split", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable))
			{

				if (so)
				{
					m_SceneSelected = nullptr;
					auto props = so->GetProperties();

					for (auto prop : props)
					{
						DrawPropInGrid(so, prop);
					}

					auto comp_iter = so->GetComponents();
					while (comp_iter.hasMoreElements())
					{
						ComponentPtr comp = GASS_STATIC_PTR_CAST<Component>(comp_iter.getNext());
						std::string class_name = comp->GetRTTI()->GetClassName();
						if (true)//comp->HasMetaData() && comp->GetMetaData()->GetFlags() & OF_VISIBLE) //we have settings!
						{
							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);
							ImGui::AlignTextToFramePadding();

							ImGui::SetNextItemOpen(true, ImGuiCond_Once);
							const bool open = ImGui::TreeNode(class_name.c_str());
							if (open)
							{
								auto imgui_comp = GASS_DYNAMIC_PTR_CAST<IImGuiComponent>(comp);
								if (imgui_comp)
								{
									imgui_comp->DrawGui();
								}
								auto comp_props = comp->GetProperties();

								for (auto comp_prop : comp_props)
								{
									DrawPropInGrid(comp.get(), comp_prop);
								}
								ImGui::TreePop();
							}
						}
					}
				}
				else if (m_SceneSelected)
				{
					std::string scene_name = m_SceneSelected->GetName();
					DrawPropInGrid("SceneName", 
						[&]() { if (ImGui::InputText("Name", &scene_name))
								{
									m_SceneSelected->SetName(scene_name);
								}
							}
					);
					auto props = m_SceneSelected->GetProperties();
					for (auto prop : props)
					{
						DrawPropInGrid(m_SceneSelected, prop);
					}
					auto sms = m_SceneSelected->GetSceneManagers();
					for (auto sm : sms)
					{
						BaseReflectionObjectPtr obj = GASS_DYNAMIC_PTR_CAST<BaseReflectionObject>(sm);
						auto sm_props = obj->GetProperties();
						for (auto prop : sm_props)
						{
							DrawPropInGrid(obj.get(), prop);
						}
					}
				}
				ImGui::EndTable();
			}
			ImGui::PopStyleVar();


			ImGui::End();
		}

#if 0
		void DrawTools()
		{
			ScenePtr scene = GetFirstScene();
			if (!scene)
				return;
			ImGuiWindowFlags window_flags = 0;
			if (!ImGui::Begin("Tools", &m_ShowTools, window_flags))
			{
				// Early out if the window is collapsed, as an optimization.
				ImGui::End();
				return;
			}

			auto* esm = scene->GetFirstSceneManagerByClass<EditorSceneManager>().get();
			std::string tool_name;
			if (auto* active_tool = esm->GetMouseToolController()->GetActiveTool())
			{
				tool_name = active_tool->GetName();
			}
			std::vector<std::string> tools = { TID_SELECT ,TID_MOVE,TID_ROTATE,TID_GOTO_POS,TID_BOX };

			int e = -1;
			for (size_t i = 0; i < tools.size(); i++)
			{
				if (tools[i] == tool_name)
					e = int(i);
				if (ImGui::RadioButton(tools[i].c_str(), &e, int(i)))
					esm->GetMouseToolController()->SelectTool(tools[i]);
			}

			ImGui::End();
				}
#endif
		void DrawTemplates()
		{
			ScenePtr scene = GetFirstScene();
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
				SceneObjectTemplatePtr sot = GASS_DYNAMIC_PTR_CAST<SceneObjectTemplate>(SimEngine::Get().GetSceneObjectTemplateManager()->GetTemplate(temp));
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
					esm->GetMouseToolController()->CreateSceneObject(m_DropTemplate, Vec2(x, y));
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


		void OnAddWaypoint(SceneObjectPtr obj)
		{
			if (obj)
			{
				WaypointListComponentPtr wpl = obj->GetFirstComponentByClass<IWaypointListComponent>();
				if (wpl)
				{
					EditorSceneManagerPtr sm = obj->GetScene()->GetFirstSceneManagerByClass<EditorSceneManager>();
					sm->GetMouseToolController()->SelectTool(TID_CREATE);
					auto tool = static_cast<CreateTool*> (sm->GetMouseToolController()->GetTool(TID_CREATE));
					tool->SetParentObject(obj);
					tool->SetTemplateName(wpl->GetWaypointTemplate());
				}
			}
		}

		void OnAddGraphNode(SceneObjectPtr obj)
		{
			if (obj)
			{
				{
					GraphComponentPtr graph = obj->GetFirstComponentByClass<IGraphComponent>();
					if (graph)
					{
						EditorSceneManagerPtr sm = obj->GetScene()->GetFirstSceneManagerByClass<EditorSceneManager>();
						sm->GetMouseToolController()->SelectTool(TID_GRAPH);
						GraphTool* tool = static_cast<GraphTool*> (sm->GetMouseToolController()->GetTool(TID_GRAPH));
						tool->SetMode(GTM_ADD);
						tool->SetParentObject(obj);
						tool->SetConnetionObject(SceneObjectPtr());
						tool->SetNodeTemplateName(graph->GetNodeTemplate());
						tool->SetEdgeTemplateName(graph->GetEdgeTemplate());
					}
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