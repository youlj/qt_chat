#include "CCMainWindow.h"
#include "UserLogin.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
// 	CCMainWindow w;
// 	w.show();
	a.setQuitOnLastWindowClosed(false);
	UserLogin*login = new UserLogin;
	login->show();
	return a.exec();
}