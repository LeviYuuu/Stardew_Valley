/****************************************************************
 * File Function: ʵ��ũ����ͼ��
 * Author:        ��׿�� ����
 ****************************************************************/
#include "FarmMap.h"
#include "../Animal/Cow.h"
#include "../Animal/Chicken.h"
#include "../Animal/Sheep.h"
#include "../Animal/Pig.h"
#include "../Skill/SkillTree.h"
#include "../Item/StoneItem.h"
#include "../Player/Player.h"
#include "../Market/MarketState.h"
#include "../Crop/FarmResourceManager.h"
#include<string>

USING_NS_CC;

// ��ʼ����̬ʵ��
FarmMap* FarmMap::instance = nullptr;

// ���캯��
FarmMap::FarmMap() : grassLayer(nullptr), soilLayer(nullptr), obstacleLayer(nullptr),
moldLayer(nullptr), waterLayer(nullptr), stoneLayer(nullptr),
animalManager(nullptr) {}

// ��������
FarmMap::~FarmMap() {
    instance = nullptr;
    animalManager=nullptr;
}

// ��ȡ����ʵ��
FarmMap* FarmMap::getInstance() {
    if (!instance) {
        instance = new (std::nothrow) FarmMap();
        if (instance && instance->init("Maps/farmSpring/farmMap.tmx")) {
            instance->autorelease();
        }
        else {
            CC_SAFE_DELETE(instance);
        }
    }
    return instance;
}

bool FarmMap::init(const std::string& tmxFile) {
    if (!SceneMap::init(tmxFile)) {
        return false;
    }

    // ��ȡ��ͼ�ĸ���ͼ��
    grassLayer = tiledMap->getLayer("Grass");
    soilLayer = tiledMap->getLayer("Soil");
    obstacleLayer = tiledMap->getLayer("Obstacles");
    moldLayer = tiledMap->getLayer("Mold");
    waterLayer = tiledMap->getLayer("Water");
    stoneLayer = tiledMap->getLayer("Stone");

    // ��Ӷ������
    animalManager = AnimalManager::create();
    addChild(animalManager);

    // ��ȡ�ߵ��ͼ�ĳ���
    int width = tiledMap->getMapSize().width;
    int height = tiledMap->getMapSize().height;
        
    // ����ͼ��Ϊÿһ��ͼ��
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (stoneLayer->getTileGIDAt(Vec2(x, y))) {
                mapTileNode[x][y] = Stone::create(Vec2(x,y));
            }
            else if (waterLayer->getTileGIDAt(Vec2(x, y))) {
                mapTileNode[x][y] = Water::create(Vec2(x, y));
            }
            else if (moldLayer->getTileGIDAt(Vec2(x, y))) {
                mapTileNode[x][y] = Mold::create(Vec2(x, y));
            }
            else if(obstacleLayer->getTileGIDAt(Vec2(x,y))){
                mapTileNode[x][y] = Obstacle::create(Vec2(x, y));  
            }
            else if (soilLayer->getTileGIDAt(Vec2(x, y))) {
                mapTileNode[x][y] = Soil::create(Vec2(x, y));
            }
            else {
                mapTileNode[x][y] = Grass::create(Vec2(x, y));
            }
        }
    }

    // ���lambda��������FarmMap����������ÿ֡ʱ�����һ��
    schedule([this](float dt) {
        // ���Ŀ��λ���Ƿ����ϰ���
        auto player = Player::getInstance();

        // ���ڵ�ͼ��������һ���ᵽ���λ��
        Vec2 playerSize2 = Vec2(0.0f, player->getContentSize().height * 1.0f);

        auto playerPositionInMap = player->getPosition() - getPosition() - playerSize2 * 0.5f + player->getVelocity() / MAP_MOVE_SPEED * 10.0f;
        if (isCollidable(playerPositionInMap)) {
             velocity = Vec2::ZERO;
             player->stopMoving();
        }
        auto position = getPosition() + velocity * dt;
        // max��֤���ڵ����½磬 min��֤С�ڵ����Ͻ�
        position.x = std::max(WINSIZE.width - getMapSize().width, std::min(position.x, 0.0f));
        position.y = std::max(WINSIZE.height - getMapSize().height, std::min(position.y, 0.0f));

        setPosition(position);
        }, "farm_map");
    return true;
}

// ���ͼ���� NPC
bool FarmMap::npcInit(const Vec2& position, Npc* npc) {
    npc->setPosition(position);
    addChild(npc, 5);
     return true;
}

