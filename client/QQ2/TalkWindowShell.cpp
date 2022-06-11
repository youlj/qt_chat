#include "TalkWindowShell.h"
#include "CommonUtils.h"
#include "TalkWindow.h"
#include "TalkWindowItem.h"
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QFile>
#include <QSqlQuery>
#include "WindowManager.h"
#include "receiveFile.h"

QString gFileName;
QString gFileData;
extern QString gLoginUserID;
int udpPort = 8888;

TalkWindowShell::TalkWindowShell(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initControl();
	initTcpSocket();
	initUdpSocket();
	QFile file("Resources/MainWindow/MsgHtml/msgtmpl.js");
	if (!file.size()) {
		QStringList userIDList;
		getUserID(userIDList);
		if (!createJSFile(userIDList)) {
			QMessageBox::critical(this, QString::fromLocal8Bit("error"), QString::fromLocal8Bit("write js file fail!"));
		}
	}
}

TalkWindowShell::~TalkWindowShell()
{
	delete m_emotionWindow;
	m_emotionWindow = nullptr;
}

void TalkWindowShell::addTalkWindow(TalkWindow * talkWindow, TalkWindowItem * talkWindowItem, const QString& id)
{
	ui.rightStackedWidget->addWidget(talkWindow);
	connect(m_emotionWindow, SIGNAL(signalEmotionWindowHide()), talkWindow, SLOT(onSetEmotionBtnStatus()));
	QListWidgetItem*item = new QListWidgetItem(ui.listWidget);
	m_talkWindowItemMap.insert(item, talkWindow);
	item->setSelected(true);
	QSqlQueryModel model;
	QString str = QString("SELECT img FROM dept WHERE deptID=%1").arg(id);
	model.setQuery(str);
	int rows = model.rowCount();
	if (rows == 0) {//单聊
		str = QString("SELECT img FROM user WHERE ID=%1").arg(id);
		model.setQuery(str);
	}
	QModelIndex index;
	index = model.index(0, 0);//0行0列,即img
	QImage img;
	img.load(model.data(index).toString());
	talkWindowItem->setHeadPixmap(QPixmap::fromImage(img));
	ui.listWidget->addItem(item);
	ui.listWidget->setItemWidget(item, talkWindowItem);
	onTalkWindowItemClicked(item);
	connect(talkWindowItem, &TalkWindowItem::signalCloseClicked, [this,item,talkWindowItem,talkWindow]() {
		m_talkWindowItemMap.remove(item);
		talkWindow->close();
		ui.listWidget->takeItem(ui.listWidget->row(item));
		delete talkWindowItem;
		ui.rightStackedWidget->removeWidget(talkWindow);
		if (ui.listWidget->count() < 1)
			close();
	});
}

void TalkWindowShell::setCurrentWidget(QWidget * widget)
{
	ui.rightStackedWidget->setCurrentWidget(widget);
}

const QMap<QListWidgetItem*, QWidget*>&TalkWindowShell::getTalkWindowItemMap() const
{
	// TODO: 在此处插入 return 语句
	return m_talkWindowItemMap;
}

void TalkWindowShell::onEmotionItemClicked(int num)
{
	TalkWindow*item = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	if (item) {
		item->addEmotionImage(num);
	}
}

void TalkWindowShell::onTalkWindowItemClicked(QListWidgetItem * item)
{
	QWidget*talkWindowWidget = m_talkWindowItemMap.value(item);
	ui.rightStackedWidget->setCurrentWidget(talkWindowWidget);
}

