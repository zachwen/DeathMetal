#include "MenuScene.h"
#include "DeathMetal.h"

USING_NS_CC;

Scene* MenuScene::createScene() {
    return MenuScene::create();
}

bool MenuScene::init() {
    if ( !Scene::init() ) {
        return false;
    }
    ecs = new MenuECS(this);
    
    auto listener = EventListenerKeyboard::create();
    listener->onKeyPressed = CC_CALLBACK_2(MenuScene::onKeyPressed, this);
    listener->onKeyReleased = CC_CALLBACK_2(MenuScene::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    this->scheduleUpdateWithPriority(42);
    return 1;
}

void MenuScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) {
    ecs->events.emit<DeathMetal::DeathMetalEvent::OnKeyPressed>(keyCode, event);
}

void MenuScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) {
    ecs->events.emit<DeathMetal::DeathMetalEvent::OnKeyReleased>(keyCode, event);
}

void MenuScene::update(float delta) {
    ecs->update((double)delta);
}
