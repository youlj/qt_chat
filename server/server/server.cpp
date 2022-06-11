#include "server.h"
#include <QMouseEvent>
#include <QFile>
#include <QMessageBox>
#include <QModelIndex>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QTimer>
#include <QFileDialog>

int tcpPort = 7777;
int udpPort = 8888;

server::server(QWidget *parent)
	: QDialog(parent),m_deptID(0),m_userID(0)
{
	ui.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground, true);
	if (!connectMySql()) {
		QMessageBox::critical(this, "error", QString::fromLocal8Bit("�������ݿ�ʧ��!"));
		close();
	}
	loadStyleSheet();
	initControl();
	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//ֻ��
	initDeptMap();
	initOnlineMap();
	initStatusMap();
	initComboData();
	m_partyID = getPartyID();
	updateUserData();
	QTimer*timer = new QTimer(this);
	timer->setInterval(300);
	timer->start();
	connect(timer, SIGNAL(timeout()), this, SLOT(onRefreshData()));
	initTcp();
	initUdp();
}

void server::initTcp()
{
	m_tcpServer = new tcpServer(tcpPort);
	m_tcpServer->run();
	connect(m_tcpServer, &tcpServer::signalTCPSendMsg, this, &server::onUDPBroadMsg);

}

void server::initUdp()
{
	m_udpSender = new QUdpSocket(this);
}
 
void server::onShowMin()
{
	showMinimized();
}

void server::onShowClose()
{
	close();
}

void server::onRefreshData()
{
	updateUserData(m_deptID, m_userID);
}

void server::on_queryDeptBtn_clicked()
{
	m_userID = 0;
	int deptID = ui.queryDeptComboBox->currentData().toInt();
	m_deptID = deptID;
}

void server::on_queryIDBtn_clicked()
{
	m_deptID = 0;
	if (!ui.queryIDLineEdit->text().length()) {
		QMessageBox::information(this, "error", QStringLiteral("QQ�Ų���Ϊ��!"));
		ui.queryIDLineEdit->setFocus();
		return;
	}
	int userID = ui.queryIDLineEdit->text().toInt();
	QSqlQuery query(QString("SELECT * FROM user WHERE ID=%1").arg(userID));
	if (!query.first()) {
		QMessageBox::information(this, "error", QStringLiteral("��������ȷ��QQ��!"));
		return;
	}
	m_userID = userID;
}

void server::on_delBtn_clicked()
{
	if (!ui.delIDLineEdit->text().length()) {
		QMessageBox::information(this, "error", QStringLiteral("QQ�Ų���Ϊ��!"));
		ui.delIDLineEdit->setFocus();
		return;
	}
	int userID = ui.delIDLineEdit->text().toInt();
	QSqlQuery query(QString("SELECT username FROM user WHERE ID=%1").arg(userID));
	if (!query.first()) {
		QMessageBox::information(this, "error", QStringLiteral("��������ȷ��QQ��!"));
		return;
	}
	QSqlQuery queryStatus(QString("UPDATE user SET status=0 WHERE ID=%1").arg(userID));
	queryStatus.exec();
	QMessageBox::information(this, "success", QString::fromLocal8Bit("��Ա%1��QQ��%2�ѱ�ע��!").arg(query.value(0).toString()).arg(userID));
}

void server::on_imgBtn_clicked()
{
	m_imgPath = QFileDialog::getOpenFileName(this, QStringLiteral("���ļ�"), ".", "*.jpg;;*.png");
	if (!m_imgPath.size()) {
		//QMessageBox::information(this, "error", QStringLiteral("��ѡ��ͼƬ!"));
		return;
	}
	QPixmap img;
	img.load(m_imgPath);
	ui.imgLabel->setPixmap(img.scaled(ui.imgLabel->size()));
}

void server::on_addBtn_clicked()
{
	if (!ui.nameLineEdit->text().length()) {
		QMessageBox::information(this, "error", QStringLiteral("��������Ϊ��!"));
		ui.nameLineEdit->setFocus();
		return;
	}
	if (!m_imgPath.length()) {
		QMessageBox::information(this, "error", QStringLiteral("ͷ����Ϊ��!"));
		ui.imgLabel->setFocus();
		return;
	}
	QString name = ui.nameLineEdit->text();
	QString sign = ui.signLineEdit->text();
	int deptID = ui.deptComboBox->currentData().toInt();
	m_imgPath.replace("/","\\\\");//��'/'�滻Ϊ'\',��"\\\\"��,ǰ����"\\"��ʾת��,������"\\"��ʾת��һ��'\'
	QSqlQuery query(QString("INSERT INTO user(deptID,username,sign,img) VALUES(%1,'%2','%3','%4')")
		.arg(deptID).arg(name).arg(sign).arg(m_imgPath));
	if (query.numRowsAffected() > 0) {
		QMessageBox::information(this, "success", QStringLiteral("�����Ա�ɹ�!"));
	}
	else {
		QMessageBox::information(this, "error", QStringLiteral("�����Աʧ��!"));
	}
}

