#include "sysTray.h"
#include "sysMenu.h"
sysTray::sysTray(QWidget *parent)
	: QSystemTrayIcon(parent),m_parent(parent)
{
	initSysTrayIcon();
	show();
}

sysTray::~sysTray()
{
}

void sysTray::initSysTrayIcon()
{
	setIcon(QIcon(":/Resources/MainWindow/app/logo.ico"));
	setToolTip("QQ");
	connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(onSysIconActivated(QSystemTrayIcon::ActivationReason)));
}

void sysTray::addMenu()
{
	sysMenu*item = new sysMenu(m_parent);
	item->addMenuItem("show", ":/Resources/MainWindow/screenImg/Finish.png", QStringLiteral("ÏÔÊ¾"));
	item->addMenuItem("quit", ":/Resources/MainWindow/app/page_close_btn_hover.png", QStringLiteral("ÍË³ö"));
	connect(item->getAction("show"), SIGNAL(triggered(bool)), m_parent, SLOT(onShowNormal(bool)));
	connect(item->getAction("quit"), SIGNAL(triggered(bool)), m_parent, SLOT(onShowQuit(bool)));
	item->exec(QCursor::pos());
	delete item;
	item = nullptr;
}

void sysTray::onSysIconActivated(QSystemTrayIcon::ActivationReason reason) {
	if (reason == QSystemTrayIcon::Trigger) {
		m_parent->show();
	}
	else if (reason == QSystemTrayIcon::Context) {
		addMenu();
	}
}
