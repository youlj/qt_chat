#include "CCMainWindow.h"
#include <QProxyStyle>
#include <QPainter>
#include "SkinWindow.h"
#include <QTimer>
#include "sysTray.h"
#include "NotifyManage.h"
#include "RootContactItem.h"
#include "ContactItem.h"
#include <QMouseEvent>
#include <QApplication>
#include "WindowManager.h"
#include <QSqlQuery>

extern QString gLoginUserID;
QString gLoginHeadPath;

class CustomProxyStyle :public QProxyStyle {
	virtual void drawPrimitive(PrimitiveElement element, const QStyleOption*option, QPainter*painter, 
		const QWidget*widget=nullptr)const {
		if (element == PE_FrameFocusRect) {
			return;
		}
		else {
			QProxyStyle::drawPrimitive(element, option, painter, widget);
		}
	}
};

CCMainWindow::CCMainWindow(QString account, bool isUsernameLogin,QWidget *parent)
	: BasicWindow(parent),m_account(account),m_isUsernameLogin(isUsernameLogin)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() | Qt::Tool);
	loadStyleSheet("CCMainWindow");
	initControl();
	initTimer();
}

void CCMainWindow::initControl()
{
	m_wea = new weather;
	ui.treeWidget->setStyle(new CustomProxyStyle);
	setLevelPixmap(0);
	setHeadPixmap(getHeadImg());
	setStatusMenuIcon(":/Resources/MainWindow/StatusSucceeded.png");
	QHBoxLayout *layout = new QHBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(addApp(":/Resources/MainWindow/app/app_7.png", "app_7"));
	layout->addWidget(addApp(":/Resources/MainWindow/app/app_2.png", "app_2"));
	layout->addWidget(addApp(":/Resources/MainWindow/app/app_3.png", "app_3"));
	layout->addWidget(addApp(":/Resources/MainWindow/app/app_4.png", "app_4"));
	layout->addWidget(addApp(":/Resources/MainWindow/app/app_5.png", "app_5"));
	layout->addWidget(addApp(":/Resources/MainWindow/app/app_6.png", "app_6"));
	layout->addWidget(addApp(":/Resources/MainWindow/app/skin.png", "skin"));
	ui.appWidget->setLayout(layout);

	ui.bottomHor->addWidget(addApp(":/Resources/MainWindow/app/app_8.png", "app_8"));
	ui.bottomHor->addWidget(addApp(":/Resources/MainWindow/app/app_9.png", "app_9"));
	ui.bottomHor->addWidget(addApp(":/Resources/MainWindow/app/app_10.png", "app_10"));
	ui.bottomHor->addWidget(addApp(":/Resources/MainWindow/app/app_11.png", "app_11"));

	connect(ui.sysmin, SIGNAL(clicked(bool)), this, SLOT(onShowMin(bool)));
	connect(ui.sysclose, SIGNAL(clicked(bool)), this, SLOT(onShowClose(bool)));

	sysTray*sys = new sysTray(this);
	
	ui.searchLineEdit->installEventFilter(this);
	ui.weatherBtn->installEventFilter(this);
	connect(NotifyManage::getInstance(), &NotifyManage::signalSkinChanged, [this]() {
		updateSearchStyle();
	});
	initContactTree();
}

void CCMainWindow::setUserName(const QString & name)
{
	ui.nameLabel->adjustSize();
	QString str=ui.nameLabel->fontMetrics().elidedText(name, Qt::ElideRight, ui.nameLabel->width());//省略超出的文本,以...代替(ElideRight表示在右边加省略号...)
	ui.nameLabel->setText(str);
}

void CCMainWindow::setLevelPixmap(int level)
{
	QPixmap lv(ui.lvBtn->size());
	lv.fill(Qt::transparent);
	QPainter painter(&lv);
	painter.drawPixmap(0, 4, QPixmap(":/Resources/MainWindow/lv.png"));
	int unitNum = level % 10;//个位
	int tenNum = level / 10;//十位(最多只有两位数)
	painter.drawPixmap(10, 4, QPixmap(":/Resources/MainWindow/levelvalue.png"), tenNum * 6, 0, 6, 7);
	painter.drawPixmap(16, 4, QPixmap(":/Resources/MainWindow/levelvalue.png"), unitNum * 6, 0, 6, 7);
	ui.lvBtn->setIcon(lv);
	ui.lvBtn->setIconSize(ui.lvBtn->size());
}

