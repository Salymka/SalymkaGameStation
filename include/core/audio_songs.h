#pragma once
#include <Arduino.h>

namespace Album {

struct Note {
    uint16_t freq;
    uint16_t durationMs;
};

struct Song {
    const Note* notes;
    uint16_t length;
};

// 0 freq = pause
constexpr Note SONG_1_NOTES[] = {
    {1200, 80},
    {0,    30},
    {1600, 80},
    {0,    30},
    {2000, 120},
};

constexpr Note SONG_2_NOTES[] = {
    {2000, 70},
    {1700, 70},
    {1400, 100},
};

constexpr Note SONG_CLICK_NOTES[] = {
    {1800, 20},
};

constexpr Note SONG_SUCCESS_NOTES[] = {
    {1400, 60},
    {1800, 60},
    {2300, 100},
};

constexpr Note SONG_FAIL_NOTES[] = {
    {1800, 80},
    {1200, 140},
};

constexpr Note HELLO_NOTES[] = {
    {1000, 70},
    {1600, 70},
    {0,    20},
    {1800, 60},
};

constexpr Note IMPOSTOR_KILL_NOTES[] = {
    {3500, 20},
    {3000, 20},
    {2500, 20},
    {2000, 25},
    {1500, 30},
    {1000, 50},

    {0,    15},

    {3000, 15},
    {900,  60},
};

constexpr Note HELLO_TA_NOTES[] = {
    {1000, 60},   // ta
    {0,    40},
    {1200, 90},  // taa
    {0,    40},
    {1700, 180},  // taaaaa
};

constexpr Song song_1        = { SONG_1_NOTES,       sizeof(SONG_1_NOTES)       / sizeof(Note) };
constexpr Song song_2        = { SONG_2_NOTES,       sizeof(SONG_2_NOTES)       / sizeof(Note) };
constexpr Song click         = { SONG_CLICK_NOTES,   sizeof(SONG_CLICK_NOTES)   / sizeof(Note) };
constexpr Song success       = { SONG_SUCCESS_NOTES, sizeof(SONG_SUCCESS_NOTES) / sizeof(Note) };
constexpr Song fail          = { SONG_FAIL_NOTES,    sizeof(SONG_FAIL_NOTES)    / sizeof(Note) };
constexpr Song hello         = { HELLO_NOTES,        sizeof(HELLO_NOTES)        / sizeof(Note) };
constexpr Song impostor_kill = { IMPOSTOR_KILL_NOTES,sizeof(IMPOSTOR_KILL_NOTES) / sizeof(Note) };
constexpr Song hello_ta      = { HELLO_TA_NOTES,     sizeof(HELLO_TA_NOTES)     / sizeof(Note) };

} // namespace Album