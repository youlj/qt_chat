#include "UserLogin.h"
#include "CCMainWindow.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>

QString gLoginUserID;//登陆者QQ号

UserLogin::UserLogin(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initTitleBar();
	setTitleBarTitle("", ":/Resources/MainWindow/qqlogoclassic.png");
	loadStyleSheet("UserLogin");
	initControl();
}

UserLogin::~UserLogin()
{
}

void UserLogin::initControl()
{
	QLabel*headLabel = new QLabel(this);
	headLabel->setFixedSize(68, 68);
	QPixmap head_mask(":/Resources/MainWindow/head_mask.png");
	headLabel->setPixmap(getRoundImage(QPixmap(":/Resources/MainWindow/app/logo.ico"),
		head_mask, headLabel->size()));
	headLabel->move(width() / 2 - 34, ui.titleWidget->height() - 34);
	connect(ui.loginBtn, SIGNAL(clicked()), this, SLOT(onLoginBtnClicked()));
	if (!connectMySql()) {
		QMessageBox::critical(this, "error", QString::fromLocal8Bit("数据库连接失败!")); 
		close();
	}
}

bool UserLogin::connectMySql()
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

bool UserLogin::verifyAccount(bool &isUsernameLogin, QString &account)
{
	QString strName = ui.editUserAccount->text();
	QString strPwd = ui.editPassword->text();
	QString password = QString("SELECT password FROM account WHERE ID=%1").arg(strName);
	QSqlQuery queryID(password);
	if (queryID.first()) {
		QString pwd = queryID.value(0).toString();
		if (pwd == strPwd) {
			gLoginUserID = strName;
			isUsernameLogin = false;
			account = strName;
			return true;
		}
		else {
			return false;
		}
	}
	password = QString("SELECT password,ID from account WHERE username='%1'").arg(strName);
	QSqlQuery queryUsername(password);
	if (queryUsername.first()) {
		QString pwd = queryUsername.value(0).toString();
		if (pwd == strPwd) {
			gLoginUserID = queryUsername.value(1).toString();
			isUsernameLogin = true;
			account = strName;
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}

void UserLogin::onLoginBtnClicked() {
	bool isUsernameLogin;
	QString account;
	if (!verifyAccount(isUsernameLogin,account)) {
		QMessageBox::critical(this, "error", QString::fromLocal8Bit("用户名或密码错误!"));
		return;
	}

	//更新登录状态为在线
	QSqlQuery query(QString("UPDATE user SET online=1 WHERE ID=%1").arg(gLoginUserID));
	query.exec();

	close();
	CCMainWindow*menu = new CCMainWindow(account,isUsernameLogin);
	menu->show();
}
