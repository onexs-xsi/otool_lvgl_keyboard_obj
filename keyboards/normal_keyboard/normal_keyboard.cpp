/*
 * SPDX-FileCopyrightText: 2025-2026 exia
 *
 * SPDX-License-Identifier: MIT
 */

#include "normal_keyboard.h"

#include <esp_log.h>
#include <ctype.h>
#include <string.h>

LV_FONT_DECLARE(lv_font_montserrat_24);

static const char* TAG = "NormalKeyboard";

namespace {
static constexpr lv_opa_t kDisabledBgOpacity = LV_OPA_70;
static constexpr lv_opa_t kDisabledTextOpacity = LV_OPA_70;
static constexpr uint32_t kDisabledBorderColor = 0x7A7A7A;
static constexpr uint32_t kNormalBorderColor = 0x111111;
static constexpr uint32_t kShiftActiveBorderColor = 0xFFF2CC;
static constexpr lv_coord_t kSpaceIconWidth = 34;
static constexpr lv_coord_t kSpaceIconHeight = 8;
static constexpr lv_coord_t kSpaceIconStrokeWidth = 2;
static constexpr lv_coord_t kSpaceIconOffsetY = 3;
static constexpr uint16_t kSpaceIconPointCount = 4;

static const lv_point_precise_t kSpaceIconPoints[kSpaceIconPointCount] = {
    {0, 0},
    {0, 8},
    {34, 8},
    {34, 0},
};

static constexpr lv_coord_t kBsIconWidth = 26;
static constexpr lv_coord_t kBsIconHeight = 16;
static constexpr lv_coord_t kBsIconStrokeWidth = 2;
static constexpr lv_coord_t kBsIconOffsetY = 0;
static constexpr uint16_t kBsIconPointCount = 6;

static const lv_point_precise_t kBsIconPoints[kBsIconPointCount] = {
    {8, 0},
    {0, 8},
    {8, 16},
    {26, 16},
    {26, 0},
    {8, 0},
};

static constexpr lv_coord_t kOkIconWidth = 24;
static constexpr lv_coord_t kOkIconHeight = 20;
static constexpr lv_coord_t kOkIconStrokeWidth = 2;
static constexpr lv_coord_t kOkIconOffsetY = 0;
static constexpr uint16_t kOkIconPointCount = 6;

static const lv_point_precise_t kOkIconPoints[kOkIconPointCount] = {
    {24, 0},
    {24, 12},
    {4, 12},
    {12, 4},
    {4, 12},
    {12, 20},
};

static uint32_t get_key_bg_color(const normal_key_t* key, normal_case_mode_t case_mode)
{
    switch (key->type) {
        case NORMAL_KEY_TYPE_SHIFT:
            return case_mode == NORMAL_CASE_MODE_UPPER ? NORMAL_KB_COLOR_BG_SHIFT_ACTIVE
                                                       : NORMAL_KB_COLOR_BG_SHIFT;
        case NORMAL_KEY_TYPE_BACKSPACE:
            return NORMAL_KB_COLOR_BG_BLACK;
        case NORMAL_KEY_TYPE_SPACE:
            return NORMAL_KB_COLOR_BG_NORMAL;
        case NORMAL_KEY_TYPE_CONFIRM:
            return NORMAL_KB_COLOR_BG_BLACK;
        case NORMAL_KEY_TYPE_CHAR:
        default:
            return NORMAL_KB_COLOR_BG_NORMAL;
    }
}

static uint32_t get_key_text_color(const normal_key_t* key)
{
    switch (key->type) {
        case NORMAL_KEY_TYPE_SHIFT:
        case NORMAL_KEY_TYPE_BACKSPACE:
        case NORMAL_KEY_TYPE_CONFIRM:
            return NORMAL_KB_COLOR_TXT_WHITE;
        case NORMAL_KEY_TYPE_SPACE:
        case NORMAL_KEY_TYPE_CHAR:
        default:
            return NORMAL_KB_COLOR_TXT_NORMAL;
    }
}

static uint32_t get_key_border_color(const normal_key_t* key, normal_case_mode_t case_mode, bool disabled)
{
    if (disabled) {
        return kDisabledBorderColor;
    }

    if (key->type == NORMAL_KEY_TYPE_SHIFT && case_mode == NORMAL_CASE_MODE_UPPER) {
        return kShiftActiveBorderColor;
    }

    return kNormalBorderColor;
}

} // namespace

