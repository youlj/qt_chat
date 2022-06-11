#pragma once

#include <QWidget>
#include <QListWidgetItem>
#include <QMap>
#include "basicwindow.h"
#include "ui_TalkWindowShell.h"
#include "EmotionWindow.h"
#include <QTcpSocket>
#include <QUdpSocket>

const int tcpPort = 7777;

class TalkWindowItem;
class TalkWindow;
class EmotionWindow;

enum GroupType {
	PARTY,//总群
	ARMY,//武装部
	POLICE,//警察部
	RELATION,//外交部
	PTOP//私人聊天
};

class TalkWindowShell : public BasicWindow
{
	Q_OBJECT

public:
	TalkWindowShell(QWidget *parent = Q_NULLPTR);
	~TalkWindowShell();
	void addTalkWindow(TalkWindow*talkWindow, TalkWindowItem*talkWindowItem,const QString& id);
	void setCurrentWidget(QWidget*widget);
	const QMap<QListWidgetItem*, QWidget*>&getTalkWindowItemMap()const;
public slots:
	void onEmotionBtnClicked(bool isClicked);
	void onEmotionItemClicked(int num);
	void onTalkWindowItemClicked(QListWidgetItem*item);
	void updateTcpSendMsg(QString&data, int&msgType, QString file = "");
	void processPendingData();
private:
	void initControl();
	void initTcpSocket();
	void initUdpSocket();
	void getUserID(QStringList&userIDList);
	bool createJSFile(QStringList&userList);
	void handleReceivedMsg(int sendUserID, int msgType, QString msg);
private:
	Ui::TalkWindowClass ui;
	QMap<QListWidgetItem*, QWidget*>m_talkWindowItemMap;
	EmotionWindow*m_emotionWindow;
	QTcpSocket*m_tcpClient;//TCP客户端
	QUdpSocket*m_udpReceiver;
};
