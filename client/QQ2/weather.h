#pragma once

#include <QWidget>
#include "ui_weather.h"
#include "basicwindow.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
class weather : public BasicWindow
{
	Q_OBJECT

public:
	weather(QWidget *parent = Q_NULLPTR);
	void getMsg();
	void getIP();
	void getLocation();
	~weather();
private slots:
	void refresh();
	void dealMsg(QNetworkReply*reply);
	void dealIP(QNetworkReply*reply);
	void dealLocation(QNetworkReply*reply);
private:
	void initControl();
	void initImg(QString str);
	void initBg(QString str);
private:
	Ui::weather ui;
	QString m_city;
	QString m_ip;
	QTimer *m_timer;
	QNetworkAccessManager*m_manager;
	QMap<QString, QString>m_weatherInfo;
};
