#ifndef __FARM_SCENE_H__
#define __FARM_SCENE_H__
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "../Player/Player.h"
#include "../Map/FarmMap.h"
#include "../Npc/Npc.h"
#include "../Festival/Festival.h"
#include "../Constant/Constant.h"
#include "../DateManage/DateManage.h"

class Farm : public cocos2d::Scene {
public:
	static cocos2d::Scene* createScene();

	bool init();

	CREATE_FUNC(Farm);

	//该地图上所有npc，静态成员变量
	static std::vector<Npc*> npcs;

	// 析构函数
	~Farm();
private:
	// 当前天气系统
	ParticleSystemQuad* currentWeatherSystem = nullptr;

	// 背景遮罩
	LayerColor* backgroundMask = nullptr;

	void closeButtonClicked(Ref* pSender);
	
	void updateWeather(Weather weather);
};

#endif