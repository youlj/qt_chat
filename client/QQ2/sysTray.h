#pragma once

#include <QSystemTrayIcon>
#include <QWidget>
class sysTray : public QSystemTrayIcon
{
	Q_OBJECT

public:
	sysTray(QWidget *parent);
	~sysTray();
	void initSysTrayIcon();
	void addMenu();
public slots:
	void onSysIconActivated(QSystemTrayIcon::ActivationReason reason);
private:
	QWidget*m_parent;
};
