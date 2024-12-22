/****************************************************************
 * File Function: ʵ��ũ����ͼ��
 * Author:        ��׿�� ����
 ****************************************************************/
#ifndef __FARMMAP_H__
#define __FARMMAP_H__
#include "cocos2d.h"
#include "../Crop/Crop.h"
#include "TileNode.h"
#include "SceneMap.h"
#include "../Animal/AnimalManager.h"
#include "../Npc/Npc.h"
#include "../Bag/Bag.h"
#include "../Tool/Kettle.h"
#include "../Festival/Festival.h"
#include "../Constant/Constant.h"
#include <string>
#include <memory>
#include <vector>

// ũ����ͼ��
class FarmMap : public SceneMap {
public:
	// ��ȡ����ʵ��
	static FarmMap* getInstance();

	// ��ʼ����ͼ
	bool init(const std::string& tmxFile) override;

	//���ͼ����npc
	bool npcInit(const cocos2d::Vec2& position, Npc* npc);

	// ���µ�ͼλ��
	void moveMapByDirection(const cocos2d::Vec2& direction);

	// ��ײ��⣺������λ���Ƿ����ϰ���
	bool isCollidable(const cocos2d::Vec2& position) const;

	// ������������ũ���Ľ����߼�
	void interactWithMap() override;

	// ��ͼʱ�����
	void mapUpdateByTime();
private:
	// ���캯������������˽�л�
	FarmMap();
	~FarmMap();

	// ���ÿ�������͸�ֵ����
	FarmMap(const FarmMap&) = delete;
	FarmMap& operator=(const FarmMap&) = delete;

	// ����ʵ��
	static FarmMap* instance;

	// �����洢TMX��ͼ�ĸ�����
	cocos2d::TMXLayer* grassLayer;
	cocos2d::TMXLayer* soilLayer;
	cocos2d::TMXLayer* obstacleLayer;
	cocos2d::TMXLayer* moldLayer;
	cocos2d::TMXLayer* waterLayer;
	cocos2d::TMXLayer* stoneLayer;

	// �������
	AnimalManager* animalManager;

	// ��������Ľ���
	void interactWithSoil(std::string itemName, const int& x, const int& y);

	// ��ʯͷ��Ľ���
	void interactWithStone(std::string itemName, const int& x, const int& y);

	// ��ݵصĽ���
	void interactWithGrass(std::string itemName, const int& x, const int& y);

	// �����ӵĽ���
	void interactWithMold(std::string itemName, const int& x, const int& y);
};
#endif