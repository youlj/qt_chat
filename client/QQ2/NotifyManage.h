#pragma once

#include <QObject>

class NotifyManage : public QObject
{
	Q_OBJECT

public:
	NotifyManage();
	~NotifyManage();
	static NotifyManage*getInstance();
	void notifyOtherWindowSkinChange(const QColor&color);
signals:
	void signalSkinChanged(const QColor&color);
private:
	static NotifyManage*instance;
	
};
