/*
 * SPDX-FileCopyrightText: 2025-2026 exia
 *
 * SPDX-License-Identifier: MIT
 */

/* 
 * 新键盘实现模板
 * 
 * 使用方法:
 * 1. 复制此文件到 keyboards/your_keyboard/your_keyboard.cpp
 * 2. 将所有 TEMPLATE 替换为你的键盘名称（大写）
 * 3. 将所有 Template 替换为你的键盘名称（驼峰命名）
 * 4. 实现所有方法
 * 5. 更新 CMakeLists.txt 添加此源文件
 */

#include "TEMPLATE_keyboard.h"
#include <esp_log.h>
#include <string.h>
#include <stdlib.h>

// LVGL 字体声明（根据需要选择）
LV_FONT_DECLARE(lv_font_montserrat_14);
LV_FONT_DECLARE(lv_font_montserrat_24);

static const char* TAG = "TemplateKeyboard";

// ============================================================================
// 构造函数和析构函数
// ============================================================================

TemplateKeyboard::TemplateKeyboard()
    : m_key_count(0),
      m_key_buttons(nullptr),
      m_key_disabled(nullptr)
{
    ESP_LOGI(TAG, "TemplateKeyboard created");
    
    // TODO: 根据实际键盘初始化按键数量
    m_key_count = 70;  // 示例：70个按键
    
    // 分配按键数组
    m_key_buttons = (lv_obj_t**)calloc(m_key_count, sizeof(lv_obj_t*));
    m_key_disabled = (bool*)calloc(m_key_count, sizeof(bool));
    
    if (!m_key_buttons || !m_key_disabled) {
        ESP_LOGE(TAG, "Failed to allocate memory for key arrays");
    }
}

TemplateKeyboard::~TemplateKeyboard()
{
    ESP_LOGI(TAG, "TemplateKeyboard destroyed");
    
    // 释放分配的内存
    if (m_key_buttons) {
        free(m_key_buttons);
        m_key_buttons = nullptr;
    }
    
    if (m_key_disabled) {
        free(m_key_disabled);
        m_key_disabled = nullptr;
    }
    
    // 注意：LVGL 对象由 LVGL 自动管理，无需手动删除
}

// ============================================================================
// 必须实现的虚函数
// ============================================================================

lv_obj_t* TemplateKeyboard::create(lv_obj_t* parent)
{
    ESP_LOGI(TAG, "Creating keyboard UI");
    
    // 1. 创建容器
    m_kb_cont = lv_obj_create(parent);
    if (!m_kb_cont) {
        ESP_LOGE(TAG, "Failed to create container");
        return nullptr;
    }
    
    // 2. 设置容器样式
    lv_obj_set_size(m_kb_cont, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(m_kb_cont, 4, 0);
    lv_obj_set_style_bg_color(m_kb_cont, lv_color_hex(0x303030), 0);
    
    // 3. 创建按键布局
    // TODO: 根据实际键盘布局创建按键
    // 示例：创建一个简单的 10x7 网格
    
    const int rows = 7;
    const int cols = 10;
    const int gap = 4;
    const int key_width = 60;
    const int key_height = 50;
    
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            uint16_t key_id = row * cols + col;
            if (key_id >= m_key_count) break;
            
            int32_t x = col * (key_width + gap);
            int32_t y = row * (key_height + gap);
            
            char label[8];
            snprintf(label, sizeof(label), "%d", key_id);
            
            lv_obj_t* btn = createKey(m_kb_cont, key_id, x, y, 
                                     key_width, key_height, label);
            if (btn) {
                m_key_buttons[key_id] = btn;
            }
        }
    }
    
    ESP_LOGI(TAG, "Keyboard UI created with %d keys", m_key_count);
    return m_kb_cont;
}

void TemplateKeyboard::setKeyDisabled(uint16_t key_id, bool disabled)
{
    if (key_id >= m_key_count) {
        ESP_LOGW(TAG, "Invalid key_id %d", key_id);
        return;
    }
    
    m_key_disabled[key_id] = disabled;
    
    lv_obj_t* btn = m_key_buttons[key_id];
    if (btn) {
        if (disabled) {
            lv_obj_add_state(btn, LV_STATE_DISABLED);
        } else {
            lv_obj_clear_state(btn, LV_STATE_DISABLED);
        }
    }
    
    ESP_LOGD(TAG, "Key %d %s", key_id, disabled ? "disabled" : "enabled");
}

