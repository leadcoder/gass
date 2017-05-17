#ifndef GASS_SAVE_SCENE_WIDGET_H
#define GASS_SAVE_SCENE_WIDGET_H
#include <QDialog>
#include "Core/Common.h"
class QComboBox;

namespace GASS
{
	class Scene;
	typedef GASS_SHARED_PTR<Scene> ScenePtr;
}

class GASSSSaveSceneWidget : public QDialog
{
	Q_OBJECT
public:
	GASSSSaveSceneWidget(GASS::ScenePtr scene);
	std::string GetSelected();
private:
	QComboBox* m_ComoBox;
};

#endif