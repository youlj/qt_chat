#pragma once

#include <QObject>
#include "TalkWindowShell.h"
class WindowManager : public QObject
{
	Q_OBJECT

public:
	WindowManager();
	~WindowManager();
	QWidget*findWindowName(const QString&name);
	void deleteWindowName(const QString&name);
	void addNewTalkWindow(const QString&id);
	void addWindowName(const QString&name, QWidget*widget);
    static WindowManager*getInstance();//µ¥ÀýÄ£Ê½
	TalkWindowShell*getTalkWindowShell();
	QString getCreateTalkID();
private:
	TalkWindowShell*m_talkwindowshell;
	QMap<QString, QWidget*>m_windowMap;
	QString m_createTalkID = "";
};
