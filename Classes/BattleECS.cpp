#include "BattleECS.h"

using namespace std;
using namespace cocos2d;
using namespace CocosDenshion;
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
    
    ex::Entity BattleEntity::createWorkshopEntity(ex::EntityManager &es, cocos2d::Vec2 position) {
        ex::Entity workshopEntity = es.create();
        
        auto sprite = DeathMetalEntity::createSpriteWithFile("workshop.png", position, WORKSHOP_SIZE);
        workshopEntity.assign<TransformComponent>(sprite, (cocos2d::Label*)NULL);
        workshopEntity.assign<RenderComponent>("workshop", 1);
        workshopEntity.assign<MobComponent>();
        workshopEntity.component<MobComponent>()->health = WORKSHOP_HP;
        workshopEntity.component<MobComponent>()->maxHealth = WORKSHOP_HP;
        workshopEntity.assign<WorkshopComponent>();
        
        return workshopEntity;
    }
    
    ex::Entity BattleEntity::createHeroEntity(ex::EntityManager &es, cocos2d::Vec2 position, DeathMetalData* data) {
        ex::Entity heroEntity = es.create();
        
        auto sprite = DeathMetalEntity::createSpriteWithFile("hero.png", position, HERO_SIZE);
        
        // add all the sprite parts
        
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
        for (int part : *(data->construct)) {
            auto partSprite = DeathMetalEntity::createSpriteWithFile(partSpriteMap[part], Vec2(4., 0.), 32.);
            sprite->addChild(partSprite, partPriorityMap[part]);
        }
        
        sprite->setAnchorPoint(Vec2(0.5, 0.5));
        
        heroEntity.assign<TransformComponent>(sprite, (cocos2d::Label*)NULL);
        heroEntity.assign<RenderComponent>("hero", 1);
        heroEntity.assign<HeroComponent>();
        heroEntity.assign<MobComponent>();
        heroEntity.component<MobComponent>()->health = HERO_HP;
        heroEntity.component<MobComponent>()->maxHealth = HERO_HP;
        if (data->construct->count(PLATE_ARMOR)) {
            heroEntity.component<MobComponent>()->health += HERO_HP;
            heroEntity.component<MobComponent>()->maxHealth += HERO_HP;
        }
        return heroEntity;
    }
    
    ex::Entity BattleEntity::createHutEntity(ex::EntityManager &es, cocos2d::Vec2 position) {
        ex::Entity hutEntity = es.create();
        
        auto sprite = DeathMetalEntity::createSpriteWithFile("hut1.png", position, HUT_SIZE);
        hutEntity.assign<TransformComponent>(sprite, (cocos2d::Label*)NULL);
        hutEntity.assign<RenderComponent>("hut", 1);
        hutEntity.assign<HutComponent>();
        hutEntity.assign<MobComponent>();
        hutEntity.component<MobComponent>()->health = HUT_HP;
        hutEntity.component<MobComponent>()->maxHealth = HUT_HP;
        return hutEntity;
    }
    
    ex::Entity BattleEntity::createMutantEntity(ex::EntityManager &es, cocos2d::Vec2 position, int type) {
        ex::Entity mutantEntity = es.create();
        
        auto sprite = DeathMetalEntity::createSpriteWithFile("mutant_frame0.png", position, MUTANT_SIZE);
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
        
        rotationMap[(int)cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW] = 90.;
        rotationMap[(int)cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW] = 180.;
        rotationMap[(int)cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW] = 270.;
        rotationMap[(int)cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW] = 0.;
        
        rotation = 0.;
    }
    
    void BattleSystem::InteractionSystem::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
        auto screenSize = Director::getInstance()->getVisibleSize();
        es.each<HeroComponent>([&](ex::Entity entity, HeroComponent heroComp) {
            auto heroSprite = entity.component<TransformComponent>()->sprite;
            auto pos = heroSprite->getPosition();
            auto spriteSize = heroSprite->getContentSize();
            spriteSize = spriteSize * heroSprite->getScale();
            pos += moveVec * HERO_SPEED;
            pos.x = fmax(fmin(pos.x, screenSize.width - spriteSize.width), 0.);
            pos.y = fmax(fmin(pos.y, screenSize.height - spriteSize.height), 0.);
            heroSprite->setPosition(pos);
            
            auto currentHeroAngle = heroSprite->getRotation();
            heroSprite->setRotation(rotation);
            
            es.each<ParticleComponent>([&](ex::Entity particleEntity, ParticleComponent particleComp) {
                float heroAngle = rotation;
                float blowTorchAngle = particleEntity.component<ParticleComponent>()->particleEffect->getRotation();
                particleEntity.component<ParticleComponent>()->particleEffect->setRotation(blowTorchAngle - heroAngle + currentHeroAngle);
            });
        });
    }
    
    void BattleSystem::InteractionSystem::receive(const DeathMetalEvent::OnKeyPressed& onKeyPressed) {
        if (movingMap.find((int)onKeyPressed.keyCode) != movingMap.end()) {
            moveVec += movingMap[(int)onKeyPressed.keyCode].second;
            rotation = rotationMap[(int)onKeyPressed.keyCode];
        }
    }
    
    void BattleSystem::InteractionSystem::receive(const DeathMetalEvent::OnKeyReleased& onKeyReleased) {
        if (movingMap.find((int)onKeyReleased.keyCode) != movingMap.end())
            moveVec -= movingMap[(int)onKeyReleased.keyCode].second;
    }
    
    void BattleSystem::GameplaySystem::configure(ex::EntityManager &entities, ex::EventManager &event_manager) {
        // TODO: init game state
        auto screenSize = Director::getInstance()->getVisibleSize();
        
        data = (DeathMetalData*)(scene->getUserData());
        
        int tileWidth = screenSize.width / UNIT + 1;
        int tileHeight = screenSize.height / UNIT + 1;
        for (int i = 0; i < tileWidth; i++)
            for (int j = 0; j < tileHeight; j++)
                DeathMetalEntity::createTileEntity(entities, i, j, RandomHelper::random_int(0, 1));
        
        scoreHUDId = DeathMetalEntity::createLabelEntity(entities, "", "Helvetica", 25, Color4B::WHITE, Size(screenSize.width / 4, screenSize.height / 16), Vec2(UNIT, screenSize.height / 8)).id();
        entities.get(scoreHUDId).component<TransformComponent>()->label->setAlignment(TextHAlignment::LEFT);
        heroHUDId = DeathMetalEntity::createLabelEntity(entities, "", "Helvetica", 25, Color4B::WHITE, Size(screenSize.width / 4, screenSize.height / 16), Vec2(UNIT, screenSize.height / 16)).id();
        entities.get(heroHUDId).component<TransformComponent>()->label->setAlignment(TextHAlignment::LEFT);
        workshopHUDId = DeathMetalEntity::createLabelEntity(entities, "", "Helvetica", 25, Color4B::WHITE, Size(screenSize.width / 4, screenSize.height / 16), Vec2(UNIT, 0)).id();
        entities.get(workshopHUDId).component<TransformComponent>()->label->setAlignment(TextHAlignment::LEFT);
        
        
        BattleEntity::createHutEntity(entities, Vec2(0, screenSize.height - HUT_SIZE));
        BattleEntity::createHutEntity(entities, Vec2(screenSize.width / 2 - HUT_SIZE, screenSize.height - HUT_SIZE));
        BattleEntity::createHutEntity(entities, Vec2(screenSize.width - HUT_SIZE, screenSize.height - HUT_SIZE));
        
        BattleEntity::createWorkshopEntity(entities, Vec2(screenSize.width / 2 - WORKSHOP_SIZE / 2, 0));
        BattleEntity::createHeroEntity(entities, Vec2(screenSize.width / 2, screenSize.height / 2), data);
        
        
        BattleEntity::createMutantEntity(entities, Vec2(screenSize.width / 2, screenSize.height / 4), MUTANT_MELEE);
        BattleEntity::createMutantEntity(entities, Vec2(0, 600), MUTANT_MELEE);
        BattleEntity::createMutantEntity(entities, Vec2(900, 200), MUTANT_MELEE);
        BattleEntity::createMutantEntity(entities, Vec2(100, 300), MUTANT_MELEE);
        BattleEntity::createMutantEntity(entities, Vec2(400, 200), MUTANT_MELEE);
        
        auto blowTorchEmitter = cocos2d::ParticleMeteor::create();
        blowTorchEmitter->setPosition(Vec2(HERO_SIZE / 2, HERO_SIZE / 2));
        blowTorchEmitter->setScale(0.2);
        // blowTorchEmitter->setStartColor(Color4F::ORANGE);
        // blowTorchEmitter->setEndColor(Color4F::YELLOW);
        blowTorchEmitter->retain();
        blowTorchParticleId = DeathMetalEntity::createParticleEntity(entities, blowTorchEmitter).id();
        
        auto audio = SimpleAudioEngine::getInstance();
        audio->preloadEffect("gunshot_heathers.wav");
        audio->stopBackgroundMusic();
        audio->playBackgroundMusic("Tagirijus_-_04_-_Hard_Fight.mp3", true);
    }
    
    void BattleSystem::GameplaySystem::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
        if (endGame)
            return;
        ex::ComponentHandle<HeroComponent> heroComp;
        ex::ComponentHandle<BulletComponent> bulletComp;
        ex::ComponentHandle<MutantComponent> mutantComp;
        ex::ComponentHandle<WorkshopComponent> workshopComp;
        ex::ComponentHandle<HutComponent> hutComp;
        ex::ComponentHandle<ParticleComponent> particleComp;
        
        Sprite* heroSprite, *workshopSprite;
        Rect heroBox, workshopBox;
        Vec2 heroPos, workshopPos;
        ex::ComponentHandle<MobComponent> heroMob, workshopMob;
        vector<ex::Entity> destroyedMutants, destroyedBullets;
        for (ex::Entity heroEntity : es.entities_with_components(heroComp)) {
            
            heroSprite = heroEntity.component<TransformComponent>()->sprite;
            heroBox = heroSprite->getBoundingBox();
            heroPos = heroSprite->getPosition();
            heroMob = heroEntity.component<MobComponent>();
            
            // find nearest mutant
            float minMutantDist = FLT_MAX;
            Vec2 minMutantPos(FLT_MAX, FLT_MAX);
            ex::Entity nearestMutantEntity;
            for (ex::Entity mutantEntity : es.entities_with_components(mutantComp)) {
                auto mutantPos = mutantEntity.component<TransformComponent>()->sprite->getPosition();
                float mutantDist = mutantPos.distance(heroPos);
                if (mutantDist < minMutantDist) {
                    minMutantDist = mutantDist;
                    minMutantPos = mutantPos;
                    nearestMutantEntity = mutantEntity;
                }
            }
            if (data->construct->count(MACHINE_GUN)) {
                if (heroBulletInterval > HERO_FIRE_INTERVAL) {
                    // auto aiming system
                    if (minMutantPos.x != FLT_MAX || minMutantPos.y != FLT_MAX) {
                        Vec2 bulletVelocity = minMutantPos - heroPos;
                        bulletVelocity.normalize();
                        bulletVelocity *= BULLET_SPEED;
                        BattleEntity::createBulletEntity(es, heroPos, bulletVelocity, 1);
                        auto audio = SimpleAudioEngine::getInstance();
                        audio->playEffect("gunshot_heathers.wav", false, 1.0f, 1.0f, 1.0f);
                    }
                }
            }
            if (data->construct->count(CIRCULAR_SAW)) {
                // damage all mutants nearby
                for (ex::Entity mutantEntity : es.entities_with_components(mutantComp)) {
                    auto mutantPos = mutantEntity.component<TransformComponent>()->sprite->getPosition();
                    float mutantDist = mutantPos.distance(heroPos);
                    if (mutantDist < CIRCULAR_SAW_RANGE) {
                        mutantEntity.component<MobComponent>()->health -= CIRCULAR_SAW_DMG;
                    }
                }
            }
            if (data->construct->count(BLOW_TORCH)) {
                if (minMutantPos.x != FLT_MAX || minMutantPos.y != FLT_MAX) {
                    if (minMutantDist < BLOW_TORCH_RANGE) {
                        nearestMutantEntity.component<MobComponent>()->health -= BLOW_TORCH_DMG;
                        // TODO: play blow torch particle effect
                        for (ex::Entity particleEntity : es.entities_with_components(particleComp)) {
                            auto blowTorchParticle = particleEntity.component<ParticleComponent>()->particleEffect;
                            auto direction = minMutantPos - heroPos;
                            blowTorchParticle->setRotation(direction.getAngle(Vec2(1, 0)));
                            if (blowTorchParticle->getParent())
                                blowTorchParticle->setRotation(direction.getAngle(Vec2(1, 0)) - heroSprite->getRotation());
                            if (!blowTorchParticle->getParent()) {
                                heroSprite->addChild(blowTorchParticle);
                                particleEntity.component<ParticleComponent>()->existedTime = 0.;
                            }
                        }
                    }
                    else {
                        // TODO: polish after the jam
                        /*
                        for (ex::Entity particleEntity : es.entities_with_components(particleComp)) {
                            auto blowTorchParticle = particleEntity.component<ParticleComponent>()->particleEffect;
                            if (blowTorchParticle->getParent()) {
                                heroSprite->removeChild(blowTorchParticle, 0); // no cleanup
                                particleEntity.component<ParticleComponent>()->existedTime = 0.;
                            }
                        }
                         */
                    }
                }
                for (ex::Entity particleEntity : es.entities_with_components(particleComp)) {
                    auto blowTorchParticle = particleEntity.component<ParticleComponent>()->particleEffect;
                    if (particleEntity.component<ParticleComponent>()->existedTime > BLOW_TORCH_PARTICLE_DURATION)
                        if (blowTorchParticle->getParent()) {
                            heroSprite->removeChild(blowTorchParticle, 0); // no cleanup
                            particleEntity.component<ParticleComponent>()->existedTime = 0.;
                            particleEntity.component<ParticleComponent>()->existedTime += dt;
                        }
                }
                
            }
            if (heroMob->health <= 0)
                endGame = 1;
        }
        // workshop
        for (ex::Entity workshopEntity : es.entities_with_components(workshopComp)) {
            workshopSprite = workshopEntity.component<TransformComponent>()->sprite;
            workshopBox = workshopSprite->getBoundingBox();
            workshopPos = workshopSprite->getPosition();
            workshopMob = workshopEntity.component<MobComponent>();
            if (workshopMob->health <= 0)
                endGame = 1;
        }
        // mutant
        for (ex::Entity mutantEntity : es.entities_with_components(mutantComp)) {
            if (mutantEntity.component<MobComponent>()->health <= 0) {
                destroyedMutants.push_back(mutantEntity);
                continue;
            }
            
            auto mutantSprite = mutantEntity.component<TransformComponent>()->sprite;
            
            float heroDist, workshopDist;
            Vec2 mutantPos = mutantSprite->getPosition();
            heroDist = heroPos.distance(mutantPos);
            workshopDist = workshopPos.distance(mutantPos);
            
            Vec2 dest = heroDist > workshopDist ? workshopPos : heroPos;
            Vec2 direction = dest - mutantPos;
            direction.normalize();
            mutantSprite->setPosition(mutantPos + direction * MUTANT_SPEED);
            
            // handle melee damaging hero
            if (heroBox.intersectsRect(mutantSprite->getBoundingBox()))
                heroMob->health -= MUTANT_MELEE_DMG;
            // handle melee damaging workshop
            if (workshopBox.intersectsRect(mutantSprite->getBoundingBox()))
                workshopMob->health -= MUTANT_MELEE_DMG;
        }
        // mutant hut
        for (ex::Entity hutEntity : es.entities_with_components(hutComp)) {
            if (mutantGenerationInterval >= MUTANT_SPAWN_INTEVAL) {
                Vec2 hutPos = hutEntity.component<TransformComponent>()->sprite->getPosition();
                float randomX = RandomHelper::random_int(-(HUT_SIZE / 2), HUT_SIZE / 2);
                float randomY = RandomHelper::random_int(-(HUT_SIZE / 2), HUT_SIZE / 2);
                BattleEntity::createMutantEntity(es, Vec2(hutPos.x + randomX, hutPos.y + randomY), MUTANT_MELEE);
                BattleEntity::createMutantEntity(es, Vec2(hutPos.x + randomY, hutPos.y + randomX), MUTANT_MELEE);
            }
        }
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
                        mutantEntity.component<MobComponent>()->health -= BULLET_DMG;
                        break;
                    }
                }
            }
            else {
                // enemy bullet
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
        // destroy entities
        for (ex::Entity bulletEntity : destroyedBullets)
            if (bulletEntity.valid()) // de-dup
                BattleEntity::destroyEntity(bulletEntity);
        for (ex::Entity mutantEntity : destroyedMutants)
            if (mutantEntity.valid()) {
                BattleEntity::destroyEntity(mutantEntity);
                score += 10;
            }
        
        // HUD updating
        string heroHudText = "Vehicle HP: " + to_string((int)fmax(heroMob->health, 0.)) + " / " + to_string((int)heroMob->maxHealth);
        es.get(heroHUDId).component<TransformComponent>()->label->setString(heroHudText);
        string workshopHudText = "Workshop HP: " + to_string((int)fmax(workshopMob->health, 0.)) + " / " + to_string((int)workshopMob->maxHealth);
        es.get(workshopHUDId).component<TransformComponent>()->label->setString(workshopHudText);
        string scoreHudText = "Score: " + to_string(score);
        es.get(scoreHUDId).component<TransformComponent>()->label->setString(scoreHudText);
        
        // interval updating
        if (mutantGenerationInterval >= MUTANT_SPAWN_INTEVAL)
            mutantGenerationInterval = 0.;
        mutantGenerationInterval += dt;
        
        if (heroBulletInterval > HERO_FIRE_INTERVAL)
            heroBulletInterval = 0.;
        heroBulletInterval += dt;
        
        if (turretBulletInterval >= TURRET_FIRE_INTERVAL)
            turretBulletInterval = 0.;
        turretBulletInterval += dt;
        
        // end game state reached
        if (endGame)
            endGameDisplay(es);
    }
    
    void BattleSystem::GameplaySystem::endGameDisplay(ex::EntityManager &es) {
        es.reset(); // TODO: fix leaked sprite memory
        auto screenSize = Director::getInstance()->getVisibleSize();
        DeathMetalEntity::createLabelEntity(es, "WASTED", "Krungthep", 100, Color4B::RED, Size(screenSize.width, screenSize.height / 4), Vec2(0, screenSize.height * 3 / 4));
        DeathMetalEntity::createLabelEntity(es, "Final Score: " + to_string(score), "Krungthep", 80, Color4B::WHITE, Size(screenSize.width, screenSize.height / 4), Vec2(0, screenSize.height / 2));
        auto finishItem = MenuItemImage::create("Button_Up_Finish.png", "Button_Down_Finish.png", [&](Ref* sender) {
            auto audio = SimpleAudioEngine::getInstance();
            audio->stopBackgroundMusic();
            audio->playBackgroundMusic("Optimus_Prim_-_02_-_Dont_you_think.mp3", true);
            Director::getInstance()->popScene();
        });
        auto finishMenu = Menu::createWithItem(finishItem);
        finishMenu->alignItemsVertically();
        finishMenu->setScale(1.5);
        finishMenu->setAnchorPoint(Vec2(0., 0.));
        finishMenu->setPosition(screenSize.width / 2, screenSize.height / 4);
        scene->addChild(finishMenu);
    }
}
