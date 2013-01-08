#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QToolBar>
#include "Sim/GASS.h"
#include "Modules/Editor/EditorMessages.h"


QT_FORWARD_DECLARE_CLASS(QAction)
QT_FORWARD_DECLARE_CLASS(QActionGroup)
QT_FORWARD_DECLARE_CLASS(QMenu)
QT_FORWARD_DECLARE_CLASS(QSpinBox)
QT_FORWARD_DECLARE_CLASS(QLabel)

class StandardToolBar : public QToolBar, public GASS::StaticMessageListener
{
    Q_OBJECT
public:
    StandardToolBar(const QString &title, QWidget *parent);
protected:
	void OnToolChanged(GASS::ToolChangedEventPtr message);

	QAction* m_OpenAct;
	QAction* m_NewAct;
	QAction* m_SaveAct;

	QAction* m_SelectAct;
	QAction* m_MoveAct;
	QAction* m_RotateAct;

	QAction* m_TerrainDeformAct;
	QAction* m_TerrainSmoothAct;
	QAction* m_TerrainFlattenAct;
	QAction* m_TerrainPaintAct;
	QAction* m_VegetationPaintAct;
private slots:
	void OnNew();
	void OnSave();
	void OnOpen();

    void OnSelect();
	void OnRotate();
	void OnMove();
	void OnTerrainDeform();
	void OnTerrainSmooth();
	void OnTerrainFlatten();
	void OnTerrainPaint();
	void OnVegetationPaint();
	void OnPaintLayerChanged(int value);

};

#endif
