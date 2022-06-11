#pragma once

#include <QTcpSocket>

class tcpSocket : public QTcpSocket
{
	Q_OBJECT

public:
	tcpSocket();
	~tcpSocket();
	void run();
private slots:
	void onReceiveData();
	void onClientDisconnect();
signals:
	void signalGetDataFromClient(QByteArray&,int );
	void signalClientDisconnect(int );
private:
	int m_desc;//描述符,用于唯一标识
};
