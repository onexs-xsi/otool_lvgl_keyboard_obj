# OTOOL LVGL Keyboard Object

一个通用的 LVGL 键盘组件，支持多种键盘实现。采用面向对象设计，易于扩展和定制。

## 功能特性

- **通用基类架构**: `OtoolKeyboard` 提供统一的键盘接口
- **多键盘支持**: 轻松添加新的键盘实现
- **热键系统**: 支持组合键热键注册（Ctrl+X, Alt+F 等）
- **虚拟键盘模拟**: 支持字符和字符串输入模拟
- **LVGL 兼容**: 支持 LVGL v8 和 v9

## 目录结构

```
otool_lvgl_keyboard_obj/
├── CMakeLists.txt               # 组件构建配置
├── README.md                    # 本文件
├── include/
│   └── otool_lvgl_keyboard.h    # 基类公共头文件
├── otool_lvgl_keyboard.cpp      # 基类实现
└── keyboards/                   # 键盘实现目录
    ├── README.md                # 添加新键盘指南
    ├── m5tab5_keyboard/         # M5Tab5 物理键盘
    │   ├── m5tab5_keyboard.h
    │   ├── m5tab5_keyboard.cpp
    │   └── m5tab5_keyboard_def.h
    └── factorytest_keyboard/    # 工厂测试键盘
        ├── factorytest_keyboard.h
        └── factorytest_keyboard.cpp
```

## 已实现的键盘

### 1. M5Tab5Keyboard

完整功能的物理键盘实现，适用于 M5Stack Tab5 设备。

**特性**:
- 14x5 网格布局（70个按键）
- 多模式支持（小写、大写、符号）
- Shift/Caps Lock 逻辑
- 组合键支持（Ctrl、Alt）
- 热键系统
- 虚拟键盘模拟 API

**使用示例**:
```cpp
#include "m5tab5_keyboard.h"

M5Tab5Keyboard kb;
lv_obj_t* kb_obj = kb.create(parent);
kb.setTextArea(textarea);
```

### 2. FactoryTestKeyboard

工厂测试专用键盘，用于测试物理按键功能。

**特性**:
- 可配置行列数（默认 5x14）
- 按键状态可视化
- 支持触摸启用/禁用
- 物理按键模拟 API

**使用示例**:
```cpp
#include "factorytest_keyboard.h"

FactoryTestKeyboard kb(5, 14);  // 5行14列
lv_obj_t* kb_obj = kb.create(parent);
kb.setTouchEnabled(false);  // 禁用触摸
kb.simulatePhysicalKeyPress(0);  // 模拟按键0按下
```

## 基类 API

### 核心接口（必须实现）

```cpp
// 创建键盘 UI
virtual lv_obj_t* create(lv_obj_t* parent) = 0;

// 设置按键禁用状态
virtual void setKeyDisabled(uint16_t key_id, bool disabled) = 0;

// 模拟单字符输入
virtual void simulateCharacter(const char* key_char) = 0;

// 模拟字符串输入
virtual void simulateString(const char* str, uint32_t interval_ms = 100) = 0;
```

### 热键系统

```cpp
// 注册热键（支持 Ctrl+X, Alt+F, Ctrl+Alt+D 等）
int registerHotkey(
    uint16_t key_id,
    hotkey_event_type_t event,
    hotkey_callback_t callback,
    void* user_data = nullptr,
    bool require_ctrl = false,
    bool require_alt = false,
    bool require_shift = false,
    bool require_ctrl_only = false,
    bool require_alt_only = false
);

// 注销热键
bool unregisterHotkey(int hotkey_id);

// 清除所有热键
void clearAllHotkeys();
```

**热键使用示例**:
```cpp
// Ctrl+S 保存
bool save_callback(const hotkey_event_t* event, void* user_data) {
    ESP_LOGI("APP", "Save triggered");
    return true;  // 拦截事件
}
kb.registerHotkey(KB_KEY_S, HOTKEY_EVENT_PRESSED, save_callback, 
                  nullptr, true, false, false, true, false);

// 或使用便捷宏
M5TAB5_REGISTER_HOTKEY_CTRL(&kb, KB_KEY_S, save_callback);
```

### 文本区域操作

```cpp
// 设置目标文本区域
void setTextArea(lv_obj_t* ta);

// 获取键盘容器
lv_obj_t* getContainer() const;
```

### 布局约束（可选覆盖）

```cpp
virtual float getPreferredAspectRatio() const;  // 首选宽高比
virtual float getMinAspectRatio() const;        // 最小宽高比  
virtual float getMaxAspectRatio() const;        // 最大宽高比
virtual float getMaxScreenHeightPct() const;    // 最大屏幕高度占比
```

## 添加新键盘

详见 [keyboards/README.md](keyboards/README.md)

**快速步骤**:
1. 在 `keyboards/` 下创建新子目录
2. 创建头文件和实现文件
3. 继承 `OtoolKeyboard` 并实现必要接口
4. 更新 `CMakeLists.txt` 添加源文件和头文件路径

## 依赖项

- `lvgl/lvgl`: Light and Versatile Graphics Library
- `esp-idf`: ESP32 开发框架

## 设计原则

1. **基于继承**: 所有键盘继承自 `OtoolKeyboard` 基类
2. **独立模块**: 每个键盘实现独立于其他键盘
3. **统一接口**: 提供一致的 API 供应用层使用
4. **易于扩展**: 添加新键盘无需修改现有代码

## 许可证

请参阅 [LICENSE](LICENSE) 文件。

