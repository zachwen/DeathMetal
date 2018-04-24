#ifndef __INTRODUCTION_ECS__
#define __INTRODUCTION_ECS__

#include "cocos2d.h"
#include "entityx/entityx.h"

namespace DeathMetal {
    
    namespace IntroductionSystem {
        class RenderSystem : public ex::System<RenderSystem> {
        private:
            cocos2d::Scene* scene; // cocos
        public:
            explicit RenderSystem(cocos2d::Scene* scene) : scene(scene) {}; // TODO: add cocos2d handle
            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;
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
