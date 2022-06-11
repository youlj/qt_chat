#include "receiveFile.h"
#include <QFileDialog>
#include <QMessageBox>
#include "WindowManager.h"

extern QString gFileName;
extern QString gFileData;

receiveFile::receiveFile(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initTitleBar(ONLY_CLOSE_BTN);
	setTitleBarTitle("", ":/Resources/MainWindow/qqlogoclassic.png");
	loadStyleSheet("ReceiveFile");
	this->move(250,366);
	m_titleBar = new titleBar;
	connect(m_titleBar, &titleBar::signalBtnCloseClicked, this, &receiveFile::refuseFile);
}

receiveFile::~receiveFile()
{
	delete m_titleBar;
}

void receiveFile::setMsg(QString & msg)
{
	ui.label->setText(msg);
}

void receiveFile::on_cancelBtn_clicked()
{
	emit refuseFile();
	this->close();
}

void receiveFile::on_okBtn_clicked() {
 	this->close();
 	QString fileSavePath = QFileDialog::getExistingDirectory(nullptr, QStringLiteral("保存文件"), "./");
 	QString filePath = fileSavePath + "/" + gFileName;           
 	QFile file(filePath);
 	if (file.open(QFile::WriteOnly)) {
 		file.write(gFileData.toUtf8());
 		QMessageBox::information(nullptr, "success", QStringLiteral("文件接收成功!"));
 		file.close();
 	}
 	else {
 		QMessageBox::information(nullptr, "fail", QStringLiteral("文件接收失败!"));
 	}
}  