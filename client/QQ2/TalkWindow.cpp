#include "TalkWindow.h"
#include "RootContactItem.h"
#include "ContactItem.h"
#include "CommonUtils.h"
#include "WindowManager.h"
#include <QToolTip>
#include <QSqlQueryModel>
#include <QSqlQuery>

TalkWindow::TalkWindow(QWidget *parent, const QString&id)
	: QWidget(parent),m_talkId(id),m_file(nullptr)
{
	ui.setupUi(this);
	WindowManager::getInstance()->addWindowName(m_talkId,this);
	setAttribute(Qt::WA_DeleteOnClose);
	initGroupTalkStatus();
	initControl();
}

TalkWindow::~TalkWindow()
{
	WindowManager::getInstance()->deleteWindowName(m_talkId);
}

void TalkWindow::addEmotionImage(int num)
{
	ui.textEdit->setFocus();
	ui.textEdit->addEmotionUrl(num);
}

void TalkWindow::setWindowName(const QString & name)
{
	ui.nameLabel->setText(name);
}

QString TalkWindow::getTalkID()
{
	return m_talkId;
}

void TalkWindow::initControl()
{
	QList<int>bodySplitterSize;
	bodySplitterSize << 600 << 133;
	ui.bodySplitter->setSizes(bodySplitterSize);
	ui.textEdit->setFocus();
	connect(ui.sysmin, SIGNAL(clicked(bool)), parent(), SLOT(onShowMin(bool)));
	connect(ui.sysclose, SIGNAL(clicked(bool)), parent(), SLOT(onShowClose(bool)));
	connect(ui.closeBtn, SIGNAL(clicked(bool)), parent(), SLOT(onShowClose(bool)));
	connect(ui.faceBtn, SIGNAL(clicked(bool)), parent(), SLOT(onEmotionBtnClicked(bool)));
	connect(ui.sendBtn, SIGNAL(clicked(bool)), this, SLOT(onSendBtnClicked(bool)));
	connect(ui.fileopenBtn, SIGNAL(clicked(bool)), this, SLOT(onFileOpenBtnClicked(bool)));
	connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));
	if (m_isGroupTalk) {
		initTalkWindow();
	}
	else {
		initPtoPTalk();
	}
}

void TalkWindow::initTalkWindow()//群聊
{
	QTreeWidgetItem*rootItem = new QTreeWidgetItem();
	rootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
	rootItem->setData(0, Qt::UserRole, 0);
	RootContactItem*item = new RootContactItem(false, ui.treeWidget);
	ui.treeWidget->setFixedHeight(647);
	QString deptName;
	QString name = QString("SELECT deptName FROM dept WHERE deptID=%1").arg(m_talkId);
	QSqlQuery queryDeptName(name);
	if (queryDeptName.first()) {
		deptName = queryDeptName.value(0).toString();
	}
	QSqlQueryModel model;
	QSqlQueryModel model1;
	if (getPartyID() == m_talkId.toInt()) {//总群
		model.setQuery(QString("SELECT ID FROM user WHERE status=1"));
		model1.setQuery(QString("SELECT ID FROM user WHERE status=1 AND online=1"));
	}
	else {
		model.setQuery(QString("SELECT ID FROM user WHERE status=1 AND deptID=%1").arg(m_talkId));
		model1.setQuery(QString("SELECT ID FROM user WHERE status=1 AND online=1 AND deptID=%1").arg(m_talkId));
	}
	int userNum = model.rowCount();//用户人数
	int onlineNum = model1.rowCount();//在线人数
	QString dept = QString::fromLocal8Bit("%1 %2/%3").arg(deptName).arg(onlineNum).arg(userNum);
	item->setText(dept);
	ui.treeWidget->addTopLevelItem(rootItem);
	ui.treeWidget->setItemWidget(rootItem, 0, item);
	rootItem->setExpanded(true);
	for (int i = 0; i < userNum; i++) {
		QModelIndex index = model.index(i, 0);
		int userID = model.data(index).toInt();
		addPeopleInfo(rootItem,userID);
	}
}

