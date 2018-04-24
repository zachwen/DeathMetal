#ifndef __DEATH_METAL__
#define __DEATH_METAL__

#define UNIT 25

namespace DeathMetal {
    namespace Input {
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
