#include "RootContactItem.h"
#include <QPainter>

RootContactItem::RootContactItem(bool hasArrow,QWidget *parent)
	: QLabel(parent),m_rotation(0),m_hasArrow(hasArrow)
{
	setFixedHeight(32);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	m_animation = new QPropertyAnimation(this, "rotation");
	m_animation->setDuration(50);
	m_animation->setEasingCurve(QEasingCurve::InQuad);
	
}

RootContactItem::~RootContactItem()
{
}

void RootContactItem::setText(const QString & title)
{
	m_title = title;
	update();
}

void RootContactItem::setExpanded(bool expand)
{
	if (expand) {
		m_animation->setEndValue(90);
	}
	else {
		m_animation->setEndValue(0);
	}
	m_animation->start();
}

int RootContactItem::rotation()
{
	return m_rotation;
}

void RootContactItem::setRotation(int rotation)
{
	m_rotation = rotation;
	update();
}

void RootContactItem::paintEvent(QPaintEvent * event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::TextAntialiasing, true);//ÎÄ±¾·´¾â³Ý
	QFont font;
	font.setPointSize(10);
	painter.setFont(font);
	painter.drawText(20, 0, width() - 24, height(), Qt::AlignLeft | Qt::AlignVCenter,m_title);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	painter.save();
	if (m_hasArrow) {
		QPixmap src;                     
		src.load(":/Resources/MainWindow/arrow.png");
		QPixmap tempImg(src.size());
		tempImg.fill(Qt::transparent);
		QPainter p(&tempImg); 
		p.setRenderHint(QPainter::SmoothPixmapTransform, true);
		p.translate(src.width() / 2, src.height() / 2);
		p.rotate(m_rotation);
		p.drawPixmap(0 - src.width() / 2, 0 - src.height() / 2, src);
		painter.drawPixmap(6, (height() - src.height()) / 2, tempImg);
		painter.restore();
	}
	QLabel::paintEvent(event);
}
