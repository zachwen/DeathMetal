#include "BattleECS.h"

#define HERO_FIRE_INTERVAL 0.2
#define TURRET_FIRE_INTERVAL 0.5
#define MUTANT_FIRE_INTERVAL 2.0

#define BULLET_SPEED 10.0
#define HERO_SPEED 5.0
#define MUTANT_SPEED 2.0

#define BULLET_SIZE UNIT * 0.3
#define HERO_SIZE UNIT * 2
#define MUTANT_SIZE UNIT
#define TURRET_SIZE UNIT * 1.5
#define WORKSHOP_SIZE UNIT * 4
#define HUT_SIZE UNIT * 1.5

#define HERO_HP 10
#define MUTANT_HP 5
#define HUT_HP 100
#define TURRET_HP 50
#define WORKSHOP_HP 100

#define BULLET_DMG 2

using namespace std;
using namespace cocos2d;
using namespace DeathMetal::DeathMetalComponent;
using namespace DeathMetal::BattleComponent;

namespace DeathMetal {
    
    BattleECS::BattleECS(cocos2d::Scene* scene) {
        this->scene = scene;
        
        // TODO: map system
        systems.add<DeathMetalSystem::RenderSystem>(scene);
        systems.add<BattleSystem::GameplaySystem>(scene);
        systems.add<BattleSystem::InteractionSystem>(scene);
        systems.configure();
    }
    
    void BattleECS::update(ex::TimeDelta dt) {
        systems.update_all(dt);
    }
    
    ex::Entity BattleEntity::createHeroEntity(ex::EntityManager &es, cocos2d::Vec2 position) {
        ex::Entity heroEntity = es.create();
        
        auto sprite = DeathMetalEntity::createSpriteWithFile("hero.png", position, HERO_SIZE);
        heroEntity.assign<TransformComponent>(sprite, (cocos2d::Label*)NULL);
        heroEntity.assign<RenderComponent>("hero", 1);
        heroEntity.assign<HeroComponent>();
        heroEntity.assign<MobComponent>();
        heroEntity.component<MobComponent>()->health = HERO_HP;
        heroEntity.component<MobComponent>()->maxHealth = HERO_HP;
        return heroEntity;
    }
    
    ex::Entity BattleEntity::createMutantEntity(ex::EntityManager &es, cocos2d::Vec2 position, int type) {
        ex::Entity mutantEntity = es.create();
        
        auto sprite = DeathMetalEntity::createSpriteWithFile("mutant.png", position, MUTANT_SIZE);
        mutantEntity.assign<TransformComponent>(sprite, (cocos2d::Label*)NULL);
        mutantEntity.assign<RenderComponent>("mutant", 1);
        mutantEntity.assign<MutantComponent>(type);
        mutantEntity.assign<MobComponent>();
        mutantEntity.component<MobComponent>()->health = MUTANT_HP;
        mutantEntity.component<MobComponent>()->maxHealth = MUTANT_HP;
        return mutantEntity;
    }
    
    ex::Entity BattleEntity::createBulletEntity(ex::EntityManager &es, cocos2d::Vec2 position, cocos2d::Vec2 velocity, bool friendlyFire) {
        ex::Entity bulletEntity = es.create();
        
        auto sprite = DeathMetalEntity::createSpriteWithFile("bullet.png", position, BULLET_SIZE);
        bulletEntity.assign<TransformComponent>(sprite, (cocos2d::Label*)NULL);
        bulletEntity.assign<RenderComponent>("bullet", 1);
        bulletEntity.assign<BulletComponent>(velocity, friendlyFire);
        
        return bulletEntity;
    }
    
    bool checkEntityInBound(ex::Entity &entity) {
        auto transComp = entity.component<TransformComponent>();
        if (!transComp || !transComp->sprite)
            return 1;
        auto screenSize = Director::getInstance()->getVisibleSize();
        auto pos = transComp->sprite->getPosition();
        if (pos.x < 0 || pos.x >= screenSize.width || pos.y < 0 || pos.y >= screenSize.height)
            return 0;
        return 1;
    }
    
    void BattleEntity::destroyEntity(ex::Entity& entity) {
        auto transComp = entity.component<TransformComponent>();
        if (transComp->sprite && transComp->sprite->getParent())
            transComp->sprite->getParent()->removeChild(transComp->sprite);
        entity.destroy();
    }
    
    void BattleSystem::InteractionSystem::configure(ex::EntityManager &entities, ex::EventManager &event_manager) {
        event_manager.subscribe<DeathMetalEvent::OnKeyPressed>(*this);
        event_manager.subscribe<DeathMetalEvent::OnKeyReleased>(*this);
        movingMap[(int)cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW] = make_pair(0, Vec2(-1, 0));
        movingMap[(int)cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW] = make_pair(0, Vec2(0, 1));
        movingMap[(int)cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW] = make_pair(0, Vec2(1, 0));
        movingMap[(int)cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW] = make_pair(0, Vec2(0, -1));
        moveVec = Vec2(0, 0);
    }
    
