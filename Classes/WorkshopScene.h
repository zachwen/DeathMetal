#ifndef __WORKSHOP_SCENE_H__
#define __WORKSHOP_SCENE_H__

#include "cocos2d.h"
#include "entityx/entityx.h"
#include "WorkshopECS.h"

using namespace DeathMetal;

class WorkshopScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    static cocos2d::Scene* createScene(void* data);
    virtual bool init() override;
    void update(float delta) override;
    void initECS(DeathMetalData* data);
    CREATE_FUNC(WorkshopScene);
    
private:
    WorkshopECS* ecs;
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
};

#endif
