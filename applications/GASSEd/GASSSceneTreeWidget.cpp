#include "GASSSceneTreeWidget.h"
#include "GASSEd.h"
#include "Modules/Editor/EditorApplication.h"
#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/Components/EditorComponent.h"

#include "Sim/GASS.h" 

GASSSceneTreeWidget::GASSSceneTreeWidget(GASSEd *parent): QTreeWidget(parent),
	m_GASSEd(parent),
	m_ObjectRootItem(NULL),
	m_SceneItem(NULL)
{
	setHeaderHidden(true);
	setMinimumSize(200,200);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	QObject::connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
	
	GASS::SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(GASSSceneTreeWidget::OnLoadScene,GASS::PreSceneCreateEvent,0));
	GASS::SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(GASSSceneTreeWidget::OnUnloadScene,GASS::SceneUnloadedEvent,0));
}

GASSSceneTreeWidget::~GASSSceneTreeWidget()
{

}

void GASSSceneTreeWidget::OnLoadScene(GASS::PreSceneCreateEventPtr message)
{
	m_Scene = message->GetScene();
	message->GetScene()->RegisterForMessage(REG_TMESS(GASSSceneTreeWidget::OnSceneObjectSelectionChanged,GASS::EditorSelectionChangedEvent,0));
	message->GetScene()->RegisterForMessage(REG_TMESS( GASSSceneTreeWidget::OnLoadSceneObject, GASS::PostComponentsInitializedEvent, 0));
	message->GetScene()->RegisterForMessage(REG_TMESS( GASSSceneTreeWidget::OnUnloadSceneObject,GASS::SceneObjectRemovedEvent,0));
	message->GetScene()->RegisterForMessage(REG_TMESS( GASSSceneTreeWidget::OnParentChanged,GASS::SceneObjectChangedParentEvent,0));

	m_SceneItem = new  QTreeWidgetItem();
	m_SceneItem->setText(0,"Scene");

	addTopLevelItem(m_SceneItem);
	
	m_ObjectRootItem = new  QTreeWidgetItem();
	m_ObjectRootItem->setText(0,"SceneObjects");

	m_ItemMap[message->GetScene()->GetRootSceneObject().get()] = m_ObjectRootItem;
	m_ObjectMap[m_ObjectRootItem]=message->GetScene()->GetRootSceneObject();
	m_SceneItem->addChild(m_ObjectRootItem);
}

void GASSSceneTreeWidget::OnUnloadScene(GASS::SceneUnloadedEventPtr message)
{
	clear();
	m_ItemMap.clear();
	m_ObjectMap.clear();
}

void GASSSceneTreeWidget::OnParentChanged(GASS::SceneObjectChangedParentEventPtr message)
{
	GASS::SceneObjectPtr obj = message->GetSceneObject();
	//reflect scene tree
	QTreeWidgetItem *item = GetTreeItem(obj);
	if(item)
	{
		//remove 
		m_ItemMap.erase(m_ItemMap.find(message->GetSceneObject().get()));
		m_ObjectMap.erase(m_ObjectMap.find(item));
		if(item->parent())
			item->parent()->removeChild(item);

		//readd
		QTreeWidgetItem *parent_item = GetTreeItem(obj->GetParentSceneObject());
		if(parent_item)
		{
			QTreeWidgetItem *item= new  QTreeWidgetItem();
			QString name = obj->GetName().c_str();
			item->setText(0,name);
			m_ItemMap[obj.get()] = item;
			m_ObjectMap[item]=obj;
			parent_item->addChild(item);
		}
	}
}

void GASSSceneTreeWidget::OnLoadSceneObject(GASS::PostComponentsInitializedEventPtr message)
{
	GASS::SceneObjectPtr obj = message->GetSceneObject();
	GASS::SceneObjectPtr parent = GASS_DYNAMIC_PTR_CAST<GASS::SceneObject>(obj->GetParent());
	if(parent)
	{
		GASS::EditorComponentPtr editor_comp = obj->GetFirstComponentByClass<GASS::EditorComponent>();
		if(editor_comp)
		{
			if(!editor_comp->GetShowInTree()) 
				return;
		}

		QTreeWidgetItem *parent_item = GetTreeItem(parent);
		if(parent_item)
		{
			QTreeWidgetItem *item= new  QTreeWidgetItem();
			QString name = obj->GetName().c_str();
			item->setText(0,name);
			//QVariant data(obj);
			//item->setData ( 0, 0, data);
			m_ItemMap[obj.get()] = item;
			m_ObjectMap[item]=obj;
			parent_item->addChild(item);
		}
	}
}

void GASSSceneTreeWidget::OnUnloadSceneObject(GASS::SceneObjectRemovedEventPtr message)
{
	QTreeWidgetItem *item = GetTreeItem(message->GetSceneObject());
	if(item)
	{
		m_ItemMap.erase(m_ItemMap.find(message->GetSceneObject().get()));
		m_ObjectMap.erase(m_ObjectMap.find(item));
		if(item->parent())
			item->parent()->removeChild(item);
	}
}

