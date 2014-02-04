#include "GASSEd.h"
#include "GASSTools.h"

#include <QAction>
#include <QLayout>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QTextEdit>
#include <QFile>
#include <QDataStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QSignalMapper>
#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QTreeWidget>
//#include <QAccel.h>
#include <qdebug.h>

#include "Sim/GASSScene.h"
#include "GASSRenderWidget.h"
#include "GASSSceneTreeWidget.h"
#include "GASSResourceTreeWidget.h"
#include "GASSPropertyWidget.h"
#include "GASSSceneSelectionWidget.h"
#include "GASSSSaveSceneWidget.h"
#include "GASSBrushSettingsWidget.h"
#include "Modules/Editor/EditorApplication.h"
#include "Modules/Editor/EditorSceneManager.h"
#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/ToolSystem/CreateTool.h"
#include "Modules/Editor/ToolSystem/GraphTool.h"
#include "Modules/Editor/ToolSystem/MouseToolController.h"
#include "Sim/Interface/GASSIWaypointListComponent.h"
#include "Sim/Interface/GASSITemplateSourceComponent.h"
#include "Sim/Interface/GASSIGraphComponent.h"
#include "Sim/Interface/GASSIGraphNodeComponent.h"
#include "Sim/Interface/GASSIGraphEdgeComponent.h"



Q_DECLARE_METATYPE(QDockWidget::DockWidgetFeatures)

	GASSEd::GASSEd( QWidget *parent, Qt::WindowFlags flags) : QMainWindow(parent, flags), 
	m_GASSApp(NULL), 
	m_Config("GASS.xml"),
	m_FileMenu(NULL),
	m_EditMenu(NULL),
	m_CopyAct(NULL),
	m_CutAct(NULL),
	m_PasteAct(NULL),
	m_DeleteAct(NULL),
	m_AddWaypointsAct(NULL),
	m_InsertGraphNodeAct(NULL),
	m_ChangeSiteAct(NULL),
	m_ExportAct(NULL),
	m_AddTemplateMenu(NULL)
{
	setObjectName("GASSEd");
	setWindowTitle("GASSEd");
	/*center = new QTextEdit(this);
	center->setReadOnly(true);
	center->setMinimumSize(400, 205);
	setCentralWidget(center);*/

	m_GASSApp = new GASS::EditorApplication(GASS::FilePath(""));
	GASSRenderWidget* main_widget = new GASSRenderWidget();
	main_widget->m_GASSEd = this;
	setCentralWidget(main_widget);

	SetupToolBar();
	SetupMenuBar();
	statusBar()->showMessage(tr("Status Bar"));
	setDockNestingEnabled(true);

	QDockWidget* res_dock = new QDockWidget("Resources");
	res_dock->setWidget(new  GASSResourceTreeWidget());
	res_dock->setAllowedAreas(Qt::AllDockWidgetAreas);
	addDockWidget(Qt::LeftDockWidgetArea , res_dock);

	QDockWidget* tree_dock = new QDockWidget("Scene");
	tree_dock->setWidget(new  GASSSceneTreeWidget(this));
	tree_dock->setAllowedAreas(Qt::AllDockWidgetAreas);
	addDockWidget(Qt::LeftDockWidgetArea , tree_dock);

	QDockWidget *prop_dock = new QDockWidget("Properties");
	prop_dock->setWidget(new GASSPropertyWidget(this));
	prop_dock->setAllowedAreas(Qt::AllDockWidgetAreas);
	splitDockWidget(tree_dock,prop_dock,Qt::Vertical);

}

void GASSEd::Initialize(void* render_win_handle)
{
	GASS::FilePath working_folder("./");
	void* main_win_handle = winId();
	m_GASSApp->Init(working_folder, m_Config, main_win_handle, render_win_handle);
	OnNew();
}

/*void GASSEd::actionTriggered(QAction *action)
{
qDebug("action '%s' triggered", action->text().toLocal8Bit().data());
}*/

void GASSEd::SetupToolBar()
{
	StandardToolBar *tb = new StandardToolBar("Object Tools", this);
	addToolBar(tb);
	BrushSettingsWidget* bsw = new BrushSettingsWidget("Brush Settings", this);
	addToolBar(bsw);
}

