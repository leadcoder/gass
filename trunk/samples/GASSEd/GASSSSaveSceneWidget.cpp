 #include <QtGui>
#include "Sim/GASS.h"
 #include "GASSSSaveSceneWidget.h"

 GASSSSaveSceneWidget::GASSSSaveSceneWidget()
 {
	 std::string scene_name;
	 if(GASS::SimEngine::Get().GetScene())
		 scene_name = GASS::Misc::GetFilename(GASS::SimEngine::Get().GetScene()->GetPath().GetFullPath());
	 int pre_select = -1;
 	 m_ComoBox = new QComboBox();
	 m_ComoBox->setEditable(true);
	 std::vector<std::string> scene_paths = GASS::SimEngine::Get().GetSavedScenes();
	 for(size_t i = 0; i < scene_paths.size(); i++)
	 {
		 m_ComoBox->addItem(scene_paths[i].c_str());
		 if(scene_name == scene_paths[i])
			 pre_select = i;
	 }

	 if(pre_select != -1)
		 m_ComoBox->setCurrentIndex(pre_select);
	 else
	 {
		 m_ComoBox->addItem("NewScene");
		 m_ComoBox->setCurrentIndex(m_ComoBox->count()-1); 
	 }

     QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);
     
	 connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
     connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

     QVBoxLayout *mainLayout = new QVBoxLayout;
	 mainLayout->addWidget(m_ComoBox);
	 mainLayout->addWidget(buttonBox);
     setLayout(mainLayout);
     setWindowTitle(tr("Save scene"));
 }

 std::string GASSSSaveSceneWidget::GetSelected()
 {
	 return m_ComoBox->currentText().toStdString();
 }