void TemplateKeyboard::simulateCharacter(const char* key_char)
{
    if (!key_char || !key_char[0]) return;
    
    ESP_LOGI(TAG, "Simulating character: %c", key_char[0]);
    
    // TODO: 实现字符模拟逻辑
    // 1. 根据字符查找对应的按键 ID
    // 2. 模拟按键按下和释放事件
    // 3. 触发输入逻辑
    
    uint16_t key_id = findKeyIdByChar(key_char[0]);
    if (key_id == 0xFFFF) {
        ESP_LOGW(TAG, "Character '%c' not found on keyboard", key_char[0]);
        return;
    }
    
    // 模拟按键点击
    lv_obj_t* btn = m_key_buttons[key_id];
    if (btn) {
        lv_event_send(btn, LV_EVENT_PRESSED, nullptr);
        lv_event_send(btn, LV_EVENT_RELEASED, nullptr);
        lv_event_send(btn, LV_EVENT_CLICKED, nullptr);
    }
}

void TemplateKeyboard::simulateString(const char* str, uint32_t interval_ms)
{
    if (!str) return;
    
    ESP_LOGI(TAG, "Simulating string: %s", str);
    
    for (size_t i = 0; str[i] != '\0'; i++) {
        char single_char[2] = {str[i], '\0'};
        simulateCharacter(single_char);
        
        if (interval_ms > 0 && str[i + 1] != '\0') {
            vTaskDelay(pdMS_TO_TICKS(interval_ms));
        }
    }
}

// ============================================================================
// 键盘特定的公共 API
// ============================================================================

void TemplateKeyboard::reset()
{
    ESP_LOGI(TAG, "Resetting keyboard state");
    
    // TODO: 重置键盘状态
    // 例如：清除所有按键的禁用状态、重置模式等
    
    for (uint16_t i = 0; i < m_key_count; i++) {
        setKeyDisabled(i, false);
    }
}

// ============================================================================
// 内部辅助方法
// ============================================================================

lv_obj_t* TemplateKeyboard::createKey(lv_obj_t* parent, uint16_t key_id,
                                      int32_t x, int32_t y,
                                      int32_t width, int32_t height,
                                      const char* label)
{
    // 创建按钮
    lv_obj_t* btn = lv_btn_create(parent);
    if (!btn) {
        ESP_LOGE(TAG, "Failed to create button for key %d", key_id);
        return nullptr;
    }
    
    // 设置位置和大小
    lv_obj_set_pos(btn, x, y);
    lv_obj_set_size(btn, width, height);
    
    // 创建标签
    lv_obj_t* lbl = lv_label_create(btn);
    lv_label_set_text(lbl, label);
    lv_obj_center(lbl);
    
    // 设置样式
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x404040), 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x606060), LV_STATE_PRESSED);
    lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
    
    // 保存按键 ID 到用户数据
    lv_obj_set_user_data(btn, (void*)(uintptr_t)key_id);
    
    // 添加事件处理器
    lv_obj_add_event_cb(btn, eventHandlerStatic, LV_EVENT_ALL, this);
    
    return btn;
}

void TemplateKeyboard::handleKeyEvent(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* btn = lv_event_get_target(e);
    uint16_t key_id = (uint16_t)(uintptr_t)lv_obj_get_user_data(btn);
    
    ESP_LOGD(TAG, "Key event: key_id=%d, code=%d", key_id, code);
    
    // TODO: 实现按键事件处理逻辑
    
    if (code == LV_EVENT_CLICKED) {
        // 示例：按键点击时向文本区域添加键号
        if (m_textarea) {
            char text[16];
            snprintf(text, sizeof(text), "[%d]", key_id);
            lv_textarea_add_text(m_textarea, text);
        }
    }
}

void TemplateKeyboard::eventHandlerStatic(lv_event_t* e)
{
    TemplateKeyboard* kb = (TemplateKeyboard*)lv_event_get_user_data(e);
    if (kb) {
        kb->handleKeyEvent(e);
    }
}

lv_obj_t* TemplateKeyboard::findButtonById(uint16_t key_id) const
{
    if (key_id >= m_key_count) {
        return nullptr;
    }
    return m_key_buttons[key_id];
}

uint16_t TemplateKeyboard::findKeyIdByChar(char ch) const
{
    // TODO: 实现字符到按键 ID 的映射
    // 这取决于你的键盘布局
    
    // 示例：假设按键 0-9 对应数字字符 '0'-'9'
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    }
    
    // 未找到
    return 0xFFFF;
}
