#include "QMsgTextEdit.h"
#include <QMovie>
#include <QUrl>

QMsgTextEdit::QMsgTextEdit(QWidget *parent)
	: QTextEdit(parent)
{
}

QMsgTextEdit::~QMsgTextEdit()
{
	deleteAllEmotionImage();
}

void QMsgTextEdit::addEmotionUrl(int num)
{
	const QString&name = QString("qrc:/Resources/MainWindow/emotion/%1.png").arg(num);
	insertHtml(QString("<img src=%1 />").arg(name));
	if (m_emotionUrl.contains(name)) {
		return;
	}
	else {
		m_emotionUrl.append(name);
	}
	QMovie*movie = new QMovie(name, "apng", this);
	m_emotionMap.insert(movie, name);
	connect(movie,SIGNAL(frameChanged(int)),this,SLOT(onEmotionImageFramChange(int)));
	movie->start();
	updateGeometry();
}

void QMsgTextEdit::deleteAllEmotionImage()
{
	for (auto it = m_emotionMap.begin(); it != m_emotionMap.end(); ++it) {
		delete it.key();
	}
	m_emotionMap.clear();  
}

void QMsgTextEdit::onEmotionImageFramChange(int frame) {
	QMovie* movie = qobject_cast<QMovie*>(sender());
	document()->addResource(QTextDocument::ImageResource, QUrl(m_emotionMap.value(movie)),movie->currentPixmap());

}