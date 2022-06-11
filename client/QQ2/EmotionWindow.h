#pragma once

#include <QWidget>
#include "ui_EmotionWindow.h"

class EmotionWindow : public QWidget
{
	Q_OBJECT

public:
	EmotionWindow(QWidget *parent = Q_NULLPTR);
	~EmotionWindow();
private:
	void initControl();
	void paintEvent(QPaintEvent*event)override;
signals:
	void signalEmotionWindowHide();
	void signalEmotionItemClicked(int num);
public slots:
	void addEmotion(int num);
private:
	Ui::EmotionWindow ui;
};
