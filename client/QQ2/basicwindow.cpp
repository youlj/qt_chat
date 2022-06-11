#include "basicwindow.h"
#include "CommonUtils.h"
#include "NotifyManage.h"
#include <QFile>
#include <QPainter>
#include <QStyleOption>
#include <QApplication>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QSqlQuery>

extern QString gLoginUserID;

BasicWindow::BasicWindow(QWidget *parent)
	: QDialog(parent)
{
	m_bgc = CommonUtils::getDefaultSkinColor();
	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground, true);
	connect(NotifyManage::getInstance(), SIGNAL(signalSkinChanged(const QColor&)),
		this, SLOT(onSignalSkinChanged(const QColor&)));
}

BasicWindow::~BasicWindow()
{
}

void BasicWindow::loadStyleSheet(const QString & sheetName)
{
	m_sheetName = sheetName;
	QFile file(":/Resources/QSS/" + sheetName + ".css");
	if (file.open(QFile::ReadOnly)) {
		QString style = QLatin1String(file.readAll());
		QString r = QString::number(m_bgc.red());
		QString g = QString::number(m_bgc.green());
		QString b = QString::number(m_bgc.blue());
		style += QString("QWidget[titleSkin=true]{background-color:rgb(%1,%2,%3);\
										border-top-right-radius:4px;border-top-left-radius:4px}QWidget[bottomSkin=true]\
										{border-top:1px solid rgba(%1,%2,%3,100);\
										background-color:rgba(%1,%2,%3,50);\
										border-bottom-left-radius:4px;border-bottom-right-radius:4px;}").arg(r).arg(g).arg(b);
		setStyleSheet(style);
	}
	file.close();
}

QPixmap BasicWindow::getRoundImage(const QPixmap & src, QPixmap & mask, QSize maskSize)
{
	if (maskSize == QSize(0, 0)) {
		maskSize = mask.size();
	}
	else {
		mask = mask.scaled(maskSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}
	QImage resultImage(maskSize, QImage::Format_ARGB32_Premultiplied);
	QPainter painter(&resultImage);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.fillRect(resultImage.rect(), Qt::transparent);
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	painter.drawPixmap(0, 0, mask);
	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
	painter.drawPixmap(0, 0, src.scaled(maskSize,Qt::KeepAspectRatio, Qt::SmoothTransformation));
	painter.end();
	return QPixmap::fromImage(resultImage);
}

void BasicWindow::initBackGroundColor()
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void BasicWindow::paintEvent(QPaintEvent * event)
{
	initBackGroundColor();
	return QDialog::paintEvent(event);
}

void BasicWindow::mouseMoveEvent(QMouseEvent * event)
{
	if (m_mousePressed && (event->buttons() && Qt::LeftButton)) {
		move(event->globalPos() - m_mousePoint);
		event->accept();
	}
}

void BasicWindow::mousePressEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton) {
		m_mousePressed = true;
		m_mousePoint = event->globalPos() - pos();
		event->accept();
	}
}

void BasicWindow::mouseReleaseEvent(QMouseEvent * event)
{
	m_mousePressed = false;
}

void BasicWindow::initTitleBar(ButtonType btnType)
{
	m_pTitleBar = new titleBar(this);
	m_pTitleBar->setButtonType(btnType);
	m_pTitleBar->move(0, 0);
	connect(m_pTitleBar, SIGNAL(signalBtnMinClicked()), this, SLOT(onBtnMinClicked()));
	connect(m_pTitleBar, SIGNAL(signalBtnRestoreClicked()), this, SLOT(onBtnRestoreClicked()));
	connect(m_pTitleBar, SIGNAL(signalBtnMaxClicked()), this, SLOT(onBtnMaxClicked()));
	connect(m_pTitleBar, SIGNAL(signalBtnCloseClicked()), this, SLOT(onBtnCloseClicked()));
}

void BasicWindow::setTitleBarTitle(const QString & title, const QString & icon)
{
	m_pTitleBar->setTitleIcon(icon);
	m_pTitleBar->setTitleContent(title);
}

void BasicWindow::onShowClose(bool) {
	close();
}

void BasicWindow::onShowHide(bool)
{
	hide();
}

void BasicWindow::onShowMin(bool)
{
	showMinimized();
}

void BasicWindow::onShowNormal(bool)
{
	show();
	activateWindow();
}

void BasicWindow::onShowQuit(bool)
{
	//更新登录状态为离线
	QSqlQuery query(QString("UPDATE user SET online=2 WHERE ID=%1").arg(gLoginUserID));
	query.exec();
	QApplication::quit();
}

void BasicWindow::onSignalSkinChanged(const QColor & color)
{
	m_bgc = color;
	loadStyleSheet(m_sheetName);
}

void BasicWindow::onBtnMinClicked()
{
	if ((windowFlags()&Qt::Tool) == Qt::Tool) {
		hide();
	}
	else {
		showMinimized();
	}
}

void BasicWindow::onBtnRestoreClicked()
{
	QPoint pos;
	QSize size;
	m_pTitleBar->getRestoreInfo(pos, size);
	setGeometry(QRect(pos, size));
}

void BasicWindow::onBtnMaxClicked()
{
	m_pTitleBar->setRestoreInfo(pos(), QSize(width(), height()));
	QRect desktopRect = QApplication::desktop()->availableGeometry();
	QRect factRect = QRect(desktopRect.x() - 3, desktopRect.y() - 3, 
		desktopRect.width() + 6, desktopRect.height() + 6);
	setGeometry(factRect);
}

void BasicWindow::onBtnCloseClicked()
{
	close();
}