void GASSEd::SetupMenuBar()
{
	m_FileMenu = menuBar()->addMenu(tr("&File"));

	QAction *action = m_FileMenu->addAction(tr("New"));
	connect(action, SIGNAL(triggered()), this, SLOT(OnNew()));

	action = m_FileMenu->addAction(tr("Save..."));
	connect(action, SIGNAL(triggered()), this, SLOT(OnSave()));

	action = m_FileMenu->addAction(tr("Load..."));
	connect(action, SIGNAL(triggered()), this, SLOT(OnOpen()));

	action = m_FileMenu->addAction(tr("Save layout..."));
	connect(action, SIGNAL(triggered()), this, SLOT(saveLayout()));

	action = m_FileMenu->addAction(tr("Load layout..."));
	connect(action, SIGNAL(triggered()), this, SLOT(loadLayout()));

	action = m_FileMenu->addAction(tr("Switch layout direction"));
	connect(action, SIGNAL(triggered()), this, SLOT(switchLayoutDirection()));

	m_FileMenu->addSeparator();

	m_FileMenu->addAction(tr("&Quit"), this, SLOT(close()));

	m_EditMenu = menuBar()->addMenu(tr("&Edit"));

	m_CopyAct = m_EditMenu->addAction(tr("&Copy"));
	m_CopyAct->setShortcut(tr("Ctrl+C"));
	m_CopyAct->setEnabled(false);
	connect(m_CopyAct, SIGNAL(triggered()), this, SLOT(OnCopy()));

	m_CutAct = m_EditMenu->addAction(tr("&Cut"));
	m_CutAct->setShortcut(tr("Ctrl+X"));
	m_CutAct->setEnabled(false);
	connect(m_CutAct, SIGNAL(triggered()), this, SLOT(OnCut()));

	m_PasteAct = m_EditMenu->addAction(tr("&Paste"));
	m_PasteAct->setShortcut(tr("Ctrl+V"));
	m_PasteAct->setEnabled(false);
	connect(m_PasteAct, SIGNAL(triggered()), this, SLOT(OnPaste()));

	m_DeleteAct = m_EditMenu->addAction(tr("&Delete"));
	m_DeleteAct->setEnabled(false);
	m_DeleteAct->setShortcut(QKeySequence::Delete);
	connect(m_DeleteAct, SIGNAL(triggered()), this, SLOT(OnDelete()));

	m_AddWaypointsAct = m_EditMenu->addAction(tr("&Add waypoints..."));
	m_AddWaypointsAct->setEnabled(false);
	m_AddWaypointsAct->setVisible(false);
	connect(m_AddWaypointsAct, SIGNAL(triggered()), this, SLOT(OnAddWaypoints()));


	

	m_AddGraphNodeAct = m_EditMenu->addAction(tr("&Add Graph Node..."));
	m_AddGraphNodeAct->setEnabled(false);
	m_AddGraphNodeAct->setVisible(false);
	connect(m_AddGraphNodeAct, SIGNAL(triggered()), this, SLOT(OnAddGraphNode()));


	m_InsertGraphNodeAct = m_EditMenu->addAction(tr("&Insert Graph Node..."));
	m_InsertGraphNodeAct->setEnabled(false);
	m_InsertGraphNodeAct->setVisible(false);
	connect(m_InsertGraphNodeAct, SIGNAL(triggered()), this, SLOT(OnInsertGraphNode()));



	m_ChangeSiteAct = m_EditMenu->addAction(tr("&Make root for new objects"));
	m_ChangeSiteAct->setEnabled(false);
	m_ChangeSiteAct->setVisible(false);
	connect(m_ChangeSiteAct, SIGNAL(triggered()), this, SLOT(OnSetAsSite()));




	m_ExportAct = m_EditMenu->addAction(tr("&Export..."));
	m_ExportAct->setEnabled(false);
	m_ExportAct->setVisible(false);
	connect(m_ExportAct, SIGNAL(triggered()), this, SLOT(OnExport()));


	m_AddTemplateMenu = m_EditMenu->addMenu(tr("&Add..."));


}

void GASSEd::saveLayout()
{
	QString fileName
		= QFileDialog::getSaveFileName(this, tr("Save layout"));
	if (fileName.isEmpty())
		return;
	QFile file(fileName);
	if (!file.open(QFile::WriteOnly)) {
		QString msg = tr("Failed to open %1\n%2")
			.arg(fileName)
			.arg(file.errorString());
		QMessageBox::warning(this, tr("Error"), msg);
		return;
	}

	QByteArray geo_data = saveGeometry();
	QByteArray layout_data = saveState();

	bool ok = file.putChar((uchar)geo_data.size());
	if (ok)
		ok = file.write(geo_data) == geo_data.size();
	if (ok)
		ok = file.write(layout_data) == layout_data.size();

	if (!ok) {
		QString msg = tr("Error writing to %1\n%2")
			.arg(fileName)
			.arg(file.errorString());
		QMessageBox::warning(this, tr("Error"), msg);
		return;
	}
}

