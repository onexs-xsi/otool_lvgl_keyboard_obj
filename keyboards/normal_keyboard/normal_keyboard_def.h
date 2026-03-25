/*
 * SPDX-FileCopyrightText: 2025-2026 exia
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef NORMAL_KEYBOARD_DEF_H
#define NORMAL_KEYBOARD_DEF_H

#include <stdint.h>

#define NORMAL_KEYBOARD_ROWS 4
#define NORMAL_KEYBOARD_COLS 10
#define NORMAL_KEYBOARD_KEY_COUNT (NORMAL_KEYBOARD_ROWS * NORMAL_KEYBOARD_COLS)

#define NORMAL_KB_PREFERRED_ASPECT_RATIO  2.4f
#define NORMAL_KB_MIN_ASPECT_RATIO        1.8f
#define NORMAL_KB_MAX_ASPECT_RATIO        3.2f
#define NORMAL_KB_MAX_SCREEN_HEIGHT_PCT   0.40f

#define NORMAL_KB_COLOR_BG_CONTAINER      0x202124
#define NORMAL_KB_COLOR_BG_NORMAL         0xF0F0F0
#define NORMAL_KB_COLOR_TXT_NORMAL        0x000000
#define NORMAL_KB_COLOR_BG_SHIFT          0xE06030
#define NORMAL_KB_COLOR_BG_SHIFT_ACTIVE   0xFF8A50
#define NORMAL_KB_COLOR_BG_BLACK          0x000000
#define NORMAL_KB_COLOR_TXT_WHITE         0xFFFFFF
#define NORMAL_KB_COLOR_BG_SPACE          0xC0C0C0
#define NORMAL_KB_COLOR_BG_DISABLED       0x9E9E9E
#define NORMAL_KB_COLOR_TXT_DISABLED      0xE0E0E0

typedef enum {
    NORMAL_CASE_MODE_LOWER = 0,
    NORMAL_CASE_MODE_UPPER
} normal_case_mode_t;

typedef enum {
    NORMAL_KEY_1 = 0,
    NORMAL_KEY_2,
    NORMAL_KEY_3,
    NORMAL_KEY_4,
    NORMAL_KEY_5,
    NORMAL_KEY_6,
    NORMAL_KEY_7,
    NORMAL_KEY_8,
    NORMAL_KEY_9,
    NORMAL_KEY_0,

    NORMAL_KEY_Q,
    NORMAL_KEY_W,
    NORMAL_KEY_E,
    NORMAL_KEY_R,
    NORMAL_KEY_T,
    NORMAL_KEY_Y,
    NORMAL_KEY_U,
    NORMAL_KEY_I,
    NORMAL_KEY_O,
    NORMAL_KEY_P,

    NORMAL_KEY_A,
    NORMAL_KEY_S,
    NORMAL_KEY_D,
    NORMAL_KEY_F,
    NORMAL_KEY_G,
    NORMAL_KEY_H,
    NORMAL_KEY_J,
    NORMAL_KEY_K,
    NORMAL_KEY_L,
    NORMAL_KEY_BS,

    NORMAL_KEY_SHIFT,
    NORMAL_KEY_Z,
    NORMAL_KEY_X,
    NORMAL_KEY_C,
    NORMAL_KEY_V,
    NORMAL_KEY_B,
    NORMAL_KEY_N,
    NORMAL_KEY_M,
    NORMAL_KEY_SPACE,
    NORMAL_KEY_OK,

    NORMAL_KEY_COUNT
} normal_key_id_t;

typedef enum {
    NORMAL_KEY_TYPE_CHAR = 0,
    NORMAL_KEY_TYPE_SHIFT,
    NORMAL_KEY_TYPE_BACKSPACE,
    NORMAL_KEY_TYPE_SPACE,
    NORMAL_KEY_TYPE_CONFIRM
} normal_key_type_t;

typedef struct {
    uint16_t id;
    uint8_t row;
    uint8_t col;
    uint8_t width;
    const char* label_lower;
    const char* value_lower;
    const char* label_upper;
    const char* value_upper;
    normal_key_type_t type;
    bool disabled;
} normal_key_t;

#define NORMAL_KEY_ITEM(id, row, col, width, ll, vl, lu, vu, type) \
    { id, row, col, width, ll, vl, lu, vu, type, false }

static const normal_key_t normal_key_map[NORMAL_KEY_COUNT] = {
    NORMAL_KEY_ITEM(NORMAL_KEY_1,     0, 0, 1, "1",  "1",  "1",  "1",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_2,     0, 1, 1, "2",  "2",  "2",  "2",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_3,     0, 2, 1, "3",  "3",  "3",  "3",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_4,     0, 3, 1, "4",  "4",  "4",  "4",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_5,     0, 4, 1, "5",  "5",  "5",  "5",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_6,     0, 5, 1, "6",  "6",  "6",  "6",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_7,     0, 6, 1, "7",  "7",  "7",  "7",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_8,     0, 7, 1, "8",  "8",  "8",  "8",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_9,     0, 8, 1, "9",  "9",  "9",  "9",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_0,     0, 9, 1, "0",  "0",  "0",  "0",  NORMAL_KEY_TYPE_CHAR),

    NORMAL_KEY_ITEM(NORMAL_KEY_Q,     1, 0, 1, "q",  "q",  "Q",  "Q",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_W,     1, 1, 1, "w",  "w",  "W",  "W",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_E,     1, 2, 1, "e",  "e",  "E",  "E",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_R,     1, 3, 1, "r",  "r",  "R",  "R",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_T,     1, 4, 1, "t",  "t",  "T",  "T",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_Y,     1, 5, 1, "y",  "y",  "Y",  "Y",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_U,     1, 6, 1, "u",  "u",  "U",  "U",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_I,     1, 7, 1, "i",  "i",  "I",  "I",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_O,     1, 8, 1, "o",  "o",  "O",  "O",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_P,     1, 9, 1, "p",  "p",  "P",  "P",  NORMAL_KEY_TYPE_CHAR),

    NORMAL_KEY_ITEM(NORMAL_KEY_A,     2, 0, 1, "a",  "a",  "A",  "A",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_S,     2, 1, 1, "s",  "s",  "S",  "S",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_D,     2, 2, 1, "d",  "d",  "D",  "D",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_F,     2, 3, 1, "f",  "f",  "F",  "F",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_G,     2, 4, 1, "g",  "g",  "G",  "G",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_H,     2, 5, 1, "h",  "h",  "H",  "H",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_J,     2, 6, 1, "j",  "j",  "J",  "J",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_K,     2, 7, 1, "k",  "k",  "K",  "K",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_L,     2, 8, 1, "l",  "l",  "L",  "L",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_BS,    2, 9, 1, "BS", "\b", "BS", "\b", NORMAL_KEY_TYPE_BACKSPACE),

    NORMAL_KEY_ITEM(NORMAL_KEY_SHIFT, 3, 0, 1, "Aa", NULL,  "Aa", NULL,  NORMAL_KEY_TYPE_SHIFT),
    NORMAL_KEY_ITEM(NORMAL_KEY_Z,     3, 1, 1, "z",  "z",  "Z",  "Z",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_X,     3, 2, 1, "x",  "x",  "X",  "X",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_C,     3, 3, 1, "c",  "c",  "C",  "C",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_V,     3, 4, 1, "v",  "v",  "V",  "V",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_B,     3, 5, 1, "b",  "b",  "B",  "B",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_N,     3, 6, 1, "n",  "n",  "N",  "N",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_M,     3, 7, 1, "m",  "m",  "M",  "M",  NORMAL_KEY_TYPE_CHAR),
    NORMAL_KEY_ITEM(NORMAL_KEY_SPACE, 3, 8, 1, "SP", " ",  "SP", " ",  NORMAL_KEY_TYPE_SPACE),
    NORMAL_KEY_ITEM(NORMAL_KEY_OK,    3, 9, 1, "OK", NULL,  "OK", NULL,  NORMAL_KEY_TYPE_CONFIRM),
};

#endif // NORMAL_KEYBOARD_DEF_H