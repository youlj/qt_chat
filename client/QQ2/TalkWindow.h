#pragma once

#include <QWidget>
#include "ui_TalkWindow.h"
#include "TalkWindowShell.h"
#include "sendFile.h"

class TalkWindow : public QWidget
{
	Q_OBJECT

public:
	TalkWindow(QWidget *parent ,const QString&id);
	~TalkWindow();
	void addEmotionImage(int num);
	void setWindowName(const QString&name);
	QString getTalkID();
private:
	void initControl();
	void initTalkWindow();//初始化群聊
	void initPtoPTalk();//单聊
	void addPeopleInfo(QTreeWidgetItem*rootItem,int userID);
	void initGroupTalkStatus();
	int getPartyID();//获取总群ID
private:
	Ui::TalkWindow ui;
	QString m_talkId;
	bool m_isGroupTalk;//是否为群聊
	QMap<QTreeWidgetItem*, QString>m_groupPeopleMap;
	sendFile*m_file;
public slots:
	void onSendBtnClicked(bool isClicked);
	void onItemDoubleClicked(QTreeWidgetItem*item, int col);
	void onFileOpenBtnClicked(bool);
	friend class TalkWindowShell;
};