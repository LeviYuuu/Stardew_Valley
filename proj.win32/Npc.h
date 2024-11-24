#ifndef NPC_H
#define NPC_H

#include <string>
#include <vector>
#include <map>

enum class RelationshipStatus {
    None,       // 陌生
    Friendship, // 友谊
    Romance     // 浪漫
};

class Npc {
private:
    std::string name;                   // NPC的名字
    std::string birthday;               // NPC的生日
    int friendshipLevel;                // NPC与玩家的亲密度（0-100）
    std::vector<std::string> gifts;     // NPC喜欢的礼物
    std::vector<std::string> dislikes;  // NPC讨厌的礼物
    std::vector<std::string> dialogues; // NPC的对话列表
    RelationshipStatus playerRelation;  // 与玩家的关系（None, Friendship, Romance）
    std::map<std::string, RelationshipStatus> npcRelations; // NPC之间的关系

public:
    // 构造函数
    Npc(std::string name, std::string birthday,
        std::vector<std::string> gifts,
        std::vector<std::string> dislikes,
        std::vector<std::string> dialogues);

    // 获取NPC的名字
    std::string getName() const;

    // 获取NPC的生日
    std::string getBirthday() const;

    // 获取NPC的亲密度
    int getFriendshipLevel() const;

    // 设置NPC的亲密度
    void setFriendshipLevel(int level);

    // 与NPC互动
    void interactWithPlayer();

    // 给NPC送礼物
    void giveGift(const std::string& gift);

    // 判断NPC是否喜欢某个礼物
    bool likesGift(const std::string& gift) const;

    // 判断NPC是否讨厌某个礼物
    bool dislikesGift(const std::string& gift) const;

    // 增加NPC的亲密度
    void increaseFriendship(int amount);

    // 减少NPC的亲密度
    void decreaseFriendship(int amount);

    // 获取NPC的所有喜欢的礼物
    std::vector<std::string> getGifts() const;

    // 获取NPC的所有讨厌的礼物
    std::vector<std::string> getDislikes() const;

    // 设置NPC的喜欢的礼物
    void setGifts(const std::vector<std::string>& giftList);

    // 设置NPC的讨厌的礼物
    void setDislikes(const std::vector<std::string>& dislikeList);

    // 获取NPC的对话
    void printDialogue() const;

    // 获取NPC与玩家的关系状态
    RelationshipStatus getPlayerRelation() const;

    // 设置与玩家的关系
    void setPlayerRelation(RelationshipStatus status);

    // 设置NPC之间的关系
    void setNpcRelation(const std::string& npcName, RelationshipStatus status);

    // 获取NPC与另一NPC的关系
    RelationshipStatus getNpcRelation(const std::string& npcName) const;

    // 输出NPC的当前状态
    void printStatus() const;

    // 询问NPC之间的关系
    void askAboutNocRelation(const std::string& otherNpcName) const;
};

#endif // NPC_H
