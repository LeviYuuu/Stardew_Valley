#include "FarmMap.h"
#include "Cow.h"
#include "Chicken.h"
#include "Sheep.h"
#include "Pig.h"
#include "../Player/Player.h"
#include<string>

USING_NS_CC;

// 初始化静态实例
FarmMap* FarmMap::instance = nullptr;

TileNode* FarmMap::getTileNode(const int x, const int y) const {
    return mapTileNode[x][y];
}

FarmMap::FarmMap() : velocity(Vec2::ZERO) {}

FarmMap::~FarmMap() {
    instance = nullptr;
    animalManager=nullptr;
}

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
    if (!Node::init()) {
        return false;
    }

    velocity = Vec2::ZERO;

        // 如果传入了文件名,加载地图
    if (!tmxFile.empty()) {
        tiledMap = TMXTiledMap::create(tmxFile);
        if (!tiledMap) {
            return false;
        }
    }
    addChild(tiledMap);

    const Size farmMapSize = getMapSize();
    setPosition(WINSIZE.width / 2 - farmMapSize.width / 2, WINSIZE.height / 2 - farmMapSize.height / 2);

    // 获取地图的各个图层
    grassLayer = tiledMap->getLayer("Grass");
    soilLayer = tiledMap->getLayer("Soil");
    obstacleLayer = tiledMap->getLayer("Obstacles");
    moldLayer = tiledMap->getLayer("Mold");
    waterLayer = tiledMap->getLayer("Water");
    stoneLayer = tiledMap->getLayer("Stone");

    animalManager = AnimalManager::create();
    addChild(animalManager);

    // 获取瓦点地图的长宽
    int width = tiledMap->getMapSize().width;
    int height = tiledMap->getMapSize().height;
        
    // 遍历图层为每一个图层
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (stoneLayer->getTileGIDAt(Vec2(x, y))) {
                mapTileNode[x][y] = new Stone(Vec2(x,y));
            }
            else if (waterLayer->getTileGIDAt(Vec2(x, y))) {
                mapTileNode[x][y] = new Water(Vec2(x,y));
            }
            else if (moldLayer->getTileGIDAt(Vec2(x, y))) {
                mapTileNode[x][y] = new Mold(Vec2(x,y));
            }
            else if(obstacleLayer->getTileGIDAt(Vec2(x,y))){
                mapTileNode[x][y] = new Obstacle(Vec2(x, y));
            }
            else if (soilLayer->getTileGIDAt(Vec2(x, y))) {
                mapTileNode[x][y] = new Soil(Vec2(x, y));
            }
            else {
                int GID = grassLayer->getTileGIDAt(Vec2(x, y));
                mapTileNode[x][y] = new Grass(Vec2(x, y),GID);
            }
        }
    }

    // 这个lambda函数会在FarmMap的生存期内每dt时间调用一次
    schedule([this](float dt) {

        // 检查目标位置是否是障碍物
        auto player = Player::getInstance();

        // 人在地图坐标中下一步会到达的位置
        // 这里的- velocity / 200.0f * 10.0f是预测下一步的位置
        // velocity / 200.0f是因为velocity的绝对值是200
        Vec2 playerSize2 = Vec2(0.0f, player->getContentSize().height * 1.0f);

        auto playerPositionInMap = player->getPosition() - getPosition() - playerSize2 * 0.5f + player->getVelocity() / MAP_MOVE_SPEED * 10.0f;
        if (isCollidable(playerPositionInMap)) {
             velocity = Vec2::ZERO;
        }
        auto position = getPosition() + velocity * dt;
        // max保证大于等于下界， min保证小于等于上界
        position.x = std::max(WINSIZE.width - getMapSize().width, std::min(position.x, 0.0f));
        position.y = std::max(WINSIZE.height - getMapSize().height, std::min(position.y, 0.0f));

        setPosition(position);
        }, "farm_map");
    return true;
}

bool FarmMap::npcInit(const Vec2& position, Npc* npc) {
    npc->setPosition(position);
    addChild(npc, 5);
     return true;
}

void FarmMap::moveMapByDirection(const Vec2& direction) {
    velocity = direction * MAP_MOVE_SPEED;
}

const Size& FarmMap::getMapSize() const {
    return tiledMap->getContentSize();
}

// 碰撞检测：检查给定位置是否是障碍物, positon是人物在地图坐标系（原点在左下角）中的坐标
bool FarmMap::isCollidable(const Vec2& position) const {
    // 获取地图的瓦片大小
    const Size tileSize = tiledMap->getTileSize();
    const Size mapSize = tiledMap->getMapSize();

    // 将人物在地图中的坐标转换为瓦片地图坐标（原点在左上角）
    int x = position.x / tileSize.width;
    int y = (mapSize.height * tileSize.height - position.y) / tileSize.height;

    // 检查是否在地图范围内
    if (x < 0 || x >= mapSize.width || y < 0 || y >= mapSize.height) {
        return true; // 超出地图范围视为障碍物
    }

    // 碰撞检测
    TileType tileType = mapTileNode[x][y]->getTileType();
    if (tileType == TileType::Stone || tileType == TileType::Obstacle || tileType == TileType::Water||tileType == TileType::Mold) {
        return true;
    }
    else {
        return false;
    }
}

    // 玩家与农场的接口
