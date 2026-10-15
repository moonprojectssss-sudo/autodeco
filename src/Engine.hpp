#pragma once
// Made by XYS
// Mesin Auto Deco: baca layout -> taruh deco sesuai tema, mood, dan beat.

#include <Geode/Geode.hpp>
#include <algorithm>
#include <cmath>
#include <limits>
#include <random>
#include <set>
#include <unordered_set>
#include "Themes.hpp"

using namespace geode::prelude;

namespace autodeco {

struct Params {
    int theme = 0;
    int mood = 0;
    int density = 1;
    bool beat = false;
    float bpm = 120.f;
    int speed = 1;
};

struct Result {
    int made = 0;
    int layoutCount = 0;
    bool usedSelection = false;
    std::string error;
};

constexpr float kGrid = 30.f;
constexpr int kMaxObjects = 3500;  // batas biar editor gak lag/crash di HP

// ---------- Palette (daftar ID deco per tema per slot) ----------

inline std::string palKey(int theme, int slot) {
    return fmt::format("pal-{}-{}", kThemes[theme].key, kSlotKeys[slot]);
}

inline std::vector<int> loadPal(int theme, int slot) {
    return Mod::get()->getSavedValue<std::vector<int>>(palKey(theme, slot), std::vector<int>{});
}

// Kalau slot kosong: pakai slot lain di tema yang sama, lalu tema lain.
inline std::vector<int> resolvePal(int theme, int slot) {
    auto p = loadPal(theme, slot);
    if (!p.empty()) return p;
    for (int s = 0; s < 4; s++) {
        auto q = loadPal(theme, s);
        if (!q.empty()) return q;
    }
    for (int t = 0; t < static_cast<int>(kThemes.size()); t++) {
        for (int s = 0; s < 4; s++) {
            auto q = loadPal(t, s);
            if (!q.empty()) return q;
        }
    }
    return {};
}

// ---------- Helper ----------

inline std::unordered_set<int>& badIds() {
    static std::unordered_set<int> s;
    return s;
}

// Bikin 1 objek. Kalau ternyata bukan deco (blok/spike/portal), langsung dihapus lagi.
// Ini pengaman supaya Auto Deco gak pernah nambah objek yang bikin player mati.
inline GameObject* spawn(LevelEditorLayer* lel, int id, CCPoint pos, float scale, float rot, ZLayer z) {
    if (badIds().count(id)) return nullptr;
    auto obj = lel->createObject(id, pos, false);
    if (!obj) {
        badIds().insert(id);
        return nullptr;
    }
    if (obj->m_objectType != GameObjectType::Decoration) {
        lel->removeObject(obj, true);
        badIds().insert(id);
        return nullptr;
    }
    obj->updateCustomScaleX(scale);
    obj->updateCustomScaleY(scale);
    if (rot != 0.f) obj->setRotation(rot);
    obj->m_zLayer = z;  // kalau baris ini error waktu build, hapus aja
    return obj;
}

inline bool isLayoutType(GameObject* o) {
    switch (o->m_objectType) {
        case GameObjectType::Solid:
        case GameObjectType::Hazard:
        case GameObjectType::Slope:
            return true;
        default:
            return false;
    }
}

// Kalau ada objek yang di-select -> itu layout-nya. Kalau tidak -> seluruh level.
inline std::vector<GameObject*> gatherLayout(LevelEditorLayer* lel, EditorUI* ui, bool& usedSelection) {
    std::vector<GameObject*> out;
    CCArray* src = nullptr;
    auto sel = ui->getSelectedObjects();
    if (sel && sel->count() > 0) {
        src = sel;
        usedSelection = true;
    } else {
        src = lel->m_objects;
    }
    if (!src) return out;
    for (auto* o : CCArrayExt<GameObject*>(src)) {
        if (o && isLayoutType(o)) out.push_back(o);
    }
    return out;
}

// ---------- Generator utama ----------

inline Result generate(Params const& p) {
    Result r;
    auto lel = LevelEditorLayer::get();
    auto ui = EditorUI::get();
    if (!lel || !ui) {
        r.error = "Editor belum kebuka.";
        return r;
    }

    auto layout = gatherLayout(lel, ui, r.usedSelection);
    r.layoutCount = static_cast<int>(layout.size());
    if (layout.empty()) {
        r.error = "Layout kosong. Bikin blok/spike dulu, atau select bagian yang mau di-deco.";
        return r;
    }

    auto edgeP = resolvePal(p.theme, Edge);
    auto hangP = resolvePal(p.theme, Hang);
    auto bgP = resolvePal(p.theme, Bg);
    auto beatP = resolvePal(p.theme, Beat);
    if (edgeP.empty()) {
        r.error = "Palette masih kosong. Select objek DECO favoritmu di editor, lalu tekan Edge/Hang/BG/Beat.";
        return r;
    }

    auto const& mood = kMoods[p.mood];
    float dens = kDensityMult[p.density];

    std::mt19937 rng{std::random_device{}()};
    auto rnd = [&]() { return std::uniform_real_distribution<float>(0.f, 1.f)(rng); };
    auto rndR = [&](float a, float b) { return a + (b - a) * rnd(); };

    // Petakan layout ke grid 30x30. Pusat blok ada di 15, 45, 75, ... jadi pakai floor.
    std::set<std::pair<int, int>> occ, solid;
    float minX = std::numeric_limits<float>::max(), maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max(), maxY = std::numeric_limits<float>::lowest();
    for (auto* o : layout) {
        auto pos = o->getPosition();
        int ix = static_cast<int>(std::floor(pos.x / kGrid));
        int iy = static_cast<int>(std::floor(pos.y / kGrid));
        occ.insert({ix, iy});
        if (o->m_objectType != GameObjectType::Hazard) solid.insert({ix, iy});
        minX = std::min(minX, pos.x);
        maxX = std::max(maxX, pos.x);
        minY = std::min(minY, pos.y);
        maxY = std::max(maxY, pos.y);
    }
    auto cellCenter = [](int ix, int iy) {
        return CCPoint{(ix + 0.5f) * kGrid, (iy + 0.5f) * kGrid};
    };

    auto created = CCArray::create();
    auto add = [&](std::vector<int> const& pal, CCPoint pos, float scale, float rot, ZLayer z) {
        if (pal.empty() || r.made >= kMaxObjects) return;
        for (int attempt = 0; attempt < 3; attempt++) {
            int id = pal[rng() % pal.size()];
            if (auto o = spawn(lel, id, pos, scale, rot, z)) {
                created->addObject(o);
                r.made++;
                return;
            }
        }
    };

    // 1) Deco di permukaan atas (edge) dan yang menggantung di bawah (hang)
    for (auto const& [ix, iy] : solid) {
        if (r.made >= kMaxObjects) break;
        if (!occ.count({ix, iy + 1}) && rnd() < mood.edge * dens) {
            add(edgeP, cellCenter(ix, iy + 1), rndR(mood.scaleMin, mood.scaleMax),
                rndR(-mood.rotJitter, mood.rotJitter), ZLayer::B2);
        }
        if (iy > 0 && !occ.count({ix, iy - 1}) && rnd() < mood.hang * dens) {
            add(hangP, cellCenter(ix, iy - 1), rndR(mood.scaleMin, mood.scaleMax),
                rndR(-mood.rotJitter, mood.rotJitter), ZLayer::B2);
        }
    }

    // 2) Deco besar di latar belakang. Kalau beat sync nyala: tiap 1 bar (4 ketukan).
    float bpm = std::clamp(p.bpm, 40.f, 400.f);
    float spacing = kSpeeds[p.speed].unitsPerSec * 60.f / bpm;  // jarak 1 ketukan (unit)
    float stepX = p.beat ? spacing * 4.f : kGrid * 5.f;
    float startX = p.beat ? std::floor(minX / stepX) * stepX : minX;
    for (float x = startX; x <= maxX + kGrid * 3.f; x += stepX) {
        for (float y = std::max(minY - kGrid * 2.f, kGrid * 2.f); y <= maxY + kGrid * 2.f; y += kGrid * 6.f) {
            if (r.made >= kMaxObjects) break;
            int ix = static_cast<int>(std::floor(x / kGrid));
            int iy = static_cast<int>(std::floor(y / kGrid));
            if (occ.count({ix, iy})) continue;
            if (rnd() < mood.bg * dens * 2.f) {
                float jx = p.beat ? 0.f : rndR(-kGrid, kGrid);
                add(bgP, CCPoint{x + jx, y + rndR(-kGrid, kGrid)}, mood.bgScale * rndR(0.8f, 1.2f),
                    rndR(-mood.rotJitter, mood.rotJitter), ZLayer::B4);
            }
        }
    }

    // 3) Aksen ketukan: dua "rel" (atas dan bawah layout). Ketukan ke-1 tiap bar lebih besar.
    if (p.beat) {
        int stride = p.density == 0 ? 4 : (p.density == 1 ? 2 : 1);
        float topY = maxY + kGrid * 4.f;
        float botY = std::max(minY - kGrid * 3.f, kGrid * 1.5f);
        int k0 = std::max(0, static_cast<int>(std::floor(minX / spacing)) - 1);
        int k1 = static_cast<int>(std::ceil(maxX / spacing)) + 1;
        for (int k = k0; k <= k1; k++) {
            bool down = (k % 4 == 0);
            if (!down && (k % stride != 0)) continue;
            float s = down ? 1.6f : 0.8f;
            add(beatP, CCPoint{k * spacing, topY}, s, 0.f, ZLayer::B3);
            add(beatP, CCPoint{k * spacing, botY}, s, 0.f, ZLayer::B3);
        }
    }

    // Select semua deco baru: gak suka? tinggal tekan Trash.
    if (r.made > 0) ui->selectObjects(created, false);
    return r;
}

}  // namespace autodeco
