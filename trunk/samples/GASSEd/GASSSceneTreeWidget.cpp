#include "GASSSceneTreeWidget.h"
#include "GASSEd.h"
#include "Modules/Editor/EditorApplication.h"
#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/Components/EditorComponent.h"

#include "Sim/GASS.h" 

GASSSceneTreeWidget::GASSSceneTreeWidget( QWidget *parent): QTreeWidget(parent)
{
	setHeaderHidden(true);
	setMinimumSize(200,200);
	QObject::connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
	
	GASS::SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(GASSSceneTreeWidget::OnLoadScene,GASS::SceneAboutToLoadNotifyMessage,0));
	GASS::SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(GASSSceneTreeWidget::OnUnloadScene,GASS::SceneUnloadNotifyMessage,0));
	GASS::SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(GASSSceneTreeWidget::OnSceneObjectSelected,GASS::ObjectSelectionChangedMessage,0));
}

GASSSceneTreeWidget::~GASSSceneTreeWidget()
{

}

void GASSSceneTreeWidget::OnLoadScene(GASS::SceneAboutToLoadNotifyMessagePtr message)
{
	GASS::ScenePtr scene = message->GetScene();
	m_Scene = scene;
	scene->RegisterForMessage(REG_TMESS( GASSSceneTreeWidget::OnLoadSceneObject, GASS::PostComponentsInitializedMessage, 0));
	scene->RegisterForMessage(REG_TMESS( GASSSceneTreeWidget::OnUnloadSceneObject,GASS::SceneObjectRemovedNotifyMessage,0));

	QTreeWidgetItem *root= new  QTreeWidgetItem();
	root->setText(0,"Root");
	m_ItemMap[scene->GetRootSceneObject().get()] = root;
	m_ObjectMap[root]=scene->GetRootSceneObject();
	addTopLevelItem(root);
}

void GASSSceneTreeWidget::OnUnloadScene(GASS::SceneUnloadNotifyMessagePtr message)
{

}

void GASSSceneTreeWidget::OnLoadSceneObject(GASS::PostComponentsInitializedMessagePtr message)
{
	GASS::SceneObjectPtr obj = message->GetSceneObject();
	GASS::SceneObjectPtr parent = boost::shared_dynamic_cast<GASS::SceneObject>(obj->GetParent());
	if(parent)
	{
		GASS::EditorComponentPtr editor_comp = obj->GetFirstComponentByClass<GASS::EditorComponent>();
		if(editor_comp)
		{
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
}


void GASSSceneTreeWidget::OnUnloadSceneObject(GASS::SceneObjectRemovedNotifyMessagePtr message)
{

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
		return GASS::SceneObjectPtr(iter->second,boost::detail::sp_nothrow_tag());
	}
	return GASS::SceneObjectPtr();
}

void GASSSceneTreeWidget::OnSceneObjectSelected(GASS::ObjectSelectionChangedMessagePtr message)
{
	if(message->GetSenderID() != (int) this)
	{
		GASS::SceneObjectPtr selected = message->GetSceneObject();
		if(selected)
		{
			QTreeWidgetItem *item = GetTreeItem(selected);
		}
	}
}

void GASSSceneTreeWidget::selectionChanged()
{
	QList<QTreeWidgetItem*> items = selectedItems();
	if(items.size() > 0)
	{
		GASS::SceneObjectPtr obj = GetSceneObject(items[0]);
		GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystem<GASS::EditorSystem>()->SelectSceneObject(obj);
	}
}


