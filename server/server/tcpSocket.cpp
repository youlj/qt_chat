#include "tcpSocket.h"

tcpSocket::tcpSocket()
{
}

tcpSocket::~tcpSocket()
{
}

void tcpSocket::run()
{
	m_desc = this->socketDescriptor();
	connect(this, SIGNAL(readyRead()), this, SLOT(onReceiveData()));
	connect(this, SIGNAL(disconnected()), this, SLOT(onClientDisconnect()));
}

void tcpSocket::onClientDisconnect()
{
	emit signalClientDisconnect(m_desc);
}

void tcpSocket::onReceiveData(){
	QByteArray buffer = this->readAll();
	if (!buffer.isEmpty()) {
		emit signalGetDataFromClient(buffer, m_desc);
	}
}