NormalKeyboard::NormalKeyboard()
    : m_case_mode(NORMAL_CASE_MODE_LOWER),
      m_confirm_callback(nullptr),
      m_confirm_user_data(nullptr),
      m_backspace_repeat_active(false)
{
    memset(m_key_buttons, 0, sizeof(m_key_buttons));
    memset(m_key_icons, 0, sizeof(m_key_icons));
    memset(m_key_disabled, 0, sizeof(m_key_disabled));
}

NormalKeyboard::~NormalKeyboard()
{
}

void NormalKeyboard::setConfirmCallback(normal_keyboard_confirm_callback_t callback, void* user_data)
{
    m_confirm_callback = callback;
    m_confirm_user_data = user_data;
}

const normal_key_t* NormalKeyboard::findKeyById(uint16_t key_id) const
{
    if (key_id >= NORMAL_KEY_COUNT) {
        return nullptr;
    }
    return &normal_key_map[key_id];
}

lv_obj_t* NormalKeyboard::findButtonById(uint16_t key_id) const
{
    if (key_id >= NORMAL_KEY_COUNT) {
        return nullptr;
    }
    return m_key_buttons[key_id];
}

void NormalKeyboard::setCaseMode(normal_case_mode_t mode)
{
    if (m_case_mode == mode) {
        return;
    }

    m_case_mode = mode;
    updateUiState();
}

void NormalKeyboard::updateKeyUiState(uint16_t key_id)
{
    if (key_id >= NORMAL_KEY_COUNT) {
        return;
    }

    const normal_key_t* key = &normal_key_map[key_id];
    lv_obj_t* btn = m_key_buttons[key_id];
    if (!btn) {
        return;
    }

    lv_obj_t* label = lv_obj_get_child(btn, 0);
    const char* text = m_case_mode == NORMAL_CASE_MODE_UPPER ? key->label_upper : key->label_lower;
    if (!text) {
        text = key->label_lower ? key->label_lower : "";
    }

    if (key->type == NORMAL_KEY_TYPE_SPACE || 
        key->type == NORMAL_KEY_TYPE_BACKSPACE || 
        key->type == NORMAL_KEY_TYPE_CONFIRM) {
        text = "";
    }

    if (label) {
        lv_opa_t text_opa = m_key_disabled[key_id] ? kDisabledTextOpacity : static_cast<lv_opa_t>(LV_OPA_COVER);
        lv_label_set_text(label, text);
        lv_obj_set_style_text_color(label,
                                    lv_color_hex(m_key_disabled[key_id] ? NORMAL_KB_COLOR_TXT_DISABLED
                                                                        : get_key_text_color(key)),
                                    0);
        lv_obj_set_style_text_opa(label, text_opa, 0);
    }

    lv_obj_t* icon = m_key_icons[key_id];
    if (icon) {
        lv_opa_t icon_opa = m_key_disabled[key_id] ? kDisabledTextOpacity : static_cast<lv_opa_t>(LV_OPA_COVER);
        lv_obj_set_style_line_color(icon,
                                    lv_color_hex(m_key_disabled[key_id] ? NORMAL_KB_COLOR_TXT_DISABLED
                                                                        : get_key_text_color(key)),
                                    0);
        lv_obj_set_style_line_opa(icon, icon_opa, 0);
    }

    lv_opa_t bg_opa = m_key_disabled[key_id] ? kDisabledBgOpacity : static_cast<lv_opa_t>(LV_OPA_COVER);
    lv_obj_set_style_bg_color(btn,
                              lv_color_hex(m_key_disabled[key_id] ? NORMAL_KB_COLOR_BG_DISABLED
                                                                  : get_key_bg_color(key, m_case_mode)),
                              0);
    lv_obj_set_style_bg_opa(btn, bg_opa, 0);
    lv_obj_set_style_border_width(btn, 2, 0);
    lv_obj_set_style_border_color(btn,
                                  lv_color_hex(get_key_border_color(key, m_case_mode, m_key_disabled[key_id])),
                                  0);
}

