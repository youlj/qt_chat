#pragma once

#include <QTcpServer>

class tcpServer : public QTcpServer
{
	Q_OBJECT

public:
	tcpServer(int port);
	~tcpServer();
	bool run();//�����������˿�
signals:
	void signalTCPSendMsg(QByteArray&);
protected:
	//���ӷ�������Ŀͻ���
	void incomingConnection(qintptr desc);
private slots:
	void dataProcess(QByteArray&data, int desc);//��������
	void disConnect(int desc);
private:
	int m_port;
	QList<QTcpSocket*>m_tcpList;
};
