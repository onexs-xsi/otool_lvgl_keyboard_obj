/*
 * SPDX-FileCopyrightText: 2025-2026 exia
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef NORMAL_KEYBOARD_H
#define NORMAL_KEYBOARD_H

#include "otool_lvgl_keyboard.h"
#include "normal_keyboard_def.h"

#ifdef __cplusplus

typedef void (*normal_keyboard_confirm_callback_t)(void* user_data);

class NormalKeyboard : public OtoolKeyboard {
public:
    NormalKeyboard();
    virtual ~NormalKeyboard();

    virtual lv_obj_t* create(lv_obj_t* parent) override;
    virtual void setKeyDisabled(uint16_t key_id, bool disabled) override;
    virtual void simulateCharacter(const char* key_char) override;
    virtual void simulateString(const char* str, uint32_t interval_ms = 100) override;

    virtual float getPreferredAspectRatio() const override { return NORMAL_KB_PREFERRED_ASPECT_RATIO; }
    virtual float getMinAspectRatio() const override { return NORMAL_KB_MIN_ASPECT_RATIO; }
    virtual float getMaxAspectRatio() const override { return NORMAL_KB_MAX_ASPECT_RATIO; }
    virtual float getMaxScreenHeightPct() const override { return NORMAL_KB_MAX_SCREEN_HEIGHT_PCT; }

    void setConfirmCallback(normal_keyboard_confirm_callback_t callback, void* user_data = nullptr);
    void updateUiState();

protected:
    lv_obj_t* m_key_buttons[NORMAL_KEY_COUNT];
    bool m_key_disabled[NORMAL_KEY_COUNT];
    normal_case_mode_t m_case_mode;
    normal_keyboard_confirm_callback_t m_confirm_callback;
    void* m_confirm_user_data;
    bool m_backspace_repeat_active;

    void updateKeyUiState(uint16_t key_id);
    const normal_key_t* findKeyById(uint16_t key_id) const;
    lv_obj_t* findButtonById(uint16_t key_id) const;
    void handleKeyEvent(lv_event_t* e);
    void processKeyInput(const normal_key_t* key);
    void setCaseMode(normal_case_mode_t mode);

    static void eventHandlerStatic(lv_event_t* e);
};

#endif // __cplusplus

#endif // NORMAL_KEYBOARD_H