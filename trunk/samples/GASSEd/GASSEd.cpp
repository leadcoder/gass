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
#include "Modules/Editor/ToolSystem/MouseToolController.h"
#include "Sim/Interface/GASSIWaypointListComponent.h"




Q_DECLARE_METATYPE(QDockWidget::DockWidgetFeatures)

GASSEd::GASSEd( QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags), m_GASSApp(NULL)
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

    setupToolBar();
    setupMenuBar();
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
	m_GASSApp->Init(working_folder, "", main_win_handle, render_win_handle);
	//GASS::FilePath gass_data_path("%GASS_DATA_HOME%");
	//GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<GASS::EditorSystem>()->GetGUISettings()->LoadAllFromPath("../configuration/schema");
	//GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<GASS::EditorSystem>()->GetGUISettings()->UpdateMetaDataForAllObjects();
	OnNew();
	//GASS::ScenePtr scene = GASS::ScenePtr(GASS::SimEngine::Get().CreateScene());
	//scene->Load("");
	//m_GASSEd->GetScene()->GetFirstSceneManagerByClass<GASS::EditorSceneManager>()->SetObjectSite(scene->GetSceneryRoot());
}

/*void GASSEd::actionTriggered(QAction *action)
{
    qDebug("action '%s' triggered", action->text().toLocal8Bit().data());
}*/

void GASSEd::setupToolBar()
{
	StandardToolBar *tb = new StandardToolBar("Object Tools", this);
	addToolBar(tb);
	BrushSettingsWidget* bsw = new BrushSettingsWidget("Brush Settings", this);
    addToolBar(bsw);
}

void GASSEd::setupMenuBar()
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
/*
QAction *addAction(QMenu *menu, const QString &text, QActionGroup *group, QSignalMapper *mapper,
                    int id)
{
    bool first = group->actions().isEmpty();
    QAction *result = menu->addAction(text);
    result->setCheckable(true);
    result->setChecked(first);
    group->addAction(result);
    QObject::connect(result, SIGNAL(triggered()), mapper, SLOT(map()));
    mapper->setMapping(result, id);
    return result;
}



void GASSEd::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
}

void GASSEd::switchLayoutDirection()
{
    if (layoutDirection() == Qt::LeftToRight)
        qApp->setLayoutDirection(Qt::RightToLeft);
    else
        qApp->setLayoutDirection(Qt::LeftToRight);
}

class CreateDockWidgetDialog : public QDialog
{
public:
    CreateDockWidgetDialog(QWidget *parent = 0);

    QString objectName() const;
    Qt::DockWidgetArea location() const;

private:
    QLineEdit *m_objectName;
    QComboBox *m_location;
};

CreateDockWidgetDialog::CreateDockWidgetDialog(QWidget *parent)
    : QDialog(parent)
{
    QGridLayout *layout = new QGridLayout(this);

    layout->addWidget(new QLabel(tr("Object name:")), 0, 0);
    m_objectName = new QLineEdit;
    layout->addWidget(m_objectName, 0, 1);

    layout->addWidget(new QLabel(tr("Location:")), 1, 0);
    m_location = new QComboBox;
    m_location->setEditable(false);
    m_location->addItem(tr("Top"));
    m_location->addItem(tr("Left"));
    m_location->addItem(tr("Right"));
    m_location->addItem(tr("Bottom"));
    m_location->addItem(tr("Restore"));
    layout->addWidget(m_location, 1, 1);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    layout->addLayout(buttonLayout, 2, 0, 1, 2);
    buttonLayout->addStretch();

    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    buttonLayout->addWidget(cancelButton);
    QPushButton *okButton = new QPushButton(tr("Ok"));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    buttonLayout->addWidget(okButton);

    okButton->setDefault(true);
}

QString CreateDockWidgetDialog::objectName() const
{
    return m_objectName->text();
}

Qt::DockWidgetArea CreateDockWidgetDialog::location() const
{
    switch (m_location->currentIndex()) {
        case 0: return Qt::TopDockWidgetArea;
        case 1: return Qt::LeftDockWidgetArea;
        case 2: return Qt::RightDockWidgetArea;
        case 3: return Qt::BottomDockWidgetArea;
        default:
            break;
    }
    return Qt::NoDockWidgetArea;
}

void GASSEd::createDockWidget()
{
    CreateDockWidgetDialog dialog(this);
    int ret = dialog.exec();
    if (ret == QDialog::Rejected)
        return;

    QDockWidget *dw = new QDockWidget;
    dw->setObjectName(dialog.objectName());
    dw->setWindowTitle(dialog.objectName());
    dw->setWidget(new QTextEdit);

    Qt::DockWidgetArea area = dialog.location();
    switch (area) {
        case Qt::LeftDockWidgetArea:
        case Qt::RightDockWidgetArea:
        case Qt::TopDockWidgetArea:
        case Qt::BottomDockWidgetArea:
            addDockWidget(area, dw);
            break;
        default:
            if (!restoreDockWidget(dw)) {
                QMessageBox::warning(this, QString(), tr("Failed to restore dock widget"));
                delete dw;
                return;
            }
            break;
    }

    extraDockWidgets.append(dw);
    destroyDockWidgetMenu->setEnabled(true);
    destroyDockWidgetMenu->addAction(new QAction(dialog.objectName(), this));
}

void GASSEd::destroyDockWidget(QAction *action)
{
    int index = destroyDockWidgetMenu->actions().indexOf(action);
    delete extraDockWidgets.takeAt(index);
    destroyDockWidgetMenu->removeAction(action);
    action->deleteLater();

    if (destroyDockWidgetMenu->isEmpty())
        destroyDockWidgetMenu->setEnabled(false);
}*/

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
	/*QMenu myMenu;
	QAction*  delete_action = myMenu.addAction("Delete");
	QAction*  copy_action = myMenu.addAction("Copy");
	QAction*  paste_action = NULL;
	GASS::SceneObjectPtr copy_obj(m_SceneObjectCopyBuffer, boost::detail::sp_nothrow_tag());
    if(copy_obj)
		paste_action = myMenu.addAction("Paste");
	QAction*  waypoint_action = NULL;
	//check if this is a waypoint list object
	SPTR<GASS::IWaypointListComponent> waypointlist = obj->GetFirstComponentByClass<GASS::IWaypointListComponent>();
	if(waypointlist)
	{
		waypoint_action = myMenu.addAction("Add waypoints...");
	}
    QAction* selectedItem = myMenu.exec(pos);
    if (selectedItem == delete_action)
    {
		obj->GetParentSceneObject()->RemoveChildSceneObject(obj);
    }
    else if(waypoint_action && selectedItem == waypoint_action)
    {
		GASS::EditorSceneManagerPtr sm = obj->GetScene()->GetFirstSceneManagerByClass<GASS::EditorSceneManager>();
		sm->GetMouseToolController()->SelectTool(TID_CREATE);
		GASS::CreateTool* tool = static_cast<GASS::CreateTool*> (sm->GetMouseToolController()->GetTool(TID_CREATE));
		tool->SetParentObject(obj);
		tool->SetTemplateName(waypointlist->GetWaypointTemplate());
    }
	else if(selectedItem == copy_action)
	{
		OnCopy();
	}
	else if(paste_action && selectedItem == paste_action)
	{
		OnPaste();
	}*/
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

	m_AddWaypointsAct->setEnabled(false);
	m_AddWaypointsAct->setVisible(false);

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