QTreeWidgetItem *GASSSceneTreeWidget::GetTreeItem(GASS::SceneObjectPtr obj) const
{
	std::map<GASS::SceneObject*,QTreeWidgetItem*>::const_iterator iter =  m_ItemMap.find(obj.get());
	if(iter != m_ItemMap.end())
	{
		return iter->second;
	}
	return NULL;
}


GASS::SceneObjectPtr GASSSceneTreeWidget::GetSceneObject(QTreeWidgetItem*  item) const
{
	std::map<QTreeWidgetItem*,GASS::SceneObjectWeakPtr>::const_iterator iter =  m_ObjectMap.find(item);
	if(iter != m_ObjectMap.end())
	{
		return iter->second.lock();
	}
	return GASS::SceneObjectPtr();
}


void GASSSceneTreeWidget::CheckSceneSelection(QTreeWidgetItem *item)
{
	{
		GASS::SceneObjectPtr obj = GetSceneObject(item);
		if (obj)
		{
			if (m_GASSEd->GetScene()->GetFirstSceneManagerByClass<GASS::EditorSceneManager>()->IsSelected(obj))
				item->setSelected(true);
			else
				item->setSelected(false);
		}
	}

	for (int i = 0; i < item->childCount(); ++i)
		CheckSceneSelection(item->child(i));
}

static bool internal_select = false;
void GASSSceneTreeWidget::OnSceneObjectSelectionChanged(GASS::EditorSelectionChangedEventPtr message)
{
	if (message->GetSenderID() != GASS_PTR_TO_INT(this))
	{
		internal_select = true;
		CheckSceneSelection(invisibleRootItem());
		internal_select = false;

		/*for (size_t i = 0; i < message->m_Selection.size(); i++)
		{
			GASS::SceneObjectPtr selected = message->m_Selection[i].lock();
			if (selected)
			{
				QTreeWidgetItem *item = GetTreeItem(selected);
				internal_select = true;
				item->setSelected(true);
				//setCurrentItem(item);
				internal_select = false;
			}
		}*/
	}
}

void GASSSceneTreeWidget::CheckTreeSelection(QTreeWidgetItem *item)
{
	if (item == m_SceneItem)
	{
		//send scene selection message
		GASS::SceneSelectionChangedEventPtr message(new GASS::SceneSelectionChangedEvent(GASS::ScenePtr(m_Scene)));
		GASS::ScenePtr(m_Scene)->PostMessage(message);
		//m_GASSEd->GetScene()->GetFirstSceneManagerByClass<GASS::EditorSceneManager>()->SelectSceneObject(GASS::SceneObjectPtr());
	}
	else
	{
		GASS::SceneObjectPtr obj = GetSceneObject(item);
		if (obj)
		{
			if(item->isSelected())
				m_GASSEd->GetScene()->GetFirstSceneManagerByClass<GASS::EditorSceneManager>()->SelectSceneObject(obj);
			else
				m_GASSEd->GetScene()->GetFirstSceneManagerByClass<GASS::EditorSceneManager>()->UnselectSceneObject(obj);
		}
	}

	for (int i = 0; i < item->childCount(); ++i)
		CheckTreeSelection(item->child(i));
}

void GASSSceneTreeWidget::selectionChanged()
{
	if (!internal_select)
		CheckTreeSelection(invisibleRootItem());
	/*QList<QTreeWidgetItem*> items = selectedItems();
	QList<QTreeWidgetItem*> items = getItems();
	if(!internal_select && items.size() > 0)
	{
		for (int i = 0; i < items.size(); i++)
		{
			if (items[i] == m_SceneItem)
			{
				//send scene selection message
				GASS::SceneSelectionChangedEventPtr message(new GASS::SceneSelectionChangedEvent(GASS::ScenePtr(m_Scene)));
				GASS::ScenePtr(m_Scene)->PostMessage(message);
				//m_GASSEd->GetScene()->GetFirstSceneManagerByClass<GASS::EditorSceneManager>()->SelectSceneObject(GASS::SceneObjectPtr());
			}
			else
			{
				GASS::SceneObjectPtr obj = GetSceneObject(items[i]);
				if (obj)
				{
					m_GASSEd->GetScene()->GetFirstSceneManagerByClass<GASS::EditorSceneManager>()->SelectSceneObject(obj);
				}
			}
		}
	}*/
}

void GASSSceneTreeWidget::showContextMenu(const QPoint& pos) 
{
    // for most widgets
    QPoint globalPos = mapToGlobal(pos);
    // for QAbstractScrollArea and derived classes you would use:
    // QPoint globalPos = myWidget->viewport()->mapToGlobal(pos); 
	
	GASS::SceneObjectPtr obj = m_GASSEd->GetScene()->GetFirstSceneManagerByClass<GASS::EditorSceneManager>()->GetFirstSelectedObject();

	QList<QTreeWidgetItem*> items = selectedItems();
	if(!internal_select && items.size() > 0)
	{
		if(items[0] == m_SceneItem)
		{

		}
		else if(items[0] == m_ObjectRootItem)
		{
			
		}
		else
			m_GASSEd->ShowObjectContextMenu(obj,globalPos);
	}
}