void NormalKeyboard::updateUiState()
{
    for (uint16_t key_id = 0; key_id < NORMAL_KEY_COUNT; ++key_id) {
        updateKeyUiState(key_id);
    }
}

void NormalKeyboard::setKeyDisabled(uint16_t key_id, bool disabled)
{
    if (key_id >= NORMAL_KEY_COUNT) {
        ESP_LOGW(TAG, "Invalid key id: %d", key_id);
        return;
    }

    if (m_key_disabled[key_id] == disabled) {
        return;
    }

    m_key_disabled[key_id] = disabled;

    lv_obj_t* btn = findButtonById(key_id);
    if (btn) {
        if (disabled) {
            lv_obj_add_state(btn, LV_STATE_DISABLED);
        } else {
            lv_obj_clear_state(btn, LV_STATE_DISABLED);
        }
    }

    updateKeyUiState(key_id);
}

void NormalKeyboard::processKeyInput(const normal_key_t* key)
{
    if (!key || key->id >= NORMAL_KEY_COUNT || m_key_disabled[key->id]) {
        return;
    }

    switch (key->type) {
        case NORMAL_KEY_TYPE_SHIFT:
            setCaseMode(m_case_mode == NORMAL_CASE_MODE_LOWER ? NORMAL_CASE_MODE_UPPER
                                                              : NORMAL_CASE_MODE_LOWER);
            break;

        case NORMAL_KEY_TYPE_BACKSPACE:
            sendTextToTextarea("\b");
            break;

        case NORMAL_KEY_TYPE_SPACE:
            sendTextToTextarea(" ");
            break;

        case NORMAL_KEY_TYPE_CONFIRM:
            if (m_confirm_callback) {
                m_confirm_callback(m_confirm_user_data);
            }
            break;

        case NORMAL_KEY_TYPE_CHAR:
        default: {
            const char* value = m_case_mode == NORMAL_CASE_MODE_UPPER ? key->value_upper : key->value_lower;
            if (value) {
                sendTextToTextarea(value);
            }
            break;
        }
    }
}

void NormalKeyboard::handleKeyEvent(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_PRESSED) {
        lv_obj_t* btn = static_cast<lv_obj_t*>(lv_event_get_target(e));
        if (!btn) {
            return;
        }

        uint16_t key_id = (uint16_t)(uintptr_t)lv_obj_get_user_data(btn);
        const normal_key_t* key = findKeyById(key_id);
        if (key && key->type == NORMAL_KEY_TYPE_BACKSPACE) {
            m_backspace_repeat_active = false;
        }
        return;
    }

    if (code != LV_EVENT_CLICKED &&
        code != LV_EVENT_LONG_PRESSED &&
        code != LV_EVENT_LONG_PRESSED_REPEAT) {
        return;
    }

    lv_obj_t* btn = static_cast<lv_obj_t*>(lv_event_get_target(e));
    if (!btn) {
        return;
    }

    uint16_t key_id = (uint16_t)(uintptr_t)lv_obj_get_user_data(btn);
    const normal_key_t* key = findKeyById(key_id);
    if (!key) {
        return;
    }

    if (key->type == NORMAL_KEY_TYPE_BACKSPACE) {
        if (code == LV_EVENT_LONG_PRESSED) {
            m_backspace_repeat_active = true;
            processKeyInput(key);
            return;
        }

        if (code == LV_EVENT_LONG_PRESSED_REPEAT) {
            m_backspace_repeat_active = true;
            processKeyInput(key);
            return;
        }

        if (code == LV_EVENT_CLICKED) {
            if (m_backspace_repeat_active) {
                m_backspace_repeat_active = false;
                return;
            }

            processKeyInput(key);
            return;
        }

        return;
    }

    if (code != LV_EVENT_CLICKED) {
        return;
    }

    processKeyInput(key);
}

void NormalKeyboard::eventHandlerStatic(lv_event_t* e)
{
    lv_obj_t* btn = static_cast<lv_obj_t*>(lv_event_get_target(e));
    lv_obj_t* cont = lv_obj_get_parent(btn);
    if (!cont) {
        return;
    }

    NormalKeyboard* kb = (NormalKeyboard*)lv_obj_get_user_data(cont);
    if (kb) {
        kb->handleKeyEvent(e);
    }
}