void server::loadStyleSheet()
{
	QFile file(":/Resources/server.css");
	if (file.open(QFile::ReadOnly)) {
		QString str = file.readAll();
		setStyleSheet(str);
		file.close();
	}
}

void server::initControl()
{
	connect(ui.sysmin, SIGNAL(clicked()), this, SLOT(onShowMin()));
	connect(ui.sysclose, SIGNAL(clicked()), this, SLOT(onShowClose()));
}

bool server::connectMySql()
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setDatabaseName("chat");
	db.setHostName("localhost");
	db.setPort(3306);
	db.setPassword("adidas123you");
	db.setUserName("root");
	if (db.open()) {
		return true;
	}
	return false;
}

void server::updateUserData(int deptID, int userID)
{
	ui.tableWidget->clear();
	if (deptID&&deptID != m_partyID) {
		m_model.setQuery(QString("SELECT * FROM user WHERE deptID=%1").arg(deptID));
	}
	else if (userID) {
		m_model.setQuery(QString("SELECT * FROM user WHERE ID=%1").arg(userID));
	}
	else {
		m_model.setQuery("SELECT * FROM user");
	}
	QModelIndex index;
	QStringList list;
	list << QString::fromLocal8Bit("����") << QString::fromLocal8Bit("��ԱID") << QString::fromLocal8Bit("����") <<
		QString::fromLocal8Bit("ǩ��") << QString::fromLocal8Bit("״̬") << QString::fromLocal8Bit("ͷ��") << QString::fromLocal8Bit("����");
	int rows = m_model.rowCount();
	int cols = m_model.columnCount();
	ui.tableWidget->setRowCount(rows);
	ui.tableWidget->setColumnCount(cols);
	ui.tableWidget->setHorizontalHeaderLabels(list);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//�����еȿ�
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			index = m_model.index(i, j);
			QString str = m_model.data(index).toString();
			QSqlRecord record = m_model.record(i);
			if (record.fieldName(j) == "deptID") {
				ui.tableWidget->setItem(i, j, new QTableWidgetItem(m_deptMap.value(str)));
				continue;
			}
			else if (record.fieldName(j) == "status") {
				ui.tableWidget->setItem(i, j, new QTableWidgetItem(m_statusMap.value(str)));
				continue;
			}
			else if (record.fieldName(j) == "online") {
				ui.tableWidget->setItem(i, j, new QTableWidgetItem(m_onlineMap.value(str)));
				continue;
			}
			ui.tableWidget->setItem(i, j, new QTableWidgetItem(str));
		}
	}
}

int server::getPartyID()
{
	QSqlQuery query(QString("SELECT deptID FROM dept WHERE deptName='%1'").arg(QString::fromLocal8Bit("��Ⱥ")));
	query.first();
	return query.value(0).toInt();
}

void server::initDeptMap()
{
	m_deptMap.insert("2001", QStringLiteral("�з���"));
	m_deptMap.insert("2002", QStringLiteral("���Բ�"));
	m_deptMap.insert("2003", QStringLiteral("���۲�"));
}

void server::initStatusMap()
{
	m_statusMap.insert("0", QStringLiteral("��ע��"));
	m_statusMap.insert("1", QStringLiteral("��Ч"));
}

void server::initOnlineMap()
{
	m_onlineMap.insert("1", QStringLiteral("����"));
	m_onlineMap.insert("2", QStringLiteral("����"));
}

void server::initComboData()
{
	int deptNum = ui.deptComboBox->count();
	for (int i = 0; i < deptNum; i++) {
		QString str=ui.deptComboBox->itemText(i);
		QSqlQuery query(QString("SELECT deptID FROM dept WHERE deptName='%1'").arg(str));
		query.first();
		ui.deptComboBox->setItemData(i, query.value(0).toInt());
	}
	int queryDeptNum = ui.queryDeptComboBox->count();
	for (int i = 0; i < queryDeptNum; i++) {
		QString str = ui.queryDeptComboBox->itemText(i);
		QSqlQuery query(QString("SELECT deptID FROM dept WHERE deptName='%1'").arg(str));
		query.first();
		ui.queryDeptComboBox->setItemData(i, query.value(0).toInt());
	}
}

void server::mousePressEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton) {
		m_mousePoint = event->globalPos() - pos();
		m_isPressed = true;
		event->accept();
	}
}

void server::mouseMoveEvent(QMouseEvent * event)
{
	if (m_isPressed &&( event->buttons()&& Qt::LeftButton)) {
		move(event->globalPos() - m_mousePoint);
		event->accept();
	}
}

void server::mouseReleaseEvent(QMouseEvent * event)
{
	m_isPressed = false;
}

void server::onUDPBroadMsg(QByteArray&bt) {
	for (quint16 i = udpPort; i < udpPort + 10; i++) {
		m_udpSender->writeDatagram(bt, bt.size(), QHostAddress::Broadcast,i);//udp�㲥����
	}
} 