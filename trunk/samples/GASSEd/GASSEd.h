#ifndef GASSEd_H
#define GASSEd_H

#include <QMainWindow>
#include <QTextEdit>

namespace GASS
{
	class EditorApplication;
};

class ToolBar;
QT_FORWARD_DECLARE_CLASS(QMenu)
QT_FORWARD_DECLARE_CLASS(QSignalMapper)

class GASSEd : public QMainWindow
{
    Q_OBJECT

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
protected:
    void showEvent(QShowEvent *event);
public slots:
    void actionTriggered(QAction *action);
    void saveLayout();
    void loadLayout();
    void switchLayoutDirection();
    void createDockWidget();
    void destroyDockWidget(QAction *action);
	
private:
    void setupToolBar();
    void setupMenuBar();
    void setupDockWidgets(const QMap<QString, QSize> &customSizeHints);

	
};


#endif
