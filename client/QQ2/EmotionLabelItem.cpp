#include "EmotionLabelItem.h"

EmotionLabelItem::EmotionLabelItem(QWidget *parent)
	: skinLabel(parent)
{
	initControl();
	connect(this, &skinLabel::clicked, [this]() {
		emit emotionClicked(m_emotionName);
	});
}

EmotionLabelItem::~EmotionLabelItem()
{
}

void EmotionLabelItem::setEmotionName(int num)
{
	m_emotionName = num;
	QString img = QString(":/Resources/MainWindow/emotion/%1.png").arg(num);
	m_apngMovie = new QMovie(img, "apng", this);
	m_apngMovie->start();
	m_apngMovie->stop();
	setMovie(m_apngMovie);
}

void EmotionLabelItem::initControl()
{
	setAlignment(Qt::AlignCenter);
	setObjectName("emotionLabelItem");
	setFixedSize(32, 32);
}
