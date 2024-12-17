#include "FarmMap.h"
#include "../Player/Player.h"
#include "../Constant/Constant.h"
#include "AnimalManager.h"
#include "Cow.h"
#include "Chicken.h"
#include "Sheep.h"
#include "Pig.h"
#include<string>

USING_NS_CC;

// 初始化静态实例
FarmMap* FarmMap::instance = nullptr;

TileNode* FarmMap::getTileNode(int x, int y)
{
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
        map = TMXTiledMap::create(tmxFile);
        if (!map) {
            return false;
    }
    addChild(map);

    const auto farmMapSize = getMapSize();
    setPosition(WINSIZE.width / 2 - farmMapSize.width / 2, WINSIZE.height / 2 - farmMapSize.height / 2);

    // 获取地图的各个图层
    grassLayer = map->getLayer("Grass");
    soilLayer = map->getLayer("Soil");
    obstacleLayer = map->getLayer("Obstacles");
    moldLayer = map->getLayer("Mold");
    waterLayer = map->getLayer("Water");
    stoneLayer = map->getLayer("Stone");

    // 添加动物管理
    animalManager = AnimalManager::create();
    addChild(animalManager);

    // 获取瓦点地图的长宽
    int width = map->getMapSize().width;
    int height = map->getMapSize().height;
        
    // 遍历图层为每一个图层
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
    }
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

Size FarmMap::getMapSize() const {
    return map->getContentSize();
}

// 碰撞检测：检查给定位置是否是障碍物, positon是人物在地图坐标系（原点在左下角）中的坐标
bool FarmMap::isCollidable(const Vec2& position) const {
    if (!obstacleLayer) {
        return false;
    }

    // 获取地图的瓦片大小
    const Size tileSize = map->getTileSize();
    const Size mapSize = map->getMapSize();

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

    // 如果是特定的工具的话，我们就是和动物交互

    // 获取要交互的土块位置
    Vec2 playerPosition = player->getPosition();
    const Size tileSize = farmMap->map->getTileSize();
    const Size mapSize = farmMap->map->getMapSize();
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
    
    if (mapTileNode[x][y]->getTileType() == TileType::Soil) {
        interactWithSoil(currentItemName,x,y);
    }
    else if (mapTileNode[x][y]->getTileType() == TileType::Grass) {
        interactWithGrass(currentItemName, x, y);
    }
    else if (mapTileNode[x][y]->getTileType() == TileType::Water) {
        interactWithWater(currentItemName, x, y);
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

// 与土壤的交互
void FarmMap::interactWithSoil(std::string itemName,const int& x,const int& y) {
    // 根据工具去分类
    if (itemName == "hoe") {
        dynamic_cast<Soil*>(mapTileNode[x][y])->soilHoe();
    }
    else if (itemName == "fertilizer") {
        dynamic_cast<Soil*>(mapTileNode[x][y])->soilFertilize();
    }
    else if (itemName == "kettle") {
        dynamic_cast<Soil*>(mapTileNode[x][y])->soilWater();
    }
    else if (itemName == "dogbaneSeed" || itemName == "cornSeed" || itemName == "carrotSeed") {
        dynamic_cast<Soil*>(mapTileNode[x][y])->plantCrop(itemName);
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

    // 操作之后更新soil土壤块
    dynamic_cast<Soil*>(mapTileNode[x][y])->updateGID();
    int soilGID = dynamic_cast<Soil*>(mapTileNode[x][y])->getCurrentGID();
    soilLayer->setTileGID(soilGID,Vec2(x,y));
}

// 与草地的交互
void FarmMap::interactWithGrass(std::string itemName, const int& x, const int& y) {

}

// 与水的交互
void FarmMap::interactWithWater(std::string itemName,const int& x,const int& y) {

}

// 与箱子的交互
void FarmMap::interactWithMold(std::string itemName, const int& x, const int& y) {

}

// 与石头的交互
void FarmMap::interactWithStone(std::string itemName, const int& x, const int& y) {
    // 镐子敲击石头
    if (itemName == "pickaxe") {
        dynamic_cast<Stone*>(mapTileNode[x][y])->knockRock();

        // 判断石头是否被击碎
        if (dynamic_cast<Stone*>(mapTileNode[x][y])->isBroken()) {
            delete mapTileNode[x][y];
            mapTileNode[x][y] = nullptr;
            mapTileNode[x][y] = Soil::create(Vec2(x,y));

            // 更新地图图块
            stoneLayer->setTileGID(0, Vec2(x, y));
            soilLayer->setTileGID(SOIL_GID, Vec2(x, y));
        }
    }
}

// 停止移动
void FarmMap::stopMoving() {
    velocity = Vec2::ZERO;
}

// 地图时间更新
void FarmMap::farmMapTimeUpdate() {
    // 动物生长

    // 植物生长
    // 获取瓦点地图的长宽
    int width = map->getMapSize().width;
    int height = map->getMapSize().height;
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