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
	if (rows == 0) {//����
		str = QString("SELECT img FROM user WHERE ID=%1").arg(id);
		model.setQuery(str);
	}
	QModelIndex index;
	index = model.index(0, 0);//0��0��,��img
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
	// TODO: �ڴ˴����� return ���
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

//�ı����ݰ���ʽ: Ⱥ�ı�־+����Ϣ�û�QQ��+����Ϣ�û�QQ��(��ΪȺ��,���ΪȺ��)+��������+���ݳ���+����
//�������ݰ���ʽ: Ⱥ�ı�־+����Ϣ�û�QQ��+����Ϣ�û�QQ��(��ΪȺ��,���ΪȺ��)+��������+�������+"images"+����
//msgType: 0Ϊ������Ϣ, 1Ϊ�ı���Ϣ ,2Ϊ�ļ���Ϣ
void TalkWindowShell::updateTcpSendMsg(QString & data, int & msgType, QString file)
{
	//��˭����
	TalkWindow*curTalkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	QString talkID = curTalkWindow->getTalkID();

	QString groupFlag;//Ⱥ�ı�־,Ϊ1��ΪȺ��
	if (talkID.size() == 4) {//QQ�ų���,ȡ���ھ������ݿ�
		groupFlag = "1";
	}
	else {
		groupFlag ="0";
	}
	int dataLength = data.length();
	int length =QString::number(dataLength).length();
	QString strDataLength;
	QString send;
	if (msgType == 1) {//�ı���Ϣ
		//Լ��"���ݳ���"��һ����ϢΪ5λ
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
			QMessageBox::critical(this, "error", QString::fromLocal8Bit("���͵���Ϣ����!"));
			strDataLength = QString::fromLocal8Bit("���ݳ��ȳ���5λ");
		}
		//�ı����ݰ���ʽ: Ⱥ�ı�־+����Ϣ�û�QQ��+����Ϣ�û�QQ��(��ΪȺ��,���ΪȺ��)+��������+���ݳ���+����
		send = groupFlag + gLoginUserID + talkID + "1" + strDataLength + data;
	}
	else if (msgType == 0) {//������Ϣ
		//�������ݰ���ʽ: Ⱥ�ı�־+����Ϣ�û�QQ��+����Ϣ�û�QQ��(��ΪȺ��,���ΪȺ��)+��������+�������+"images"+����
		send = groupFlag + gLoginUserID + talkID + "0" + data;
	}
	else if (msgType == 2) {//�ļ�
		//�ļ����ݰ���ʽ:Ⱥ�ı�־+����Ϣ�û�QQ��+����Ϣ�û�QQ��(��ΪȺ��,���ΪȺ��)+��������+�ļ�����+"bytes"+�ļ���+"fileBegin:"+�ļ�����
		QByteArray bt = data.toUtf8();
		QString fileLength = QString::number(bt.length());
		send = groupFlag + gLoginUserID + talkID + "2" + fileLength + "bytes" + file + "fileBegin" + data;
	}
	QByteArray sendData;
	sendData = send.toUtf8();
	m_tcpClient->write(sendData);
}

