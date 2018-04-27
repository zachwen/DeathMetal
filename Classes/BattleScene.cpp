#include "BattleScene.h"
#include "DeathMetal.h"

USING_NS_CC;

Scene* BattleScene::createScene() {
    return BattleScene::create();
}

bool BattleScene::init() {
    if ( !Scene::init() ) {
        return false;
    }
    ecs = new BattleECS(this);
    
    auto listener = EventListenerKeyboard::create();
    listener->onKeyPressed = CC_CALLBACK_2(BattleScene::onKeyPressed, this);
    listener->onKeyReleased = CC_CALLBACK_2(BattleScene::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    this->scheduleUpdateWithPriority(42);
    return 1;
}

void BattleScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) {
    ecs->events.emit<DeathMetal::DeathMetalEvent::OnKeyPressed>(keyCode, event);
}

void BattleScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) {
    ecs->events.emit<DeathMetal::DeathMetalEvent::OnKeyReleased>(keyCode, event);
}

void BattleScene::update(float delta) {
    ecs->update((double)delta);
}
