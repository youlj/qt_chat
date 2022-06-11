#pragma once

#include <QWebEngineView>
#include <QDomNode>
#include <QWebEnginePage>

class msgHtmlObj :public QObject {
	Q_OBJECT
	Q_PROPERTY(QString msgLHtmlTmpl MEMBER m_msgLHtml NOTIFY signalMsgHtml)
	Q_PROPERTY(QString msgRHtmlTmpl MEMBER m_msgRHtml NOTIFY signalMsgHtml)
public:
	msgHtmlObj(QObject*parent,QString imgPath="");
signals:
	void signalMsgHtml(const QString&html);
private:
	QString m_imgPath;
	QString m_msgLHtml;//��߷�������Ϣ
	QString m_msgRHtml;//�ұ��Լ���������Ϣ
private:
	void initHtml();//��ʼ��������ҳ
	QString getMsgHtml(const QString&code);
};

class msgWebPage :public QWebEnginePage {
	Q_OBJECT
public:
	msgWebPage(QObject*parent=nullptr):QWebEnginePage(parent){}
protected:
	bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame);
};

class msgWebView : public QWebEngineView
{
	Q_OBJECT

public:
	msgWebView(QWidget *parent);
	~msgWebView();
	void appendMsg(const QString&html,QString obj="0");
private:
	QList<QStringList>parseHtml(const QString&html);
	QList<QStringList>parseDocNode(const QDomNode&node);
private:
	msgHtmlObj*m_msgHtmlObj;
	QWebChannel*m_channel;
signals:
	void signalSendMsg(QString & data, int & msgType, QString file = "");
};
 