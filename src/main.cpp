#include <QApplication>
#include "windowMain.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(icon_src);

	QApplication app(argc, argv);
	new WindowMain();
	return app.exec();
}
