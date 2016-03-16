 #include <QtGui>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include "Sim/GASS.h"
 #include "GASSSceneSelectionWidget.h"

 GASSSceneSelectionWidget::GASSSceneSelectionWidget()
 {
 	 m_ComoBox = new QComboBox();
	 std::vector<std::string> scene_paths = GASS::SimEngine::Get().GetSavedScenes();
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
	 return m_ComoBox->currentText().toStdString();
 }


