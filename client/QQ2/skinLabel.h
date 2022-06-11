#pragma once

#include <QLabel>
#include <QMouseEvent>
class skinLabel : public QLabel
{
	Q_OBJECT

public:
	skinLabel(QWidget *parent);
	~skinLabel();
	void mousePressEvent(QMouseEvent *event);
signals:
	void clicked();

};
