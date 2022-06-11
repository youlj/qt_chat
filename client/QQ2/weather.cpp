#include "weather.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
weather::weather(QWidget *parent)
	: BasicWindow(parent),m_city(QString::fromLocal8Bit("ÉîÛÚ")),m_ip("")
{
	ui.setupUi(this);
 	//getIP();
 	//getLocation();
	getMsg();
	m_timer = new QTimer(this);
	m_timer->setInterval(5000);
	connect(m_timer, SIGNAL(timeout()),this,SLOT(refresh()));
	m_timer->start();
}

void weather::getMsg()
{
	m_manager = new QNetworkAccessManager;
	QNetworkRequest request;
	request.setUrl(QUrl(QString("http://wthrcdn.etouch.cn/weather_mini?city=%1").arg(m_city)));
	m_manager->get(request);
	connect(m_manager, SIGNAL(finished(QNetworkReply*)), this,SLOT(dealMsg(QNetworkReply*)));
}

void weather::getIP()
{
	m_manager = new QNetworkAccessManager;
	QNetworkRequest request;
	request.setUrl(QUrl("https://bird.ioliu.cn/ip"));
	m_manager->get(request);
	connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(dealIP(QNetworkReply*)));
}

void weather::getLocation()
{
	m_manager = new QNetworkAccessManager;
	QNetworkRequest request;
	request.setUrl(QUrl(QString("http://freeapi.ipip.net/%1").arg(m_ip)));
	m_manager->get(request);
	connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(dealLocation(QNetworkReply*)));
}

weather::~weather()
{
	m_weatherInfo.clear();
	delete m_manager;
}

void weather::dealMsg(QNetworkReply * reply)
{
	QByteArray array = reply->readAll();
	QJsonParseError error;
	QJsonDocument data = QJsonDocument::fromJson(array, &error);
	if (!data.isNull()) {
		QJsonObject obj = data.object();
		if (obj.contains("data")) {
			QJsonValue val = obj.value("data");
			if (val.isObject()) {
				QJsonObject dataObj = val.toObject();
				QJsonValue tempVal = dataObj.value("wendu");
				m_weatherInfo.insert("temperature", tempVal.toString()+QString::fromLocal8Bit("¡æ"));
				m_weatherInfo.insert("tip", dataObj.value("ganmao").toString());
				m_weatherInfo.insert("city", dataObj.value("city").toString());
				if (dataObj.contains("forecast")) {
					QJsonValue foreObj = dataObj.value("forecast");
					if (foreObj.isArray()) {
						QJsonArray array = foreObj.toArray();
						m_weatherInfo.insert("date", array[0].toObject().value("date").toString());
						m_weatherInfo.insert("type", array[0].toObject().value("type").toString());
						m_weatherInfo.insert("wind", array[0].toObject().value("fengxiang").toString());
						QString high = array[0].toObject().value("high").toString();
						QString low = array[0].toObject().value("low").toString();
						QString power = array[0].toObject().value("fengli").toString();
						m_weatherInfo.insert("high", high.mid(3));
						m_weatherInfo.insert("low", low.mid(3));
						m_weatherInfo.insert("power", power.mid(9, 2));	
					}
				}
			}
			initControl();
		}
		else {
			ui.cityLabel->setText(QString::fromLocal8Bit("»ñÈ¡ÌìÆøÐÅÏ¢Ê§°Ü!5ÃëÖÓºóË¢ÐÂ"));
		}
	}
	else {
		ui.cityLabel->setText(QString::fromLocal8Bit("»ñÈ¡ÌìÆøÐÅÏ¢Ê§°Ü!5ÃëÖÓºóË¢ÐÂ"));
	}
	reply->deleteLater();
}

void weather::dealIP(QNetworkReply * reply)
{
	QByteArray array = reply->readAll();
	QJsonParseError error;
	QJsonDocument doc = QJsonDocument::fromJson(array, &error);
	if (!doc.isNull()) {
		QJsonObject obj = doc.object();
		if (obj.contains("data")) {
			QJsonValue data = obj.value("data");
			if (data.isObject()) {
				QJsonObject dataObj = data.toObject();
				if (dataObj.contains("ip")) {
					m_ip = dataObj.value("ip").toString();
				}
			}
		}
	}
	delete m_manager;
	m_manager = NULL;
	reply->deleteLater();
}

