#pragma once

#include <QWidget>
#include "ui_sendFile.h"
#include "basicwindow.h"

class sendFile : public BasicWindow
{
	Q_OBJECT

public:
	sendFile(QWidget *parent = Q_NULLPTR);
	~sendFile();
signals:
	void sendFileClicked(QString&data, int&msgType, QString fileName);
private slots:
	void on_openBtn_clicked();
	void on_sendBtn_clicked();
private:
	Ui::sendFile ui;
	QString m_filePath;
};
