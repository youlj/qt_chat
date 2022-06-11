#include "tcpServer.h"
#include <QMessageBox>
#include <QTcpSocket>
#include "tcpSocket.h"

tcpServer::tcpServer(int port):m_port(port)
{
}

tcpServer::~tcpServer()
{
}

//服务器监听端口
bool tcpServer::run()
{
	if (this->listen(QHostAddress::AnyIPv4, m_port)) {
		qDebug() << QString::fromLocal8Bit("服务器监听%1端口成功!").arg(m_port);
		return true;
	}
	else {
		qDebug() << QString::fromLocal8Bit("服务器监听%1端口失败!").arg(m_port);
		return false;
	}
}

void tcpServer::incomingConnection(qintptr desc)
{
	qDebug() << QString::fromLocal8Bit("新的连接:%1").arg(desc);
	tcpSocket*tcp = new tcpSocket();
	tcp->setSocketDescriptor(desc);
	tcp->run();
	connect(tcp, SIGNAL(signalGetDataFromClient(QByteArray&, int)), this, SLOT(dataProcess(QByteArray&, int)));
	connect(tcp, SIGNAL(signalClientDisconnect(int)), this, SLOT(disConnect(int)));
	m_tcpList.append(tcp);
}

void tcpServer::disConnect(int desc)
{
	for (int i = 0; i < m_tcpList.count(); i++) {
		QTcpSocket*item = m_tcpList[i];
		if (item->socketDescriptor() == desc || item->socketDescriptor() == -1) {
			m_tcpList.removeAt(i);
			item->deleteLater();
			qDebug() << QString::fromLocal8Bit("断开与客户端%1").arg(desc) <<QString::fromLocal8Bit("的连接");
			return;
		}
	}
}

void tcpServer::dataProcess(QByteArray&data, int desc) {
	for (int i = 0; i < m_tcpList.count(); i++) {
		QTcpSocket*item = m_tcpList[i];
		if (item->socketDescriptor() == desc) {
			qDebug() << QString::fromLocal8Bit("来自IP:%1").arg(item->peerAddress().toString())
				<< QString::fromLocal8Bit("发来的信息:") << QString(data);
			emit signalTCPSendMsg(data);
		}
	}
}