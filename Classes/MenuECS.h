#ifndef __MENU_ECS__
#define __MENU_ECS__

#include "DeathMetal.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "entityx/entityx.h"

namespace DeathMetal {
    
    namespace MenuEntity {
    }
    
    namespace MenuComponent {
    }
    
    namespace MenuSystem {
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
