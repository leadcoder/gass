#ifndef GASS_SAVE_SCENE_WIDGET_H
#define GASS_SAVE_SCENE_WIDGET_H
#include <QDialog>

class QComboBox;

class GASSSSaveSceneWidget : public QDialog
{
	Q_OBJECT
public:
	GASSSSaveSceneWidget();
	std::string GetSelected();
private:
	QComboBox* m_ComoBox;
};

#endif