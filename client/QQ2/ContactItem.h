#pragma once

#include <QWidget>
#include "ui_ContactItem.h"

class ContactItem : public QWidget
{
	Q_OBJECT

public:
	ContactItem(QWidget *parent = Q_NULLPTR);
	~ContactItem();
	void setUserName(const QString&name);
	void setSignName(const QString&name);
	void setHeadPixmap(const QPixmap&path);
	QString getUserName() const;
	QSize getHeadLabelSize()const;
private:
	Ui::ContactItem ui;
	void initControl();
};
