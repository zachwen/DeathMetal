#ifndef __INTRODUCTION_SCENE_H__
#define __INTRODUCTION_SCENE_H__

#include "cocos2d.h"
#include "entityx/entityx.h"
#include "IntroductionECS.h"

using namespace DeathMetal;

class IntroductionScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init() override;
    void update(float delta) override;
    CREATE_FUNC(IntroductionScene);
private:
    IntroductionECS* ecs;
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
};

#endif