void FarmMap::interactWithFarmMap() {
    // 获取玩家、地图、背包实例
    Player* player = Player::getInstance();
    FarmMap* farmMap = FarmMap::getInstance();
    Bag* bag = Bag::getInstance();

    if (!player->getUseItemEnable()) {
        return;
    }

    // 获取要交互的土块位置
    Vec2 playerPosition = player->getPosition();
    const Size tileSize = farmMap->tiledMap->getTileSize();
    const Size mapSize = farmMap->tiledMap->getMapSize();
    playerPosition = playerPosition - farmMap->getPosition();
    playerPosition.y = playerPosition.y - player->getContentSize().height / 2 + 10.0f;
    playerPosition.x = playerPosition.x / tileSize.width;
    playerPosition.y = (mapSize.height * tileSize.height - playerPosition.y) / tileSize.height;
    const Vec2 lastDirection = player->getLastDirection();

    // 当前人物前面的瓦点
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

    // 获得当前人物所使用的工具
    std::string currentItemName = player->getCurrentItemName();

        // 与地图块的交互
        if (mapTileNode[x][y]->getTileType() == TileType::Stone) {                                      // 当前是石头层
            if (currentItemName == "pickaxe") {
                dynamic_cast<Stone*>(mapTileNode[x][y])->knockRock();                                   // 敲击一次石头
                if (dynamic_cast<Stone*>(mapTileNode[x][y])->isBroken() == true) {                      // 判断石头是否击碎
                    delete mapTileNode[x][y];
                    mapTileNode[x][y] = nullptr;
                    mapTileNode[x][y] = new Soil(Vec2(x, y));
                    stoneLayer->setTileGID(0, Vec2(x, y));
                    soilLayer->setTileGID(mapTileNode[x][y]->getCurrentGID(), Vec2(x, y));
                }
            }
        }
        else if (mapTileNode[x][y]->getTileType() == TileType::Water) {  // 当前交互的是水层
            if (currentItemName == "kettle") {
                int wateringCanIndex = bag->getItemIndex("kettle");
                int waterShortageAmount = MAX_WATERINGCAN_CAPACITY - dynamic_cast<Kettle*>(bag->getItem(wateringCanIndex))->getCurrentWaterLevel();
                if (waterShortageAmount <= dynamic_cast<Water*>(mapTileNode[x][y])->getCurrentWaterResource()) {
                    dynamic_cast<Kettle*>(bag->getItem(wateringCanIndex))->refillWateringCan(waterShortageAmount);
                    dynamic_cast<Water*>(mapTileNode[x][y])->pumpWater(waterShortageAmount);
                }
                else {
                    dynamic_cast<Kettle*>(bag->getItem(wateringCanIndex))->refillWateringCan(dynamic_cast<Kettle*>(bag->getItem(wateringCanIndex))->getCurrentWaterLevel());
                    dynamic_cast<Water*>(mapTileNode[x][y])->pumpWater(dynamic_cast<Kettle*>(bag->getItem(wateringCanIndex))->getCurrentWaterLevel());
                }
            }
        }
        else if (mapTileNode[x][y]->getTileType() == TileType::Soil) {   // 当前交互的是土壤层
            if (currentItemName == "hoe") {
                dynamic_cast<Soil*>(mapTileNode[x][y])->hoe();
            }
            else if (currentItemName == "kettle") {
                dynamic_cast<Soil*>(mapTileNode[x][y])->water();
            }
            else if (currentItemName == "appleSeed" || currentItemName == "cornSeed" || currentItemName == "carrotSeed") {
                dynamic_cast<Soil*>(mapTileNode[x][y])->plantCrop(currentItemName);
            }
            else if (currentItemName == "fertilize") {
                dynamic_cast<Soil*>(mapTileNode[x][y])->fertilize();
            }
            else if (currentItemName == "scythe") {
                // 有待完善 这里背包的物品需要去添加
                dynamic_cast<Soil*>(mapTileNode[x][y])->harvest();
            }
            dynamic_cast<Soil*>(mapTileNode[x][y])->gidUpdateByEvent();
            // 更新图块
            soilLayer->setTileGID(mapTileNode[x][y]->getCurrentGID(), Vec2(x, y));
        
        }
        else if (mapTileNode[x][y]->getTileType() == TileType::Mold) {

    }
    else if (mapTileNode[x][y]->getTileType() == TileType::Obstacle) {

    }
    else{                                                               // 当前交互的是草地
        // 将瓦点地图的坐标转化
        Vec2 animalPosition;
        animalPosition.x = x * tileSize.width;
        animalPosition.y = mapSize.height * tileSize.height - tileSize.height * y;

        // 下面是养殖逻辑
        if (currentItemName == "cow") {
            Animal* cow = Cow::create(animalPosition);
            animalManager->addAnimal(cow,animalPosition);
        }
        else if (currentItemName == "chicken") {
            Animal* chicken = Chicken::create(animalPosition);
            animalManager->addAnimal(chicken, animalPosition);
        }
        else if (currentItemName == "sheep") {
            Animal* sheep = Sheep::create(animalPosition);
            animalManager->addAnimal(sheep, animalPosition);
        }
        else if (currentItemName == "pig") {
            Animal* pig = Pig::create(animalPosition);
            animalManager->addAnimal(pig, animalPosition);
        }
        else {
        }
    }
}

void FarmMap::stopMoving() {
    velocity = Vec2::ZERO;
}

// 地图随时间的更新
void FarmMap::farmMapUpdateByTime() {

    // 先完成作物随着时间的增长
    for (int x = 0; x < FARMMAP_WIDTH; x++) {
        for (int y = 0; y < FARMMAP_HEIGHT; y++) {
            if (mapTileNode[x][y]->getTileType() == TileType::Soil) {
                dynamic_cast<Soil*>(mapTileNode[x][y])->gidUpdateByTime();
                soilLayer->setTileGID(mapTileNode[x][y]->getCurrentGID(),Vec2(x,y));
            }
        }
    }
}

TMXTiledMap* FarmMap::getTiledMap() {
    return tiledMap;
}