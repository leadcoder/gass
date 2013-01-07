#ifndef GASS_RESOURCE_TREE_WIDGET_H
#define GASS_RESOURCE_TREE_WIDGET_H
#pragma warning (disable : 4100)
#include "Sim/GASS.h"
#include "Modules/Editor/EditorMessages.h"
#include <QtGui>

class GASSResourceTreeWidget : public QTreeWidget, public GASS::StaticMessageListener
{
	Q_OBJECT;
public:
	GASSResourceTreeWidget( QWidget *parent=0 );
	virtual ~GASSResourceTreeWidget();
	void OnLoadScene(GASS::SceneAboutToLoadNotifyMessagePtr message);
	void OnUnloadScene(GASS::SceneUnloadNotifyMessagePtr message);

	void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
	void mousePressEvent(QMouseEvent *event);
protected:
	GASS::SceneWeakPtr m_Scene;
 public slots:
	void selectionChanged();
	
};

#endif