#include "MenuECS.h"

using namespace std;
using namespace cocos2d;
using namespace DeathMetal::DeathMetalComponent;
using namespace DeathMetal::MenuComponent;

namespace DeathMetal {
    
    MenuECS::MenuECS(cocos2d::Scene* scene) {
        this->scene = scene;
        systems.configure();
    }
    
    void MenuECS::update(ex::TimeDelta dt) {
        systems.update_all(dt);
    }
}
