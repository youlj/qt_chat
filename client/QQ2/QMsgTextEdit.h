#pragma once

#include <QTextEdit>

class QMsgTextEdit : public QTextEdit
{
	Q_OBJECT

public:
	QMsgTextEdit(QWidget *parent=nullptr);
	~QMsgTextEdit();
	void addEmotionUrl(int num);
	void deleteAllEmotionImage();

private slots:
	void onEmotionImageFramChange(int frame);
private:
	QList<QString>m_emotionUrl;
	QMap<QMovie*, QString>m_emotionMap;
};
