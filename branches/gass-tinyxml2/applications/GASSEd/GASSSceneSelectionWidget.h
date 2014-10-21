#ifndef GASS_SCENE_SELECTION_WIDGET_H
#define GASS_SCENE_SELECTION_WIDGET_H
#include <QDialog>

class QComboBox;

class GASSSceneSelectionWidget : public QDialog
{
	Q_OBJECT
public:
	GASSSceneSelectionWidget();
	std::string GetSelected();
private:
	QComboBox* m_ComoBox;
};

#endif