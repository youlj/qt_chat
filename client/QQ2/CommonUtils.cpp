#include "CommonUtils.h"
#include <QPainter>
#include <QFile>
#include <QWidget>
#include <QSettings>
#include <QApplication>
CommonUtils::CommonUtils()
{
}

QPixmap CommonUtils::getRoundImage(const QPixmap & src, QPixmap & mask, QSize maskSize)
{
	if (maskSize == QSize(0, 0)) {
		maskSize = mask.size();
	}
	else {
		mask = mask.scaled(maskSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}
	QImage resultImg(maskSize,QImage::Format_ARGB32_Premultiplied);
	QPainter painter(&resultImg);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.fillRect(resultImg.rect(), Qt::transparent);
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	painter.drawPixmap(0, 0, mask);
	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
	painter.drawPixmap(0, 0, src.scaled(maskSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	painter.end();
	return QPixmap::fromImage(resultImg);
}

void CommonUtils::setDefaultSkinColor(const QColor & color)
{
	const QString &&path = QApplication::applicationDirPath() + "/skinColor.ini";
	QSettings setting(path, QSettings::IniFormat);
	setting.setValue("defaultSkin/red", color.red());
	setting.setValue("defaultSkin/green", color.green());
	setting.setValue("defaultSkin/blue", color.blue());
}

QColor CommonUtils::getDefaultSkinColor()
{
	const QString &&path = QApplication::applicationDirPath() + "/skinColor.ini";
	QSettings setting(path, QSettings::IniFormat);
	QColor color;
	color.setRed(setting.value("defaultSkin/red").toInt());
	color.setGreen(setting.value("defaultSkin/green").toInt());
	color.setBlue(setting.value("defaultSkin/blue").toInt());
	return color;
}

void CommonUtils::loadStyleSheet(QWidget * widget, const QString & sheetName)
{
	QFile file(":/Resources/QSS/" + sheetName + ".css");
	if (file.open(QFile::ReadOnly)) {
		QString str = QLatin1String(file.readAll());
		widget->setStyleSheet(str);
		file.close();
	}
}
