#include "FishingRod.h"
#include "../Player/Player.h"
#include "../Scene/FarmScene.h" 
#include "../Constant/Constant.h"
#include "../Bag/Bag.h"
#include "cocos2d.h"
#include "cocos/editor-support/cocostudio/ActionTimeline/CSLoader.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

bool FishingRod::isUsed = false;

FishingRod* FishingRod::create() {
	FishingRod* fishingRod = new (std::nothrow) FishingRod();
	if (fishingRod && fishingRod->init()) {
		fishingRod->autorelease();
		return fishingRod;
	}
	CC_SAFE_DELETE(fishingRod);
	return nullptr;
}

bool FishingRod::init()
{
    // 调用基类的初始化方法
    return Tool::init("fishingRod");
}

void FishingRod::useTool()
{
    // 获取玩家实例
    Player* player = Player::getInstance();
    // 创建动画帧
    Vector<SpriteFrame*> frames;

    Vec2 direction = player->getLastDirection();
    // 每个方向帧数不同不能复用
    if (direction.x > 0) { // 玩家面向右侧
        // 创建扔出鱼竿的动画
        frames.pushBack(SpriteFrame::create(ResPath::RIGHT_FISHING_OUT_1, ToolRects::RIGHT_FISHING_OUT_1));
        frames.pushBack(SpriteFrame::create(ResPath::RIGHT_FISHING_OUT_2, ToolRects::RIGHT_FISHING_OUT_2));
        frames.pushBack(SpriteFrame::create(ResPath::RIGHT_FISHING_OUT_3, ToolRects::RIGHT_FISHING_OUT_3));
        frames.pushBack(SpriteFrame::create(ResPath::RIGHT_FISHING_OUT_4, ToolRects::RIGHT_FISHING_OUT_4));
    }
    else if (direction.x < 0) { // 玩家面向左侧
        frames.pushBack(SpriteFrame::create(ResPath::LEFT_FISHING_OUT_1, ToolRects::LEFT_FISHING_OUT_1));
        frames.pushBack(SpriteFrame::create(ResPath::LEFT_FISHING_OUT_2, ToolRects::LEFT_FISHING_OUT_2));
        frames.pushBack(SpriteFrame::create(ResPath::LEFT_FISHING_OUT_3, ToolRects::LEFT_FISHING_OUT_3));
        frames.pushBack(SpriteFrame::create(ResPath::LEFT_FISHING_OUT_4, ToolRects::LEFT_FISHING_OUT_4));
    }
    else if (direction.y > 0) { // 玩家面向上方
        frames.pushBack(SpriteFrame::create(ResPath::UP_FISHING_OUT_1, ToolRects::UP_FISHING_OUT_1));
        frames.pushBack(SpriteFrame::create(ResPath::UP_FISHING_OUT_2, ToolRects::UP_FISHING_OUT_2));
        frames.pushBack(SpriteFrame::create(ResPath::UP_FISHING_OUT_3, ToolRects::UP_FISHING_OUT_3));
    }
    else if (direction.y < 0) { // 玩家面向下方
        frames.pushBack(SpriteFrame::create(ResPath::DOWN_FISHING_OUT_1, ToolRects::DOWN_FISHING_OUT_1));
        frames.pushBack(SpriteFrame::create(ResPath::DOWN_FISHING_OUT_2, ToolRects::DOWN_FISHING_OUT_2));
        frames.pushBack(SpriteFrame::create(ResPath::DOWN_FISHING_OUT_3, ToolRects::DOWN_FISHING_OUT_3));
        frames.pushBack(SpriteFrame::create(ResPath::DOWN_FISHING_OUT_4, ToolRects::DOWN_FISHING_OUT_4));  
    }

    // 创建动画
    auto animation = Animation::createWithSpriteFrames(frames, 0.1f);  // 每帧持续 0.1 秒
    auto animate = Animate::create(animation);

    // 动作序列：播放动画后，执行额外的逻辑
    auto sequence = Sequence::create(
        animate,
        CallFunc::create([=]() {
            // 创建动画帧
            Vector<SpriteFrame*> framesWaiting;

            Vec2 direction = player->getLastDirection();

            // 创建扔出鱼竿的动画
            if (direction.x > 0) { // 玩家面向右侧
                // 创建扔出鱼竿的动画
                framesWaiting.pushBack(SpriteFrame::create(ResPath::RIGHT_FISHING_WAITING_1, ToolRects::RIGHT_FISHING_WAITING_1));
                framesWaiting.pushBack(SpriteFrame::create(ResPath::RIGHT_FISHING_WAITING_2, ToolRects::RIGHT_FISHING_WAITING_2));
            }
            else if (direction.x < 0) { // 玩家面向左侧
                framesWaiting.pushBack(SpriteFrame::create(ResPath::LEFT_FISHING_WAITING_1, ToolRects::LEFT_FISHING_WAITING_1));
                framesWaiting.pushBack(SpriteFrame::create(ResPath::LEFT_FISHING_WAITING_2, ToolRects::LEFT_FISHING_WAITING_2));
            }
            else if (direction.y > 0) { // 玩家面向上方
                framesWaiting.pushBack(SpriteFrame::create(ResPath::UP_FISHING_WAITING_1, ToolRects::UP_FISHING_WAITING_1));
                framesWaiting.pushBack(SpriteFrame::create(ResPath::UP_FISHING_WAITING_2, ToolRects::UP_FISHING_WAITING_2));
            }
            else if (direction.y < 0) { // 玩家面向下方
                framesWaiting.pushBack(SpriteFrame::create(ResPath::DOWN_FISHING_WAITING_1, ToolRects::DOWN_FISHING_WAITING_1));
                framesWaiting.pushBack(SpriteFrame::create(ResPath::DOWN_FISHING_WAITING_2, ToolRects::DOWN_FISHING_WAITING_2));
            }


            auto animationWaiting = Animation::createWithSpriteFrames(framesWaiting, 0.5f);
            animationWaiting->setLoops(-1);
            auto animateWait = Animate::create(animationWaiting);

            player->runAction(animateWait);
            }),

        nullptr
    );
    if (isUsed == true) {
        player->stopAllActions(); // 停止浮动动画
        this->reelInRod();      // 播放收回动画
        isUsed = false;
    }
    else {
        player->runAction(sequence);
        isUsed = true;
    }
}

