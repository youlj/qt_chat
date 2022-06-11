#pragma once

#include <QWidget>
#include "ui_UserLogin.h"
#include "basicwindow.h"
class UserLogin : public BasicWindow
{
	Q_OBJECT

public:
	UserLogin(QWidget *parent = Q_NULLPTR);
	~UserLogin();
private slots:
	void onLoginBtnClicked();
private:
	void initControl();
	bool connectMySql();
	bool verifyAccount(bool &isUsernameLogin,QString &account);
private:
	Ui::UserLogin ui;
};
