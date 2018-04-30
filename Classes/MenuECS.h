#ifndef __MENU_ECS__
#define __MENU_ECS__

#include "DeathMetal.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "entityx/entityx.h"
#include "WorkshopScene.h"
#include "BattleScene.h"

namespace DeathMetal {
    
    namespace MenuEntity {
    }
    
    namespace MenuComponent {
    }
    
    namespace MenuSystem {
        class MenuSystem : public ex::System<MenuSystem> {
        private:
            cocos2d::Scene* scene; // cocos
            ex::Entity::Id mentorEntityId;
            ex::Entity::Id dialogueEntityId;
            DeathMetalData* data;
            double dialogueInterval;
        public:
            explicit MenuSystem(cocos2d::Scene* scene) : scene(scene) {};
            void configure(ex::EntityManager &entities, ex::EventManager &event_manager) override;
            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;
            void hint(ex::EntityManager &es, std::string hintText);
        };
    }
    
    class MenuECS : public ex::EntityX {
    private:
        cocos2d::Scene* scene = 0;
    public:
        explicit MenuECS(cocos2d::Scene* scene);
        void update(ex::TimeDelta dt);
    };
}

#endif
