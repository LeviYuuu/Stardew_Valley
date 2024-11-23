#ifndef __COOPERATION_SCENE_H__
#define __COOPERATION_SCENE_H__

#include "cocos2d.h"

class Cooperation : public cocos2d::Scene {
public:
	static cocos2d::Scene* createScene();
	
	virtual bool init();

	// 实现静态 Cooperation* create()函数
	CREATE_FUNC(Cooperation);

	// 返回项的回调函数
	void backCallBack(cocos2d::Ref* pSender);
};


#endif