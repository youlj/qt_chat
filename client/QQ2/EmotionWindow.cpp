#include "EmotionWindow.h"
#include "CommonUtils.h"
#include "EmotionLabelItem.h"
#include <QStyleOption>
#include <QPainter>

const int rows = 12;
const int cols = 14;

EmotionWindow::EmotionWindow(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_DeleteOnClose);
	initControl();
}

EmotionWindow::~EmotionWindow()
{
}

void EmotionWindow::initControl()
{
	CommonUtils::loadStyleSheet(this, "EmotionWindow");
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			EmotionLabelItem*item = new EmotionLabelItem(this);
			item->setEmotionName(i*cols + j);
			connect(item, &EmotionLabelItem::emotionClicked, this, &EmotionWindow::addEmotion);
			ui.gridLayout->addWidget(item, i, j);
		}
	}
}

void EmotionWindow::paintEvent(QPaintEvent * event)
{
	QStyleOption opt;
	opt.init(this);
	QPainter painter(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
	__super::paintEvent(event);
}

void EmotionWindow::addEmotion(int num) {
	hide();
	emit signalEmotionWindowHide();
	emit signalEmotionItemClicked(num);
}
