#include "SkinWindow.h"
#include "skinLabel.h"
#include "NotifyManage.h"
SkinWindow::SkinWindow(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	loadStyleSheet("SkinWindow");
	initControl();
}

SkinWindow::~SkinWindow()
{
}

void SkinWindow::initControl()
{
	QList<QColor> colorList = {
		QColor(161, 196, 253),QColor(150, 230, 161),QColor(132, 250, 176),QColor(143, 211, 244),
		QColor(226, 235, 240),QColor(224, 195, 252),QColor(142, 197, 252),QColor(79, 172, 254),
		QColor(67, 233, 123),QColor(56, 249, 215),QColor(254, 225, 64),QColor(163, 189, 237)
	};
	for (int row = 0; row < 3; row++) {
		for (int col = 0; col < 4; col++) {
			skinLabel*label = new skinLabel(this);
			label->setFixedSize(83, 83);
			QPalette color;
			color.setColor(QPalette::Background, colorList.at(row * 4 + col));
			label->setAutoFillBackground(true);
			label->setPalette(color);
			label->setCursor(Qt::PointingHandCursor);
			connect(label, &skinLabel::clicked, [row,col,colorList]() {
				NotifyManage::getInstance()->notifyOtherWindowSkinChange(colorList.at(row * 4 + col));
			});
			ui.gridLayout->addWidget(label,row,col);
		}
	}
	connect(ui.sysmin, SIGNAL(clicked()), this, SLOT(onShowMin()));
	connect(ui.sysclose, SIGNAL(clicked()), this, SLOT(onShowClose()));
}

void SkinWindow::onShowClose()
{
	close();
}

void SkinWindow::onShowMin() {
	showMinimized();
}