lv_obj_t* NormalKeyboard::create(lv_obj_t* parent)
{
    m_kb_cont = lv_obj_create(parent);
    lv_obj_set_user_data(m_kb_cont, this);

    lv_disp_t* disp = lv_obj_get_disp(parent);
    if (!disp) disp = lv_disp_get_default();
    int32_t screen_w = lv_disp_get_hor_res(disp);
    int32_t screen_h = lv_disp_get_ver_res(disp);

    int32_t target_w = (int32_t)(screen_w * 0.98f);
    float ratio = getPreferredAspectRatio();
    if (ratio < 0.1f) ratio = 1.0f;
    int32_t target_h = (int32_t)(target_w / ratio);

    float min_r = getMinAspectRatio();
    float max_r = getMaxAspectRatio();
    int32_t h_allowed_max = (int32_t)(target_w / min_r);
    int32_t h_allowed_min = (int32_t)(target_w / max_r);

    if (target_h > h_allowed_max) target_h = h_allowed_max;
    if (target_h < h_allowed_min) target_h = h_allowed_min;

    int32_t screen_h_limit = (int32_t)(screen_h * getMaxScreenHeightPct());
    if (target_h > screen_h_limit) {
        target_h = screen_h_limit;
    }

    lv_obj_set_size(m_kb_cont, target_w, target_h);
    lv_obj_align(m_kb_cont, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_obj_set_style_bg_color(m_kb_cont, lv_color_hex(NORMAL_KB_COLOR_BG_CONTAINER), 0);
    lv_obj_set_style_radius(m_kb_cont, 10, 0);
    lv_obj_set_style_border_width(m_kb_cont, 0, 0);
    lv_obj_set_style_pad_all(m_kb_cont, 6, 0);
    lv_obj_set_style_pad_gap(m_kb_cont, 4, 0);
    lv_obj_clear_flag(m_kb_cont, LV_OBJ_FLAG_GESTURE_BUBBLE);

    static int32_t col_dsc[] = {
        LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
        LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
        LV_GRID_TEMPLATE_LAST
    };
    static int32_t row_dsc[] = {
        LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
        LV_GRID_TEMPLATE_LAST
    };

    lv_obj_set_layout(m_kb_cont, LV_LAYOUT_GRID);
    lv_obj_set_grid_dsc_array(m_kb_cont, col_dsc, row_dsc);

    for (int i = 0; i < NORMAL_KEY_COUNT; ++i) {
        const normal_key_t* key = &normal_key_map[i];

#if LVGL_VERSION_MAJOR >= 9
        lv_obj_t* btn = lv_button_create(m_kb_cont);
#else
        lv_obj_t* btn = lv_btn_create(m_kb_cont);
#endif
        lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_STRETCH, key->col, key->width,
                                  LV_GRID_ALIGN_STRETCH, key->row, 1);
        lv_obj_set_style_radius(btn, 6, 0);
        lv_obj_set_style_border_width(btn, 2, 0);
        lv_obj_set_style_border_color(btn, lv_color_hex(kNormalBorderColor), 0);
        lv_obj_set_style_bg_color(btn, lv_color_hex(get_key_bg_color(key, m_case_mode)), 0);
        lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
        lv_obj_clear_flag(btn, LV_OBJ_FLAG_GESTURE_BUBBLE);
        lv_obj_add_flag(btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
        lv_obj_set_user_data(btn, (void*)(uintptr_t)key->id);

        lv_obj_t* label = lv_label_create(btn);
        lv_label_set_text(label, key->label_lower ? key->label_lower : "");
        lv_obj_set_style_text_color(label, lv_color_hex(get_key_text_color(key)), 0);
        lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
        lv_obj_set_style_text_font(label, &lv_font_montserrat_24, 0);
        lv_obj_center(label);

        if (key->type == NORMAL_KEY_TYPE_SPACE) {
            lv_obj_t* icon = lv_line_create(btn);
            lv_line_set_points(icon, kSpaceIconPoints, kSpaceIconPointCount);
            lv_obj_set_size(icon, kSpaceIconWidth, kSpaceIconHeight);
            lv_obj_set_style_line_width(icon, kSpaceIconStrokeWidth, 0);
            lv_obj_set_style_line_color(icon, lv_color_hex(get_key_text_color(key)), 0);
            lv_obj_set_style_line_rounded(icon, false, 0);
            lv_obj_center(icon);
            lv_obj_set_y(icon, kSpaceIconOffsetY);
            m_key_icons[i] = icon;
        } else if (key->type == NORMAL_KEY_TYPE_BACKSPACE) {
            lv_obj_t* icon = lv_line_create(btn);
            lv_line_set_points(icon, kBsIconPoints, kBsIconPointCount);
            lv_obj_set_size(icon, kBsIconWidth, kBsIconHeight);
            lv_obj_set_style_line_width(icon, kBsIconStrokeWidth, 0);
            lv_obj_set_style_line_color(icon, lv_color_hex(get_key_text_color(key)), 0);
            lv_obj_set_style_line_rounded(icon, true, 0);
            lv_obj_center(icon);
            m_key_icons[i] = icon;
        } else if (key->type == NORMAL_KEY_TYPE_CONFIRM) {
            lv_obj_t* icon = lv_line_create(btn);
            lv_line_set_points(icon, kOkIconPoints, kOkIconPointCount);
            lv_obj_set_size(icon, kOkIconWidth, kOkIconHeight);
            lv_obj_set_style_line_width(icon, kOkIconStrokeWidth, 0);
            lv_obj_set_style_line_color(icon, lv_color_hex(get_key_text_color(key)), 0);
            lv_obj_set_style_line_rounded(icon, true, 0);
            lv_obj_center(icon);
            m_key_icons[i] = icon;
        }

        lv_obj_add_event_cb(btn, eventHandlerStatic, LV_EVENT_PRESSED, nullptr);
        lv_obj_add_event_cb(btn, eventHandlerStatic, LV_EVENT_CLICKED, nullptr);
        if (key->type == NORMAL_KEY_TYPE_BACKSPACE) {
            lv_obj_add_event_cb(btn, eventHandlerStatic, LV_EVENT_LONG_PRESSED, nullptr);
            lv_obj_add_event_cb(btn, eventHandlerStatic, LV_EVENT_LONG_PRESSED_REPEAT, nullptr);
        }

        m_key_buttons[i] = btn;
    }

    updateUiState();
    return m_kb_cont;
}

void NormalKeyboard::simulateCharacter(const char* key_char)
{
    if (!key_char || !key_char[0]) {
        return;
    }

    char ch = key_char[0];
    uint16_t key_id = NORMAL_KEY_COUNT;

    if (ch >= 'a' && ch <= 'z') {
        setCaseMode(NORMAL_CASE_MODE_LOWER);
        key_id = static_cast<uint16_t>(NORMAL_KEY_A + (ch - 'a'));
    } else if (ch >= 'A' && ch <= 'Z') {
        setCaseMode(NORMAL_CASE_MODE_UPPER);
        key_id = static_cast<uint16_t>(NORMAL_KEY_A + (tolower((unsigned char)ch) - 'a'));
    } else if (ch >= '0' && ch <= '9') {
        key_id = ch == '0'
                     ? static_cast<uint16_t>(NORMAL_KEY_0)
                     : static_cast<uint16_t>(NORMAL_KEY_1 + (ch - '1'));
    } else if (ch == ' ') {
        key_id = NORMAL_KEY_SPACE;
    } else {
        ESP_LOGW(TAG, "Unsupported character for simulation: %c", ch);
        return;
    }

    processKeyInput(findKeyById(key_id));
}

void NormalKeyboard::simulateString(const char* str, uint32_t interval_ms)
{
    if (!str) {
        return;
    }

    for (size_t index = 0; str[index] != '\0'; ++index) {
        char single_char[2] = {str[index], '\0'};
        simulateCharacter(single_char);

        if (interval_ms > 0 && str[index + 1] != '\0') {
            lv_delay_ms(interval_ms);
        }
    }
}