// Auto Deco - Made by XYS
#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <functional>
#include "DecoPopup.hpp"
#include "LayoutPopup.hpp"

using namespace geode::prelude;

class $modify(AutoDecoEditorUI, EditorUI) {
    bool init(LevelEditorLayer* editor) {
        if (!EditorUI::init(editor)) return false;

        auto winSize = CCDirector::get()->getWinSize();

        // Default spot: top-left row, right next to the Trash button.
        CCPoint start{168.f, winSize.height - 20.f};

        // Better: measure the undo / redo / trash row and continue it to the right.
        if (auto undo = this->getChildByIDRecursive("undo-button")) {
            auto parent = undo->getParent();
            if (parent && parent->getChildrenCount() > 0) {
                float rightEdge = -1e9f;
                for (auto* child : CCArrayExt<CCNode*>(parent->getChildren())) {
                    if (std::abs(child->getPositionY() - undo->getPositionY()) > 10.f) continue;
                    float right = child->getPositionX() +
                                  child->getScaledContentSize().width * (1.f - child->getAnchorPoint().x);
                    rightEdge = std::max(rightEdge, right);
                }
                if (rightEdge > -1e8f) {
                    auto world = parent->convertToWorldSpace({rightEdge, undo->getPositionY()});
                    auto local = this->convertToNodeSpace(world) + CCPoint{26.f, 0.f};
                    // Only trust it if it lands in the top-left area of the screen
                    if (local.y > winSize.height * 0.6f && local.x < winSize.width * 0.5f) start = local;
                }
            }
        }

        auto makeButton = [](char const* text, CircleBaseColor color, std::function<void()> onClick) {
            auto label = CCLabelBMFont::create(text, "bigFont.fnt");
            label->limitLabelWidth(28.f, 0.5f, 0.1f);
            auto spr = CircleButtonSprite::create(label, color, CircleBaseSize::Small);
            spr->setScale(0.85f);
            return CCMenuItemExt::createSpriteExtra(spr, [onClick](auto*) { onClick(); });
        };

        auto decoBtn = makeButton("Deco", CircleBaseColor::Green, [] {
            if (auto popup = AutoDecoPopup::create()) popup->show();
        });
        decoBtn->setID("autodeco-button"_spr);
        decoBtn->setPosition(start);

        auto layoutBtn = makeButton("Gen", CircleBaseColor::Blue, [] {
            if (auto popup = AutoDecoLayoutPopup::create()) popup->show();
        });
        layoutBtn->setID("autodeco-layout-button"_spr);
        layoutBtn->setPosition(start + CCPoint{44.f, 0.f});

        auto menu = CCMenu::create();
        menu->setID("autodeco-menu"_spr);
        menu->setPosition({0.f, 0.f});
        menu->addChild(decoBtn);
        menu->addChild(layoutBtn);
        this->addChild(menu, 50);
        return true;
    }
};
