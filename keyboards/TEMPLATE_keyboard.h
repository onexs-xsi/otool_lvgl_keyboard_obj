/*
 * SPDX-FileCopyrightText: 2025-2026 exia
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file TEMPLATE_keyboard.h
 * @brief 新键盘模板
 * 
 * 使用方法:
 * 1. 复制此文件到 keyboards/your_keyboard/your_keyboard.h
 * 2. 将所有 TEMPLATE 替换为你的键盘名称（大写）
 * 3. 将所有 Template 替换为你的键盘名称（驼峰命名）
 * 4. 实现所有纯虚函数
 * 5. 创建对应的 .cpp 实现文件
 * 6. 更新 CMakeLists.txt
 */

#ifndef TEMPLATE_KEYBOARD_H
#define TEMPLATE_KEYBOARD_H

#include "otool_lvgl_keyboard.h"
#include <stdint.h>

#ifdef __cplusplus

/**
 * @brief 键盘类模板
 * 
 * 继承自 OtoolKeyboard 基类，提供完整的键盘实现示例。
 * 请根据实际需求修改和扩展此类。
 * 
 * @see OtoolKeyboard
 */
class TemplateKeyboard : public OtoolKeyboard {
public:
    TemplateKeyboard();
    virtual ~TemplateKeyboard();

    // ========================================================================
    // 必须实现的虚函数
    // ========================================================================

    /**
     * @brief 创建键盘 UI
     * @param parent 父对象
     * @return 键盘容器对象指针
     */
    virtual lv_obj_t* create(lv_obj_t* parent) override;

    /**
     * @brief 设置按键禁用状态
     * @param key_id 按键 ID
     * @param disabled true=禁用, false=启用
     */
    virtual void setKeyDisabled(uint16_t key_id, bool disabled) override;

    /**
     * @brief 模拟单字符输入
     * @param key_char 要模拟的字符
     */
    virtual void simulateCharacter(const char* key_char) override;

    /**
     * @brief 模拟字符串输入
     * @param str 要模拟的字符串
     * @param interval_ms 字符间隔时间（毫秒）
     */
    virtual void simulateString(const char* str, uint32_t interval_ms = 100) override;

    // ========================================================================
    // 可选：覆盖布局约束（根据实际键盘调整这些值）
    // ========================================================================

    virtual float getPreferredAspectRatio() const override { return 3.0f; }  // 首选宽高比
    virtual float getMinAspectRatio() const override { return 2.0f; }        // 最小宽高比
    virtual float getMaxAspectRatio() const override { return 4.0f; }        // 最大宽高比
    virtual float getMaxScreenHeightPct() const override { return 0.60f; }   // 最大屏幕高度占比

    // ========================================================================
    // 键盘特定的公共 API（根据需要添加）
    // ========================================================================

    /**
     * @brief 示例：获取按键数量
     * @return 按键总数
     */
    uint16_t getKeyCount() const { return m_key_count; }

    /**
     * @brief 示例：重置键盘状态
     */
    void reset();

protected:
    // ========================================================================
    // 键盘特定的成员变量
    // ========================================================================

    uint16_t m_key_count;                     // 按键数量
    lv_obj_t** m_key_buttons;                 // 按键对象数组
    bool* m_key_disabled;                     // 按键禁用状态数组

    // ========================================================================
    // 键盘特定的内部方法
    // ========================================================================

    /**
     * @brief 创建单个按键
     * @param parent 父对象
     * @param key_id 按键 ID
     * @param x X 坐标
     * @param y Y 坐标
     * @param width 宽度
     * @param height 高度
     * @param label 标签文本
     * @return 按键对象指针
     */
    lv_obj_t* createKey(lv_obj_t* parent, uint16_t key_id, 
                        int32_t x, int32_t y, 
                        int32_t width, int32_t height,
                        const char* label);

    /**
     * @brief 处理按键事件（LVGL 事件回调）
     * @param e 事件对象
     */
    void handleKeyEvent(lv_event_t* e);

    /**
     * @brief 静态事件处理器（用于 LVGL 回调）
     * @param e 事件对象
     */
    static void eventHandlerStatic(lv_event_t* e);

    /**
     * @brief 根据按键 ID 查找按键对象
     * @param key_id 按键 ID
     * @return 按键对象指针，未找到返回 nullptr
     */
    lv_obj_t* findButtonById(uint16_t key_id) const;

    /**
     * @brief 根据字符查找按键 ID
     * @param ch 字符
     * @return 按键 ID，未找到返回 0xFFFF
     */
    uint16_t findKeyIdByChar(char ch) const;
};

#endif // __cplusplus
#endif // TEMPLATE_KEYBOARD_H
