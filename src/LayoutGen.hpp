#pragma once
// Auto Deco - Made by XYS
// Layout generator: builds a fresh cube layout from a library of jump patterns.
//
// Safety model (cube, normal speed): one jump covers about 4 blocks of ground and
// peaks around 2 blocks. Gaps between obstacle groups are never smaller than the
// distance needed to land and take off again. Everything scales with speed.

#include <Geode/Geode.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <random>
#include <vector>

using namespace geode::prelude;

namespace autodeco::gen {

constexpr int kBlock = 1;
constexpr int kSpike = 8;
constexpr int kPad = 35;  // yellow jump pad
constexpr float kCell = 30.f;
constexpr int kMaxObjects = 4000;

struct Difficulty {
    char const* name;
    char const* desc;
    float gapMin, gapMax;   // flat blocks between obstacle groups (at 1x speed)
    int maxSpikes;          // widest spike group
    float platformWeight;   // how often platforms and stairs appear
    float padWeight;        // how often jump pads appear
    float comboChance;      // chance of a tight combo
    float comboGap;         // gap used inside a combo (never below the safe minimum)
};

inline const std::array<Difficulty, 6> kDifficulties = {{
    {"Easy", "Wide gaps with single or double spikes. Relaxed timing.",
     8.f, 12.f, 2, 0.15f, 0.00f, 0.0f, 5.0f},
    {"Normal", "Adds platforms, stairs and spikes on top of platforms.",
     7.f, 10.f, 2, 0.30f, 0.00f, 0.1f, 4.5f},
    {"Hard", "Triple spikes, double-single rhythms and the odd pad jump.",
     6.f, 9.f, 3, 0.35f, 0.10f, 0.2f, 4.2f},
    {"Harder", "Tighter rhythm, more combos and more pads.",
     5.5f, 8.f, 3, 0.40f, 0.15f, 0.3f, 4.0f},
    {"Insane", "Dense combos with short breathers. Needs fast reactions.",
     4.5f, 7.f, 3, 0.40f, 0.20f, 0.4f, 3.7f},
    {"Demon", "Near-constant action with the tightest gaps this tool allows.",
     4.f, 6.f, 3, 0.45f, 0.25f, 0.5f, 3.5f},
}};

struct Length {
    char const* name;
    float seconds;
};
inline const std::array<Length, 4> kLengths = {{
    {"Tiny (~15s)", 15.f},
    {"Short (~30s)", 30.f},
    {"Medium (~60s)", 60.f},
    {"Long (~90s)", 90.f},
}};

struct SpeedOpt {
    char const* name;
    float ups;     // units per second
    int portalId;  // speed portal object ID
};
inline const std::array<SpeedOpt, 4> kGenSpeeds = {{
    {"1x", 311.58f, 201},
    {"2x", 387.42f, 202},
    {"3x", 468.0f, 203},
    {"4x", 576.0f, 1334},
}};

// ---------- Patterns ----------

struct Piece {
    int id;
    int c;      // column, relative to the pattern start
    int r;      // row above the ground (0 = ground level)
    float rot;
};

enum Kind { S1, S2, S3, Plat, PlatSpike, Stairs, Pad, DoubleSingle, KindCount };

struct Pattern {
    std::vector<Piece> pieces;
    int width = 0;
    int spikeW = 1;         // widest spike group the player has to clear last
    float extraAfter = 0.f; // extra safe distance needed after this pattern
};

// Minimum flat distance (in blocks) needed after a pattern before the next one.
inline float minGapAfter(Pattern const& p, float f) {
    return (3.5f + 0.7f * static_cast<float>(std::max(p.spikeW, 1) - 1)) * f + p.extraAfter;
}

inline Pattern makeSpikes(int n) {
    Pattern p;
    p.spikeW = n;
    p.width = n;
    for (int i = 0; i < n; i++) p.pieces.push_back({kSpike, i, 0, 0.f});
    return p;
}

inline Pattern makePlatform(int w) {
    Pattern p;
    p.width = w;
    p.spikeW = 1;
    p.extraAfter = 0.5f;
    for (int i = 0; i < w; i++) p.pieces.push_back({kBlock, i, 0, 0.f});
    return p;
}

// Long platform with one spike in the middle (odd width keeps it centered).
inline Pattern makePlatformSpike(int w) {
    auto p = makePlatform(w);
    p.pieces.push_back({kSpike, w / 2, 1, 0.f});
    return p;
}

// One-block step up, then a two-block step.
inline Pattern makeStairs() {
    Pattern p;
    p.width = 6;
    p.spikeW = 1;
    p.extraAfter = 0.5f;
    for (int i = 0; i < 3; i++) p.pieces.push_back({kBlock, i, 0, 0.f});
    for (int i = 0; i < 3; i++) {
        p.pieces.push_back({kBlock, 3 + i, 0, 0.f});
        p.pieces.push_back({kBlock, 3 + i, 1, 0.f});
    }
    return p;
}

// Yellow pad followed by three spikes: the pad throws you over them.
inline Pattern makePad() {
    Pattern p;
    p.spikeW = 3;
    p.extraAfter = 1.f;
    p.pieces.push_back({kPad, 0, 0, 0.f});
    for (int i = 0; i < 3; i++) p.pieces.push_back({kSpike, 2 + i, 0, 0.f});
    p.width = 5;
    return p;
}

// Two single spikes with just enough room between them for a second jump.
inline Pattern makeDoubleSingle(float f) {
    int gap = static_cast<int>(std::ceil(3.5f * f));
    Pattern p;
    p.spikeW = 1;
    p.pieces.push_back({kSpike, 0, 0, 0.f});
    p.pieces.push_back({kSpike, 1 + gap, 0, 0.f});
    p.width = 2 + gap;
    return p;
}

// ---------- Generator ----------

struct Result {
    int made = 0;
    int cols = 0;
    uint32_t seed = 0;
    std::string error;
};

inline Result generate(int diffIdx, int lenIdx, int speedIdx, uint32_t seed) {
    Result res;
    res.seed = seed;
    auto lel = LevelEditorLayer::get();
    auto ui = EditorUI::get();
    if (!lel || !ui) {
        res.error = "The editor is not open.";
        return res;
    }

    auto const& d = kDifficulties[diffIdx];
    auto const& sp = kGenSpeeds[speedIdx];
    float f = sp.ups / 311.58f;  // jump distances scale with speed

    std::mt19937 rng(seed);
    auto rnd = [&]() { return std::min(std::uniform_real_distribution<float>(0.f, 1.f)(rng), 0.9999f); };
    auto rndR = [&](float a, float b) { return a + (b - a) * rnd(); };

    // Start after anything that already exists, or near the beginning of an empty level.
    int startC = 10;
    bool hasContent = false;
    float maxX = 0.f;
    if (lel->m_objects) {
        for (auto* o : CCArrayExt<GameObject*>(lel->m_objects)) {
            if (!o) continue;
            hasContent = true;
            maxX = std::max(maxX, o->getPositionX());
        }
    }
    if (hasContent) startC = static_cast<int>(std::floor(maxX / kCell)) + 8;
    int endC = startC + static_cast<int>(std::round(kLengths[lenIdx].seconds * sp.ups / kCell));

    // Pattern weights for this difficulty, plus a random "motif": three favourite
    // patterns get boosted, so every generated layout has its own character.
    std::array<float, KindCount> w{};
    w[S1] = 4.f;
    w[S2] = d.maxSpikes >= 2 ? 3.f : 0.f;
    w[S3] = d.maxSpikes >= 3 ? 2.f : 0.f;
    w[Plat] = d.platformWeight * 6.f;
    w[PlatSpike] = diffIdx >= 1 ? d.platformWeight * 5.f : 0.f;
    w[Stairs] = diffIdx >= 1 ? d.platformWeight * 4.f : 0.f;
    w[Pad] = d.padWeight * 10.f;
    w[DoubleSingle] = diffIdx >= 2 ? 2.f : 0.f;

    std::vector<int> avail;
    for (int k = 0; k < KindCount; k++) {
        if (w[k] > 0.f) avail.push_back(k);
    }
    std::shuffle(avail.begin(), avail.end(), rng);
    for (size_t i = 0; i < avail.size() && i < 3; i++) w[avail[i]] *= 3.f;

    auto pick = [&](int last1, int last2) {
        auto ww = w;
        if (last1 >= 0 && last1 == last2) ww[last1] = 0.f;  // never the same pattern three times
        float total = 0.f;
        for (float x : ww) total += x;
        if (total <= 0.f) return static_cast<int>(S1);
        float t = rnd() * total;
        for (int k = 0; k < KindCount; k++) {
            if (ww[k] <= 0.f) continue;
            t -= ww[k];
            if (t <= 0.f) return k;
        }
        return static_cast<int>(S1);
    };

    auto build = [&](int k) {
        switch (k) {
            case S1: return makeSpikes(1);
            case S2: return makeSpikes(2);
            case S3: return makeSpikes(3);
            case Plat: return makePlatform(3 + static_cast<int>(rnd() * 3.f));
            case PlatSpike: return makePlatformSpike(rnd() < 0.5f ? 7 : 9);
            case Stairs: return makeStairs();
            case Pad: return makePad();
            case DoubleSingle: return makeDoubleSingle(f);
            default: return makeSpikes(1);
        }
    };

    auto created = CCArray::create();
    auto put = [&](Piece const& pc, int baseC) {
        if (res.made >= kMaxObjects) return;
        auto o = lel->createObject(pc.id, CCPoint{(baseC + pc.c + 0.5f) * kCell, (pc.r + 0.5f) * kCell}, false);
        if (!o) return;
        if (pc.rot != 0.f) o->setRotation(pc.rot);
        created->addObject(o);
        res.made++;
    };

    // Speed portal at the start (skipped for 1x in an empty level).
    if (hasContent || speedIdx != 0) put({sp.portalId, 0, 1, 0.f}, startC);

    int c = startC + static_cast<int>(std::ceil(12.f * f));  // calm intro
    Pattern prev;
    bool hasPrev = false;
    int last1 = -1, last2 = -1;
    while (c < endC && res.made < kMaxObjects) {
        int k = pick(last1, last2);
        Pattern p = build(k);

        float g = rndR(d.gapMin, d.gapMax) * f;
        if (hasPrev && rnd() < d.comboChance) g = d.comboGap * f;
        if (hasPrev) g = std::max(g, minGapAfter(prev, f));
        c += static_cast<int>(std::ceil(g));
        if (c >= endC) break;

        for (auto const& pc : p.pieces) put(pc, c);
        c += p.width;
        prev = p;
        hasPrev = true;
        last2 = last1;
        last1 = k;
    }

    res.cols = c - startC;
    if (res.made > 0) ui->selectObjects(created, false);
    return res;
}

}  // namespace autodeco::gen