//文本数据包格式: 群聊标志+发信息用户QQ号+收信息用户QQ号(如为群聊,则此为群号)+数据类型+数据长度+数据
//表情数据包格式: 群聊标志+发信息用户QQ号+收信息用户QQ号(如为群聊,则此为群号)+数据类型+表情个数+"images"+数据
//msgType: 0为表情信息, 1为文本信息 ,2为文件信息
void TalkWindowShell::updateTcpSendMsg(QString & data, int & msgType, QString file)
{
	//跟谁聊天
	TalkWindow*curTalkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	QString talkID = curTalkWindow->getTalkID();

	QString groupFlag;//群聊标志,为1则为群聊
	if (talkID.size() == 4) {//QQ号长度,取决于具体数据库
		groupFlag = "1";
	}
	else {
		groupFlag ="0";
	}
	int dataLength = data.length();
	int length =QString::number(dataLength).length();
	QString strDataLength;
	QString send;
	if (msgType == 1) {//文本信息
		//约定"数据长度"这一包信息为5位
		if (length == 1) {
			strDataLength = "0000" + QString::number(dataLength);
		}
		else if (length == 2) {
			strDataLength = "000" + QString::number(dataLength);
		}
		else if (length == 3) {
			strDataLength = "00" + QString::number(dataLength);
		}
		else if (length == 4) {
			strDataLength = "0" + QString::number(dataLength);
		}
		else if (length == 5) {
			strDataLength = QString::number(dataLength);
		}
		else {
			QMessageBox::critical(this, "error", QString::fromLocal8Bit("发送的信息过长!"));
			strDataLength = QString::fromLocal8Bit("数据长度超出5位");
		}
		//文本数据包格式: 群聊标志+发信息用户QQ号+收信息用户QQ号(如为群聊,则此为群号)+数据类型+数据长度+数据
		send = groupFlag + gLoginUserID + talkID + "1" + strDataLength + data;
	}
	else if (msgType == 0) {//表情信息
		//表情数据包格式: 群聊标志+发信息用户QQ号+收信息用户QQ号(如为群聊,则此为群号)+数据类型+表情个数+"images"+数据
		send = groupFlag + gLoginUserID + talkID + "0" + data;
	}
	else if (msgType == 2) {//文件
		//文件数据包格式:群聊标志+发信息用户QQ号+收信息用户QQ号(如为群聊,则此为群号)+数据类型+文件长度+"bytes"+文件名+"fileBegin:"+文件内容
		QByteArray bt = data.toUtf8();
		QString fileLength = QString::number(bt.length());
		send = groupFlag + gLoginUserID + talkID + "2" + fileLength + "bytes" + file + "fileBegin" + data;
	}
	QByteArray sendData;
	sendData = send.toUtf8();
	m_tcpClient->write(sendData);
}