void FishingRod::reelInRod()
{
    // 获取玩家实例
    Player* player = Player::getInstance();
    // 创建动画帧
    Vector<SpriteFrame*> frames;

    Vec2 direction = player->getLastDirection();
    // 每个方向帧数不同不能复用
    if (direction.x > 0) { // 玩家面向右侧
        // 创建扔出鱼竿的动画
        frames.pushBack(SpriteFrame::create(ResPath::RIGHT_FISHING_IN_1, ToolRects::RIGHT_FISHING_IN_1));
        frames.pushBack(SpriteFrame::create(ResPath::RIGHT_FISHING_IN_2, ToolRects::RIGHT_FISHING_IN_2));
        frames.pushBack(SpriteFrame::create(ResPath::RIGHT_FISHING_IN_3, ToolRects::RIGHT_FISHING_IN_3));
        frames.pushBack(SpriteFrame::create(ResPath::RIGHT_FISHING_IN_4, ToolRects::RIGHT_FISHING_IN_4));
        frames.pushBack(SpriteFrame::create(ResPath::RIGHT_FISHING_IN_5, ToolRects::RIGHT_FISHING_IN_5));
        frames.pushBack(SpriteFrame::create(ResPath::STAND_RIGHT, ToolRects::STAND_RIGHT));
    }
    else if (direction.x < 0) { // 玩家面向左侧
        frames.pushBack(SpriteFrame::create(ResPath::LEFT_FISHING_IN_1, ToolRects::LEFT_FISHING_IN_1));
        frames.pushBack(SpriteFrame::create(ResPath::LEFT_FISHING_IN_2, ToolRects::LEFT_FISHING_IN_2));
        frames.pushBack(SpriteFrame::create(ResPath::LEFT_FISHING_OUT_3, ToolRects::LEFT_FISHING_IN_3));
        frames.pushBack(SpriteFrame::create(ResPath::LEFT_FISHING_IN_4, ToolRects::LEFT_FISHING_IN_4));
        frames.pushBack(SpriteFrame::create(ResPath::LEFT_FISHING_IN_5, ToolRects::LEFT_FISHING_IN_5));
        frames.pushBack(SpriteFrame::create(ResPath::STAND_LEFT, ToolRects::STAND_LEFT));
    }
    else if (direction.y > 0) { // 玩家面向上方
        frames.pushBack(SpriteFrame::create(ResPath::UP_FISHING_IN_1, ToolRects::UP_FISHING_IN_1));
        frames.pushBack(SpriteFrame::create(ResPath::UP_FISHING_IN_2, ToolRects::UP_FISHING_IN_2));
        frames.pushBack(SpriteFrame::create(ResPath::UP_FISHING_IN_3, ToolRects::UP_FISHING_IN_3));
        frames.pushBack(SpriteFrame::create(ResPath::STAND_UP, ToolRects::STAND_UP));
    }
    else if (direction.y < 0) { // 玩家面向下方
        frames.pushBack(SpriteFrame::create(ResPath::DOWN_FISHING_IN_1, ToolRects::DOWN_FISHING_IN_1));
        frames.pushBack(SpriteFrame::create(ResPath::DOWN_FISHING_IN_2, ToolRects::DOWN_FISHING_IN_2));
        frames.pushBack(SpriteFrame::create(ResPath::STAND_DOWN, ToolRects::STAND_DOWN));
    }

    // 创建动画
    auto animation = Animation::createWithSpriteFrames(frames, 0.1f);  // 每帧持续 0.1 秒
    auto animate = Animate::create(animation);

    // 动作序列：播放动画后，执行额外的逻辑
    auto sequence = Sequence::create(
        animate,
        CallFunc::create([=]() {
            // 随机选择钓到的鱼
            const Item pufferFish("pufferFish", "items/pufferFish.png");
            const Item tuna("tuna", "items/tuna.png");
            const Item anchovy("anchovy", "items/anchovy.png");

            const std::vector<Item> fishList = {
                pufferFish,
                tuna,
                anchovy
            };
            int randomIndex = rand() % fishList.size();  // 随机索引
            Item fishCaught = fishList[randomIndex];  // 获取随机钓到的鱼

            Bag* bag = Bag::getInstance();
            bag->addItem(&fishCaught);
            // 在玩家上方显示钓到的鱼的提示
            auto label = Label::createWithTTF(fishCaught.name + " Caught!", "fonts/Marker Felt.ttf", 24);
            label->setPosition(player->getPosition() + Vec2(0, 100));  // 显示在玩家上方
            label->setTextColor(Color4B::WHITE);
            player->getParent()->addChild(label);

            // 动画效果：淡入淡出
            label->runAction(Sequence::create(
                FadeIn::create(0.5f),  // 淡入
                DelayTime::create(2.0f),  // 显示 2 秒
                FadeOut::create(0.5f),  // 淡出
                RemoveSelf::create(),  // 移除自己
                nullptr
            ));
            }),
            nullptr
            );
    player->runAction(sequence);  // 播放动画
}
