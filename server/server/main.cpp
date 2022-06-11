#include "server.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	server w;
	w.show();
	return a.exec();
}
