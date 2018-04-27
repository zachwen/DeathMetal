#ifndef __INTRODUCTION_ECS__
#define __INTRODUCTION_ECS__

#include "DeathMetal.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "entityx/entityx.h"

namespace DeathMetal {
    
    namespace IntroductionSystem {
        class IntroductionSystem : public ex::System<IntroductionSystem>, public ex::Receiver<IntroductionSystem> {
        private:
            cocos2d::Scene* scene; // cocos
            cocos2d::Label* introductionLabel;
        public:
            explicit IntroductionSystem(cocos2d::Scene* scene) : scene(scene) {};
            void configure(ex::EntityManager &entities, ex::EventManager &event_manager) override;
            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;
            void receive(const DeathMetalEvent::OnKeyReleased& onKeyReleased);
        };
    }
    
    class IntroductionECS : public ex::EntityX {
    private:
        cocos2d::Scene* scene = 0;
    public:
        explicit IntroductionECS(cocos2d::Scene* scene);
        void update(ex::TimeDelta dt);
    };
}

#endif
