#pragma once
// Auto Deco - Made by XYS
// Deco generator GUI (written for Geode v5: Popup is no longer a template).

#include <Geode/Geode.hpp>
#include <Geode/ui/Notification.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>
#include <functional>
#include "Engine.hpp"
#include "UiKit.hpp"

using namespace geode::prelude;

class AutoDecoPopup : public geode::Popup {
protected:
    int m_theme = 0;
    int m_mood = 0;
    int m_density = 1;
    int m_speed = 1;
    bool m_beat = false;
    TextInput* m_bpmInput = nullptr;
    CCLabelBMFont* m_status = nullptr;
    CCLabelBMFont* m_moodHint = nullptr;

    void setStatus(std::string const& s) {
        m_status->setString(s.c_str());
        m_status->limitLabelWidth(400.f, 0.55f, 0.2f);
    }

    // Save the deco objects currently selected in the editor into a palette slot.
    void capture(int slot) {
        auto ui = EditorUI::get();
        auto sel = ui ? ui->getSelectedObjects() : nullptr;
        std::vector<int> ids;
        if (sel) {
            for (auto* o : CCArrayExt<GameObject*>(sel)) {
                if (!o || o->m_objectType != GameObjectType::Decoration) continue;
                if (std::find(ids.begin(), ids.end(), o->m_objectID) == ids.end()) {
                    ids.push_back(o->m_objectID);
                }
            }
        }
        if (ids.empty()) {
            setStatus("Select some DECO objects in the editor first (not blocks or spikes).");
            return;
        }
        Mod::get()->setSavedValue<std::vector<int>>(autodeco::palKey(m_theme, slot), ids);
        autodeco::badIds().clear();
        setStatus(fmt::format("Saved {} object type(s) to {} / {}.", ids.size(),
                              autodeco::kThemes[m_theme].name, autodeco::kSlotNames[slot]));
    }

    void resetPalette() {
        for (int s = 0; s < 4; s++) {
            Mod::get()->setSavedValue<std::vector<int>>(autodeco::palKey(m_theme, s), std::vector<int>{});
        }
        setStatus(fmt::format("Palette for {} was reset.", autodeco::kThemes[m_theme].name));
    }

    void saveSettings(float bpm) {
        auto mod = Mod::get();
        mod->setSavedValue<int>("theme", m_theme);
        mod->setSavedValue<int>("mood", m_mood);
        mod->setSavedValue<int>("density", m_density);
        mod->setSavedValue<int>("speed", m_speed);
        mod->setSavedValue<bool>("beat", m_beat);
        mod->setSavedValue<float>("bpm", bpm);
    }

    void onGenerate() {
        float bpm = 120.f;
        if (m_beat) {
            bpm = utils::numFromString<float>(m_bpmInput->getString()).unwrapOr(0.f);
            if (bpm < 40.f || bpm > 400.f) {
                setStatus("BPM must be between 40 and 400.");
                return;
            }
        }
        saveSettings(bpm);

        autodeco::Params p;
        p.theme = m_theme;
        p.mood = m_mood;
        p.density = m_density;
        p.beat = m_beat;
        p.bpm = bpm;
        p.speed = m_speed;

        auto res = autodeco::generate(p);
        if (!res.error.empty()) {
            setStatus(res.error);
            return;
        }
        if (res.made == 0) {
            setStatus("Nothing was placed. Your palette has no valid deco. Save new ones from a selection.");
            return;
        }
        auto msg = fmt::format("Auto Deco: added {} deco objects. They are selected: recolor them or press Trash.", res.made);
        this->keyBackClicked();  // close the popup
        Notification::create(msg, NotificationIcon::Success, 3.5f)->show();
    }

