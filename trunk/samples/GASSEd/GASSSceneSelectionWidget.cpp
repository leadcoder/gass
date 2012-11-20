 #include <QtGui>
#include "Sim/GASS.h"
 #include "GASSSceneSelectionWidget.h"

 GASSSceneSelectionWidget::GASSSceneSelectionWidget()
 {
 	 m_ComoBox = new QComboBox();
	 std::vector<std::string> scene_paths = GASS::Scene::GetScenes(GASS::FilePath("%GASS_DATA_HOME%/sceneries/ogre"));
	 for(size_t i = 0; i < scene_paths.size(); i++)
	 {
		 m_ComoBox->addItem(scene_paths[i].c_str());
	 }
     QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
     
	 connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
     connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

     QVBoxLayout *mainLayout = new QVBoxLayout;
	 mainLayout->addWidget(m_ComoBox);
	 mainLayout->addWidget(buttonBox);
     setLayout(mainLayout);
     setWindowTitle(tr("Select scene"));
 }

 std::string GASSSceneSelectionWidget::GetSelected()
 {
	 GASS::FilePath path("%GASS_DATA_HOME%/sceneries/ogre/" + m_ComoBox->currentText().toStdString());
	 return path.GetFullPath();
 }