void TalkWindow::initPtoPTalk()//单聊
{
	QPixmap skin;
	skin.load(":/Resources/MainWindow/skin.png");
	ui.widget->setFixedSize(skin.size());
	QLabel*label = new QLabel(ui.widget);
	label->setPixmap(skin);
	label->setFixedSize(ui.widget->size());
}

void TalkWindow::addPeopleInfo(QTreeWidgetItem * rootItem,int userID)
{
	QTreeWidgetItem*childItem = new QTreeWidgetItem();
	childItem->setData(0, Qt::UserRole, 1);
	childItem->setData(0, Qt::UserRole + 1, userID);
	QPixmap mask;
	mask.load(":/Resources/MainWindow/head_mask.png");
	ContactItem*item = new ContactItem(ui.treeWidget);
	QString name, sign, img;
	QSqlQueryModel model;
	model.setQuery(QString("SELECT username,sign,img FROM user WHERE ID=%1").arg(userID));
	QModelIndex nameIndex = model.index(0, 0);
	QModelIndex signIndex = model.index(0, 1);
	QModelIndex imgIndex = model.index(0, 2);
	name = model.data(nameIndex).toString();
	sign = model.data(signIndex).toString();
	img = model.data(imgIndex).toString();
	QImage head;
	head.load(img);
	item->setHeadPixmap(CommonUtils::getRoundImage(QPixmap::fromImage(head),mask,item->getHeadLabelSize()));
	item->setUserName(name);
	item->setSignName(sign);
	rootItem->addChild(childItem);
	ui.treeWidget->setItemWidget(childItem, 0, item);
	QString username = item->getUserName();
	m_groupPeopleMap.insert(childItem, username);
}

void TalkWindow::initGroupTalkStatus()
{
	QSqlQueryModel model;
	QString str = QString("SELECT * FROM dept WHERE deptID=%1 ").arg(m_talkId);
	model.setQuery(str);
	int rows = model.rowCount();
	if (rows == 0) {//单聊
		m_isGroupTalk = false;
	}
	else {
		m_isGroupTalk = true;
	}
}

int TalkWindow::getPartyID()
{
	QSqlQuery query(QString("SELECT deptID from dept WHERE deptName='%1'").arg(QString::fromLocal8Bit("总群")));
	query.first();
	return query.value(0).toInt();
}

void TalkWindow::onItemDoubleClicked(QTreeWidgetItem * item, int col)
{
	bool isChild = item->data(0, Qt::UserRole).toBool();
	if (isChild) {
		QString name = m_groupPeopleMap.value(item);
		WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString());
	}
}

void TalkWindow::onFileOpenBtnClicked(bool)
{
	m_file = new sendFile(this);
	TalkWindowShell*talkWindowShell = new TalkWindowShell;
	QPoint point;
	point = talkWindowShell->mapToGlobal(QPoint(0, 0));
	point.setX(point.x());
	point.setY(point.y() +366);
	m_file->move(point);
	m_file->show();
}

void TalkWindow::onSendBtnClicked(bool isClicked) {
	if (ui.textEdit->toPlainText().isEmpty()) {
		QToolTip::showText(this->mapToGlobal(QPoint(250, 365)), QString::fromLocal8Bit("信息不能为空!"), this,
			QRect(0, 0, 120, 88), 2000);
		return;
	}
	QString&html = ui.textEdit->document()->toHtml();

	//更改字体,让其显示出来,仍有瑕疵
 	html.replace("<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">",
 		"<span style=\"font-size:10pt;\">");
 	html.replace("/p>", "/span>");

	ui.textEdit->clear();
	ui.textEdit->deleteAllEmotionImage();
	ui.msgWidget->appendMsg(html);//在聊天窗口追加信息
}
