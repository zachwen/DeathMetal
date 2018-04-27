#include "IntroductionScene.h"
#include "DeathMetal.h"

USING_NS_CC;

Scene* IntroductionScene::createScene() {
    return IntroductionScene::create();
}

bool IntroductionScene::init() {
    if ( !Scene::init() ) {
        return false;
    }
    ecs = new IntroductionECS(this);
    
    auto listener = EventListenerKeyboard::create();
    listener->onKeyReleased = CC_CALLBACK_2(IntroductionScene::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    this->scheduleUpdateWithPriority(42);
    return 1;
}

void IntroductionScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) {
    ecs->events.emit<DeathMetal::DeathMetalEvent::OnKeyReleased>(keyCode, event);
}

void IntroductionScene::update(float delta) {
    ecs->update((double)delta);
}