void CCMainWindow::setHeadPixmap(const QString & path)
{
	QPixmap mask;
	mask.load(":/Resources/MainWindow/head_mask.png");
	ui.headLabel->setPixmap(getRoundImage(QPixmap(path), mask, ui.headLabel->size()));
}

void CCMainWindow::setStatusMenuIcon(const QString & path)
{
	QPixmap pix(ui.statusBtn->size());
	pix.fill(Qt::transparent);
	QPainter painter(&pix);
	painter.drawPixmap(4, 0, QPixmap(path));
	ui.statusBtn->setIcon(pix);
	ui.statusBtn->setIconSize(ui.statusBtn->size());
}

QWidget * CCMainWindow::addApp(const QString & path, const QString & name)
{
	QPushButton*btn = new QPushButton;
	btn->setFixedSize(20, 20);
	QPixmap pix(btn->size());
	pix.fill(Qt::transparent);
	QPixmap src(path);
	QPainter painter(&pix);
	painter.drawPixmap((btn->width() - src.width()) / 2, (btn->height() - src.height()) / 2, src);
	btn->setIcon(pix);
	btn->setIconSize(btn->size());
	btn->setObjectName(name);
	btn->setProperty("hasborder", true);
	connect(btn, SIGNAL(clicked()), this, SLOT(onAppIconClicked()));
	return btn;
}

void CCMainWindow::initTimer()
{
	QTimer*timer = new QTimer(this);
	timer->setInterval(1000*60);
	connect(timer, &QTimer::timeout, [this,timer]() {
		static int level = 0;
		level++;
		if (level == 99) {
			timer->stop();
		}
		this->setLevelPixmap(level);
	});
	timer->start();
}

void CCMainWindow::updateSearchStyle()
{
	ui.searchWidget->setStyleSheet(QString("QWidget#searchWidget{background-color:rgba(%1,%2,%3,50);\
			border-bottom:1px solid rgba(%1,%2,%3,100)}\
			QPushButton#searchBtn{border-image:url(:/Resources/MainWindow/search/search_icon.png)}\
			").arg(m_bgc.red()).arg(m_bgc.green()).arg(m_bgc.blue()));
}

void CCMainWindow::initContactTree()
{
	connect(ui.treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onItemClicked(QTreeWidgetItem*,int)));
	connect(ui.treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(onItemExpanded(QTreeWidgetItem*)));
	connect(ui.treeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(onItemCollapsed(QTreeWidgetItem*)));
	connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));
	QTreeWidgetItem*pRoot = new QTreeWidgetItem;
	pRoot->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);//不管有没有子项,都会显示在那里
	pRoot->setData(0, Qt::UserRole, 0);
	RootContactItem*item = new RootContactItem(ui.treeWidget);
	item->setText(QString::fromLocal8Bit("聊聊天"));
	ui.treeWidget->addTopLevelItem(pRoot);
	ui.treeWidget->setItemWidget(pRoot, 0, item);
	QSqlQuery queryPartyID(QString("SELECT deptID FROM dept WHERE deptName='%1'").arg(QString::fromLocal8Bit("总群")));
	queryPartyID.first();
	int partyID = queryPartyID.value(0).toInt();
	QSqlQuery queryUserDeptID(QString("SELECT deptID FROM user where ID=%1").arg(gLoginUserID));
	queryUserDeptID.first();
	int userDeptID = queryUserDeptID.value(0).toInt();
	addDepts(pRoot, partyID);
	addDepts(pRoot, userDeptID);
}

void CCMainWindow::addDepts(QTreeWidgetItem * rootItem,int deptID)
{
	QTreeWidgetItem*childItem = new QTreeWidgetItem;
	childItem->setData(0, Qt::UserRole, 1);
	childItem->setData(0, Qt::UserRole + 1, deptID);
	QPixmap pix;
	pix.load(":/Resources/MainWindow/head_mask.png");
	ContactItem*item = new ContactItem(ui.treeWidget);
	QPixmap deptImg;
	QSqlQuery queryImg(QString("SELECT img FROM dept WHERE deptID=%1").arg(deptID));
 	queryImg.first();
	deptImg.load(queryImg.value(0).toString());
	QString deptName;
	QSqlQuery queryDeptName(QString("SELECT deptName FROM dept WHERE deptID=%1").arg(deptID));
 	queryDeptName.first();
	deptName = queryDeptName.value(0).toString();
	item->setUserName(deptName);
	item->setHeadPixmap(getRoundImage(deptImg, pix, item->getHeadLabelSize()));
	rootItem->addChild(childItem);
	ui.treeWidget->setItemWidget(childItem, 0, item);
}

