#include "GASSResourceTreeWidget.h"
#include "GASSEd.h"
#include "Modules/Editor/EditorApplication.h"
#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/Components/EditorComponent.h"

#include "Sim/GASS.h" 

GASSResourceTreeWidget::GASSResourceTreeWidget( QWidget *parent): QTreeWidget(parent)
{
	setHeaderHidden(true);
	setMinimumSize(200,200);
	
	setDragEnabled(true);
	setDragDropMode(QAbstractItemView::DragOnly);
	QObject::connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
	
	GASS::SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(GASSResourceTreeWidget::OnLoadScene,GASS::PreSceneLoadEvent,0));
	GASS::SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(GASSResourceTreeWidget::OnUnloadScene,GASS::SceneUnloadedEvent,0));
}

GASSResourceTreeWidget::~GASSResourceTreeWidget()
{

}

void GASSResourceTreeWidget::OnLoadScene(GASS::PreSceneLoadEventPtr message)
{
	GASS::ScenePtr scene = message->GetScene();
	m_Scene = scene;
	QTreeWidgetItem *root= new  QTreeWidgetItem();
	root->setText(0,"Resources");
	addTopLevelItem(root);

	//Add all loaded templates
	std::vector<std::string> temps = GASS::SimEngine::Get().GetSceneObjectTemplateManager()->GetTemplateNames();
	
	for(size_t i = 0; i < temps.size(); i++)
	{
		QTreeWidgetItem *item= new  QTreeWidgetItem();
		item->setText(0,temps[i].c_str());
		root->addChild(item);
	}
}

void GASSResourceTreeWidget::OnUnloadScene(GASS::SceneUnloadedEventPtr message)
{
	clear();
}

void GASSResourceTreeWidget::selectionChanged()
{
	QList<QTreeWidgetItem*> items = selectedItems();
	if(items.size() > 0)
	{
		//GASS::SceneObjectPtr obj = GetSceneObject(items[0]);
		//m_GASSEd->GetScene()->GetFirstSceneManagerByClass<GASS::EditorSceneManager>()->SelectSceneObject(obj);
	}
}

void GASSResourceTreeWidget::dragEnterEvent(QDragEnterEvent *event)
{
    /*if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }*/
}

void GASSResourceTreeWidget::dragMoveEvent(QDragMoveEvent *event)
{
    /*if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }*/
}

void GASSResourceTreeWidget::dropEvent(QDropEvent *event)
{

}

void GASSResourceTreeWidget::mousePressEvent(QMouseEvent *event)
{
	QTreeWidget::mousePressEvent(event);
      // if not left button - return
    if (!(event->buttons() & Qt::LeftButton)) return;
 
    // if no item selected, return (else it would crash)
    if (currentItem() == NULL) return;
 
    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;
    // mime stuff
    mimeData->setText(currentItem()->text(0)); 
    drag->setMimeData(mimeData);
 
    // start drag
    drag->start(Qt::CopyAction | Qt::MoveAction);
}


