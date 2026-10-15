#pragma once
// Made by XYS
// Semua pilihan tema / mood / kecepatan ada di sini. Gampang ditambah.

#include <array>

namespace autodeco {

struct Theme {
    char const* key;   // dipakai buat nama penyimpanan palette (jangan diganti-ganti)
    char const* name;  // tampil di GUI
};

inline const std::array<Theme, 5> kThemes = {{
    {"space", "Space"},
    {"slaughter", "Slaughter"},
    {"neon", "Neon"},
    {"forest", "Forest"},
    {"cyber", "Cyber"},
}};

struct Mood {
    char const* key;
    char const* name;
    float edge;        // peluang deco di permukaan atas blok (0..1)
    float hang;        // peluang deco yang menggantung di bawah blok
    float bg;          // peluang deco besar di latar belakang
    float scaleMin;    // ukuran deco kecil
    float scaleMax;
    float rotJitter;   // acak rotasi (derajat)
    float bgScale;     // ukuran deco besar
};

inline const std::array<Mood, 4> kMoods = {{
    //  key               nama              edge  hang  bg    sMin  sMax  rot   bgScale
    {"clean",          "Clean",          0.30f, 0.15f, 0.10f, 0.8f, 1.1f,  0.f, 1.2f},
    {"slaughterhouse", "Slaughterhouse", 0.60f, 0.55f, 0.12f, 0.9f, 1.4f, 25.f, 1.6f},
    {"limbo",          "Limbo",          0.18f, 0.10f, 0.16f, 0.9f, 1.2f,  0.f, 2.2f},
    {"dreamy",         "Dreamy",         0.30f, 0.20f, 0.22f, 1.0f, 1.5f, 10.f, 1.8f},
}};

inline const std::array<char const*, 3> kDensityNames = {"Low", "Medium", "High"};
inline const std::array<float, 3> kDensityMult = {0.6f, 1.0f, 1.6f};

struct Speed {
    char const* name;
    float unitsPerSec;  // kecepatan player dalam unit GD per detik
};

inline const std::array<Speed, 5> kSpeeds = {{
    {"0.7x (Slow)", 251.16f},
    {"1x (Normal)", 311.58f},
    {"2x", 387.42f},
    {"3x", 468.0f},
    {"4x", 576.0f},
}};

enum Slot { Edge = 0, Hang = 1, Bg = 2, Beat = 3 };
inline const std::array<char const*, 4> kSlotKeys = {"edge", "hang", "bg", "beat"};
inline const std::array<char const*, 4> kSlotNames = {"Edge", "Hang", "BG", "Beat"};

}  // namespace autodeco
