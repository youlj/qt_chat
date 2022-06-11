#include "ContactItem.h"

ContactItem::ContactItem(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

ContactItem::~ContactItem()
{
}

void ContactItem::setUserName(const QString & name)
{
	ui.nameLabel->setText(name);
}

void ContactItem::setSignName(const QString & name)
{
	ui.signLabel->setText(name);
}

void ContactItem::setHeadPixmap(const QPixmap & path)
{
	ui.headLabel->setPixmap(path);
}

QString ContactItem::getUserName() const
{
	return ui.nameLabel->text();
}

QSize ContactItem::getHeadLabelSize() const
{
	return ui.headLabel->size();
}

void ContactItem::initControl()
{
}
