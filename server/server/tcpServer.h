#pragma once

#include <QTcpServer>

class tcpServer : public QTcpServer
{
	Q_OBJECT

public:
	tcpServer(int port);
	~tcpServer();
	bool run();//服务器监听端口
signals:
	void signalTCPSendMsg(QByteArray&);
protected:
	//连接发送请求的客户端
	void incomingConnection(qintptr desc);
private slots:
	void dataProcess(QByteArray&data, int desc);//处理数据
	void disConnect(int desc);
private:
	int m_port;
	QList<QTcpSocket*>m_tcpList;
};
