#ifndef __GASSSceneTreeWidget_H__
#define __GASSSceneTreeWidget_H__
#pragma warning (disable : 4100)
#include "Sim/GASS.h"
#include "Modules/Editor/EditorMessages.h"
#include <QtGui>
class GASSSceneTreeWidget : public QTreeWidget, public GASS::StaticMessageListener
{
	Q_OBJECT;
public:
	GASSSceneTreeWidget( QWidget *parent=0 );
	virtual ~GASSSceneTreeWidget();
	void OnLoadScene(GASS::SceneAboutToLoadNotifyMessagePtr message);
	void OnUnloadScene(GASS::SceneUnloadNotifyMessagePtr message);
	void OnLoadSceneObject(GASS::PostComponentsInitializedEventPtr message);
	void OnUnloadSceneObject(GASS::SceneObjectRemovedEventPtr message);
	void OnSceneObjectSelected(GASS::ObjectSelectionChangedMessagePtr message);
	QTreeWidgetItem *GetTreeItem(GASS::SceneObjectPtr obj) const;
	GASS::SceneObjectPtr GetSceneObject(QTreeWidgetItem*  item) const;

protected:
	GASS::SceneWeakPtr m_Scene;
	QTreeWidgetItem *m_Root;
	std::map<GASS::SceneObject*,QTreeWidgetItem*> m_ItemMap;
	std::map<QTreeWidgetItem*,GASS::SceneObjectWeakPtr> m_ObjectMap;
 public slots:
	void selectionChanged();
	void showContextMenu(const QPoint& pos);
};

#endif