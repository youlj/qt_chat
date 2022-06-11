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
	void initTalkWindow();//��ʼ��Ⱥ��
	void initPtoPTalk();//����
	void addPeopleInfo(QTreeWidgetItem*rootItem,int userID);
	void initGroupTalkStatus();
	int getPartyID();//��ȡ��ȺID
private:
	Ui::TalkWindow ui;
	QString m_talkId;
	bool m_isGroupTalk;//�Ƿ�ΪȺ��
	QMap<QTreeWidgetItem*, QString>m_groupPeopleMap;
	sendFile*m_file;
public slots:
	void onSendBtnClicked(bool isClicked);
	void onItemDoubleClicked(QTreeWidgetItem*item, int col);
	void onFileOpenBtnClicked(bool);
	friend class TalkWindowShell;
};