void weather::dealLocation(QNetworkReply * reply)
{
	QByteArray array = reply->readAll();
	QJsonParseError error;
	QJsonDocument doc = QJsonDocument::fromJson(array, &error);
	if (!doc.isNull()) {
		if (doc.isArray()) {
			QJsonArray ary = doc.array();
			m_city = ary[2].toString();
		}
	}
	delete m_manager;
	m_manager = NULL;
	reply->deleteLater();
}

void weather::initControl()
{
	ui.cityLabel->setText(m_weatherInfo.value("city"));
	ui.dateLabel->setText(m_weatherInfo.value("date"));
	ui.highLabel->setText(m_weatherInfo.value("high"));
	ui.lowLabel->setText(m_weatherInfo.value("low"));
	ui.powerLabel->setText(m_weatherInfo.value("power"));
	ui.tempLabel->setText(m_weatherInfo.value("temperature"));
	ui.tipLabel->setText(m_weatherInfo.value("tip"));
	ui.typeLabel->setText(m_weatherInfo.value("type"));
	ui.windLabel->setText(m_weatherInfo.value("wind"));
	if (m_weatherInfo.value("type") == QString::fromLocal8Bit("¶àÔÆ")) {
		initImg(QString::fromLocal8Bit("¶àÔÆ"));
		initBg(QString::fromLocal8Bit("UI1"));
	}
	else if (m_weatherInfo.value("type") == QString::fromLocal8Bit("Çç")) {
		initImg(QString::fromLocal8Bit("Çç"));
		initBg(QString::fromLocal8Bit("UI"));
	}
	else if (m_weatherInfo.value("type") == QString::fromLocal8Bit("±©Ñ©")) {
		initImg(QString::fromLocal8Bit("±©Ñ©"));
		initBg(QString::fromLocal8Bit("UI2"));
	}
	else if (m_weatherInfo.value("type") == QString::fromLocal8Bit("±©Óê")) {
		initImg(QString::fromLocal8Bit("±©Óê"));
		initBg(QString::fromLocal8Bit("UI3"));
	}
	else if (m_weatherInfo.value("type") == QString::fromLocal8Bit("´ó±©Óê")) {
		initImg(QString::fromLocal8Bit("´ó±©Óê"));
		initBg(QString::fromLocal8Bit("UI3"));
	}
	else if (m_weatherInfo.value("type") == QString::fromLocal8Bit("´óÑ©")) {
		initImg(QString::fromLocal8Bit("´óÑ©"));
		initBg(QString::fromLocal8Bit("UI2"));
	}
	else if (m_weatherInfo.value("type") == QString::fromLocal8Bit("´óÓê")) {
		initImg(QString::fromLocal8Bit("´óÓê"));
		initBg(QString::fromLocal8Bit("UI5"));
	}
	else if (m_weatherInfo.value("type") == QString::fromLocal8Bit("¶³Óê")) {
		initImg(QString::fromLocal8Bit("¶³Óê"));
		initBg(QString::fromLocal8Bit("UI3"));
	}
	else if (m_weatherInfo.value("type") == QString::fromLocal8Bit("¸¡³¾")) {
		initImg(QString::fromLocal8Bit("¸¡³¾"));
		initBg(QString::fromLocal8Bit("UI4"));
	}
	else if (m_weatherInfo.value("type") == QString::fromLocal8Bit("À×ÕóÓê")) {
		initImg(QString::fromLocal8Bit("À×ÕóÓê"));
		initBg(QString::fromLocal8Bit("UI3"));
	}
	else if (m_weatherInfo.value("type") == QString::fromLocal8Bit("À×ÕóÓê°éÓÐ±ù±¢")) {
		initImg(QString::fromLocal8Bit("À×ÕóÓê°éÓÐ±ù±¢"));
		initBg(QString::fromLocal8Bit("UI3"));
	}
	else if (m_weatherInfo.value("type") == QString::fromLocal8Bit("ö²")) {
		initImg(QString::fromLocal8Bit("ö²"));
		initBg(QString::fromLocal8Bit("UI4"));
	}
	else if (m_weatherInfo.value("type") == QString::fromLocal8Bit("Ç¿É³³¾±©")) {
		initImg(QString::fromLocal8Bit("Ç¿É³³¾±©"));
		initBg(QString::fromLocal8Bit("UI4"));
	}
	else if (m_weatherInfo.value("type") == QString::fromLocal8Bit("ÌØ´ó±©Óê")) {
		initImg(QString::fromLocal8Bit("ÌØ´ó±©Óê"));
		initBg(QString::fromLocal8Bit("UI3"));
	}
	else if (m_weatherInfo.value("type") == QString::fromLocal8Bit("Îí")) {
		initImg(QString::fromLocal8Bit("Îí"));
		initBg(QString::fromLocal8Bit("UI4"));
	}
	else if (m_weatherInfo.value("type") == QString::fromLocal8Bit("Ð¡Ñ©")) {
		initImg(QString::fromLocal8Bit("Ð¡Ñ©"));
		initBg(QString::fromLocal8Bit("UI2"));
	}
	else if (m_weatherInfo.value("type") == QString::fromLocal8Bit("Ð¡Óê")) {
		initImg(QString::fromLocal8Bit("Ð¡Óê"));
		initBg(QString::fromLocal8Bit("UI5"));
	}
	else if (m_weatherInfo.value("type") == QString::fromLocal8Bit("ÑïÉ³")) {
		initImg(QString::fromLocal8Bit("ÑïÉ³"));
		initBg(QString::fromLocal8Bit("UI4"));
	}
	else if (m_weatherInfo.value("type") == QString::fromLocal8Bit("Òõ")) {
		initImg(QString::fromLocal8Bit("Òõ"));
		initBg(QString::fromLocal8Bit("UI4"));
	}
	else if (m_weatherInfo.value("type") == QString::fromLocal8Bit("Óê¼ÐÑ©")) {
		initImg(QString::fromLocal8Bit("Óê¼ÐÑ©"));
		initBg(QString::fromLocal8Bit("UI2"));
	}
	else if (m_weatherInfo.value("type") == QString::fromLocal8Bit("ÕóÑ©")) {
		initImg(QString::fromLocal8Bit("ÕóÑ©"));
		initBg(QString::fromLocal8Bit("UI2"));
	}
	else if (m_weatherInfo.value("type") == QString::fromLocal8Bit("ÕóÓê")) {
		initImg(QString::fromLocal8Bit("ÕóÓê"));
		initBg(QString::fromLocal8Bit("UI3"));
	}
	else if (m_weatherInfo.value("type") == QString::fromLocal8Bit("ÖÐÑ©")) {
		initImg(QString::fromLocal8Bit("ÖÐÑ©"));
		initBg(QString::fromLocal8Bit("UI2"));
	}
	else if (m_weatherInfo.value("type") == QString::fromLocal8Bit("ÖÐÓê")) {
		initImg(QString::fromLocal8Bit("ÖÐÓê"));
		initBg(QString::fromLocal8Bit("UI5"));
	}
}

void weather::initImg(QString str)
{
	QImage img;
	img.load(QString::fromLocal8Bit(":/Resources/weather/%1.png").arg(str));
	ui.imgLabel->setPixmap(QPixmap::fromImage(img));
}

void weather::initBg(QString str)
{
	this->setStyleSheet(QString::fromLocal8Bit("*{color: rgb(52, 73, 94);font-weight:bold;}\
			#weather{background:url(:/Resources/weather/%1.png);\
	     background-size:cover;background-position:center center;}").arg(str));
}

void weather::refresh() {
	ui.cityLabel->clear();
	ui.dateLabel->clear();
	ui.highLabel->clear();
	ui.imgLabel->clear();
	ui.lowLabel->clear();
	ui.powerLabel->clear();
	ui.tempLabel->clear();
	ui.tipLabel->clear();
	ui.typeLabel->clear();
	ui.windLabel->clear();
	m_weatherInfo.clear();
	delete m_manager;
	m_manager = NULL;
 	if (m_ip == "") {
 		getIP();
 	}
 	if (m_city == "") {
 		getLocation();
 	}
	getMsg();
}
