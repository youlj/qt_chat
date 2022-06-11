#pragma once

#include <QWidget>
#include "basicwindow.h"
#include "ui_SkinWindow.h"

class SkinWindow : public BasicWindow
{
	Q_OBJECT

public:
	SkinWindow(QWidget *parent = Q_NULLPTR);
	~SkinWindow();
	void initControl();
private slots:
	void onShowMin();
	void onShowClose();
private:
	Ui::SkinWindow ui;
};
