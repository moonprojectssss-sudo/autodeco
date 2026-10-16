#pragma once
// Auto Deco - Made by XYS
// Layout generator GUI.

#include <Geode/Geode.hpp>
#include <Geode/ui/Notification.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>
#include <random>
#include "LayoutGen.hpp"
#include "UiKit.hpp"

using namespace geode::prelude;

class AutoDecoLayoutPopup : public geode::Popup {
protected:
    int m_diff = 1;
    int m_len = 1;
    int m_speed = 0;
    TextInput* m_seedInput = nullptr;
    CCLabelBMFont* m_status = nullptr;
    CCLabelBMFont* m_diffHint = nullptr;

    void setStatus(std::string const& s) {
        m_status->setString(s.c_str());
        m_status->limitLabelWidth(380.f, 0.55f, 0.2f);
    }

    void updateHint() {
        m_diffHint->setString(autodeco::gen::kDifficulties[m_diff].desc);
        m_diffHint->limitLabelWidth(380.f, 0.6f, 0.2f);
    }

    void onGenerate() {
        auto mod = Mod::get();
        mod->setSavedValue<int>("lg-diff", m_diff);
        mod->setSavedValue<int>("lg-len", m_len);
        mod->setSavedValue<int>("lg-speed", m_speed);

        // Empty or invalid seed = random layout. Reuse a seed to get the same layout again.
        int parsed = utils::numFromString<int>(m_seedInput->getString()).unwrapOr(0);
        uint32_t seed = parsed > 0 ? static_cast<uint32_t>(parsed)
                                   : static_cast<uint32_t>(std::random_device{}() % 999999u + 1u);
        m_seedInput->setString(std::to_string(seed));

        auto res = autodeco::gen::generate(m_diff, m_len, m_speed, seed);
        if (!res.error.empty()) {
            setStatus(res.error);
            return;
        }
        if (res.made == 0) {
            setStatus("Nothing was generated. Try a longer length.");
            return;
        }
        auto msg = fmt::format("Layout ready: {} objects, seed {}. Playtest it!", res.made, res.seed);
        this->keyBackClicked();  // close the popup
        Notification::create(msg, NotificationIcon::Success, 3.5f)->show();
    }

    bool initPopup() {
        if (!Popup::init(420.f, 280.f, "GJ_square01.png", CCRect{0.f, 0.f, 80.f, 80.f})) return false;
        this->setTitle("Layout Generator");

        auto mod = Mod::get();
        m_diff = std::clamp(mod->getSavedValue<int>("lg-diff", 1), 0, static_cast<int>(autodeco::gen::kDifficulties.size()) - 1);
        m_len = std::clamp(mod->getSavedValue<int>("lg-len", 1), 0, static_cast<int>(autodeco::gen::kLengths.size()) - 1);
        m_speed = std::clamp(mod->getSavedValue<int>("lg-speed", 0), 0, static_cast<int>(autodeco::gen::kGenSpeeds.size()) - 1);

        auto addCard = [&](float cy, float h) {
            m_mainLayer->addChildAtPosition(autodeco::ui::makeCard(390.f, h), Anchor::Center, {0.f, cy});
        };
        addCard(49.f, 112.f);
        addCard(-26.f, 36.f);

        auto rowLabel = [&](char const* text, float y) {
            auto l = autodeco::ui::makeLabel(text, "goldFont.fnt", 0.6f);
            l->setAnchorPoint({0.f, 0.5f});
            m_mainLayer->addChildAtPosition(l, Anchor::Center, {-182.f, y});
        };

        rowLabel("Difficulty", 88.f);
        autodeco::ui::addSelector(m_mainLayer, m_buttonMenu, {70.f, 88.f}, 110.f,
                                  autodeco::gen::kDifficulties[m_diff].name, [this](int d) {
            int n = static_cast<int>(autodeco::gen::kDifficulties.size());
            m_diff = (m_diff + d + n) % n;
            updateHint();
            return std::string(autodeco::gen::kDifficulties[m_diff].name);
        });

        rowLabel("Length", 62.f);
        autodeco::ui::addSelector(m_mainLayer, m_buttonMenu, {70.f, 62.f}, 110.f,
                                  autodeco::gen::kLengths[m_len].name, [this](int d) {
            int n = static_cast<int>(autodeco::gen::kLengths.size());
            m_len = (m_len + d + n) % n;
            return std::string(autodeco::gen::kLengths[m_len].name);
        });

        rowLabel("Speed", 36.f);
        autodeco::ui::addSelector(m_mainLayer, m_buttonMenu, {70.f, 36.f}, 110.f,
                                  autodeco::gen::kGenSpeeds[m_speed].name, [this](int d) {
            int n = static_cast<int>(autodeco::gen::kGenSpeeds.size());
            m_speed = (m_speed + d + n) % n;
            return std::string(autodeco::gen::kGenSpeeds[m_speed].name);
        });

        rowLabel("Seed", 10.f);
        m_seedInput = TextInput::create(130.f, "empty = random", "bigFont.fnt");
        m_seedInput->setCommonFilter(CommonFilter::Int);
        m_seedInput->setMaxCharCount(7);
        m_mainLayer->addChildAtPosition(m_seedInput, Anchor::Center, {70.f, 10.f});

        m_diffHint = autodeco::ui::makeLabel(autodeco::gen::kDifficulties[m_diff].desc, "chatFont.fnt", 0.6f);
        m_diffHint->setColor({190, 190, 190});
        m_diffHint->limitLabelWidth(380.f, 0.6f, 0.2f);
        m_mainLayer->addChildAtPosition(m_diffHint, Anchor::Center, {0.f, -26.f});

        auto genSpr = ButtonSprite::create("Generate Layout");
        genSpr->setScale(0.9f);
        auto gen = CCMenuItemExt::createSpriteExtra(genSpr, [this](auto*) { onGenerate(); });
        m_buttonMenu->addChildAtPosition(gen, Anchor::Center, {0.f, -72.f});

        m_status = autodeco::ui::makeLabel("New content is added after anything already in your level.", "chatFont.fnt", 0.55f);
        m_status->limitLabelWidth(380.f, 0.55f, 0.2f);
        m_mainLayer->addChildAtPosition(m_status, Anchor::Center, {0.f, -102.f});

        auto credit = autodeco::ui::makeLabel("Made by XYS", "goldFont.fnt", 0.4f);
        m_mainLayer->addChildAtPosition(credit, Anchor::Center, {0.f, -124.f});

        autodeco::ui::addInfoButton(m_buttonMenu, {180.f, 118.f}, "Layout Generator - Help",
            "Builds a fresh cube layout every time.\n"
            "Difficulty sets gap size, spike width and how often platforms, stairs and pads appear.\n"
            "Length is the level time at the chosen speed.\n"
            "Seed: leave it empty for a random layout, or type a seed to get the same layout again.\n"
            "New content is added after anything already in the level, and stays selected.\n"
            "Always playtest the result: the tool follows safe jump distances but cannot play for you.");
        return true;
    }

public:
    static AutoDecoLayoutPopup* create() {
        auto ret = new AutoDecoLayoutPopup();
        if (ret->initPopup()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
};
