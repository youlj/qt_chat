#pragma once

#include <QtWidgets/QWidget>
#include <QTreeWidgetItem>
#include <QMap>
#include "ui_CCMainWindow.h"
#include "basicwindow.h"
#include "weather.h"

class CCMainWindow : public BasicWindow
{
	Q_OBJECT

public:
	CCMainWindow(QString account,bool isUsernameLogin,QWidget *parent = Q_NULLPTR);
	void initControl();
	void setUserName(const QString&name);
	void setLevelPixmap(int level);
	void setHeadPixmap(const QString&path);
	void setStatusMenuIcon(const QString&path);
	QWidget* addApp(const QString&path, const QString&name);
	void initTimer();
	void updateSearchStyle();
	void initContactTree();
	void addDepts(QTreeWidgetItem*item, int deptID);
	QString getHeadImg();
	QString getUsername();
private slots:
	void onAppIconClicked();
	void onItemClicked(QTreeWidgetItem*item, int col);
	void onItemExpanded(QTreeWidgetItem*item);
	void onItemDoubleClicked(QTreeWidgetItem*item, int col);
	void onItemCollapsed(QTreeWidgetItem*item);
private:
	void resizeEvent(QResizeEvent *event);
	bool eventFilter(QObject *obj, QEvent *event);
	void mousePressEvent(QMouseEvent *event);
private:
	Ui::CCMainWindowClass ui;
	bool m_isUsernameLogin;
	QString m_account;
	weather*m_wea;
	//QMap<QTreeWidgetItem*, QString>m_groupMap;
};
