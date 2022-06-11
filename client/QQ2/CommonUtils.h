#pragma once
#include <QPixmap>
#include <QSize>
#include <QProxyStyle>

class CustomProxyStyle :public QProxyStyle {
public:
	CustomProxyStyle(QObject*parent) {
		setParent(parent);
	}
	virtual void drawPrimitive(PrimitiveElement elememt, const QStyleOption*option, QPainter*painter, const QWidget*widget = 0)
		const {
		if (elememt == PE_FrameFocusRect) {
			//ȥ��windows����Ĭ�ϱ߿�,������ȡ����ʱ�����л���
			return;
		}
		else {
			QProxyStyle::drawPrimitive(elememt, option, painter, widget);
		}
	}
};

class CommonUtils
{
public:
	CommonUtils();
	static QPixmap getRoundImage(const QPixmap&src, QPixmap&mask, QSize maskSize = QSize(0, 0));
	static void setDefaultSkinColor(const QColor&color);
	static QColor getDefaultSkinColor();
	static void loadStyleSheet(QWidget*widget,const QString&sheetName);
};

