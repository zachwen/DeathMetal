#include "WorkshopECS.h"
#include "2d/CCFontAtlasCache.h"
#include "2d/CCFontFreeType.h"

#define CAPACITY 2
#define HINT_RESET_INTERVAL 5.0

using namespace std;
using namespace cocos2d;
using namespace CocosDenshion;
using namespace DeathMetal::DeathMetalComponent;
using namespace DeathMetal::WorkshopComponent;

namespace DeathMetal {
    
    WorkshopECS::WorkshopECS(cocos2d::Scene* scene) {
        this->scene = scene;
        systems.add<WorkshopSystem::WorkshopSystem>(scene);
        systems.add<DeathMetalSystem::RenderSystem>(scene);
        systems.configure();
    }
    
    void WorkshopECS::update(ex::TimeDelta dt) {
        systems.update_all(dt);
    }
    
    void WorkshopSystem::WorkshopSystem::configure(ex::EntityManager &entities, ex::EventManager &event_manager) {
        event_manager.subscribe<DeathMetalEvent::OnKeyPressed>(*this);
        event_manager.subscribe<DeathMetalEvent::OnKeyReleased>(*this);
        
        data = (DeathMetalData*)(scene->getUserData());
        editingData = new DeathMetalData;
        editingData->chassis = data->chassis >= 0 ? data->chassis : 0;
        editingData->construct = new unordered_set<int>(*(data->construct));
        
        auto screenSize = Director::getInstance()->getVisibleSize();
        int tileWidth = screenSize.width / UNIT + 1;
        int tileHeight = screenSize.height / UNIT + 1;
        for (int i = 0; i < tileWidth; i++)
            for (int j = 0; j < tileHeight; j++)
                DeathMetalEntity::createTileEntity(entities, i, j, 2);
        
        auto background = new Sprite();
        background->initWithFile("desert.png"); // TODO: replace with workshop background
        background->setPosition(screenSize.width/2, screenSize.height/2);
        background->setScale(screenSize.width / background->getContentSize().width);
        scene->addChild(background, INT_MIN);
        
        vehicleConstructEntityId = DeathMetalEntity::createSpriteEntity(entities, "hero.png", Vec2(screenSize.width / 2 - screenSize.width / 6, screenSize.height / 3), screenSize.width / 3).id();
        updateVehicleConstruct(entities);
        
        auto blockSize = Size(screenSize.width, screenSize.height / 8);
        hintLabelEntityId = DeathMetalEntity::createLabelEntity(entities, "Add parts to your war machine via the interface on your left.", "Krungthep", 20, Color4B::BLACK, blockSize, Vec2(0, screenSize.height / 8)).id();
        
        // finish editing
        auto finishItem = MenuItemImage::create("Button_Up_Finish.png", "Button_Down_Finish.png", [&](Ref* sender) {
            if (editingData->construct->empty())
                hint(entities, "At least one part is required!");
            else {
                save();
                auto audio = SimpleAudioEngine::getInstance();
                audio->stopBackgroundMusic();
                audio->playBackgroundMusic("Optimus_Prim_-_02_-_Dont_you_think.mp3", true);
                Director::getInstance()->popScene();
            }
        });
        auto finishMenu = Menu::createWithItem(finishItem);
        finishMenu->alignItemsVertically();
        finishMenu->setScale(1.5);
        finishMenu->setAnchorPoint(Vec2(0., 0.));
        finishMenu->setPosition(screenSize.width * 5 / 6, screenSize.height / 8);
        scene->addChild(finishMenu);
        
        // workshop interface
        Vector<MenuItem*> menuItems;
        auto machineGunItem = MenuItemImage::create("Button_Up_Bullet.png", "Button_Down_Bullet.png", [&](Ref* sender) {
            if (editingData->construct->count(MACHINE_GUN)) {
                editingData->construct->erase(MACHINE_GUN);
                hint(entities, "Machine gun removed...");
            }
            else {
                if (editingData->construct->size() >= CAPACITY)
                    hint(entities, "No more parts can be added to this chassis... (Maximum: " + to_string(CAPACITY) + ")");
                else {
                    editingData->construct->insert(MACHINE_GUN);
                    hint(entities, "Machine gun added...");
                }
            }
            updateVehicleConstruct(entities);
        });
        menuItems.pushBack(machineGunItem);
        auto circularSawItem = MenuItemImage::create("Button_Up_Saw.png", "Button_Down_Saw.png", [&](Ref* sender) {
            if (editingData->construct->count(CIRCULAR_SAW)) {
                editingData->construct->erase(CIRCULAR_SAW);
                hint(entities, "Circular saw removed...");
            }
            else {
                if (editingData->construct->size() >= CAPACITY)
                    hint(entities, "No more parts can be added to this chassis... (Maximum: " + to_string(CAPACITY) + ")");
                else {
                    editingData->construct->insert(CIRCULAR_SAW);
                    hint(entities, "Circular saw added...");
                }
            }
            updateVehicleConstruct(entities);
        });
        menuItems.pushBack(circularSawItem);
        auto blowTorchItem = MenuItemImage::create("Button_Up_Flame.png", "Button_Down_Flame.png", [&](Ref* sender) {
            if (editingData->construct->count(BLOW_TORCH)) {
                editingData->construct->erase(BLOW_TORCH);
                hint(entities, "Blow torch removed...");
            }
            else {
                if (editingData->construct->size() >= CAPACITY)
                    hint(entities, "No more parts can be added to this chassis... (Maximum: " + to_string(CAPACITY) + ")");
                else {
                    editingData->construct->insert(BLOW_TORCH);
                    hint(entities, "Blow torch added...");
                }
            }
            updateVehicleConstruct(entities);
        });
        menuItems.pushBack(blowTorchItem);
        auto plateArmorItem = MenuItemImage::create("Button_Up_Shield.png", "Button_Down_Shield.png", [&](Ref* sender) {
            if (editingData->construct->count(PLATE_ARMOR)) {
                editingData->construct->erase(PLATE_ARMOR);
                hint(entities, "Plate armor removed...");
            }
            else {
                if (editingData->construct->size() >= CAPACITY)
                    hint(entities, "No more parts can be added to this chassis... (Maximum: " + to_string(CAPACITY) + ")");
                else {
                    editingData->construct->insert(PLATE_ARMOR);
                    hint(entities, "Plate armor added...");
                }
            }
            updateVehicleConstruct(entities);
        });
        menuItems.pushBack(plateArmorItem);
        
        auto menu = Menu::createWithArray(menuItems);
        menu->alignItemsVertically();
        menu->setScale(1.5);
        menu->setAnchorPoint(Vec2(0., 0.));
        menu->setPosition(Vec2(UNIT + screenSize.width / 6, 360));
        scene->addChild(menu);
        
        auto audio = SimpleAudioEngine::getInstance();
        audio->stopBackgroundMusic();
        audio->playBackgroundMusic("Blue_Dot_Sessions_-_01_-_KeoKeo.mp3", true);
    }
    
