#include "WorkshopScene.h"
#include "DeathMetal.h"

USING_NS_CC;

Scene* WorkshopScene::createScene() {
    return WorkshopScene::create();
}

bool WorkshopScene::init() {
    if ( !Scene::init() ) {
        return false;
    }
    
    auto listener = EventListenerKeyboard::create();
    listener->onKeyPressed = CC_CALLBACK_2(WorkshopScene::onKeyPressed, this);
    listener->onKeyReleased = CC_CALLBACK_2(WorkshopScene::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    this->scheduleUpdateWithPriority(42);
    return 1;
}

void WorkshopScene::initECS(DeathMetalData* data) {
    this->setUserData(data);
    ecs = new WorkshopECS(this);
}

void WorkshopScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) {
    ecs->events.emit<DeathMetal::DeathMetalEvent::OnKeyPressed>(keyCode, event);
}

void WorkshopScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) {
    ecs->events.emit<DeathMetal::DeathMetalEvent::OnKeyReleased>(keyCode, event);
}

void WorkshopScene::update(float delta) {
    ecs->update((double)delta);
}
