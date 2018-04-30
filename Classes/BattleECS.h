#ifndef __BATTLE_ECS__
#define __BATTLE_ECS__

#include "DeathMetal.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "entityx/entityx.h"

namespace DeathMetal {
    
    namespace BattleEntity {
        ex::Entity createWorkshopEntity(ex::EntityManager &es, cocos2d::Vec2 position);
        ex::Entity createHeroEntity(ex::EntityManager &es, cocos2d::Vec2 position, DeathMetalData* data);
        ex::Entity createHutEntity(ex::EntityManager &es, cocos2d::Vec2 position);
        ex::Entity createMutantEntity(ex::EntityManager &es, cocos2d::Vec2 position, int type);
        ex::Entity createBulletEntity(ex::EntityManager &es, cocos2d::Vec2 position, cocos2d::Vec2 velocity, bool friendlyFire);
        bool checkEntityInBound(ex::Entity &entity);
        void destroyEntity(ex::Entity& entity);
    }
    
    namespace BattleComponent {
        struct MobComponent {
            float maxHealth;
            float health;
            float moveSpeed;
        };
        struct HeroComponent {};
        struct WorkshopComponent {};
        struct TurretComponent {};
        struct HutComponent {};
        struct MutantComponent {
            MutantComponent(int type) : type(type) {}
            int type;
        };
        struct BulletComponent {
            BulletComponent(cocos2d::Vec2 velocity, bool friendlyFire) : velocity(velocity), friendlyFire(friendlyFire) {}
            cocos2d::Vec2 velocity;
            bool friendlyFire;
        };
    }
    
    namespace BattleSystem {
        class InteractionSystem : public ex::System<InteractionSystem>, public ex::Receiver<InteractionSystem> {
        private:
            cocos2d::Scene* scene; // cocos
            std::unordered_map<int, std::pair<int, cocos2d::Vec2>> movingMap; // keycode -> hold state -> velocity update
            std::unordered_map<int, float> rotationMap;
            float rotation = 0.;
            cocos2d::Vec2 moveVec;
        public:
            explicit InteractionSystem(cocos2d::Scene* scene) : scene(scene) {};
            void configure(ex::EntityManager &entities, ex::EventManager &event_manager) override;
            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;
            void receive(const DeathMetalEvent::OnKeyPressed& onKeyPressed);
            void receive(const DeathMetalEvent::OnKeyReleased& onKeyReleased);
        };
        
        class GameplaySystem : public ex::System<GameplaySystem> {
        private:
            cocos2d::Scene* scene;
            DeathMetalData* data;
            double heroBulletInterval = 0.;
            double turretBulletInterval = 0.;
            double mutantGenerationInterval = 0.;
            ex::Entity::Id workshopHUDId, heroHUDId, scoreHUDId;
            ex::Entity::Id blowTorchParticleId;
            int score = 0;
            bool endGame = 0;
        public:
            explicit GameplaySystem(cocos2d::Scene* scene) : scene(scene) {};
            void configure(ex::EntityManager &entities, ex::EventManager &event_manager) override;
            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;
            void endGameDisplay(ex::EntityManager &es);
        };
    }
    
    class BattleECS : public ex::EntityX {
    private:
        cocos2d::Scene* scene = 0;
    public:
        explicit BattleECS(cocos2d::Scene* scene);
        void update(ex::TimeDelta dt);
    };
}

#endif
