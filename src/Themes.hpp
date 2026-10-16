#pragma once
// Auto Deco - Made by XYS
// All themes, moods and speeds live here. Easy to extend.

#include <array>

namespace autodeco {

struct Theme {
    char const* key;   // used in the saved palette name, do not rename
    char const* name;  // shown in the GUI
    char const* hint;  // tip about what to save into this theme
};

inline const std::array<Theme, 5> kThemes = {{
    {"space", "Space", "Space: save stars, planets, gears and glowing blocks."},
    {"slaughter", "Slaughter", "Slaughter: save saws, hooks, chains, skulls and pentagrams."},
    {"neon", "Neon", "Neon: save outlines, light lines and glowing shapes."},
    {"forest", "Forest", "Forest: save trees, leaves, vines and rocks."},
    {"cyber", "Cyber", "Cyber: save circuits, panels, grids and arrows."},
}};

struct Mood {
    char const* key;
    char const* name;
    float edge;        // chance of deco on top of a block surface (0..1)
    float hang;        // chance of deco hanging below a block
    float bg;          // chance of a big background piece
    float scaleMin;    // size range of small deco
    float scaleMax;
    float rotJitter;   // random tilt in degrees
    float bgScale;     // size of big background pieces
    char const* desc;  // shown under the mood selector
};

inline const std::array<Mood, 4> kMoods = {{
    {"clean", "Clean", 0.30f, 0.15f, 0.10f, 0.8f, 1.1f, 0.f, 1.2f,
     "Tidy and sparse accents. Great for modern and glow styles."},
    {"slaughterhouse", "Slaughterhouse", 0.60f, 0.55f, 0.12f, 0.9f, 1.4f, 25.f, 1.6f,
     "Dense and chaotic: lots of hanging pieces and tilted deco."},
    {"limbo", "Limbo", 0.18f, 0.10f, 0.16f, 0.9f, 1.2f, 0.f, 2.2f,
     "Very sparse, with huge silhouettes in the background."},
    {"dreamy", "Dreamy", 0.30f, 0.20f, 0.22f, 1.0f, 1.5f, 10.f, 1.8f,
     "Soft and floaty: large background pieces with a slight tilt."},
}};

inline const std::array<char const*, 3> kDensityNames = {"Low", "Medium", "High"};
inline const std::array<float, 3> kDensityMult = {0.6f, 1.0f, 1.6f};

struct Speed {
    char const* name;
    float unitsPerSec;  // player speed in GD units per second
};

inline const std::array<Speed, 5> kSpeeds = {{
    {"0.7x", 251.16f},
    {"1x", 311.58f},
    {"2x", 387.42f},
    {"3x", 468.0f},
    {"4x", 576.0f},
}};

enum Slot { Edge = 0, Hang = 1, Bg = 2, Beat = 3 };
inline const std::array<char const*, 4> kSlotKeys = {"edge", "hang", "bg", "beat"};
inline const std::array<char const*, 4> kSlotNames = {"Edge", "Hang", "BG", "Beat"};

}  // namespace autodeco
