#include "titlebar.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QFile>

#define BTN_HEIGHT 27
#define BTN_WIDTH 27
#define TITLE_HEIGHT 27

titleBar::titleBar(QWidget *parent)
	: QWidget(parent),m_isPressed(false),m_btnType(MIN_MAX_BTN)
{
	initControl();
	initConnections();
	loadStyleSheet("Title");
}

titleBar::~titleBar()
{
}

void titleBar::setTitleIcon(const QString & filePath)
{
	QPixmap icon(filePath);
	m_pIcon->setFixedSize(icon.size());
	m_pIcon->setPixmap(icon);
}

void titleBar::setTitleContent(const QString & titleContent)
{
	m_pTitleContent->setText(titleContent);
	m_titleContent = titleContent;
}

void titleBar::setTitleWidth(int width)
{
	setFixedWidth(width);
}

void titleBar::setButtonType(ButtonType btnType)
{
	m_btnType = btnType;
	switch (btnType)
	{
	case MIN_BTN:
		m_pRestoreBtn->setVisible(false);
		m_pMaxBtn->setVisible(false);
		break;
	case MIN_MAX_BTN:
		m_pRestoreBtn->setVisible(false);
		break;
	case ONLY_CLOSE_BTN:
		m_pMinBtn->setVisible(false);
		m_pRestoreBtn->setVisible(false);
		m_pMaxBtn->setVisible(false);
		break;
	default:
		break;
	}
}

void titleBar::setRestoreInfo(const QPoint & point, const QSize & size)
{
	m_restorePos = point;
	m_restoreSize = size;
}

void titleBar::getRestoreInfo(QPoint & point, QSize & size)
{
	point = m_restorePos;
	size = m_restoreSize;
}

void titleBar::paintEvent(QPaintEvent * event)
{
	QPainter painter(this);
	QPainterPath path;
	path.setFillRule(Qt::WindingFill);
	path.addRoundedRect(QRect(0, 0, width(), height()), 3, 3);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	if (width() != parentWidget()->width()) {
		setFixedWidth(parentWidget()->width());
	}
	QWidget::paintEvent(event);
}

void titleBar::mouseDoubleClickEvent(QMouseEvent * event)
{
	if (m_btnType == MIN_MAX_BTN) {
		if (m_pMaxBtn->isVisible()) {
			onBtnMaxClicked();
		}
		else {
			onBtnResoreClicked();
		}
	}
	return QWidget::mouseDoubleClickEvent(event);
}

void titleBar::mouseMoveEvent(QMouseEvent * event)
{
	if (m_isPressed) {
		QPoint movePoint = event->globalPos() - m_startMovePos;
		QPoint widgetPoint = parentWidget()->pos();
		m_startMovePos = event->globalPos();
		parentWidget()->move(widgetPoint.x() + movePoint.x(), widgetPoint.y() + movePoint.y());
	}
	return QWidget::mouseMoveEvent(event);
}

void titleBar::mousePressEvent(QMouseEvent * event)
{
	if (m_btnType == MIN_MAX_BTN) {
		if (m_pMaxBtn->isVisible()) {
			m_isPressed = true;
			m_startMovePos = event->globalPos();
		}
		else {
			m_isPressed = false;
		}
	}
	else {
		m_isPressed = true;
		m_startMovePos = event->globalPos();
	}
	return QWidget::mousePressEvent(event);
}

void titleBar::mouseReleaseEvent(QMouseEvent * event)
{
	m_isPressed = false;
	return QWidget::mouseReleaseEvent(event);
}

void titleBar::initControl()
{
	m_pIcon = new QLabel(this);
	m_pTitleContent = new QLabel(this);
	m_pMinBtn = new QPushButton(this);
	m_pMaxBtn = new QPushButton(this);
	m_pRestoreBtn = new QPushButton(this);
	m_pCloseBtn = new QPushButton(this);
  	m_pMinBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
  	m_pMaxBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
  	m_pRestoreBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
  	m_pCloseBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
	m_pTitleContent->setObjectName("TitleContent");
	m_pMinBtn->setObjectName("ButtonMin");
	m_pMaxBtn->setObjectName("ButtonMax");
	m_pRestoreBtn->setObjectName("ButtonRestore");
	m_pCloseBtn->setObjectName("ButtonClose");

	QHBoxLayout*layout = new QHBoxLayout(this);
	layout->addWidget(m_pIcon);
	layout->addWidget(m_pTitleContent);
	layout->addWidget(m_pMinBtn);
	layout->addWidget(m_pRestoreBtn);
	layout->addWidget(m_pMaxBtn);
	layout->addWidget(m_pCloseBtn);
	layout->setContentsMargins(5, 0, 0, 0);
	layout->setSpacing(0);

	m_pTitleContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	setFixedHeight(TITLE_HEIGHT);
	setWindowFlags(Qt::FramelessWindowHint);
}

void titleBar::initConnections()
{
	connect(m_pMinBtn, SIGNAL(clicked()), this, SLOT(onBtnMinClicked()));
	connect(m_pRestoreBtn, SIGNAL(clicked()), this, SLOT(onBtnResoreClicked()));
	connect(m_pMaxBtn, SIGNAL(clicked()), this, SLOT(onBtnMaxClicked()));
	connect(m_pCloseBtn, SIGNAL(clicked()), this, SLOT(onBtnCloseClicked()));
}

void titleBar::loadStyleSheet(const QString & sheetName)
{
	QFile file(":/Resources/QSS/" + sheetName + ".css");
	file.open(QFile::ReadOnly);
	if (file.isOpen()) {
		QString style =this->styleSheet();
		style += QLatin1String(file.readAll());
		setStyleSheet(style);
	}
}

void titleBar::onBtnMinClicked() {
	emit signalBtnMinClicked();
}

void titleBar::onBtnResoreClicked()
{
	m_pRestoreBtn->setVisible(false);
	m_pMaxBtn->setVisible(true);
	emit signalBtnRestoreClicked();
}

void titleBar::onBtnMaxClicked()
{
	m_pMaxBtn->setVisible(false);
	m_pRestoreBtn->setVisible(true);
	emit signalBtnMaxClicked();
}

void titleBar::onBtnCloseClicked()
{
	emit signalBtnCloseClicked();
}
