#ifndef __BATTLE_SCENE_H__
#define __BATTLE_SCENE_H__

#include "cocos2d.h"
#include "entityx/entityx.h"
#include "BattleECS.h"

using namespace DeathMetal;

class BattleScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init() override;
    void update(float delta) override;
    CREATE_FUNC(BattleScene);
private:
    BattleECS* ecs;
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
};

#endif