void GASSEd::loadLayout()
{
	QString fileName
		= QFileDialog::getOpenFileName(this, tr("Load layout"));
	if (fileName.isEmpty())
		return;
	QFile file(fileName);
	if (!file.open(QFile::ReadOnly)) {
		QString msg = tr("Failed to open %1\n%2")
			.arg(fileName)
			.arg(file.errorString());
		QMessageBox::warning(this, tr("Error"), msg);
		return;
	}

	uchar geo_size;
	QByteArray geo_data;
	QByteArray layout_data;

	bool ok = file.getChar((char*)&geo_size);
	if (ok) {
		geo_data = file.read(geo_size);
		ok = geo_data.size() == geo_size;
	}
	if (ok) {
		layout_data = file.readAll();
		ok = layout_data.size() > 0;
	}

	if (ok)
		ok = restoreGeometry(geo_data);
	if (ok)
		ok = restoreState(layout_data);

	if (!ok) {
		QString msg = tr("Error reading %1")
			.arg(fileName);
		QMessageBox::warning(this, tr("Error"), msg);
		return;
	}
}

void GASSEd::OnNew()
{
	if(GetScene())
		GASS::SimEngine::Get().DestroyScene(GetScene());
	m_Scene = GASS::SimEngine::Get().CreateScene("NewScene");
	GetScene()->GetFirstSceneManagerByClass<GASS::EditorSceneManager>()->SetObjectSite(GetScene()->GetSceneryRoot());
	GetScene()->GetFirstSceneManagerByClass<GASS::EditorSceneManager>()->CreateCamera();
	GetScene()->RegisterForMessage(REG_TMESS(GASSEd::OnSceneObjectSelected,GASS::ObjectSelectionChangedEvent,0));
}

void GASSEd::OnSave()
{
	GASSSSaveSceneWidget dialog(GetScene());
	if(dialog.exec())
	{
		std::string selected_scene_name = dialog.GetSelected();
		GetScene()->Save(selected_scene_name);
	}
}

void GASSEd::OnOpen()
{
	GASSSceneSelectionWidget dialog;
	if(dialog.exec() == 0)
		return;

	std::string selected_scene = dialog.GetSelected();
	if(selected_scene != "")
	{
		if(GetScene())
			GASS::SimEngine::Get().DestroyScene(GetScene());
		m_Scene = GASS::SimEngine::Get().CreateScene("NewScene");

		GetScene()->Load(selected_scene);
		GetScene()->GetFirstSceneManagerByClass<GASS::EditorSceneManager>()->SetObjectSite(GetScene()->GetSceneryRoot());
		GetScene()->GetFirstSceneManagerByClass<GASS::EditorSceneManager>()->CreateCamera();
		GetScene()->RegisterForMessage(REG_TMESS(GASSEd::OnSceneObjectSelected,GASS::ObjectSelectionChangedEvent,0));
	}
}

void GASSEd::ShowObjectContextMenu(GASS::SceneObjectPtr obj, const QPoint& pos)
{
	m_SelectedObject = obj;
	m_EditMenu->exec(pos);
}

void GASSEd::OnCopy()
{
	m_SceneObjectCutBuffer.reset();
	m_SceneObjectCopyBuffer = m_SelectedObject;
}


void GASSEd::OnCut()
{
	m_SceneObjectCopyBuffer.reset();
	m_SceneObjectCutBuffer = m_SelectedObject;
}

void GASSEd::OnSetAsSite()
{
	GASS::SceneObjectPtr current_object = GASS::SceneObjectPtr(m_SelectedObject);
	if(current_object)
	{
		GASS::EditorSceneManagerPtr sm = current_object->GetScene()->GetFirstSceneManagerByClass<GASS::EditorSceneManager>();
		sm->SetObjectSite(current_object);
	}
}