    void WorkshopSystem::WorkshopSystem::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
        if (hintResetInterval > HINT_RESET_INTERVAL)
            hint(es, "");
        hintResetInterval += dt;
    }
    
    void WorkshopSystem::WorkshopSystem::receive(const DeathMetalEvent::OnKeyPressed& onKeyPressed) {
    }
    
    void WorkshopSystem::WorkshopSystem::receive(const DeathMetalEvent::OnKeyReleased& onKeyReleased) {
    }
    
    void WorkshopSystem::WorkshopSystem::save() {
        data->chassis = editingData->chassis;
        delete data->construct;
        data->construct = editingData->construct;
        delete editingData;
        data->chassis = 0;
    }
    
    void WorkshopSystem::WorkshopSystem::hint(ex::EntityManager &es, std::string hintText) {
        es.get(hintLabelEntityId).component<TransformComponent>()->label->setString(hintText);
        hintResetInterval = 0.;
    }
    
    void WorkshopSystem::WorkshopSystem::updateVehicleConstruct(ex::EntityManager &es) {
        auto screenSize = Director::getInstance()->getVisibleSize();
        // auto chassisSprite = es.get(vehicleConstructEntityId).component<TransformComponent>()->sprite;
        auto chassisSprite = DeathMetalEntity::createSpriteWithFile("hero.png", Vec2(screenSize.width / 2 - screenSize.width / 6, screenSize.height / 3), screenSize.width / 3);
        unordered_map<int, string> partSpriteMap;
        partSpriteMap[MACHINE_GUN] = "machine_gun.png";
        partSpriteMap[CIRCULAR_SAW] = "circular_saw.png";
        partSpriteMap[BLOW_TORCH] = "blow_torch.png";
        partSpriteMap[PLATE_ARMOR] = "plate_armor.png";
        unordered_map<int, int> partPriorityMap;
        partPriorityMap[CIRCULAR_SAW] = 1;
        partPriorityMap[BLOW_TORCH] = 2;
        partPriorityMap[PLATE_ARMOR] = 3;
        partPriorityMap[MACHINE_GUN] = 4;
        for (int part : *(editingData->construct)) {
            auto partSprite = DeathMetalEntity::createSpriteWithFile(partSpriteMap[part], Vec2(4., 0.), 32.);
            chassisSprite->addChild(partSprite, partPriorityMap[part]);
        }
        auto beforeChassis = es.get(vehicleConstructEntityId).component<TransformComponent>()->sprite;
        scene->removeChild(beforeChassis);
        delete beforeChassis;
        es.get(vehicleConstructEntityId).component<TransformComponent>()->sprite = chassisSprite;
    }
}
