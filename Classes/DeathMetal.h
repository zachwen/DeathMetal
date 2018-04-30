#ifndef __DEATH_METAL__
#define __DEATH_METAL__

#define UNIT 20

#define MACHINE_GUN 0
#define CIRCULAR_SAW 1
#define BLOW_TORCH 2
#define PLATE_ARMOR 3

#define MUTANT_MELEE 0
#define MUTANT_RANGED 1

#define HERO_FIRE_INTERVAL 0.1
#define TURRET_FIRE_INTERVAL 0.5
#define MUTANT_FIRE_INTERVAL 2.0
#define MUTANT_SPAWN_INTEVAL 3.0

#define BULLET_SPEED 10.0
#define HERO_SPEED 5.0
#define MUTANT_SPEED 1.0

#define BULLET_SIZE UNIT * 0.3
#define HERO_SIZE UNIT * 2.5
#define MUTANT_SIZE UNIT * 1.5
#define TURRET_SIZE UNIT * 1.5
#define WORKSHOP_SIZE UNIT * 8
#define HUT_SIZE UNIT * 4.0

#define HERO_HP 50
#define MUTANT_HP 6
#define HUT_HP 100
#define TURRET_HP 50
#define WORKSHOP_HP 200

#define BULLET_DMG 0.7 // dps: 10 b/s * 0.7 dmg/b = 7
#define BLOW_TORCH_DMG 0.14 // dps: 60 f/s * 0.14 dmg/f = 8
#define CIRCULAR_SAW_DMG 0.26 // dps: 60 f/s * 0.1 dmg/f = 16
#define MUTANT_MELEE_DMG 1.

#define BLOW_TORCH_RANGE 196
#define CIRCULAR_SAW_RANGE 50.

#define BLOW_TORCH_PARTICLE_DURATION 1.

#include "LeetCode.h"
#include "cocos2d.h"
#include "entityx/entityx.h"

namespace DeathMetal {
    
    struct DeathMetalData {
        int chassis = -1;
        std::unordered_set<int>* construct = 0;
    };
    
    namespace DeathMetalEntity {
        cocos2d::Sprite* createSpriteWithFile(std::string filename, cocos2d::Vec2 position, float renderWidth);
        ex::Entity createParticleEntity(ex::EntityManager &es, cocos2d::ParticleSystemQuad* particleEffect);
        ex::Entity createLabelEntity(ex::EntityManager &es, std::string text, std::string font, float fontSize, cocos2d::Color4B fontColor, cocos2d::Size blockSize, cocos2d::Vec2 position);
        ex::Entity createSpriteEntity(ex::EntityManager &es, std::string filename, cocos2d::Vec2 position, float renderWidth);
        ex::Entity createTileEntity(ex::EntityManager &es, float x, float y, int tileType = -1);
    }
    
    namespace DeathMetalComponent {
        struct TransformComponent {
            TransformComponent(cocos2d::Sprite* sprite, cocos2d::Label* label) : sprite(sprite), label(label) {}
            cocos2d::Sprite* sprite = 0;
            cocos2d::Label* label = 0;
        };
        struct RenderComponent {
            // RenderComponent(std::string name, std::unordered_map<std::string, int> animSet, int z, bool render) : name(name), animSet(animSet), z(z), render(render) {}
            RenderComponent(std::string name, bool render) : name(name), render(render) {}
            std::string name;
            std::unordered_map<std::string, int> animSet; // animation state name -> frame number
            bool render = 1;
            bool cleanup = 0;
        };
        struct TileComponent {
            TileComponent(int type, int x, int y) : tileType(type), x(x), y(y) {}
            int tileType;
            int x, y;
        };
        struct ParticleComponent {
            ParticleComponent(cocos2d::ParticleSystemQuad* particleEffect) : particleEffect(particleEffect) {}
            cocos2d::ParticleSystemQuad* particleEffect;
            double existedTime = 0.;
        };
    }
    
    namespace DeathMetalSystem {
        class RenderSystem : public ex::System<RenderSystem> {
        private:
            cocos2d::Scene* scene; // cocos
        public:
            explicit RenderSystem(cocos2d::Scene* scene) : scene(scene) {};
            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;
        };
    }
    
    namespace DeathMetalEvent {
        struct OnKeyPressed {
            OnKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) : keyCode(keyCode), event(event) {};
            cocos2d::EventKeyboard::KeyCode keyCode;
            cocos2d::Event* event;
        };
        struct OnKeyReleased {
            OnKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) : keyCode(keyCode), event(event) {};
            cocos2d::EventKeyboard::KeyCode keyCode;
            cocos2d::Event* event;
        };
    }
}

#endif