void GASSEd::OnPaste()
{
	GASS::SceneObjectPtr copy_obj(m_SceneObjectCopyBuffer, boost::detail::sp_nothrow_tag());
	GASS::SceneObjectPtr obj(m_SelectedObject, boost::detail::sp_nothrow_tag());
	if(copy_obj && obj)
	{
		GASS::SceneObjectPtr new_obj = copy_obj->CreateCopy();
		obj->AddChildSceneObject(new_obj,true);
	}

	GASS::SceneObjectPtr cut_obj(m_SceneObjectCutBuffer, boost::detail::sp_nothrow_tag());
	if(cut_obj && obj)
	{
		cut_obj->GetParentSceneObject()->RemoveChild(cut_obj);
		obj->AddChild(cut_obj);
		int id = (int) this;
		cut_obj->PostMessage(GASS::MessagePtr(new GASS::ParentChangedMessage(id)));
		cut_obj.reset();
		m_PasteAct->setEnabled(false);
	}
}

void GASSEd::OnSceneObjectSelected(GASS::ObjectSelectionChangedEventPtr message)
{
	m_SelectedObject = message->GetSceneObject();

	m_CopyAct->setEnabled(false);
	m_PasteAct->setEnabled(false);
	m_CutAct->setEnabled(false);
	m_DeleteAct->setEnabled(false);

	m_ChangeSiteAct->setEnabled(false);
	m_ChangeSiteAct->setVisible(false);

	m_AddWaypointsAct->setEnabled(false);
	m_AddWaypointsAct->setVisible(false);

	m_AddGraphNodeAct->setEnabled(false);
	m_AddGraphNodeAct->setVisible(false);

	m_InsertGraphNodeAct->setEnabled(false);
	m_InsertGraphNodeAct->setVisible(false);

	GASS::SceneObjectPtr obj(m_SelectedObject, NO_THROW);
	if(obj)
	{
		m_DeleteAct->setEnabled(true);
		m_CopyAct->setEnabled(true);
		m_CutAct->setEnabled(true);

		if(GASS::SceneObjectPtr(m_SceneObjectCopyBuffer, NO_THROW))
		{
			m_PasteAct->setEnabled(true);
		}

		m_ChangeSiteAct->setEnabled(true);
		m_ChangeSiteAct->setVisible(true);

		if(GASS::SceneObjectPtr(m_SceneObjectCutBuffer, NO_THROW))
		{
			m_PasteAct->setEnabled(true);
		}

		GASS::WaypointListComponentPtr waypointlist = obj->GetFirstComponentByClass<GASS::IWaypointListComponent>();
		if(waypointlist)
		{
			m_AddWaypointsAct->setEnabled(true);
			m_AddWaypointsAct->setVisible(true);
		}

		GASS::GraphComponentPtr graph = obj->GetFirstComponentByClass<GASS::IGraphComponent>();
		if(graph)
		{
			m_AddGraphNodeAct->setEnabled(true);
			m_AddGraphNodeAct->setVisible(true);
			m_InsertGraphNodeAct->setEnabled(true);
			m_InsertGraphNodeAct->setVisible(true);
		}

		GASS::GraphNodeComponentPtr node = obj->GetFirstComponentByClass<GASS::IGraphNodeComponent>();
		if(node)
		{
			m_AddGraphNodeAct->setEnabled(true);
			m_AddGraphNodeAct->setVisible(true);
		}

		m_AddTemplateMenu->clear();
		m_AddTemplateMenu->setVisible(false);

		GASS::TemplateSourceComponentPtr template_source = obj->GetFirstComponentByClass<GASS::ITemplateSourceComponent>();
		if(template_source)
		{
			std::vector<std::string> templates = template_source->GetTemplates();
			if(templates.size() > 0)
			{
				m_AddTemplateMenu->setVisible(true);
				for(size_t i = 0; i < templates.size(); i++)
				{
					QAction * action = m_AddTemplateMenu->addAction(tr(templates[i].c_str()));
					action->setEnabled(true);
					connect(action, SIGNAL(triggered()), this, SLOT(OnAddTemplate()));
				}
				//connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(mySlot(QAction*)));
			}
		}

		GASS::ComponentPtr terrain_comp = obj->GetFirstComponentByClass("OgreTerrainPageComponent");
		if(terrain_comp)
		{
			m_AddWaypointsAct->setEnabled(true);
			m_AddWaypointsAct->setVisible(true);
		}
	}
}


void GASSEd::OnAddTemplate()
{
	QAction *action= (QAction *)sender();

	if(action)
	{
		QString name = action->text();
		GASS::SceneObjectPtr obj(m_SelectedObject, boost::detail::sp_nothrow_tag());
		if(obj)
		{
			GASS::SceneObjectPtr new_object = obj->GetScene()->LoadObjectFromTemplate(name.toStdString(),obj);
		}
	}
}