/*
	���ݰ��ĸ�ʽ�����������Լ����еģ�����Լ����

	�ı����ݰ���ʽ��Ⱥ�ı�־ + ����ϢԱ��QQ�� + ����ϢԱ��QQ�ţ�ȺQQ�ţ�
		+ ��Ϣ����(1) + ���ݳ��� + ����

	�������ݰ���ʽ��Ⱥ�ı�־ + ����ϢԱ��QQ�� + ����ϢԱ��QQ�ţ�ȺQQ�ţ�
		+ ��Ϣ����(0) + ������� + images + ��������

	�ļ����ݰ���ʽ��Ⱥ�ı�־ + ����ϢԱ��QQ�� + ����ϢԱ��QQ�ţ�ȺQQ�ţ�
		+ ��Ϣ����(2) + �ļ��ֽ��� + bytes + �ļ��� + data_begin + �ļ�����

	Ⱥ�ı�־ռ1λ��0���ģ�1Ⱥ��
	��Ϣ����ռ1λ��0������Ϣ��1�ı���Ϣ��2�ļ���Ϣ

	QQ��ռ5λ��ȺQQ��ռ4λ��
	���ݳ���ռ5λ��
	��������ռ3λ������3λ��ǰ����0��ȫ
	�����ռλ���������ݿ�ȵ���˼����������ϸ���ͣ�

	ע�⣺
	��Ⱥ�ı�־Ϊ0ʱ�������ݰ��У��Ͳ��ǡ�����ϢȺQQ�š������ǡ�����ϢԱ��QQ�š�
	��Ⱥ�ı�־Ϊ1ʱ�������ݰ��У��Ͳ��ǡ�����ϢԱ��QQ�š������ǡ�����ϢȺQQ�š�

	Ⱥ���ı���Ϣ������
	��1100012001100005Hello��
	��1 10001 2001 1 00005 Hello��
	Ⱥ���ı���Ϣ��������Ⱥ�� ������Ϣ��Ա��QQ�� ������Ϣ��ȺQQ�� �ı����� ���ݿ��Ϊ5 �������ݡ�
	��ʾ��QQ��10001 ��Ⱥ2001�����ı���Ϣ��������5����������Ϊ Hello

	����ͼƬ��Ϣ������
	��0100011000201images060��
	��0 10001 10002 0 1images 060��
	����ͼƬ��Ϣ������������ ������Ϣ��Ա��QQ�� ������Ϣ��Ա��QQ�� ͼƬ���� ������� �������ơ�
	��ʾ��Ա��QQ��10001 ��Ա��QQ��10002������Ϣ ͼƬ���� 1������ ��������60(�������Ʋ���3λ��ǰ����0��ȫ)

	Ⱥ���ļ���Ϣ������
	��1100052001210bytestest.txtdata_beginhelloworld��
	��1 10005 2001 2 10bytes test.txt data_begin helloworld��
	Ⱥ���ļ���Ϣ��������Ⱥ�� ������Ϣ��Ա��QQ�� ������Ϣ��ȺQQ�� �ļ����� �ļ����ݳ��� �ļ��� ���ַ��� �ļ����� ��
	��ʾ��QQ��10001 ��Ⱥ2001 �����ļ���Ϣ���ļ����ݳ�����10���ļ���test.txt�����ַ���data_begin���ļ�����helloworld

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
		
		//��ȡudp����
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

		//�Լ�������Ϣ��������
		if (sendUserID == gLoginUserID) {
			return;
		}
		if (bt[0] == '1') {//Ⱥ��
			windowID = strData.mid(groupFlagWidth + userWidth, groupWidth);
			QChar infoType = bt[groupFlagWidth + userWidth + groupWidth];
			if (infoType == '1') {//�ı���Ϣ
				msgType = 1;
				msgLength = strData.mid(groupFlagWidth + userWidth + groupWidth + msgTypeWidth, msgLengthWidth).toInt();
				msg= strData.mid(groupFlagWidth + userWidth + groupWidth + msgTypeWidth+msgLengthWidth, msgLength);
			}
			else if (infoType == '0') {//������Ϣ
				msgType = 0;
				int imgPos = strData.indexOf("images");
				msg = strData.right(strData.length() - imgPos - QString("images").length());
			}
			else if (infoType == '2') {//�ļ���Ϣ
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
				QString msgLabel = QString::fromLocal8Bit("�յ�����%1���͵��ļ�%2,�Ƿ����?").arg(sendUsername).arg(filename);
				recvFile->setMsg(msgLabel);
				recvFile->show();
			}
		}
		else {//����
			receiveUserID = strData.mid(groupFlagWidth + userWidth, userWidth);
			windowID = sendUserID;
			if (receiveUserID != gLoginUserID) {//���Ƿ����ҵľͲ�����
				return;
			}
			QChar infoType = bt[groupFlagWidth + userWidth + userWidth];
			if (infoType == '1') {//�ı���Ϣ
				msgType = 1;
				msgLength = strData.mid(groupFlagWidth + userWidth + userWidth + msgTypeWidth, msgLengthWidth).toInt();
				msg = strData.mid(groupFlagWidth + userWidth + userWidth + msgTypeWidth + msgLengthWidth, msgLength);

			}
			else if (infoType == '0') {//����
				msgType = 0;
				int imgPos = strData.indexOf("images");
				msg = strData.mid(imgPos + QString("images").length());
			}
			else if (infoType == '2') {//�ļ�
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
				QString msgLabel = QString::fromLocal8Bit("�յ�����%1���͵��ļ�%2,�Ƿ����?").arg(sendUsername).arg(filename);
				recvFile->setMsg(msgLabel);
				recvFile->show();
			}
		}
		
		//�����촰����Ϊ��Ĵ���
		QWidget*widget = WindowManager::getInstance()->findWindowName(windowID);
		if (widget) {//���촰�ڴ���
			this->setCurrentWidget(widget);

			//ͬ������������촰��
			QListWidgetItem*item = m_talkWindowItemMap.key(widget);
			item->setSelected(true);
		}
		else {
			return;
		}
		if (msgType != 2) {//�ļ���������
			handleReceivedMsg(sendUserID.toInt(), msgType, msg);
		}
	}    
}

void TalkWindowShell::initControl()
{
	loadStyleSheet("TalkWindow");
	setWindowTitle(QString::fromLocal8Bit("�Ƴ���"));
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
		//QString�в�֧�ֳ���˫����,��ʱ��ת���ַ�Ҳû��Ч��,������������ļ��ķ�ʽ
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
	if (msgType == 1) {//�ı���Ϣ
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
	
	//û���������������
 	html.replace("<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">",
 		"<span style=\"font-size:10pt;\">");
 	html.replace("/p>", "/span>");
	TalkWindow*talkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	talkWindow->ui.msgWidget->appendMsg(html, QString::number(sendUserID));
}

void TalkWindowShell::onEmotionBtnClicked(bool isClicked) {
	m_emotionWindow->setVisible(!m_emotionWindow->isVisible());
	QPoint emotionPoint = this->mapToGlobal(QPoint(0, 0));//����ǰ�ؼ������λ��ת��Ϊ��Ļ�ľ���λ��
	emotionPoint.setX(emotionPoint.x() + 250);
	emotionPoint.setY(emotionPoint.y() + 170);
	m_emotionWindow->move(emotionPoint);

}