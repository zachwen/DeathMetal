#include "IntroductionECS.h"
#include "MenuScene.h"
#include "entityx/deps/Dependencies.h"
#include "2d/CCFontAtlasCache.h"
#include "2d/CCFontFreeType.h"

#define SCROLL_SPEED 1.2

using namespace std;
using namespace cocos2d;
using namespace CocosDenshion;
using namespace DeathMetal::DeathMetalComponent;

namespace DeathMetal {
    
    IntroductionECS::IntroductionECS(cocos2d::Scene* scene) {
        this->scene = scene;
        systems.add<IntroductionSystem::IntroductionSystem>(scene);
        systems.add<DeathMetalSystem::RenderSystem>(scene);
        systems.configure();
    }
    
    void IntroductionECS::update(ex::TimeDelta dt) {
        systems.update_all(dt);
    }
    
    void IntroductionSystem::IntroductionSystem::configure(ex::EntityManager &entities, ex::EventManager &event_manager) {
        event_manager.subscribe<DeathMetalEvent::OnKeyReleased>(*this);
        
        string introduction = "The year is A.D. 2098.\n\nWith scientific development of controlled nuclear fusion and strong artificial intelligence all failed miserably, a global war erupted among nations desperate for the scarce remaining resources. Large-scale nuclear warfare had turned metropolis and countryside alike into wastelands…\n\nIn this barren land in the heart of once proud United States, mutant cultists and cannibals run rampant, terrorizing the only remaining decent folks. You, the last and greatest symbol of human dignity, shall cleanse the land with your workshop-crafted war machines…\n\nYou are the operator of...\n\nDEATH METAL";
        
        auto screenSize = Director::getInstance()->getVisibleSize();
        
        auto background = new Sprite();
        background->initWithFile("desert.png");
        background->setPosition(screenSize.width/2, screenSize.height/2);
        background->setScale(screenSize.width / background->getContentSize().width);
        scene->addChild(background);
        
        auto blockSize = Size(screenSize.width * 3 / 4, screenSize.height * 2);
        DeathMetalEntity::createLabelEntity(entities, introduction, "Krungthep", 40., Color4B::WHITE, blockSize, Vec2(screenSize.width / 8, - 2 * screenSize.height));
        
        auto audio = SimpleAudioEngine::getInstance();
        audio->playBackgroundMusic("Optimus_Prim_-_02_-_Dont_you_think.mp3", true);
    }
    
    void IntroductionSystem::IntroductionSystem::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
        es.each<RenderComponent, TransformComponent>([&](ex::Entity entity, RenderComponent renderComp, TransformComponent transComp) {
            auto label = transComp.label;
            auto pos = label->getPosition();
            if (pos.y < Director::getInstance()->getVisibleSize().height)
                label->setPosition(pos.x, pos.y + SCROLL_SPEED);
            else {
                auto menuScene = MenuScene::create();
                Director::getInstance()->replaceScene(menuScene);
            }
        });
    }
    
    void IntroductionSystem::IntroductionSystem::receive(const DeathMetalEvent::OnKeyReleased& onKeyReleased) {
        // TODO: transition to menu scene
        auto menuScene = MenuScene::create();
        Director::getInstance()->replaceScene(menuScene);
    }
}
