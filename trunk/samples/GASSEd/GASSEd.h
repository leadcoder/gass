#ifndef GASSED_H
#define GASSED_H

#include <QMainWindow>
#include <QTextEdit>
#include "Sim/GASS.h"
#include "Modules/Editor/EditorMessages.h"


namespace GASS
{
	class EditorApplication;
}
	

	
class ToolBar;
QT_FORWARD_DECLARE_CLASS(QMenu)
QT_FORWARD_DECLARE_CLASS(QSignalMapper)

class GASSEd : public QMainWindow, public GASS::StaticMessageListener
{
    Q_OBJECT
private:
    QTextEdit *center;
    QList<ToolBar*> toolBars;
	QMenu *m_FileMenu;
	QMenu *m_EditMenu;
    
	QAction *m_CopyAct;
	QAction *m_CutAct;
	QAction *m_PasteAct;
	QAction *m_DeleteAct;
	QAction *m_AddWaypointsAct;
    
	GASS::SceneWeakPtr m_Scene;
	GASS::SceneObjectWeakPtr m_SceneObjectCopyBuffer;
	GASS::SceneObjectWeakPtr m_SceneObjectCutBuffer;
	GASS::SceneObjectWeakPtr m_SelectedObject;
public:
    GASSEd(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	GASS::EditorApplication* m_GASSApp;
	void Initialize(void* render_win_handle);
	void ShowObjectContextMenu(GASS::SceneObjectPtr obj, const QPoint& pos);
	GASS::ScenePtr GetScene() const {return GASS::ScenePtr(m_Scene,boost::detail::sp_nothrow_tag());}
	void OnSceneObjectSelected(GASS::ObjectSelectionChangedEventPtr message);
protected:
    //void showEvent(QShowEvent *event);
public slots:
    //void actionTriggered(QAction *action);
    void saveLayout();
    void loadLayout();
    //void switchLayoutDirection();
    //void createDockWidget();
    //void destroyDockWidget(QAction *action);
	void OnNew();
	void OnSave();
	void OnOpen();
	void OnCopy();
	void OnCut();
	void OnPaste();
	void OnDelete();
	void OnAddWaypoints();
private:
    void setupToolBar();
    void setupMenuBar();
    //void setupDockWidgets(const QMap<QString, QSize> &customSizeHints);
};


#endif
