#pragma once

#include "titlebar.h"
#include <QDialog>

class BasicWindow : public QDialog
{
	Q_OBJECT
		 
public:
	BasicWindow(QWidget *parent=nullptr);
	virtual ~BasicWindow();
	void loadStyleSheet(const QString&sheetName);
	QPixmap getRoundImage(const QPixmap &src, QPixmap &mask, QSize maskSize = QSize(0, 0));
private:
	void initBackGroundColor();
protected:
	void paintEvent(QPaintEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
protected:
	void initTitleBar(ButtonType btnType = MIN_BTN);
	void setTitleBarTitle(const QString &title, const QString&icon = "");
public slots:
	void onShowClose(bool);
	void onShowHide(bool);
	void onShowMin(bool);
	void onShowNormal(bool);
	void onShowQuit(bool);
	void onSignalSkinChanged(const QColor&color);

	void onBtnMinClicked();
	void onBtnRestoreClicked();
	void onBtnMaxClicked();
	void onBtnCloseClicked();
protected:
	QPoint m_mousePoint;
	bool m_mousePressed;
	QColor m_bgc;
	QString m_sheetName;
	titleBar* m_pTitleBar;
};
