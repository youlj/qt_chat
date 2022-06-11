#pragma once

#include <QLabel>
#include <QPropertyAnimation>

class RootContactItem : public QLabel
{
	Q_OBJECT
	Q_PROPERTY(int rotation READ rotation WRITE setRotation)
public:
	RootContactItem(bool hasArrow=true,QWidget *parent=nullptr);
	~RootContactItem();
	void setText(const QString &title);
	void setExpanded(bool expand);
private:
	int rotation();
	void setRotation(int rotation);
protected:
	void paintEvent(QPaintEvent *event);
private:
	QPropertyAnimation*m_animation;
	int m_rotation;
	bool m_hasArrow;
	QString m_title;
};
