#include "GASSEd.h"
#include <QApplication>
#include <QPainterPath>
#include <QPainter>
#include <QMap>
#include <qdebug.h>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
 //   QMap<QString, QSize> customSizeHints = parseCustomSizeHints(argc, argv);
    GASSEd gass_win;
    gass_win.resize(800, 600);
    gass_win.show();
	return app.exec();
}
