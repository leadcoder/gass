#ifndef __GASSSceneTreeWidget_H__
#define __GASSSceneTreeWidget_H__
#pragma warning (disable : 4100)
#include "Sim/GASS.h"
#include "Modules/Editor/EditorMessages.h"
#include "GASSEd.h"
#include <QtGui>
#include <QTreeWidget>
class GASSSceneTreeWidget : public QTreeWidget, public GASS::StaticMessageListener
{
	Q_OBJECT;
public:
	GASSSceneTreeWidget(GASSEd *parent);
	virtual ~GASSSceneTreeWidget();
	void OnLoadScene(GASS::PreSceneCreateEventPtr message);
	void OnUnloadScene(GASS::SceneUnloadedEventPtr message);
	void OnLoadSceneObject(GASS::PostComponentsInitializedEventPtr message);
	void OnUnloadSceneObject(GASS::SceneObjectRemovedEventPtr message);
	void OnSceneObjectSelectionChanged(GASS::EditorSelectionChangedEventPtr message);
	
	QTreeWidgetItem *GetTreeItem(GASS::SceneObjectPtr obj) const;
	GASS::SceneObjectPtr GetSceneObject(QTreeWidgetItem*  item) const;
	void OnParentChanged(GASS::SceneObjectChangedParentEventPtr message);
protected:
	void CheckTreeSelection(QTreeWidgetItem *item);
	void CheckSceneSelection(QTreeWidgetItem *item);

	GASS::SceneWeakPtr m_Scene;
	QTreeWidgetItem *m_ObjectRootItem;
	QTreeWidgetItem *m_SceneItem;
	GASSEd* m_GASSEd;
	std::map<GASS::SceneObject*,QTreeWidgetItem*> m_ItemMap;
	std::map<QTreeWidgetItem*,GASS::SceneObjectWeakPtr> m_ObjectMap;
 public slots:
	void selectionChanged();
	void showContextMenu(const QPoint& pos);
};

#endif