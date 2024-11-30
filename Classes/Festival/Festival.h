#ifndef __FESTIVAL_H__
#define __FESTIVAL_H__

#include "../CommunityEvent/CommunityEvent.h"
#include "../DateManage/DateManage.h"

class Festival : public CommunityEvent {
public:
    static Festival* create(const std::string& name, const std::string& description, const std::string& date, bool isHoliday);

    // 初始化方法
    bool init(const std::string& name, const std::string& description, const std::string& date, bool isHoliday);

    void startEvent(DateManage* dateManager);  // 重写事件开始逻辑

    bool isHoliday() const;

protected:
    bool holiday;  // 是否为节假日

    Festival();
};

#endif // __FESTIVAL_H__
