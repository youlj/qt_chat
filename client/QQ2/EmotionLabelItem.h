#pragma once

#include "skinLabel.h"
#include <QMovie>

class EmotionLabelItem : public skinLabel
{
	Q_OBJECT

public:
	EmotionLabelItem(QWidget *parent);
	~EmotionLabelItem();
	void setEmotionName(int num);
signals:
	void emotionClicked(int num);
private:
	void initControl();
private:
	int m_emotionName;
	QMovie*m_apngMovie;
};
