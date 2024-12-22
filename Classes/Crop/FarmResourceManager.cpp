/****************************************************************
 * File Function: ũ����Դ�������ʵ��
 * Author:        ����
 ****************************************************************/

#include "FarmResourceManager.h"
#include "../Constant/Constant.h"
#include "cocos2d.h"

 // ��̬��������
FarmResourceManager* FarmResourceManager::create() {
	FarmResourceManager* farmResourceManager = new (std::nothrow) FarmResourceManager();
	if (farmResourceManager && farmResourceManager->init()) {
		return farmResourceManager;
	}
	CC_SAFE_DELETE(farmResourceManager);
	return nullptr;
}

// ��ʼ��
bool FarmResourceManager::init() {
	waterResource=MAX_WATER_RESOURCE;				// ˮ��Դ
	seedResource = MAX_SEED_RESOURCE;				// ������Դ
	ferilizerResource = MAX_FERTILIZE_RESOURCE;		// ������Դ
	return true;
}

// ��ˮ
void FarmResourceManager::useWater() {
	// ˮ��Դ����
	waterResource -= AMOUNT_OF_WATER_PER_USE;
}

// ʩ��
void FarmResourceManager::useFertilize() {
	// ������Դ����
	ferilizerResource-= AMOUNT_OF_FERTILIZE_PER_USE;
}

// ����
void FarmResourceManager::useSeed() {
	// ���Ӽ���
	seedResource--;
}