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
        
        // IMPORTANT: disable the default cocos2d anti-aliasing to present pixel art
        // Texture2D::TexParams texParams = { GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE };
        // sprite->getTexture()->setTexParameters(texParams);
        sprite->getTexture()->setAliasTexParameters();

        return sprite;
    }
    
    ex::Entity DeathMetalEntity::createParticleEntity(ex::EntityManager &es, ParticleSystemQuad* particleEffect) {
        ex::Entity particleEntity = es.create();
        
        particleEntity.assign<ParticleComponent>(particleEffect);
        return particleEntity;
    }
    
    ex::Entity DeathMetalEntity::createLabelEntity(ex::EntityManager &es, std::string text, std::string font, float fontSize, Color4B fontColor, Size blockSize, Vec2 position) {
        ex::Entity labelEntity = es.create();
        
        Label* label = new Label();
        label->setString(text);
        label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
        label->setSystemFontName(font);
        label->setSystemFontSize(fontSize);
        label->setTextColor(fontColor);
        label->setDimensions(blockSize.width, blockSize.height);
        label->setAnchorPoint(Vec2(0, 0));
        label->setPosition(position);
        
        labelEntity.assign<RenderComponent>("", 1);
        labelEntity.assign<TransformComponent>((cocos2d::Sprite*)NULL, label);
        return labelEntity;
    }

    ex::Entity DeathMetalEntity::createSpriteEntity(ex::EntityManager &es, std::string filename, cocos2d::Vec2 position, float renderWidth) {
        ex::Entity spriteEntity = es.create();
        
        Sprite* sprite = createSpriteWithFile(filename, position, renderWidth);
        
        spriteEntity.assign<RenderComponent>("", 1);
        spriteEntity.assign<TransformComponent>(sprite, (cocos2d::Label*)NULL);
        return spriteEntity;
    }
    
    ex::Entity DeathMetalEntity::createTileEntity(ex::EntityManager &es, float x, float y, int tileType) {
        ex::Entity tileEntity = es.create();
        
        std::string tileFilename;
        if (tileType == 2)
            tileFilename = "workshop_tile.png";
        else
            tileFilename = tileType == 0 ? "sand0.png" : "sand1.png";
        Sprite* tileSprite = createSpriteWithFile(tileFilename, Vec2(x * UNIT, y * UNIT), UNIT);
        
        tileEntity.assign<RenderComponent>("", 1);
        tileEntity.assign<TransformComponent>(tileSprite, (cocos2d::Label*)NULL);
        tileEntity.assign<TileComponent>(tileType, x, y);
        return tileEntity;
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
                        this->scene->reorderChild(sprite, (int)-sprite->getPosition().y);
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
