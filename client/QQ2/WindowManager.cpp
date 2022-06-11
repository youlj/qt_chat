#include "WindowManager.h"
#include "TalkWindow.h"
#include "TalkWindowItem.h"
#include <QSqlQueryModel>
//创建单例模式,全局静态对象
Q_GLOBAL_STATIC(WindowManager,theInstance)

WindowManager::WindowManager():QObject(nullptr)
									,m_talkwindowshell(nullptr)
{
} 

WindowManager::~WindowManager()
{
}

QWidget * WindowManager::findWindowName(const QString & name)
{
	if (m_windowMap.contains(name)) {
		return m_windowMap.value(name);
	}
	return nullptr;
}

void WindowManager::deleteWindowName(const QString & name)
{
	m_windowMap.remove(name);
}

void WindowManager::addNewTalkWindow(const QString & id)
{
	if (m_talkwindowshell == nullptr) {
		m_talkwindowshell = new TalkWindowShell;
		connect(m_talkwindowshell, &TalkWindowShell::destroyed, [this](QObject*obj) {
			m_talkwindowshell = nullptr;
		});
	}
	QWidget*widget = findWindowName(id);
	if (!widget) {
		m_createTalkID = id;
		TalkWindow*talkWindow = new TalkWindow(m_talkwindowshell, id);
		TalkWindowItem*talkWindowItem = new TalkWindowItem(talkWindow);
		m_createTalkID = "";
		QSqlQueryModel deptModel;
		QString str = QString("SELECT deptName,sign FROM dept WHERE deptID=%1").arg(id);
		deptModel.setQuery(str);
		int rows = deptModel.rowCount();
		QString name, sign;
		if (rows == 0) {//单聊
			QString str = QString("SELECT username,sign FROM user WHERE ID=%1").arg(id);
			deptModel.setQuery(str);
		}
		QModelIndex nameIndex, signIndex;
		nameIndex = deptModel.index(0, 0);//0行0列,即deptName或username
		signIndex = deptModel.index(0, 1);//0行1列,即sign
		name = deptModel.data(nameIndex).toString();
		sign = deptModel.data(signIndex).toString();
		talkWindow->setWindowName(sign);
		talkWindowItem->setMsgLabelContent(name);
		m_talkwindowshell->addTalkWindow(talkWindow, talkWindowItem,id);
	}
	else {
		QListWidgetItem*item = m_talkwindowshell->getTalkWindowItemMap().key(widget);
		item->setSelected(true);
		m_talkwindowshell->setCurrentWidget(widget);
	}
	m_talkwindowshell->show();
	m_talkwindowshell->activateWindow();
}

void WindowManager::addWindowName(const QString & name, QWidget * widget)
{
	if (!m_windowMap.contains(name)) {
		m_windowMap.insert(name, widget);
	}
}

WindowManager * WindowManager::getInstance()
{
	return theInstance;
}

TalkWindowShell * WindowManager::getTalkWindowShell()
{
	return m_talkwindowshell;
}

QString WindowManager::getCreateTalkID()
{
	return m_createTalkID;
}