// ���µ�ͼλ��
void FarmMap::moveMapByDirection(const Vec2& direction) {
    velocity = direction * MAP_MOVE_SPEED;
}

// ��ײ��⣺������λ���Ƿ����ϰ���, positon�������ڵ�ͼ����ϵ��ԭ�������½ǣ��е�����
bool FarmMap::isCollidable(const Vec2& position) const {
    // ��ȡ��ͼ����Ƭ��С
    const Size tileSize = tiledMap->getTileSize();
    const Size mapSize = tiledMap->getMapSize();

    // �������ڵ�ͼ�е�����ת��Ϊ��Ƭ��ͼ���꣨ԭ�������Ͻǣ�
    int x = position.x / tileSize.width;
    int y = (mapSize.height * tileSize.height - position.y) / tileSize.height;

    // ����Ƿ��ڵ�ͼ��Χ��
    if (x < 0 || x >= mapSize.width || y < 0 || y >= mapSize.height) {
        return true; // ������ͼ��Χ��Ϊ�ϰ���
    }

    // ��ײ���
    TileType tileType = mapTileNode[x][y]->getTileType();
    if (tileType == TileType::Stone || tileType == TileType::Obstacle || tileType == TileType::Water||tileType == TileType::Mold) {
 
        return true;
    }
    else {
        return false;
    }
}

// �����ũ���Ľӿ�
void FarmMap::interactWithMap() {
    // ��ȡ��ҡ���ͼ������ʵ��
    Player* player = Player::getInstance();
    FarmMap* farmMap = FarmMap::getInstance();
    Bag* bag = Bag::getInstance();

    if (!player->getUseItemEnable()) {
        return;
    }

    // ������ض��Ĺ��ߵĻ������Ǿ��ǺͶ��ｻ��

    // ��ȡҪ����������λ��
    Vec2 playerPosition = player->getPosition();
    const Size tileSize = farmMap->tiledMap->getTileSize();
    const Size mapSize = farmMap->tiledMap->getMapSize();
    playerPosition = playerPosition - farmMap->getPosition();
    playerPosition.y = playerPosition.y - player->getContentSize().height / 2 + 10.0f;
    playerPosition.x = playerPosition.x / tileSize.width;
    playerPosition.y = (mapSize.height * tileSize.height - playerPosition.y) / tileSize.height;
    const Vec2 lastDirection = player->getLastDirection();

    // ��ǰ����ǰ����ߵ�
    if (lastDirection == Vec2(1, 0) && playerPosition.x + 1 < mapSize.width - 1) {
        playerPosition.x++;
    }
    else if (lastDirection == Vec2(0, 1) && playerPosition.y - 1 >= 0) {
        playerPosition.y--;
    }
    else if (lastDirection == Vec2(-1, 0) && playerPosition.x - 1 >= 0) {
        playerPosition.x--;
    }
    else if (lastDirection == Vec2(0, -1) && playerPosition.y + 1 < mapSize.height - 1) {
        playerPosition.y++;
    }

    const int x = playerPosition.x;
    const int y = playerPosition.y;

    // ��õ�ǰ������ʹ�õĹ���
    std::string currentItemName = player->getCurrentItemName();

    if (mapTileNode[x][y]->getTileType() == TileType::Soil) {
        interactWithSoil(currentItemName, x, y);
    }
    else if (mapTileNode[x][y]->getTileType() == TileType::Grass) {
        interactWithGrass(currentItemName, x, y);
    }
    else if (mapTileNode[x][y]->getTileType() == TileType::Mold) {
        interactWithMold(currentItemName, x, y);
    }
    else if (mapTileNode[x][y]->getTileType() == TileType::Stone) {
        interactWithStone(currentItemName, x, y);
    }
    else {
        return;
    }
}

