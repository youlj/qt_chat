#include "sendFile.h"
#include "WindowManager.h"
#include <QFileDialog>
#include <QMessageBox>

sendFile::sendFile(QWidget *parent)
	: BasicWindow(parent),m_filePath("")
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initTitleBar(ONLY_CLOSE_BTN);
	setTitleBarTitle("", ":/Resources/MainWindow/qqlogoclassic.png");
	loadStyleSheet("SendFile");
	TalkWindowShell*talkWindowShell = WindowManager::getInstance()->getTalkWindowShell();
	connect(this, &sendFile::sendFileClicked, talkWindowShell, &TalkWindowShell::updateTcpSendMsg);

}

sendFile::~sendFile()
{
}

void sendFile::on_sendBtn_clicked()
{
	if (!m_filePath.isEmpty()) {
		QFile file(m_filePath);
		if (file.open(QFile::ReadOnly)) {
			QString str = file.readAll();
			QFileInfo info(m_filePath);
			QString fileName = info.fileName();
			int msgType = 2;
			emit sendFileClicked(str,msgType,fileName);
			file.close();
		}
		else {
			QMessageBox::information(this, "error", QString::fromLocal8Bit("���ļ�%1ʧ��!").arg(m_filePath));
		}
		m_filePath = "";
		this->close();
	}
}

void sendFile::on_openBtn_clicked() {
	m_filePath = QFileDialog::getOpenFileName(this, QStringLiteral("���ļ�"), "./", QString::fromLocal8Bit("�ļ�(*.txt *.doc);;�����ļ�(*.*)"));
	ui.lineEdit->setText(m_filePath);
}
