#include "NotifyManage.h"
#include "CommonUtils.h"
NotifyManage*NotifyManage::instance = nullptr;
NotifyManage::NotifyManage():QObject(nullptr)
{

}

NotifyManage::~NotifyManage()
{
}

NotifyManage * NotifyManage::getInstance()
{
	if (instance == nullptr) {
		instance = new NotifyManage();
	}
	return instance;
}

void NotifyManage::notifyOtherWindowSkinChange(const QColor & color)
{
	emit signalSkinChanged(color);
	CommonUtils::setDefaultSkinColor(color);
}
