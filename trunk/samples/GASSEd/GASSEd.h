#ifndef GASSED_H
#define GASSED_H

#include <QMainWindow>
#include <QTextEdit>
#include "Sim/GASSSceneObject.h"


namespace GASS
{
	class EditorApplication;
}
	

	
class ToolBar;
QT_FORWARD_DECLARE_CLASS(QMenu)
QT_FORWARD_DECLARE_CLASS(QSignalMapper)

class GASSEd : public QMainWindow
{
    Q_OBJECT
private:
    QTextEdit *center;
    QList<ToolBar*> toolBars;
    QMenu *dockWidgetMenu;
    QMenu *GASSEdMenu;
    QSignalMapper *mapper;
    QList<QDockWidget*> extraDockWidgets;
    QAction *createDockWidgetAction;
    QMenu *destroyDockWidgetMenu;

public:
    GASSEd(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	GASS::EditorApplication* m_GASSApp;
	void Initialize(void* render_win_handle);
	void ShowObjectContextMenu(GASS::SceneObjectPtr obj, const QPoint& pos);
protected:
    void showEvent(QShowEvent *event);
public slots:
    void actionTriggered(QAction *action);
    void saveLayout();
    void loadLayout();
    void switchLayoutDirection();
    void createDockWidget();
    void destroyDockWidget(QAction *action);
	void OnNew();
	void OnSave();
	void OnOpen();
private:
    void setupToolBar();
    void setupMenuBar();
    void setupDockWidgets(const QMap<QString, QSize> &customSizeHints);
};


#endif
