#include "MenuECS.h"

#define DIALOGUE_INTERVAL 4.0

using namespace std;
using namespace cocos2d;
using namespace DeathMetal::DeathMetalComponent;
using namespace DeathMetal::MenuComponent;

namespace DeathMetal {
    
    string workshopHint = "You need to craft a war machine in your workshop first!";
    string settingsHint = "How naive... You think we had time for settings in a game rushed in 10 days?";
    vector<string> dialogues = {
        "The mutant cultists are coming. We must defend the workshop. Get ready to craft my... I mean your war machine!",
        "The cultists are attracted to your vehicle or the workshop, whichever is closer, so try to draw them away from here!",
        "Remember, it takes 4 machine gun bullets to kill a mutant.",
        "The vehicle has a built-in auto aiming system, so all you gotta do is drive.",
        "The machine gun is a long range but low damage weapon.",
        "The blow torch is a mid range and mid damage weapon.",
        "Using the circular saw up close is extremely dangerous, for both you and the mutants. Try to get close to the mutants, but not too close!",
        "I have not bathed for... since before the great war?",
        "Use the arrow keys to drive! That's all you need to know!"
    };
    
    MenuECS::MenuECS(cocos2d::Scene* scene) {
        this->scene = scene;
        
        systems.add<MenuSystem::MenuSystem>(scene);
        systems.add<DeathMetalSystem::RenderSystem>(scene);
        systems.configure();
    }
    
    void MenuECS::update(ex::TimeDelta dt) {
        systems.update_all(dt);
    }
    
    void MenuSystem::MenuSystem::configure(ex::EntityManager &entities, ex::EventManager &event_manager) {
        
        DeathMetalData* deathMetalData = new DeathMetalData;
        deathMetalData->chassis = -1;
        deathMetalData->construct = new unordered_set<int>;
        data = deathMetalData;

        auto screenSize = Director::getInstance()->getVisibleSize();
        
        auto background = new Sprite();
        background->initWithFile("desert.png");
        background->setPosition(screenSize.width/2, screenSize.height/2);
        background->setScale(screenSize.width / background->getContentSize().width);
        scene->addChild(background, INT_MIN);
        
        mentorEntityId = DeathMetalEntity::createSpriteEntity(entities, "mentor.png", Vec2(screenSize.width / 3, 0.), screenSize.width / 3).id();
        
        auto seq = Sequence::create(MoveBy::create(0.5, Vec2(-UNIT, 0)), MoveBy::create(0.5, Vec2(UNIT, 0)), nullptr);
        seq->retain();
        Action* wiggle = RepeatForever::create(seq);
        entities.get(mentorEntityId).component<TransformComponent>()->sprite->runAction(wiggle);
        
        
        auto dialogueTextSize = Size(screenSize.width / 3 - UNIT, screenSize.height / 2);
        dialogueEntityId = DeathMetalEntity::createLabelEntity(entities, dialogues[0], "Krungthep", 18, Color4B::BLACK, dialogueTextSize, Vec2(screenSize.width * 2 / 3, 0)).id();
        DeathMetalEntity::createSpriteEntity(entities, "speech_balloon.png", Vec2(screenSize.width * 2 / 3, 0), screenSize.width / 3);
        DeathMetalEntity::createLabelEntity(entities, "\"DEATH\nMETAL\"", "Krungthep", 100, Color4B::BLACK, dialogueTextSize, Vec2(screenSize.width * 2 / 3, screenSize.height / 2));
        
        Vector<MenuItem*> menuItems;
        
        auto battleItem = MenuItemImage::create("Button_Up_Battle.png", "Button_Down_Battle.png", [&](Ref* sender) {
            if (data->chassis >= 0) {
                auto battleScene = BattleScene::create();
                battleScene->initECS(data);
                Director::getInstance()->pushScene(battleScene);
            }
            else
                hint(entities, workshopHint);
        });
        menuItems.pushBack(battleItem);
        
        auto workshopItem = MenuItemImage::create("Button_Up_Workshop.png", "Button_Down_Workshop.png", [&](Ref* sender) {
            auto workshopScene = WorkshopScene::create();
            workshopScene->initECS(data);
            Director::getInstance()->pushScene(TransitionSlideInB::create( 0.5, workshopScene));
        });
        menuItems.pushBack(workshopItem);
        
        auto settingsItem = MenuItemImage::create("Button_Up_Settings.png", "Button_Down_Settings.png", [&](Ref* sender) {
            hint(entities, settingsHint);
        });
        menuItems.pushBack(settingsItem);
        
        auto quitItem = MenuItemImage::create("Button_Up_Quit.png", "Button_Down_Quit.png", [&](Ref* sender) {
            Director::getInstance()->end();
            exit(0);
        });
        menuItems.pushBack(quitItem);
        
        auto menu = Menu::createWithArray(menuItems);
        menu->alignItemsVertically();
        menu->setScale(1.5);
        menu->setAnchorPoint(Vec2(0., 0.));
        menu->setPosition(Vec2(screenSize.width / 6, screenSize.height / 2));
        scene->addChild(menu);
    }
    
    void MenuSystem::MenuSystem::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
        if (dialogueInterval > DIALOGUE_INTERVAL) {
            string dialogue = dialogues[RandomHelper::random_int(0, (int)dialogues.size() - 1)];
            es.get(dialogueEntityId).component<TransformComponent>()->label->setString(dialogue);
            dialogueInterval = 0.;
        }
        dialogueInterval += dt;
    }
    
    void MenuSystem::MenuSystem::hint(ex::EntityManager &es, string hintText) {
        auto dialogueLabel = es.get(dialogueEntityId).component<TransformComponent>()->label;
        dialogueLabel->setString(hintText);
        dialogueInterval = 0.0;
    }
}