/*
	数据包的格式（都是咱们自己进行的，数据约定）

	文本数据包格式：群聊标志 + 发信息员工QQ号 + 收信息员工QQ号（群QQ号）
		+ 信息类型(1) + 数据长度 + 数据

	表情数据包格式：群聊标志 + 发信息员工QQ号 + 收信息员工QQ号（群QQ号）
		+ 信息类型(0) + 表情个数 + images + 表情名称

	文件数据包格式：群聊标志 + 发信息员工QQ号 + 收信息员工QQ号（群QQ号）
		+ 信息类型(2) + 文件字节数 + bytes + 文件名 + data_begin + 文件数据

	群聊标志占1位，0单聊，1群聊
	信息类型占1位，0表情信息，1文本信息，2文件信息

	QQ号占5位，群QQ号占4位，
	数据长度占5位，
	表情名称占3位，不足3位，前面用0补全
	（这个占位，就是数据宽度的意思，下面有仔细解释）

	注意：
	当群聊标志为0时，则数据包中，就不是【收信息群QQ号】，而是【收信息员工QQ号】
	当群聊标志为1时，则数据包中，就不是【收信息员工QQ号】，而是【收信息群QQ号】

	群聊文本信息，例：
	【1100012001100005Hello】
	【1 10001 2001 1 00005 Hello】
	群聊文本信息解析：【群聊 发送消息的员工QQ号 接收消息的群QQ号 文本类型 数据宽度为5 数据内容】
	表示：QQ号10001 向群2001发送文本信息，长度是5，数据内容为 Hello

	单聊图片信息，例：
	【0100011000201images060】
	【0 10001 10002 0 1images 060】
	单聊图片信息解析：【单聊 发送消息的员工QQ号 接受消息的员工QQ号 图片类型 表情个数 表情名称】
	表示：员工QQ号10001 向员工QQ号10002发送信息 图片类型 1个表情 表情名称60(表情名称不足3位，前面用0补全)

	群聊文件信息，例：
	【1100052001210bytestest.txtdata_beginhelloworld】
	【1 10005 2001 2 10bytes test.txt data_begin helloworld】
	群聊文件信息解析：【群聊 发送消息的员工QQ号 接收消息的群QQ号 文件类型 文件内容长度 文件名 区分符号 文件内容 】
	表示：QQ号10001 向群2001 发送文件信息，文件内容长度是10，文件名test.txt，区分符号data_begin，文件内容helloworld

*/
void TalkWindowShell::processPendingData()
{
	while (m_udpReceiver->hasPendingDatagrams()) {
		const static int groupFlagWidth = 1;
		const static int groupWidth = 4;
		const static int userWidth = 5;
		const static int msgTypeWidth = 1;
		const static int msgLengthWidth = 5;
		const static int imgWidth = 3;
		
		//读取udp数据
		QByteArray bt;
		bt.resize(m_udpReceiver->pendingDatagramSize());
		m_udpReceiver->readDatagram(bt.data(), bt.length());

		QString strData = bt.data();
		QString windowID;
		QString sendUserID, receiveUserID;
		QString msg;
		int msgLength;
		int msgType;

		sendUserID = strData.mid(groupFlagWidth, userWidth);

		//自己发的信息不做处理
		if (sendUserID == gLoginUserID) {
			return;
		}
		if (bt[0] == '1') {//群聊
			windowID = strData.mid(groupFlagWidth + userWidth, groupWidth);
			QChar infoType = bt[groupFlagWidth + userWidth + groupWidth];
			if (infoType == '1') {//文本信息
				msgType = 1;
				msgLength = strData.mid(groupFlagWidth + userWidth + groupWidth + msgTypeWidth, msgLengthWidth).toInt();
				msg= strData.mid(groupFlagWidth + userWidth + groupWidth + msgTypeWidth+msgLengthWidth, msgLength);
			}
			else if (infoType == '0') {//表情信息
				msgType = 0;
				int imgPos = strData.indexOf("images");
				msg = strData.right(strData.length() - imgPos - QString("images").length());
			}
			else if (infoType == '2') {//文件信息
				msgType = 2;
				QString filename = strData.mid(strData.indexOf("bytes") + QString("bytes").length(), 
					strData.indexOf("fileBegin")-strData.indexOf("bytes")-QString("bytes").length());
				gFileName = filename;
				msg = strData.mid(strData.indexOf("fileBegin") + QString("fileBegin").length());
				gFileData = msg;
				QString sendUsername;
				QSqlQuery query(QString("SELECT username FROM user WHERE ID=%1").arg(sendUserID.toInt()));
				if (query.first()) {
					sendUsername = query.value(0).toString();
				}
				receiveFile*recvFile = new receiveFile(this);
				connect(recvFile, &receiveFile::refuseFile, [this]() {return; });
				QString msgLabel = QString::fromLocal8Bit("收到来自%1发送的文件%2,是否接收?").arg(sendUsername).arg(filename);
				recvFile->setMsg(msgLabel);
				recvFile->show();
			}
		}
		else {//单聊
			receiveUserID = strData.mid(groupFlagWidth + userWidth, userWidth);
			windowID = sendUserID;
			if (receiveUserID != gLoginUserID) {//不是发给我的就不接收
				return;
			}
			QChar infoType = bt[groupFlagWidth + userWidth + userWidth];
			if (infoType == '1') {//文本信息
				msgType = 1;
				msgLength = strData.mid(groupFlagWidth + userWidth + userWidth + msgTypeWidth, msgLengthWidth).toInt();
				msg = strData.mid(groupFlagWidth + userWidth + userWidth + msgTypeWidth + msgLengthWidth, msgLength);

			}
			else if (infoType == '0') {//表情
				msgType = 0;
				int imgPos = strData.indexOf("images");
				msg = strData.mid(imgPos + QString("images").length());
			}
			else if (infoType == '2') {//文件
				msgType = 2;
				QString sendUsername;
				QSqlQuery query(QString("SELECT username FROM user WHERE ID=%1").arg(sendUserID.toInt()));
				if (query.first()) {
					sendUsername = query.value(0).toString();
				}
				QString filename = strData.mid(strData.indexOf("bytes") + QString("bytes").length(),
					strData.indexOf("fileBegin") - strData.indexOf("bytes") - QString("bytes").length());
				gFileName = filename;
				msg = strData.mid(strData.indexOf("fileBegin") + QString("fileBegin").length());
				gFileData = msg;
				receiveFile*recvFile = new receiveFile(this);
				connect(recvFile, &receiveFile::refuseFile, [this]() {return; });
				QString msgLabel = QString::fromLocal8Bit("收到来自%1发送的文件%2,是否接收?").arg(sendUsername).arg(filename);
				recvFile->setMsg(msgLabel);
				recvFile->show();
			}
		}
		
		//将聊天窗口设为活动的窗口
		QWidget*widget = WindowManager::getInstance()->findWindowName(windowID);
		if (widget) {//聊天窗口存在
			this->setCurrentWidget(widget);

			//同步激活左侧聊天窗口
			QListWidgetItem*item = m_talkWindowItemMap.key(widget);
			item->setSelected(true);
		}
		else {
			return;
		}
		if (msgType != 2) {//文件另做处理
			handleReceivedMsg(sendUserID.toInt(), msgType, msg);
		}
	}    
}

void TalkWindowShell::initControl()
{
	loadStyleSheet("TalkWindow");
	setWindowTitle(QString::fromLocal8Bit("云畅聊"));
	m_emotionWindow = new EmotionWindow;
	m_emotionWindow->hide();
	QList<int>sizes;
	sizes << 250 << width() - 250;
	ui.splitter->setSizes(sizes);
	ui.listWidget->setStyle(new CustomProxyStyle(this));
	connect(ui.listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onTalkWindowItemClicked(QListWidgetItem*)));
	connect(m_emotionWindow, SIGNAL(signalEmotionItemClicked(int)), this, SLOT(onEmotionItemClicked(int)));

}


void TalkWindowShell::initTcpSocket()
{
	m_tcpClient = new QTcpSocket(this);
	m_tcpClient->connectToHost("127.0.0.1", tcpPort);

}

