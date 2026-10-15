// Auto Deco - Made by XYS
#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "DecoPopup.hpp"

using namespace geode::prelude;

class $modify(AutoDecoEditorUI, EditorUI) {
    bool init(LevelEditorLayer* editor) {
        if (!EditorUI::init(editor)) return false;

        auto label = CCLabelBMFont::create("Deco", "bigFont.fnt");
        label->setScale(0.5f);
        auto spr = CircleButtonSprite::create(label, CircleBaseColor::Green, CircleBaseSize::Small);
        auto btn = CCMenuItemExt::createSpriteExtra(spr, [](auto*) {
            if (auto popup = AutoDecoPopup::create()) popup->show();
        });
        btn->setID("autodeco-button"_spr);

        // Taruh tepat di samping tombol trash
        CCNode* trash = nullptr;
        for (auto id : {"trash-button", "delete-button", "trash-btn"}) {
            trash = this->getChildByIDRecursive(id);
            if (trash) break;
        }

        if (trash && trash->getParent()) {
            auto parent = trash->getParent();
            parent->addChild(btn);
            if (parent->getLayout()) {
                parent->updateLayout();
            } else {
                btn->setPosition(trash->getPosition() + CCPoint{trash->getContentSize().width + 8.f, 0.f});
            }
        } else {
            // Cadangan kalau ID tombol trash gak ketemu: pojok kiri atas, kira-kira di sebelah trash
            auto menu = CCMenu::create();
            menu->setPosition({0.f, 0.f});
            btn->setPosition({160.f, CCDirector::get()->getWinSize().height - 20.f});
            menu->addChild(btn);
            this->addChild(menu, 100);
        }
        return true;
    }
};