    void BattleSystem::InteractionSystem::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
        auto screenSize = Director::getInstance()->getVisibleSize();
        es.each<HeroComponent>([&](ex::Entity entity, HeroComponent heroComp) {
            auto pos = entity.component<TransformComponent>()->sprite->getPosition();
            auto spriteSize = entity.component<TransformComponent>()->sprite->getContentSize();
            spriteSize = spriteSize * entity.component<TransformComponent>()->sprite->getScale();
            pos += moveVec * HERO_SPEED;
            pos.x = fmax(fmin(pos.x, screenSize.width - spriteSize.width), 0.);
            pos.y = fmax(fmin(pos.y, screenSize.height - spriteSize.height), 0.);
            entity.component<TransformComponent>()->sprite->setPosition(pos);
        });
    }
    
    void BattleSystem::InteractionSystem::receive(const DeathMetalEvent::OnKeyPressed& onKeyPressed) {
        if (movingMap.find((int)onKeyPressed.keyCode) != movingMap.end())
            moveVec += movingMap[(int)onKeyPressed.keyCode].second;
    }
    
    void BattleSystem::InteractionSystem::receive(const DeathMetalEvent::OnKeyReleased& onKeyReleased) {
        if (movingMap.find((int)onKeyReleased.keyCode) != movingMap.end())
            moveVec -= movingMap[(int)onKeyReleased.keyCode].second;
    }
    
    void BattleSystem::GameplaySystem::configure(ex::EntityManager &entities, ex::EventManager &event_manager) {
        // TODO: init game state
        // auto screenSize = Director::getInstance()->getVisibleSize();
        // BattleEntity::createHeroEntity(entities, Vec2(screenSize.width / 2, screenSize.height / 2));
    }
    
    void BattleSystem::GameplaySystem::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
        ex::ComponentHandle<HeroComponent> heroComp;
        ex::ComponentHandle<BulletComponent> bulletComp;
        ex::ComponentHandle<MutantComponent> mutantComp;
        vector<ex::Entity> destroyedMutants, destroyedBullets;
        // TODO: update to hash table to avoid duplicates
        // TODO: destroyedMutantHuts
        // TODO: destroyedturrets
        // hero
        for (ex::Entity heroEntity : es.entities_with_components(heroComp)) {
            if (/* Check whether hero has machine gun */true) {
                if (bulletInterval > HERO_FIRE_INTERVAL) {
                    auto heroPos = heroEntity.component<TransformComponent>()->sprite->getPosition();
                    // auto aiming system
                    float minHeroDist = FLT_MAX, minWorkshopDist = FLT_MAX;
                    Vec2 minMutantPos(FLT_MAX, FLT_MAX);
                    int count = 0;
                    for (ex::Entity mutantEntity : es.entities_with_components(mutantComp)) {
                        auto mutantPos = mutantEntity.component<TransformComponent>()->sprite->getPosition();
                        float mutantDist = mutantPos.distance(heroPos);
                        if (mutantDist < minHeroDist) {
                            minHeroDist = mutantDist;
                            minMutantPos = mutantPos;
                        }
                        count++;
                    }
                    if (minMutantPos.x != FLT_MAX || minMutantPos.y != FLT_MAX) {
                        Vec2 bulletVelocity = minMutantPos - heroPos;
                        bulletVelocity.normalize();
                        bulletVelocity *= BULLET_SPEED;
                        BattleEntity::createBulletEntity(es, heroPos, bulletVelocity, 1);
                    }
                    bulletInterval = 0.;
                }
            }
        }
        // TODO: mutant
        // TODO: mutant hut
        // TODO: workshop
        // TODO: turret
        // bullet
        for (ex::Entity bulletEntity : es.entities_with_components(bulletComp)) {
            auto bulletTransComp = bulletEntity.component<TransformComponent>();
            
            auto pos = bulletTransComp->sprite->getPosition();
            bulletTransComp->sprite->setPosition(pos.x + bulletComp->velocity.x, pos.y + bulletComp->velocity.y);
            
            if (!checkEntityInBound(bulletEntity)) {
                destroyedBullets.push_back(bulletEntity);
                continue;
            }
            
            auto bulletBox = bulletTransComp->sprite->getBoundingBox();
            if (bulletComp->friendlyFire) {
                for (ex::Entity mutantEntity : es.entities_with_components(mutantComp)) {
                    auto mutantTransComp = mutantEntity.component<TransformComponent>();
                    auto mutantBox = mutantTransComp->sprite->getBoundingBox();
                    if (mutantBox.intersectsRect(bulletBox)) {
                        destroyedBullets.push_back(bulletEntity);
                        // TODO: change to health dec and leave to destroy to the mutant loop
                        destroyedMutants.push_back(mutantEntity);
                        break;
                    }
                }
            }
            else {
                es.each<HeroComponent>([&](ex::Entity heroEntity, HeroComponent heroComp) {
                    // TODO: if collision destroy bullet and hero health dec
                });
                es.each<TurretComponent>([&](ex::Entity turretEntity, TurretComponent turretComp) {
                    // TODO: if collision destroy bullet and turret health dec
                });
                es.each<WorkshopComponent>([&](ex::Entity workshopEntity, WorkshopComponent workshopComp) {
                    // TODO: if collision destroy bullet and workshop health dec
                });
            }
        }
        for (ex::Entity bulletEntity : destroyedBullets)
            BattleEntity::destroyEntity(bulletEntity);
        for (ex::Entity mutantEntity : destroyedMutants)
            BattleEntity::destroyEntity(mutantEntity);
        // TODO: destroy mutant hut
        bulletInterval += dt;
    }
}
