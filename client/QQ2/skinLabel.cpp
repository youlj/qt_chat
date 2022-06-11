#include "skinLabel.h"

skinLabel::skinLabel(QWidget *parent)
	: QLabel(parent)
{
}

skinLabel::~skinLabel()
{
}

void skinLabel::mousePressEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton) {
		emit clicked();
	}
}
