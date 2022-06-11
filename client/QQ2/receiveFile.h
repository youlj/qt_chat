#pragma once

#include <QWidget>
#include "ui_receiveFile.h"
#include "basicwindow.h"

class receiveFile : public BasicWindow
{
	Q_OBJECT

public:
	receiveFile(QWidget *parent = Q_NULLPTR);
	~receiveFile();
	void setMsg(QString&msg);
signals:
	void refuseFile();
private slots:
	void on_okBtn_clicked();
	void on_cancelBtn_clicked();
private:
	Ui::receiveFile ui;
	titleBar*m_titleBar;
};
