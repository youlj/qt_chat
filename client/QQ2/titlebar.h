#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLabel>

enum ButtonType {
	MIN_BTN,
	MIN_MAX_BTN,
	ONLY_CLOSE_BTN
};

class titleBar : public QWidget
{
	Q_OBJECT

public:
	titleBar(QWidget *parent=nullptr);
	~titleBar();
	void setTitleIcon(const QString&filePath);
	void setTitleContent(const QString&titleContent);
	void setTitleWidth(int width);
	void setButtonType(ButtonType btnType);
	void setRestoreInfo(const QPoint &point, const QSize&size);
	void getRestoreInfo(QPoint&point, QSize&size);
private:
	void paintEvent(QPaintEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

	void initControl();
	void initConnections();
	void loadStyleSheet(const QString&sheetName);
signals:
	void signalBtnMinClicked();
	void signalBtnRestoreClicked();
	void signalBtnMaxClicked();
	void signalBtnCloseClicked();
private slots:
	void onBtnMinClicked();
	void onBtnResoreClicked();
	void onBtnMaxClicked();
	void onBtnCloseClicked();
private:
	QLabel*m_pIcon;
	QLabel*m_pTitleContent;
	QPushButton*m_pMinBtn;
	QPushButton*m_pRestoreBtn;
	QPushButton*m_pMaxBtn;
	QPushButton*m_pCloseBtn;
	QPoint m_restorePos;
	QSize m_restoreSize;
	bool m_isPressed;
	QPoint m_startMovePos;
	QString m_titleContent;
	ButtonType m_btnType;
};
