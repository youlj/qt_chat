#pragma once

#include <QMenu>
#include <QMap>
#include <QAction>

class sysMenu : public QMenu
{
	Q_OBJECT

public:
	sysMenu(QWidget *parent=nullptr);
	~sysMenu();
	void addMenuItem(const QString&action, const QString&icon, const QString&name);
	QAction* getAction(const QString&action);
private:
	QMap<QString, QAction*>m_menu;
};
