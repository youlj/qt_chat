#include "sysMenu.h"
#include "CommonUtils.h"
sysMenu::sysMenu(QWidget *parent)
	: QMenu(parent)
{
	setAttribute(Qt::WA_TranslucentBackground);
	CommonUtils::loadStyleSheet(this, "Menu");
}

sysMenu::~sysMenu()
{
}

void sysMenu::addMenuItem(const QString & action, const QString & icon, const QString & name)
{
 	QAction*act=addAction(QIcon(icon), name);
	m_menu.insert(action, act);
}

QAction * sysMenu::getAction(const QString & action)
{
	return m_menu[action];
}