QString CCMainWindow::getHeadImg()
{
	QString img;
	if (m_isUsernameLogin) {
		QSqlQuery query(QString("SELECT img FROM user WHERE username='%1'").arg(m_account));
		query.first();
		img = query.value(0).toString();
	}
	else {
		QSqlQuery query(QString("SELECT img FROM user WHERE ID=%1").arg(m_account));
		query.first();
		img = query.value(0).toString();
	}
	gLoginHeadPath = img;
	return img;
}

QString CCMainWindow::getUsername()
{
	QString username;
	if (m_isUsernameLogin) {
		username = m_account;
	}
	else {
		QSqlQuery query(QString("SELECT username FROM account WHERE ID=%1").arg(m_account));
		query.first();
		username = query.value(0).toString();
	}
	return username;
}

void CCMainWindow::resizeEvent(QResizeEvent * event)
{
	setUserName(getUsername());//防止乱码
	BasicWindow::resizeEvent(event);
}

bool CCMainWindow::eventFilter(QObject * obj, QEvent * event)
{
	if (obj == ui.searchLineEdit) {
		if (event->type() == QEvent::FocusIn) {
			ui.searchWidget->setStyleSheet(QString("QWidget#searchWidget{background-color:rgb(255,255,255);\
			border-bottom:1px solid rgb(%1,%2,%3)}\
			QPushButton#searchBtn{border-image:url(:/Resources/MainWindow/search/main_search_deldown.png)}\
			QPushButton#searchBtn:hover{border-image:url(:/Resources/MainWindow/search/main_search_delhighdown.png)}\
			QPushButton#searchBtn:pressed{border-image:url(:/Resources/MainWindow/search/main_search_delhighlight.png)}\
			").arg(m_bgc.red()).arg(m_bgc.green()).arg(m_bgc.blue()));
		}
		else if (event->type() == QEvent::FocusOut) {
			updateSearchStyle();
		}
	}
 	if (obj == ui.weatherBtn) {
 		if (event->type() == QEvent::HoverEnter) {
			QPoint point = this->mapToGlobal(QPoint(0,0));
			point.setX(point.x() + 390);
			point.setY(point.y());
			m_wea->move(point);
 			m_wea->show();
 		}
 		else if (event->type() == QEvent::HoverLeave) {
 			m_wea->hide();
 		}
 	}
	return false;
}

void CCMainWindow::mousePressEvent(QMouseEvent * event)
{
	if (qApp->widgetAt(event->pos()) != ui.lineEdit&&ui.lineEdit->hasFocus()) {
		ui.lineEdit->clearFocus();
	}
	else if (qApp->widgetAt(event->pos()) != ui.searchLineEdit&&ui.searchLineEdit->hasFocus()) {
		ui.searchLineEdit->clearFocus();
	}
	BasicWindow::mousePressEvent(event);
}

void CCMainWindow::onItemClicked(QTreeWidgetItem * item, int col)
{
	bool isChild = item->data(0, Qt::UserRole).toBool();
	if (!isChild) {
		item->setExpanded(!item->isExpanded());
	}
}

void CCMainWindow::onItemExpanded(QTreeWidgetItem * item)
{
	bool isChild = item->data(0, Qt::UserRole).toBool();
	if (!isChild) {
		RootContactItem*rootItem = dynamic_cast<RootContactItem*>(ui.treeWidget->itemWidget(item, 0));
		rootItem->setExpanded(true);
	}
}

void CCMainWindow::onItemDoubleClicked(QTreeWidgetItem * item, int col)
{
	bool isChild = item->data(0, Qt::UserRole).toBool();
	if (isChild) {

		WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString());
	}
}

void CCMainWindow::onItemCollapsed(QTreeWidgetItem * item)
{
	bool isChild = item->data(0, Qt::UserRole).toBool();
	if (!isChild) {
		RootContactItem*rootItem = dynamic_cast<RootContactItem*>(ui.treeWidget->itemWidget(item, 0));
		rootItem->setExpanded(false);
	}
}

void CCMainWindow::onAppIconClicked() {
	if (sender()->objectName() == "skin") {
		SkinWindow*skin = new SkinWindow;
		skin->show();
	}
}