void TalkWindowShell::initUdpSocket()
{
	m_udpReceiver = new QUdpSocket(this);
	for (quint16 i = udpPort; i < udpPort + 10; i++) {
		if (m_udpReceiver->bind(i, QUdpSocket::ShareAddress))
			break;
	}
	connect(m_udpReceiver, &QUdpSocket::readyRead, this, &TalkWindowShell::processPendingData);
}

void TalkWindowShell::getUserID(QStringList&userIDList)
{
	QSqlQueryModel model;
	model.setQuery(QString("SELECT ID FROM user WHERE status=1"));
	int userNum = model.rowCount();
	QModelIndex index;
	for (int i = 0; i < userNum; i++) {
		index = model.index(i, 0);
		userIDList << model.data(index).toString();
	}
}

bool TalkWindowShell::createJSFile(QStringList & userList)
{
	QFile file(":/Resources/MainWindow/MsgHtml/msgtmpl.txt");
	QString str;
	if (file.open(QFile::ReadOnly)) {
		str = file.readAll();
		file.close();
	}
	else {
		QMessageBox::critical(this, "error", "open msgtmpl.txt fail");
		return false;
	}
	QFile js(":/Resources/MainWindow/MsgHtml/msgtmpl.js");
	if (js.open(QFile::WriteOnly | QFile::Truncate)) {
		QString initNull = "var external = null;";
		QString initObj = "external = channel.objects.external;";
// 		QString initNew = "new QWebChannel(qt.webChannelTransport,\
// 			function(channel) {\
// 			external = channel.objects.external;\
// 		}\
// 		); ";
		QString initRecv;
		//QString中不支持出现双引号,即时用转义字符也没有效果,所以这里采用文件的方式
		QFile recvFile(":/Resources/MainWindow/MsgHtml/recvHtml.txt");
		if (recvFile.open(QIODevice::ReadOnly)) {
			initRecv = recvFile.readAll();
			recvFile.close();
		}
		else {
			QMessageBox::critical(this, "error", "open recvHtml.txt fail");
			return false;
		}

		QString replaceNull;
		QString replaceObj;
		//QString replaceNew;
		QString replaceRecv;
		for (int i = 0; i < userList.length(); i++) {
			QString strNull = initNull;
			strNull.replace("external", QString("external_%1").arg(userList[i]));
			replaceNull += strNull;
			replaceNull += "\n";

			QString strObj = initObj;
			strObj.replace("external", QString("external_%1").arg(userList[i]));
			replaceObj += strObj;
			replaceObj += "\n";

// 			QString strNew = initNew;
// 			strNew.replace("external", QString("external_%1").arg(userList[i]));
// 			replaceNew += strNew;
// 			replaceNew += "\n";

			QString strRecv = initRecv;
			strRecv.replace("external", QString("external_%1").arg(userList[i]));
			strRecv.replace("recvHtml", QString("recvHtml_%1").arg(userList[i]));
			replaceRecv += strRecv;
			replaceRecv += "\n";
	}
		str.replace(initNull, replaceNull);
		str.replace(initObj, replaceObj);
		//str.replace(initNew, replaceNew);
		str.replace(initRecv, replaceRecv);
		QTextStream stream(&js);
		stream << str;
		js.close();
		return true;
	}
	else {
		QMessageBox::critical(this, "error", "write msgtmpl.js fail");
		return false;
	}
}

void TalkWindowShell::handleReceivedMsg(int sendUserID, int msgType, QString msg)
{
	QMsgTextEdit msgText;
	if (msgType == 1) {//文本信息
		msgText.setText(msg);
		msgText.document()->toHtml();
	}
	else if (msgType == 0) {
		const int emotionWidth = 3;
		int emotionNum = msg.length() / emotionWidth;
		for (int i = 0; i < emotionNum; i++) {
			msgText.addEmotionUrl(msg.mid(i * 3, emotionWidth).toInt());
		}
	}
	QString html = msgText.document()->toHtml();
	
	//没有字体则添加字体
 	html.replace("<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">",
 		"<span style=\"font-size:10pt;\">");
 	html.replace("/p>", "/span>");
	TalkWindow*talkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	talkWindow->ui.msgWidget->appendMsg(html, QString::number(sendUserID));
}

void TalkWindowShell::onEmotionBtnClicked(bool isClicked) {
	m_emotionWindow->setVisible(!m_emotionWindow->isVisible());
	QPoint emotionPoint = this->mapToGlobal(QPoint(0, 0));//将当前控件的相对位置转换为屏幕的绝对位置
	emotionPoint.setX(emotionPoint.x() + 250);
	emotionPoint.setY(emotionPoint.y() + 170);
	m_emotionWindow->move(emotionPoint);

}