    bool initPopup() {
        if (!Popup::init(440.f, 300.f, "GJ_square01.png", CCRect{0.f, 0.f, 80.f, 80.f})) return false;
        this->setTitle("Auto Deco");

        // Restore the last used settings
        auto mod = Mod::get();
        m_theme = std::clamp(mod->getSavedValue<int>("theme", 0), 0, static_cast<int>(autodeco::kThemes.size()) - 1);
        m_mood = std::clamp(mod->getSavedValue<int>("mood", 0), 0, static_cast<int>(autodeco::kMoods.size()) - 1);
        m_density = std::clamp(mod->getSavedValue<int>("density", 1), 0, 2);
        m_speed = std::clamp(mod->getSavedValue<int>("speed", 1), 0, static_cast<int>(autodeco::kSpeeds.size()) - 1);
        m_beat = mod->getSavedValue<bool>("beat", false);
        float savedBpm = mod->getSavedValue<float>("bpm", 120.f);

        // Panels behind the three control groups
        auto addCard = [&](float cy, float h) {
            m_mainLayer->addChildAtPosition(autodeco::ui::makeCard(410.f, h), Anchor::Center, {0.f, cy});
        };
        addCard(60.f, 100.f);
        addCard(-8.f, 32.f);
        addCard(-48.f, 46.f);

        auto rowLabel = [&](char const* text, float x, float y, float scale) {
            auto l = autodeco::ui::makeLabel(text, "goldFont.fnt", scale);
            l->setAnchorPoint({0.f, 0.5f});
            m_mainLayer->addChildAtPosition(l, Anchor::Center, {x, y});
        };

        // Style: theme, mood, density
        rowLabel("Theme", -196.f, 94.f, 0.6f);
        autodeco::ui::addSelector(m_mainLayer, m_buttonMenu, {60.f, 94.f}, 120.f,
                                  autodeco::kThemes[m_theme].name, [this](int d) {
            int n = static_cast<int>(autodeco::kThemes.size());
            m_theme = (m_theme + d + n) % n;
            setStatus(autodeco::kThemes[m_theme].hint);
            return std::string(autodeco::kThemes[m_theme].name);
        });

        rowLabel("Mood", -196.f, 68.f, 0.6f);
        autodeco::ui::addSelector(m_mainLayer, m_buttonMenu, {60.f, 68.f}, 120.f,
                                  autodeco::kMoods[m_mood].name, [this](int d) {
            int n = static_cast<int>(autodeco::kMoods.size());
            m_mood = (m_mood + d + n) % n;
            m_moodHint->setString(autodeco::kMoods[m_mood].desc);
            m_moodHint->limitLabelWidth(390.f, 0.55f, 0.2f);
            return std::string(autodeco::kMoods[m_mood].name);
        });

        rowLabel("Density", -196.f, 42.f, 0.6f);
        autodeco::ui::addSelector(m_mainLayer, m_buttonMenu, {60.f, 42.f}, 120.f,
                                  autodeco::kDensityNames[m_density], [this](int d) {
            m_density = (m_density + d + 3) % 3;
            return std::string(autodeco::kDensityNames[m_density]);
        });

        m_moodHint = autodeco::ui::makeLabel(autodeco::kMoods[m_mood].desc, "chatFont.fnt", 0.55f);
        m_moodHint->setColor({190, 190, 190});
        m_moodHint->limitLabelWidth(390.f, 0.55f, 0.2f);
        m_mainLayer->addChildAtPosition(m_moodHint, Anchor::Center, {0.f, 20.f});

        // Beat sync: toggle, BPM, speed
        rowLabel("Beat Sync", -196.f, -8.f, 0.5f);
        auto tg = CCMenuItemExt::createTogglerWithStandardSprites(0.5f, [this](CCMenuItemToggler* t) {
            m_beat = !t->isToggled();  // the callback runs before the toggle flips
        });
        tg->toggle(m_beat);
        m_buttonMenu->addChildAtPosition(tg, Anchor::Center, {-80.f, -8.f});

        rowLabel("BPM", -55.f, -8.f, 0.5f);
        m_bpmInput = TextInput::create(70.f, "BPM", "bigFont.fnt");
        m_bpmInput->setCommonFilter(CommonFilter::Float);
        m_bpmInput->setMaxCharCount(6);
        m_bpmInput->setString(fmt::format("{:.0f}", savedBpm));
        m_bpmInput->setScale(0.7f);
        m_mainLayer->addChildAtPosition(m_bpmInput, Anchor::Center, {5.f, -8.f});

        rowLabel("Speed", 52.f, -8.f, 0.45f);
        autodeco::ui::addSelector(m_mainLayer, m_buttonMenu, {150.f, -8.f}, 36.f,
                                  autodeco::kSpeeds[m_speed].name, [this](int d) {
            int n = static_cast<int>(autodeco::kSpeeds.size());
            m_speed = (m_speed + d + n) % n;
            return std::string(autodeco::kSpeeds[m_speed].name);
        });

        // Palette
        auto palLbl = autodeco::ui::makeLabel("Select deco in the editor, then save it to a slot:", "goldFont.fnt", 0.4f);
        palLbl->limitLabelWidth(380.f, 0.4f, 0.15f);
        m_mainLayer->addChildAtPosition(palLbl, Anchor::Center, {0.f, -37.f});

        for (int i = 0; i < 4; i++) {
            auto spr = ButtonSprite::create(autodeco::kSlotNames[i]);
            spr->setScale(0.55f);
            auto b = CCMenuItemExt::createSpriteExtra(spr, [this, i](auto*) { capture(i); });
            m_buttonMenu->addChildAtPosition(b, Anchor::Center, {-150.f + i * 75.f, -60.f});
        }
        auto rsSpr = ButtonSprite::create("Reset");
        rsSpr->setScale(0.55f);
        auto rs = CCMenuItemExt::createSpriteExtra(rsSpr, [this](auto*) { resetPalette(); });
        m_buttonMenu->addChildAtPosition(rs, Anchor::Center, {150.f, -60.f});

        // Main button, status and credit
        auto genSpr = ButtonSprite::create("Generate Deco");
        genSpr->setScale(0.85f);
        auto gen = CCMenuItemExt::createSpriteExtra(genSpr, [this](auto*) { onGenerate(); });
        m_buttonMenu->addChildAtPosition(gen, Anchor::Center, {0.f, -96.f});

        m_status = autodeco::ui::makeLabel(autodeco::kThemes[m_theme].hint, "chatFont.fnt", 0.55f);
        m_status->limitLabelWidth(400.f, 0.55f, 0.2f);
        m_mainLayer->addChildAtPosition(m_status, Anchor::Center, {0.f, -124.f});

        auto credit = autodeco::ui::makeLabel("Made by XYS", "goldFont.fnt", 0.4f);
        m_mainLayer->addChildAtPosition(credit, Anchor::Center, {0.f, -139.f});

        autodeco::ui::addInfoButton(m_buttonMenu, {200.f, 128.f}, "Auto Deco - Help",
            "Theme: the folder your saved deco is stored in.\n"
            "Mood: sets density, size and tilt.\n"
            "Density: how much deco is placed.\n"
            "Beat Sync: enter the song BPM and the level speed. Big pieces land on every bar, small accents on the beats.\n"
            "Palette: select deco objects in the editor, then tap Edge, Hang, BG or Beat. Do this once per theme.\n"
            "After generating, everything stays selected: use Edit Object to recolor it all at once, or press Trash to undo.");
        return true;
    }

public:
    static AutoDecoPopup* create() {
        auto ret = new AutoDecoPopup();
        if (ret->initPopup()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
};
