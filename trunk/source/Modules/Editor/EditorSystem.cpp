/****************************************************************************
*                                                                           *
* HiFiEngine                                                                *
* Copyright (C)2003 - 2005 Johan Hedstrom                                   *
* Email: hifiengine@gmail.com                                               *
* Web page: http://n00b.dyndns.org/HiFiEngine                               *
*                                                                           *
* HiFiEngine is only used with knowledge from the author. This software     *
* is not allowed to redistribute without permission from the author.        *
* For further license information, please turn to the product home page or  *
* contact author. Abuse against the HiFiEngine license is prohibited by law.*
*                                                                           *
*****************************************************************************/ 


#include <stdio.h>
#include "EditorSystem.h"
#include "EditorMessages.h"
#include "CEGUISceneTree.h"
#include "CEGUIToolsDialog.h"
#include "CEGUITemplatesDialog.h"
#include "CEGUIPropertiesDialog.h"

#include "FileDialog/CEGUIHashMapTemplates.h"
#include "FileDialog/CEGUICommonFileDialog.h"

#include "ToolSystem/MouseToolController.h"
#include "ScriptingModules/LuaScriptModule/CEGUILua.h"

#include "Core/System/SystemFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Sim/Scenario/Scene/SceneManagerFactory.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/SimEngine.h"
#include "Core/ComponentSystem/BaseComponentContainerTemplateManager.h"
#include "Core/ComponentSystem/IComponentContainerTemplate.h"
#include "Sim/Scheduling/IRuntimeController.h"
#include "EditorManager.h"
#include "ToolSystem/MoveTool.h"
#include "ToolSystem/SelectTool.h"
#include "ToolSystem/PaintTool.h"
#include "ToolSystem/VerticalMoveTool.h"
#include "ToolSystem/RotateTool.h"
#include "ToolSystem/CreateTool.h"
#include "ToolSystem/MeasurementTool.h"

namespace GASS
{
	EditorSystem::EditorSystem()
	{

	}

	EditorSystem::~EditorSystem()
	{

	}

