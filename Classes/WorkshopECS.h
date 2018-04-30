#ifndef __WORKSHOP_ECS__
#define __WORKSHOP_ECS__

#include "DeathMetal.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "entityx/entityx.h"

namespace DeathMetal {
    
    namespace WorkshopEntity {
    }
    
    namespace WorkshopComponent {
    }
    
    namespace WorkshopSystem {
        class WorkshopSystem : public ex::System<WorkshopSystem>, public ex::Receiver<WorkshopSystem> {
        private:
            cocos2d::Scene* scene; // cocos
            DeathMetalData* data;
            DeathMetalData* editingData;
            ex::Entity::Id hintLabelEntityId;
            ex::Entity::Id vehicleConstructEntityId;
            double hintResetInterval = 0.0;
        public:
            explicit WorkshopSystem(cocos2d::Scene* scene) : scene(scene) {};
            void configure(ex::EntityManager &entities, ex::EventManager &event_manager) override;
            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;
            void receive(const DeathMetalEvent::OnKeyPressed& onKeyPressed);
            void receive(const DeathMetalEvent::OnKeyReleased& onKeyReleased);
            void save();
            void hint(ex::EntityManager &es, std::string hintText);
            void updateVehicleConstruct(ex::EntityManager &es);
        };
    }
    
    class WorkshopECS : public ex::EntityX {
    private:
        cocos2d::Scene* scene = 0;
    public:
        explicit WorkshopECS(cocos2d::Scene* scene);
        void update(ex::TimeDelta dt);
    };
}

#endif
