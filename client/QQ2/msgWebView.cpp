#include "msgWebView.h"
#include <QFile>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QWebChannel>
#include "TalkWindowShell.h"
#include "WindowManager.h"
#include <QSqlQueryModel>

extern QString gLoginHeadPath;

msgHtmlObj::msgHtmlObj(QObject*parent, QString imgPath):QObject(parent) {
	m_imgPath = imgPath;
	initHtml();
}

msgWebView::msgWebView(QWidget *parent)
	: QWebEngineView(parent),m_channel(new QWebChannel(this))
{
	msgWebPage*page = new msgWebPage(this);
	setPage(page);
	m_msgHtmlObj = new msgHtmlObj(this);
	m_channel->registerObject("external0", m_msgHtmlObj);
	QString talkID = WindowManager::getInstance()->getCreateTalkID();
	QSqlQueryModel query;
	QString userID, imgPath, external;
	bool isGroupTalk = false;
	query.setQuery(QString("SELECT deptID FROM dept WHERE deptName='%1'").arg(QStringLiteral("总群")));
	QModelIndex index = query.index(0, 0);
	QString partyID = query.data(index).toString();
	if (talkID == partyID) {//总群
		isGroupTalk = true;
		query.setQuery("SELECT ID,img FROM user WHERE status=1");
	}
	else {
		if (talkID.length() == 4) {//其他群聊
			isGroupTalk = true;
			query.setQuery(QString("SELECT ID,img FROM user WHERE status=1 AND deptID=%1").arg(talkID));
		}
		else {//单聊
			query.setQuery(QString("SELECT img FROM user WHERE status=1 AND ID=%1").arg(talkID));
			QModelIndex imgIndex = query.index(0, 0);
			imgPath = query.data(imgIndex).toString();
			external = "external_" + talkID;
			msgHtmlObj*obj = new msgHtmlObj(this, imgPath);
			m_channel->registerObject(external,obj);
		}
	}
	if (isGroupTalk) {
		QModelIndex userIdIndex, imgIndex;
		int rows = query.rowCount();
		for (int i = 0; i < rows; i++) {
			userIdIndex = query.index(i, 0);
			imgIndex = query.index(i, 1);
			userID = query.data(userIdIndex).toString();
			imgPath = query.data(imgIndex).toString();
			external = "external_" + userID;
			msgHtmlObj*obj = new msgHtmlObj(this, imgPath);
			m_channel->registerObject(external, obj);
		}
	}
	this->page()->setWebChannel(m_channel);
	this->load(QUrl("qrc:/Resources/MainWindow/MsgHtml/msgTmpl.html"));//初始化收信息页面
	TalkWindowShell*talkWindowShell = WindowManager::getInstance()->getTalkWindowShell();
	connect(this, &msgWebView::signalSendMsg, talkWindowShell, &TalkWindowShell::updateTcpSendMsg);
}
           
msgWebView::~msgWebView()
{
}

void msgWebView::appendMsg(const QString & html,QString obj)
{
	QJsonObject msgObj;
	QString msg="";
	QList<QStringList>msgList;
	msgList = parseHtml(html);
	int msgType = 1;//信息类型,0表情 1文本 2文件
	QString strData="";//发送的数据
	int imgNum = 0;//表情数量
	bool isImg = false;
	for (int i = 0; i < msgList.size(); i++) {
		if (msgList[i].at(0) == "img") {
			isImg = true;
			QString imgPath = msgList[i].at(1);
			QPixmap pix;
			QString emotionPath = "qrc:/Resources/MainWindow/emotion/";
			int pos = emotionPath.size();
			QString emotionName = imgPath.mid(pos);
			emotionName.replace(".png", "");

			//表情长度为1到3位(取决于表情资源),统一补全到3位
			int emotionNameLength = emotionName.length();
			if (emotionNameLength == 1) {
				strData += "00" + emotionName;
			}
			else if (emotionNameLength == 2) {
				strData += "0" + emotionName;
			}
			else {
				strData += emotionName;
			}
			
			msgType = 0;
			imgNum++;
			if (imgPath.left(3) == "qrc") {//格式为qrc:/...,需要把qrc去除
				pix.load(imgPath.mid(3));//从第三个位置往后全部截取,去除了qrc
			}
			else {
				pix.load(imgPath);
			}
			QString img = QString("<img src=\"%1\" width=\"%2\" height=\"%3\" />")
				.arg(imgPath).arg(pix.width()).arg(pix.height());
			msg += img;
		}
		else if (msgList[i].at(0) == "text") {
			msg += msgList[i].at(1);
			strData += msg;
			msgType = 1;
		}
	}
	msgObj.insert("MSG", msg);
	const QString &Msg=QJsonDocument(msgObj).toJson(QJsonDocument::Compact);//转换成utf-8格式,紧凑模式,现在就是一个字符串了
	if (obj == "0") {//发信息  external0
		this->page()->runJavaScript(QString("appendHtml0(%1)").arg(Msg));
		if (isImg) {
			strData = QString::number(imgNum) +"images"+ strData;
		}
		emit signalSendMsg(strData, msgType);
	}
	else {//收信息
		this->page()->runJavaScript(QString("recvHtml_%1(%2)").arg(obj).arg(Msg));
	}
}

QList<QStringList> msgWebView::parseHtml(const QString & html)
{
	QDomDocument doc;
	doc.setContent(html);
	const QDomElement&elememt = doc.documentElement();
	const QDomNode&node = elememt.firstChildElement("body");
	return parseDocNode(node);
}

QList<QStringList> msgWebView::parseDocNode(const QDomNode & node)
{
	QList<QStringList>attr;
	const QDomNodeList &nodes = node.childNodes();
	for (int i = 0; i < nodes.count(); i++) {
		const QDomNode&item = nodes.at(i);
		if (item.isElement()) {
			const QDomElement&element = item.toElement();
			if (element.tagName() == "img") {
				QStringList attrList;
				attrList << "img" << element.attribute("src");
				attr << attrList;
			}
			if (element.tagName() == "span") {
				QStringList attrList;
				attrList << "text" << element.text();
				attr << attrList;
			}
			if (item.hasChildNodes()) {
				attr<<parseDocNode(item);
			}
		}
	}
	return attr;
}

void msgHtmlObj::initHtml()
{
	m_msgLHtml = getMsgHtml("msgleftTmpl");
	m_msgLHtml.replace("%1", m_imgPath);
	m_msgRHtml = getMsgHtml("msgrightTmpl");
	m_msgRHtml.replace("%1", gLoginHeadPath);
}

QString msgHtmlObj::getMsgHtml(const QString & code)
{
	QFile file(":/Resources/MainWindow/MsgHtml/" + code + ".html");
	QString str = "";
	if (file.open(QFile::ReadOnly)) {
		str = QLatin1String(file.readAll());
		file.close();
	}
	else {
		QMessageBox::warning(nullptr, "error!", "init html fail!");
	}
	return str;
}

bool msgWebPage::acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) {
	//仅接受qrc里面的文件,不接受外部链接等
	if (url.scheme() == QString("qrc"))//判断url类型
		return true;
	return false;
}