	void EditorSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("EditorSystem",new GASS::Creator<EditorSystem, ISystem>);
	}

	void EditorSystem::OnCreate()
	{
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(EditorSystem::OnInit,InitMessage,1));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(EditorSystem::OnLoadGUIScript,GUIScriptMessage,3));

		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(EditorSystem::OnLoadScene,ScenarioSceneAboutToLoadNotifyMessage,0));
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(EditorSystem::OnUnloadScene,ScenarioSceneUnloadNotifyMessage,0));
	}

	void EditorSystem::OnInit(InitMessagePtr message)
	{
		//Create editor manager
		EditorManager* em = new EditorManager();
		em->Init();
		GASS::SimEngine::GetPtr()->GetRuntimeController()->Register(this);


		InputSystemPtr input_system = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<IInputSystem>();
		input_system->AddMouseListener(this);
		input_system->AddKeyListener(this);

		MessagePtr focus_msg(new WindowFocusChangedMessage("RenderWindow",true,0));
		EditorManager::GetPtr()->GetMessageManager()->SendImmediate(focus_msg);


		//Create tool controller
		MouseToolController* tools = EditorManager::Get().GetMouseToolController();

		IMouseTool* tool = new MoveTool(tools);
		tools->AddTool(tool);

		/*tool = new VerticalMoveTool(tools);
		tools->AddTool(tool);*/

		tool = new SelectTool(tools);
		tools->AddTool(tool);

		tool = new RotateTool(tools);
		tools->AddTool(tool);

		tool = new CreateTool(tools);
		tools->AddTool(tool);

		tool = new MeasurementTool(tools);
		tools->AddTool(tool);

		tools->SelectTool("SelectTool");

		
	}

	void EditorSystem::OnLoadGUIScript(GUIScriptMessagePtr message)
	{
		using namespace CEGUI;
		WindowManager* win_man = WindowManager::getSingletonPtr();

		//DefaultWindow* root = (DefaultWindow*)win_man->createWindow("DefaultWindow", "Root");
		Window* root = win_man->getWindow("root");
		// create a menubar.
		// this will fit in the top of the screen and have options for the demo
		//UDim bar_bottom(0,d_font->getLineSpacing(2));
		Window* bar = win_man->createWindow("OgreTray/Menubar");
		bar->setArea(UDim(0,0),UDim(0,0),UDim(1,0),UDim(0,20));
		bar->setAlwaysOnTop(true); // we want the menu on top
		root->addChildWindow(bar);
		// file menu item
		Window* file = win_man->createWindow("OgreTray/MenuItem");
		file->setText("File");
		bar->addChildWindow(file);

		// file popup
		Window* popup = win_man->createWindow("OgreTray/PopupMenu");
		file->addChildWindow(popup);

		// quit item in file menu
		Window* quit_item = win_man->createWindow("OgreTray/MenuItem");
		quit_item->setText("Quit");
		quit_item->subscribeEvent("Clicked", Event::Subscriber(&EditorSystem::HandleQuit, this));
		popup->addChildWindow(quit_item);


		Window* open_item = win_man->createWindow("OgreTray/MenuItem");
		open_item->setText("Load Scenario");
		open_item->subscribeEvent("Clicked", Event::Subscriber(&EditorSystem::HandleLoadScenario, this));
		popup->addChildWindow(open_item);

		Window* close_item = win_man->createWindow("OgreTray/MenuItem");
		close_item->setText("Close Scenario");
		close_item->subscribeEvent("Clicked", Event::Subscriber(&EditorSystem::HandleCloseScenario, this));
		popup->addChildWindow(close_item);
		close_item->setVisible(true);

		bar->setEnabled(true);
		bar->setVisible(true);

		CEGUI::CommonFileDialog::getSingleton().getWindow()->subscribeEvent (CEGUI::FrameWindow::EventHidden, CEGUI::Event::Subscriber(&EditorSystem::HandleScenarioFileDialogOutput, this));


		root->addChildWindow(CEGUI::CommonFileDialog::getSingleton().getWindow());




		/*Window* dlg = win_man->createWindow("OgreTray/FrameWindow");

		dlg->setPosition(UVector2(cegui_reldim(0.23f), cegui_reldim( 0.2f)));
		dlg->setSize(UVector2(cegui_reldim(0.5f), cegui_reldim( 0.5f)));
		dlg->setText("Templates");
		root->addChildWindow(dlg);*/



		/*Listbox* lbox = static_cast<Listbox *> (win_man->createWindow("OgreTray/Listbox"));
		dlg->addChildWindow(lbox);
		lbox->setSize(UVector2(cegui_reldim(0.9f), cegui_reldim( 0.9f)));
		std::vector<std::string> templates = GASS::SimEngine::Get().GetSimObjectManager()->GetTemplateNames();
		//std::vector<std::string>::iterator iter = templates.begin();
		for(std::size_t i = 0; i < templates.size(); i++)
		{
			ListboxTextItem* itm = new ListboxTextItem(templates[i],i);
			itm->setSelectionBrushImage("OgreTrayImages", "GenericBrush");
			lbox->addItem(itm);
		}

		Window* tree_dlg = win_man->createWindow("OgreTray/FrameWindow");
		tree_dlg->setPosition(UVector2(cegui_reldim(0.23f), cegui_reldim( 0.2f)));
		tree_dlg->setSize(UVector2(cegui_reldim(0.5f), cegui_reldim( 0.5f)));
		tree_dlg->setText("Templates");
		root->addChildWindow(tree_dlg);

		Tree* tree = static_cast<Tree *> (win_man->createWindow("OgreTray/Tree"));
		tree->setSize(UVector2(cegui_reldim(0.9f), cegui_reldim( 0.9f)));
		tree_dlg->addChildWindow(tree);
		tree->initialise();
		m_Added.clear();
		for(std::size_t i = 0; i < templates.size(); i++)
		{
			//only add add new templates at top level
			if(m_Added.find(templates[i]) == m_Added.end())
				AddChildren(templates[i], tree,NULL);
		}*/
		CEGUIToolsDialog* tools  = new CEGUIToolsDialog();
		//CEGUITemplatesDialog* temp  = new CEGUITemplatesDialog();
	}

	bool EditorSystem::HandleLoadScenario(const CEGUI::EventArgs&)
	{
		CEGUI::CommonFileDialogInput input;
        input.setHandle (1);                                        // Set the handle to determine the originator
        input.setFilter ("Scenario (*.xml)|*.xml|All files|*.*|");    // Extensions
        input.setOpenFileDialog (true);                                // Load mode
        input.setDefaultExtension ("*.xml");                        // Zip is the first one to display
        CEGUI::CommonFileDialog::getSingleton().openDialog(input);    // Open the common file dialog

		return true;
	}

	bool EditorSystem::HandleCloseScenario(const CEGUI::EventArgs&)
	{
		MessagePtr message(new ShutdownSceanrioRequestMessage());
		SimEngine::Get().GetSimSystemManager()->PostMessage(message);
		return true;
	}

	bool EditorSystem::HandleQuit(const CEGUI::EventArgs&)
	{
		return true;
	}

	bool EditorSystem::HandleScenarioFileDialogOutput(const CEGUI::EventArgs& e)
    {
	  CEGUI::CommonFileDialogOutput result = CEGUI::CommonFileDialog::getSingleton().getResult();
        switch (result.getHandle())
        {
            case 1:
                //_mlstListbox->addItem(new CEGUI::ListboxTextItem ((CEGUI::utf8*)"Load button pressed"));
            break;
 
            case 2:
                //_mlstListbox->addItem(new CEGUI::ListboxTextItem ((CEGUI::utf8*)"Save button pressed"));
            break;
        }
 
        switch (result.getAction())
        {
            case CEGUI::ACTION_OK:
                //_mlstListbox->addItem(new CEGUI::ListboxTextItem ((CEGUI::utf8*)"Returned with ACTION_OK"));
				{
					std::string file_path = std::string(result.getFullQualifiedFileName().c_str());
					file_path = file_path.substr(0,file_path.find_last_of("\\"));
				MessagePtr message(new StartSceanrioRequestMessage(file_path));
				SimEngine::Get().GetSimSystemManager()->PostMessage(message);
				}
				break;
            case CEGUI::ACTION_CANCEL:
                //_mlstListbox->addItem(new CEGUI::ListboxTextItem ((CEGUI::utf8*)"Returned with ACTION_CANCEL"));
            break;
 
            case CEGUI::ACTION_WINDOW_CLOSED:
                //_mlstListbox->addItem(new CEGUI::ListboxTextItem ((CEGUI::utf8*)"Returned with ACTION_WINDOW_CLOSED"));
            break;
        }
       
        return true;
    }


	void EditorSystem::OnLoadScene(ScenarioSceneAboutToLoadNotifyMessagePtr message)
	{
		GASS::ScenarioScenePtr scene = message->GetScenarioScene();
		CEGUISceneTree* tree = new CEGUISceneTree(scene);

		CEGUIPropertiesDialog* prop_dialog = new CEGUIPropertiesDialog();
		//Create scene tree class

	}

	void EditorSystem::OnUnloadScene(ScenarioSceneUnloadNotifyMessagePtr message)
	{

	}

	void EditorSystem::AddChildren(const std::string &name, CEGUI::Tree* tree, CEGUI::TreeItem* parent)
	{
		ComponentContainerTemplatePtr cc_temp = GASS::SimEngine::Get().GetSimObjectManager()->GetTemplate(name);
		IComponentContainerTemplate::ComponentContainerTemplateIterator children = cc_temp->GetChildren();

		m_Added[name] = 1;
		CEGUI::TreeItem *item = new CEGUI::TreeItem(name);
		item->setSelectionBrushImage("OgreTrayImages", "GenericBrush");
		if(parent)
			parent->addItem(item);
		else
			tree->addItem(item);

		while(children.hasMoreElements())
		{

			ComponentContainerTemplatePtr child = children.getNext();
			AddChildren(child->GetName(), tree,item);
		}
	}

	bool EditorSystem::MouseMoved(float x,float y, float z)
	{
		int from_id = (int)this;
		CEGUI::Point pos = CEGUI::MouseCursor::getSingleton().getDisplayIndependantPosition();
		Vec2 screen_pos(pos.d_x,pos.d_y);
		MessagePtr cursor_msg(new CursorMoved2DMessage(screen_pos,from_id));
		EditorManager::GetPtr()->GetMessageManager()->PostMessage(cursor_msg);

		return true;
	}
	bool EditorSystem::MousePressed( int id )
	{
		int from_id = (int)this;

		if(id == 0)
		{
			MessagePtr mouse_msg(new MouseButtonMessage(MouseButtonMessage::MBID_LEFT,true,from_id));
			EditorManager::GetPtr()->GetMessageManager()->PostMessage(mouse_msg);
		}
		else if (id == 1)
		{
			MessagePtr mouse_msg(new MouseButtonMessage(MouseButtonMessage::MBID_RIGHT,true,from_id));
			EditorManager::GetPtr()->GetMessageManager()->PostMessage(mouse_msg);
		}
		//CEGUI::System::getSingleton().injectMouseButtonDown(ConvertOISButtonToCegui(id));
		return true;
	}
	bool EditorSystem::MouseReleased( int id )
	{
		//CEGUI::System::getSingleton().injectMouseButtonUp(ConvertOISButtonToCegui(id));
		int from_id = (int)this;

		if(id == 0)
		{
			MessagePtr mouse_msg(new MouseButtonMessage(MouseButtonMessage::MBID_LEFT,false,from_id));
			EditorManager::GetPtr()->GetMessageManager()->PostMessage(mouse_msg);
		}
		else if (id == 1)
		{
			MessagePtr mouse_msg(new MouseButtonMessage(MouseButtonMessage::MBID_RIGHT,false,from_id));
			EditorManager::GetPtr()->GetMessageManager()->PostMessage(mouse_msg);
		}
		return true;

	}

	bool EditorSystem::KeyPressed(int key, unsigned int text)
	{


		return true;
	}

	bool EditorSystem::KeyReleased( int key, unsigned int text)
	{
		CEGUI::System::getSingleton().injectKeyUp(key);
		return true;
	}


	CEGUI::MouseButton EditorSystem::ConvertOISButtonToCegui(int buttonID)
	{
		switch (buttonID)
		{
		case 0:
			return CEGUI::LeftButton;
		case 1:
			return CEGUI::RightButton;
		case 2:
			return CEGUI::MiddleButton;
		default:
			return CEGUI::LeftButton;
		}
	}

	void EditorSystem::Update(double delta)
	{
		//std::cout << "update messages \n";
		EditorManager::Get().GetMessageManager()->Update(delta);
		EditorManager::Get().GetMouseToolController()->Update();
	}

	GASS::TaskGroup EditorSystem::GetTaskGroup() const
	{
		//we modify gui so we have to run in gfx thread
		return "MAIN_TASK_GROUP";
		//return "EDITOR_TASK_GROUP";
	}
}
