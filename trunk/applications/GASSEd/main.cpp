#include "GASSEd.h"
#include <QApplication>
#include <QPainterPath>
#include <QMessageBox>
#include <QPainter>
#include <QMap>
#include <qdebug.h>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
 //   QMap<QString, QSize> customSizeHints = parseCustomSizeHints(argc, argv);
    GASSEd gass_win;

	if(argc > 1)
		gass_win.SetConfigPrefix(argv[1]);
    gass_win.resize(800, 600);
    try
	{
		gass_win.show();
		return app.exec();
	}
	catch(std::exception& e)
	{
		 QMessageBox msgBox;
		 msgBox.setText("GASSEd Exception");
		 msgBox.setInformativeText(e.what());
		 msgBox.setStandardButtons(QMessageBox::Ok);
		 //msgBox.setDefaultButton(QMessageBox::Save);
		 int ret = msgBox.exec();
	}
}
