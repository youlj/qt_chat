#pragma once

#include <QtWidgets/QDialog>
#include "ui_server.h"
#include "tcpServer.h"
#include <QSqlQueryModel>
#include <QUdpSocket>

class server : public QDialog
{
	Q_OBJECT

public:
	server(QWidget *parent = Q_NULLPTR);
	void initTcp();
	void initUdp();
private slots:
	void onUDPBroadMsg(QByteArray&bt);
	void onShowMin();
	void onShowClose();
	void onRefreshData();
	void on_queryDeptBtn_clicked();
	void on_queryIDBtn_clicked();
	void on_delBtn_clicked();
	void on_imgBtn_clicked();
	void on_addBtn_clicked();
private:
	void loadStyleSheet();
	void initControl();
	bool connectMySql();
	void updateUserData(int deptID=0,int userID=0);
	int getPartyID();
	void initDeptMap();
	void initStatusMap();
	void initOnlineMap();
	void initComboData();
protected:
	void mousePressEvent(QMouseEvent*event);
	void mouseMoveEvent(QMouseEvent*event);
	void mouseReleaseEvent(QMouseEvent*event);
private:
	Ui::serverClass ui;
	tcpServer*m_tcpServer;
	QUdpSocket*m_udpSender;
	QPoint m_mousePoint;
	bool m_isPressed;
	QSqlQueryModel m_model;
	int m_partyID;//×ÜÈººÅ
	int m_deptID;
	int m_userID;
	QString m_imgPath;
	QMap<QString, QString>m_deptMap;
	QMap<QString, QString>m_statusMap;
	QMap<QString, QString>m_onlineMap;
};
