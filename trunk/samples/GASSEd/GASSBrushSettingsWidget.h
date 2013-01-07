#ifndef BRUSH_SETTINGS_WIDGET_H
#define BRUSH_SETTINGS_WIDGET_H

#include <QToolBar>
#include "Sim/GASS.h"
#include "Modules/Editor/EditorMessages.h"


QT_FORWARD_DECLARE_CLASS(QAction)
QT_FORWARD_DECLARE_CLASS(QActionGroup)
QT_FORWARD_DECLARE_CLASS(QMenu)
QT_FORWARD_DECLARE_CLASS(QSpinBox)
QT_FORWARD_DECLARE_CLASS(QLabel)

class BrushSettingsWidget : public QToolBar, public GASS::StaticMessageListener
{
    Q_OBJECT
public:
    BrushSettingsWidget(const QString &title, QWidget *parent);
protected:
	double m_BrushSize;
	double m_BrushFade;
private slots:
    void OnBrushSizeChanged(int value);
	void OnBrushFadeChanged(int value);
	void OnBrushIntChanged(int value);
	void OnBrushNoiseChanged(int value);
};

#endif
