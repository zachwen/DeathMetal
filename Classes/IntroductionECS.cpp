#include "IntroductionECS.h"
#include "entityx/deps/Dependencies.h"

namespace DeathMetal {
    IntroductionECS::IntroductionECS(cocos2d::Scene* scene) {
        this->scene = scene;
        systems.configure();
    }
    
    void IntroductionECS::update(ex::TimeDelta dt) {
        systems.update_all(dt);
    }
}
