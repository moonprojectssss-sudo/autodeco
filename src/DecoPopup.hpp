#pragma once
// Made by XYS
// GUI Auto Deco. Ditulis buat Geode v5 (Popup tidak lagi template).

#include <Geode/Geode.hpp>
#include <Geode/ui/Notification.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>
#include <functional>
#include "Engine.hpp"

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

    void setStatus(std::string const& s) {
        m_status->setString(s.c_str());
        m_status->limitLabelWidth(360.f, 0.55f, 0.2f);
    }

    // Baris pilihan:  Judul   <  Nilai  >
    void addCycle(char const* title, float y, std::string initial, std::function<std::string(int)> step) {
        auto lbl = CCLabelBMFont::create(title, "goldFont.fnt");
        lbl->setScale(0.55f);
        lbl->setAnchorPoint({0.f, 0.5f});
        m_mainLayer->addChildAtPosition(lbl, Anchor::Center, {-170.f, y});

        auto val = CCLabelBMFont::create(initial.c_str(), "bigFont.fnt");
        val->limitLabelWidth(100.f, 0.5f, 0.2f);
        m_mainLayer->addChildAtPosition(val, Anchor::Center, {40.f, y});

        auto apply = [val, step](int d) {
            val->setString(step(d).c_str());
            val->limitLabelWidth(100.f, 0.5f, 0.2f);
        };

        auto l = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        l->setScale(0.6f);
        auto r = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        r->setScale(0.6f);
        r->setFlipX(true);
        auto lb = CCMenuItemExt::createSpriteExtra(l, [apply](auto*) { apply(-1); });
        auto rb = CCMenuItemExt::createSpriteExtra(r, [apply](auto*) { apply(+1); });
        m_buttonMenu->addChildAtPosition(lb, Anchor::Center, {-30.f, y});
        m_buttonMenu->addChildAtPosition(rb, Anchor::Center, {110.f, y});
    }

    // Simpan deco yang lagi di-select di editor ke palette (tema aktif + slot)
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
            setStatus("Select dulu objek DECO (bukan blok/spike) di editor.");
            return;
        }
        Mod::get()->setSavedValue<std::vector<int>>(autodeco::palKey(m_theme, slot), ids);
        autodeco::badIds().clear();
        setStatus(fmt::format("{}: {} ID disimpan ke tema {}", autodeco::kSlotNames[slot], ids.size(),
                              autodeco::kThemes[m_theme].name));
    }

    void resetPalette() {
        for (int s = 0; s < 4; s++) {
            Mod::get()->setSavedValue<std::vector<int>>(autodeco::palKey(m_theme, s), std::vector<int>{});
        }
        setStatus(fmt::format("Palette tema {} direset.", autodeco::kThemes[m_theme].name));
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
                setStatus("BPM harus antara 40 sampai 400.");
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
            setStatus("0 deco berhasil. Palette berisi ID yang bukan deco, ambil ulang dari seleksi.");
            return;
        }
        auto msg = fmt::format("Auto Deco: {} deco ditambah (udah ke-select, Trash kalau gak suka)", res.made);
        this->keyBackClicked();  // tutup popup
        Notification::create(msg, NotificationIcon::Success, 3.f)->show();
    }

    bool initPopup() {
        if (!Popup::init(400.f, 290.f, "GJ_square01.png", CCRect{0.f, 0.f, 80.f, 80.f})) return false;
        this->setTitle("Auto Deco");

        // Muat pilihan terakhir
        auto mod = Mod::get();
        m_theme = std::clamp(mod->getSavedValue<int>("theme", 0), 0, static_cast<int>(autodeco::kThemes.size()) - 1);
        m_mood = std::clamp(mod->getSavedValue<int>("mood", 0), 0, static_cast<int>(autodeco::kMoods.size()) - 1);
        m_density = std::clamp(mod->getSavedValue<int>("density", 1), 0, 2);
        m_speed = std::clamp(mod->getSavedValue<int>("speed", 1), 0, static_cast<int>(autodeco::kSpeeds.size()) - 1);
        m_beat = mod->getSavedValue<bool>("beat", false);
        float savedBpm = mod->getSavedValue<float>("bpm", 120.f);

        addCycle("Tema", 92.f, autodeco::kThemes[m_theme].name, [this](int d) {
            int n = static_cast<int>(autodeco::kThemes.size());
            m_theme = (m_theme + d + n) % n;
            return std::string(autodeco::kThemes[m_theme].name);
        });
        addCycle("Mood", 66.f, autodeco::kMoods[m_mood].name, [this](int d) {
            int n = static_cast<int>(autodeco::kMoods.size());
            m_mood = (m_mood + d + n) % n;
            return std::string(autodeco::kMoods[m_mood].name);
        });
        addCycle("Density", 40.f, autodeco::kDensityNames[m_density], [this](int d) {
            m_density = (m_density + d + 3) % 3;
            return std::string(autodeco::kDensityNames[m_density]);
        });

        // Beat sync: toggle + BPM
        auto beatLbl = CCLabelBMFont::create("Beat Sync", "goldFont.fnt");
        beatLbl->setScale(0.55f);
        beatLbl->setAnchorPoint({0.f, 0.5f});
        m_mainLayer->addChildAtPosition(beatLbl, Anchor::Center, {-170.f, 14.f});

        auto tg = CCMenuItemExt::createTogglerWithStandardSprites(0.55f, [this](CCMenuItemToggler* t) {
            m_beat = !t->isToggled();  // callback jalan sebelum toggle berubah
        });
        tg->toggle(m_beat);
        m_buttonMenu->addChildAtPosition(tg, Anchor::Center, {-30.f, 14.f});

        auto bpmLbl = CCLabelBMFont::create("BPM", "goldFont.fnt");
        bpmLbl->setScale(0.5f);
        m_mainLayer->addChildAtPosition(bpmLbl, Anchor::Center, {20.f, 14.f});

        m_bpmInput = TextInput::create(70.f, "BPM", "bigFont.fnt");
        m_bpmInput->setCommonFilter(CommonFilter::Float);
        m_bpmInput->setMaxCharCount(6);
        m_bpmInput->setString(fmt::format("{:.0f}", savedBpm));
        m_bpmInput->setScale(0.8f);
        m_mainLayer->addChildAtPosition(m_bpmInput, Anchor::Center, {95.f, 14.f});

        addCycle("Kecepatan", -12.f, autodeco::kSpeeds[m_speed].name, [this](int d) {
            int n = static_cast<int>(autodeco::kSpeeds.size());
            m_speed = (m_speed + d + n) % n;
            return std::string(autodeco::kSpeeds[m_speed].name);
        });

        // Palette
        auto palLbl = CCLabelBMFont::create("Simpan deco yang kamu select ke palette:", "goldFont.fnt");
        palLbl->setScale(0.4f);
        m_mainLayer->addChildAtPosition(palLbl, Anchor::Center, {0.f, -36.f});

        for (int i = 0; i < 4; i++) {
            auto spr = ButtonSprite::create(autodeco::kSlotNames[i]);
            spr->setScale(0.55f);
            auto b = CCMenuItemExt::createSpriteExtra(spr, [this, i](auto*) { capture(i); });
            m_buttonMenu->addChildAtPosition(b, Anchor::Center, {-135.f + i * 70.f, -56.f});
        }
        auto rsSpr = ButtonSprite::create("Reset");
        rsSpr->setScale(0.55f);
        auto rs = CCMenuItemExt::createSpriteExtra(rsSpr, [this](auto*) { resetPalette(); });
        m_buttonMenu->addChildAtPosition(rs, Anchor::Center, {145.f, -56.f});

        // Tombol utama
        auto genSpr = ButtonSprite::create("Generate!");
        genSpr->setScale(0.9f);
        auto gen = CCMenuItemExt::createSpriteExtra(genSpr, [this](auto*) { onGenerate(); });
        m_buttonMenu->addChildAtPosition(gen, Anchor::Center, {0.f, -92.f});

        m_status = CCLabelBMFont::create("", "chatFont.fnt");
        m_status->setScale(0.55f);
        m_mainLayer->addChildAtPosition(m_status, Anchor::Center, {0.f, -114.f});

        auto credit = CCLabelBMFont::create("Made by XYS", "goldFont.fnt");
        credit->setScale(0.45f);
        m_mainLayer->addChildAtPosition(credit, Anchor::Center, {0.f, -132.f});

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