void GASSEd::OnDelete()
{
	GASS::SceneObjectPtr obj(m_SelectedObject, boost::detail::sp_nothrow_tag());
	if(obj)
	{
		obj->GetParentSceneObject()->RemoveChildSceneObject(obj);
	}
}


void GASSEd::OnAddWaypoints()
{
	GASS::SceneObjectPtr obj(m_SelectedObject, boost::detail::sp_nothrow_tag());
	if(obj)
	{
		GASS::WaypointListComponentPtr waypointlist = obj->GetFirstComponentByClass<GASS::IWaypointListComponent>();
		if(waypointlist)
		{
			GASS::EditorSceneManagerPtr sm = obj->GetScene()->GetFirstSceneManagerByClass<GASS::EditorSceneManager>();
			sm->GetMouseToolController()->SelectTool(TID_CREATE);
			GASS::CreateTool* tool = static_cast<GASS::CreateTool*> (sm->GetMouseToolController()->GetTool(TID_CREATE));
			tool->SetParentObject(obj);
			tool->SetTemplateName(waypointlist->GetWaypointTemplate());
		}
	}
}

void GASSEd::OnAddGraphNode()
{
	GASS::SceneObjectPtr obj(m_SelectedObject, boost::detail::sp_nothrow_tag());
	if(obj)
	{
		GASS::GraphComponentPtr graph = obj->GetFirstComponentByClass<GASS::IGraphComponent>();
		if(graph)
		{
			GASS::EditorSceneManagerPtr sm = obj->GetScene()->GetFirstSceneManagerByClass<GASS::EditorSceneManager>();
			sm->GetMouseToolController()->SelectTool(TID_GRAPH);
			GASS::GraphTool* tool = static_cast<GASS::GraphTool*> (sm->GetMouseToolController()->GetTool(TID_GRAPH));
			tool->SetMode(GASS::GTM_ADD);
			tool->SetParentObject(obj);
			tool->SetNodeTemplateName(graph->GetNodeTemplate());
			tool->SetEdgeTemplateName(graph->GetEdgeTemplate());
		}
		GASS::GraphNodeComponentPtr node = obj->GetFirstComponentByClass<GASS::IGraphNodeComponent>();
		if(node)
		{
			GASS::GraphComponentPtr graph = obj->GetParentSceneObject()->GetFirstComponentByClass<GASS::IGraphComponent>();
			if(graph)
			{
				GASS::EditorSceneManagerPtr sm = obj->GetScene()->GetFirstSceneManagerByClass<GASS::EditorSceneManager>();
				sm->GetMouseToolController()->SelectTool(TID_GRAPH);
				GASS::GraphTool* tool = static_cast<GASS::GraphTool*> (sm->GetMouseToolController()->GetTool(TID_GRAPH));
				tool->SetParentObject(obj->GetParentSceneObject());
				tool->SetMode(GASS::GTM_ADD);
				tool->SetConnetionObject(obj);
				tool->SetNodeTemplateName(graph->GetNodeTemplate());
				tool->SetEdgeTemplateName(graph->GetEdgeTemplate());
			}
		}
	}
}

void GASSEd::OnInsertGraphNode()
{
	GASS::SceneObjectPtr obj(m_SelectedObject, boost::detail::sp_nothrow_tag());
	if(obj)
	{
		GASS::GraphComponentPtr graph = obj->GetFirstComponentByClass<GASS::IGraphComponent>();
		if(graph)
		{
			GASS::EditorSceneManagerPtr sm = obj->GetScene()->GetFirstSceneManagerByClass<GASS::EditorSceneManager>();
			sm->GetMouseToolController()->SelectTool(TID_GRAPH);
			GASS::GraphTool* tool = static_cast<GASS::GraphTool*> (sm->GetMouseToolController()->GetTool(TID_GRAPH));
			tool->SetMode(GASS::GTM_INSERT);
			tool->SetParentObject(obj);
			tool->SetNodeTemplateName(graph->GetNodeTemplate());
			tool->SetEdgeTemplateName(graph->GetEdgeTemplate());
		}
	}
}

void GASSEd::OnExport()
{
	GASS::SceneObjectPtr obj(m_SelectedObject, boost::detail::sp_nothrow_tag());
	if(obj)
	{
		///obj->GetParentSceneObject()->PostMessage(Export);
	}
}
