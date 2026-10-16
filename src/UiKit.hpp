#pragma once
// Auto Deco - Made by XYS
// Small UI helpers shared by both popups.

#include <Geode/Geode.hpp>
#include <functional>
#include <string>

using namespace geode::prelude;

namespace autodeco::ui {

// Dark translucent rounded panel used behind groups of controls.
inline CCScale9Sprite* makeCard(float w, float h, GLubyte opacity = 90) {
    auto card = CCScale9Sprite::create("square02b_001.png", CCRect{0.f, 0.f, 80.f, 80.f});
    card->setContentSize({w, h});
    card->setColor({0, 0, 0});
    card->setOpacity(opacity);
    return card;
}

inline CCLabelBMFont* makeLabel(char const* text, char const* font, float scale) {
    auto l = CCLabelBMFont::create(text, font);
    l->setScale(scale);
    return l;
}

// Arrow selector:  <  value  >
// step(delta) must update the caller's state and return the new text.
inline CCLabelBMFont* addSelector(CCNode* layer, CCNode* menu, CCPoint center, float valueWidth,
                                  std::string const& initial, std::function<std::string(int)> step) {
    auto val = CCLabelBMFont::create(initial.c_str(), "bigFont.fnt");
    val->limitLabelWidth(valueWidth, 0.5f, 0.2f);
    layer->addChildAtPosition(val, Anchor::Center, center);

    auto apply = [val, step, valueWidth](int d) {
        val->setString(step(d).c_str());
        val->limitLabelWidth(valueWidth, 0.5f, 0.2f);
    };

    auto l = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    l->setScale(0.55f);
    auto r = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    r->setScale(0.55f);
    r->setFlipX(true);
    auto lb = CCMenuItemExt::createSpriteExtra(l, [apply](auto*) { apply(-1); });
    auto rb = CCMenuItemExt::createSpriteExtra(r, [apply](auto*) { apply(1); });
    menu->addChildAtPosition(lb, Anchor::Center, {center.x - valueWidth / 2.f - 16.f, center.y});
    menu->addChildAtPosition(rb, Anchor::Center, {center.x + valueWidth / 2.f + 16.f, center.y});
    return val;
}

// Small round "i" button that opens an explanation dialog.
inline void addInfoButton(CCNode* menu, CCPoint pos, std::string title, std::string body) {
    auto spr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    spr->setScale(0.7f);
    auto btn = CCMenuItemExt::createSpriteExtra(spr, [title, body](auto*) {
        FLAlertLayer::create(title.c_str(), body.c_str(), "OK")->show();
    });
    menu->addChildAtPosition(btn, Anchor::Center, pos);
}

}  // namespace autodeco::ui