// �������Ľ���
void FarmMap::interactWithSoil(std::string itemName, const int& x, const int& y) {
    // ���ݹ���ȥ����
    if (itemName == "hoe") {
        dynamic_cast<Soil*>(mapTileNode[x][y])->soilHoe();
    }
    else if (itemName == "fertilizer") {
        dynamic_cast<Soil*>(mapTileNode[x][y])->soilFertilize();
        FarmResourceManager* farmResourceManager = FarmResourceManager::create();
        farmResourceManager->useFertilize();
    }
    else if (itemName == "kettle") {
        dynamic_cast<Soil*>(mapTileNode[x][y])->soilWater();
        FarmResourceManager* farmResourceManager = FarmResourceManager::create();
        farmResourceManager->useWater();
    }
    else if (itemName == "dogbaneSeed" || itemName == "cornSeed" || itemName == "carrotSeed") {
        SkillTree::getInstance()->updatePlantingCount(1);
        dynamic_cast<Soil*>(mapTileNode[x][y])->plantCrop(itemName);
        FarmResourceManager* farmResourceManager = FarmResourceManager::create();
        farmResourceManager->useSeed();
    }
    else if (itemName == "scythe") {
        dynamic_cast<Soil*>(mapTileNode[x][y])->harvest();
    }
    else if (itemName == "agrochemical") {
        dynamic_cast<Soil*>(mapTileNode[x][y])->applyAgrochemical();
    }
    else {
        return;
    }

    // ����֮�����soil������
    dynamic_cast<Soil*>(mapTileNode[x][y])->updateGID();
    int soilGID = dynamic_cast<Soil*>(mapTileNode[x][y])->getCurrentGID();
    soilLayer->setTileGID(soilGID,Vec2(x,y));
}

// ��ݵصĽ���
void FarmMap::interactWithGrass(std::string itemName, const int& x, const int& y) {
    
    // ���ߵ�����ת��Ϊ��ͼ����
    Vec2 animalPosition;
    const Size tileSize = tiledMap->getTileSize();                      
    const Size mapSize = tiledMap->getMapSize();
    animalPosition.x = x * tileSize.width;
    animalPosition.y = mapSize.height * tileSize.height - y * tileSize.height;

    // ��Ӷ���
    if (itemName == "cow") {
        Cow* cow = Cow::create(animalPosition);
        animalManager->addAnimal(cow, animalPosition);
    }
    else if (itemName == "chicken") {
        Chicken* chicken = Chicken::create(animalPosition);
        animalManager->addAnimal(chicken, animalPosition);
    }
    else if (itemName == "sheep") {
        Sheep* sheep = Sheep::create(animalPosition);
        animalManager->addAnimal(sheep,animalPosition);
    }
    else if(itemName=="pig") {
        Pig* pig = Pig::create(animalPosition);
        animalManager->addAnimal(pig,animalPosition);
    }
    else {
        return;
    }
}

// �����ӵĽ���
void FarmMap::interactWithMold(std::string itemName, const int& x, const int& y) {
    // �����г�ϵͳ���н���
    Player* player = Player::getInstance();
    MarketState* marketState = MarketState::create();

    // �г�����
    int getMoney = marketState->sellingGoods(itemName);
    player->addMoney(getMoney);
}

// ��ʯͷ�Ľ���
void FarmMap::interactWithStone(std::string itemName, const int& x, const int& y) {
    // �����û�ʯͷ
    if (itemName == "pickaxe") {
        // �û�ʯͷ
        dynamic_cast<Stone*>(mapTileNode[x][y])->knockRock();

        // �ж�ʯͷ�Ƿ񱻻���
        if (dynamic_cast<Stone*>(mapTileNode[x][y])->isBroken()) {
            delete mapTileNode[x][y];
            mapTileNode[x][y] = nullptr;
            mapTileNode[x][y] = Soil::create(Vec2(x, y));

            // ���µ�ͼͼ��
            stoneLayer->setTileGID(0, Vec2(x, y));
            soilLayer->setTileGID(SOIL_GID, Vec2(x, y));


            //�ɿ��������
            auto bag = Bag::getInstance();
            srand(static_cast<unsigned int>(time(NULL)));
            auto skills = SkillTree::getInstance()->getAllSkills();
            int level = skills["Mining"]->getCurrentLevel();
            int random = level * rand() % 100;  // �������
            StoneItem* stoneItem = StoneItem::create(STONE_ITEM);
            // ���ʻ�ȡ�ڶ���ʯͷ
            if (random > 50) {
                SkillTree::getInstance()->updateMiningCount(1);
                bag->addItem(stoneItem);
            }
            SkillTree::getInstance()->updateMiningCount(1);
            bag->addItem(stoneItem);
        }
    }
}

// ��ͼʱ�����
void FarmMap::mapUpdateByTime() {
    // ��������
    // ֲ������
    // ��ȡ�ߵ��ͼ�ĳ���
    int width = tiledMap->getMapSize().width;
    int height = tiledMap->getMapSize().height;
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            if (mapTileNode[x][y]->getTileType() == TileType::Soil) {
                dynamic_cast<Soil*>(mapTileNode[x][y])->updateByTime();
                int soilGID = dynamic_cast<Soil*>(mapTileNode[x][y])->getCurrentGID();
                soilLayer->setTileGID(soilGID, Vec2(x, y));
            }
        }
    }
}