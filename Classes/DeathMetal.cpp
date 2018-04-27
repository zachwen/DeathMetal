#include "DeathMetal.h"

using namespace DeathMetal::DeathMetalComponent;
using namespace cocos2d;

namespace DeathMetal {

    // entity creation methods
    
    Sprite* DeathMetalEntity::createSpriteWithFile(std::string filename, cocos2d::Vec2 position, float renderWidth) {
        auto sprite = new Sprite();
        sprite->initWithFile(filename);
        sprite->setAnchorPoint(Vec2(0, 0));
        sprite->setPosition(position);
        sprite->setScale(renderWidth / sprite->getContentSize().width);
        return sprite;
    }
    
    ex::Entity DeathMetalEntity::createLabelEntity(ex::EntityManager &es, std::string text, std::string font, float fontSize, Size blockSize, Vec2 position) {
        ex::Entity labelEntity = es.create();
        
        Label* label = new Label();
        label->setString(text);
        label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
        label->setSystemFontName(font);
        label->setSystemFontSize(fontSize);
        label->setDimensions(blockSize.width, blockSize.height);
        label->setAnchorPoint(Vec2(0, 0));
        label->setPosition(position);
        
        labelEntity.assign<RenderComponent>("", 1);
        labelEntity.assign<TransformComponent>((cocos2d::Sprite*)NULL, label);
        return labelEntity;
    }

    ex::Entity DeathMetalEntity::createSpriteEntity(ex::EntityManager &es, std::string filename, cocos2d::Vec2 pos, float scale) {
        // TODO: implement if the need rises
        return es.create();
    }

    // system implementations

    void DeathMetalSystem::RenderSystem::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
        es.each<RenderComponent, TransformComponent>([&](ex::Entity entity, RenderComponent renderComp, TransformComponent transComp) {
            if (renderComp.render) {
                Sprite* sprite = transComp.sprite;
                if (sprite) {
                    if (!(sprite->getParent()))
                        this->scene->addChild(sprite, INT_MIN); // default to render first (tile)
                    if (!entity.component<TileComponent>())
                        sprite->setLocalZOrder(-sprite->getPosition().y);
                }
                Label* label = transComp.label;
                if (label) {
                    if (!(label->getParent()))
                        this->scene->addChild(label, INT_MAX); // default to render last (text)
                }
            }
        });
    }
}
