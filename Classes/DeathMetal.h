#ifndef __DEATH_METAL__
#define __DEATH_METAL__

#define UNIT 15

#include "LeetCode.h"
#include "cocos2d.h"
#include "entityx/entityx.h"

namespace DeathMetal {
    
    namespace DeathMetalEntity {
        cocos2d::Sprite* createSpriteWithFile(std::string filename, cocos2d::Vec2 position, float renderWidth);
        ex::Entity createLabelEntity(ex::EntityManager &es, std::string text, std::string font, float fontSize, cocos2d::Size blockSize, cocos2d::Vec2 position);
        ex::Entity createSpriteEntity(ex::EntityManager &es, std::string filename, cocos2d::Vec2 position, float scale);
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
            TileComponent(std::string type, int x, int y) : tileData(type), x(x), y(y) {}
            std::string tileData;
            int x, y;
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
