# OTOOL LVGL Keyboard 组件架构说明

本文档详细说明了 OTOOL LVGL Keyboard 组件的架构设计和扩展机制。

## 架构概览

### 设计模式

本组件采用**面向对象设计**，使用**继承和多态**实现键盘的通用框架：

```
┌─────────────────────────┐
│   OtoolKeyboard         │  基类（抽象）
│  (otool_lvgl_keyboard)  │
├─────────────────────────┤
│ + create()              │  纯虚函数
│ + setKeyDisabled()      │  纯虚函数
│ + simulateCharacter()   │  纯虚函数
│ + simulateString()      │  纯虚函数
│ + registerHotkey()      │  热键系统
│ + setTextArea()         │  文本区域管理
└───────────┬─────────────┘
            │ 继承
            ├──────────────────┬──────────────────┐
            │                  │                  │
    ┌───────▼────────┐  ┌──────▼────────┐  ┌─────▼──────┐
    │ M5Tab5Keyboard │  │ FactoryTest   │  │ YourKeyboard│
    │                │  │   Keyboard    │  │  (未来)     │
    └────────────────┘  └───────────────┘  └────────────┘
```

### 核心组件

#### 1. 基类 `OtoolKeyboard`

**位置**: `otool_lvgl_keyboard.h` / `otool_lvgl_keyboard.cpp`

**职责**:
- 定义键盘通用接口
- 实现热键系统（组合键注册、触发、管理）
- 提供文本区域操作的通用方法
- 管理键盘容器对象

**关键特性**:
- 纯虚函数定义了派生类必须实现的接口
- 热键系统完全在基类中实现，所有派生类自动继承
- 提供 `sendTextToTextarea()` 处理常见控制字符（退格、回车等）

#### 2. 派生键盘实现

**位置**: `keyboards/` 目录下的各个子目录

**职责**:
- 实现具体的键盘布局和 UI
- 处理按键事件逻辑
- 实现键盘特定的功能（如 Shift、Caps Lock 等）

**设计要求**:
- 必须实现所有纯虚函数
- 可以覆盖默认的布局约束方法
- 可以添加键盘特定的公共 API

## 目录结构详解

```
otool_lvgl_keyboard_obj/
│
├── CMakeLists.txt                    # 组件构建脚本
│   └── 定义源文件列表和头文件路径
│
├── README.md                         # 用户使用文档
├── ARCHITECTURE.md                   # 本文件
│
├── include/                          # 公共头文件目录
│   └── otool_lvgl_keyboard.h         # 基类头文件
│
├── otool_lvgl_keyboard.cpp           # 基类实现
│
└── keyboards/                        # 键盘实现目录
    ├── README.md                     # 添加新键盘指南
    │
    ├── m5tab5_keyboard/              # M5Tab5 键盘
    │   ├── m5tab5_keyboard.h         # 类声明
    │   ├── m5tab5_keyboard.cpp       # 实现
    │   └── m5tab5_keyboard_def.h     # 按键布局定义
    │
    └── factorytest_keyboard/         # 工厂测试键盘
        ├── factorytest_keyboard.h    # 类声明
        └── factorytest_keyboard.cpp  # 实现
```

### 头文件包含路径规则

```
CMakeLists.txt 中的 INCLUDE_DIRS:
├── include/                          → 基类头文件
├── keyboards/m5tab5_keyboard/        → M5Tab5 键盘头文件
└── keyboards/factorytest_keyboard/   → FactoryTest 键盘头文件
```

**包含规则**:
- **基类头文件**: 所有键盘都可以通过 `#include "otool_lvgl_keyboard.h"` 包含
- **键盘头文件**: 应用代码通过 `#include "m5tab5_keyboard.h"` 包含特定键盘
- **内部头文件**: 键盘实现内部的 `_def.h` 文件在同目录下，使用相对路径

## 接口设计

### 必须实现的虚函数

#### 1. `create()`

```cpp
virtual lv_obj_t* create(lv_obj_t* parent) = 0;
```

**目的**: 创建键盘的 LVGL UI 对象  
**实现要点**:
- 创建容器对象并保存到 `m_kb_cont`
- 创建所有按键对象
- 设置事件回调
- 返回容器对象指针

**示例**:
```cpp
lv_obj_t* MyKeyboard::create(lv_obj_t* parent) {
    m_kb_cont = lv_obj_create(parent);
    // ... 创建按键 UI ...
    return m_kb_cont;
}
```

#### 2. `setKeyDisabled()`

```cpp
virtual void setKeyDisabled(uint16_t key_id, bool disabled) = 0;
```

**目的**: 控制单个按键的启用/禁用状态  
**实现要点**:
- 查找对应的按键对象
- 设置 `LV_STATE_DISABLED` 标志
- 更新视觉样式（可选）

#### 3. `simulateCharacter()`

```cpp
virtual void simulateCharacter(const char* key_char) = 0;
```

**目的**: 模拟输入单个字符  
**实现要点**:
- 查找与字符对应的按键
- 模拟按键按下和释放事件
- 触发相应的输入逻辑

#### 4. `simulateString()`

```cpp
virtual void simulateString(const char* str, uint32_t interval_ms = 100) = 0;
```

**目的**: 模拟输入字符串  
**实现要点**:
- 逐字符调用 `simulateCharacter()`
- 在字符间添加延迟
- 处理组合键（大写字母需要 Shift）

### 可选覆盖的虚函数

#### 布局约束

```cpp
virtual float getPreferredAspectRatio() const { return 2.9f; }
virtual float getMinAspectRatio() const { return 2.0f; }
virtual float getMaxAspectRatio() const { return 4.0f; }
virtual float getMaxScreenHeightPct() const { return 0.60f; }
```

**目的**: 定义键盘的布局约束  
**使用场景**: 应用层可以根据这些值动态调整键盘大小

## 热键系统架构

### 热键注册流程

```
应用层
  │
  │ registerHotkey(KB_KEY_S, HOTKEY_EVENT_PRESSED, callback, ...)
  ▼
OtoolKeyboard::registerHotkey()
  │
  │ 创建 hotkey_registration_t
  │ 添加到 m_hotkey_registrations
  ▼
返回 hotkey_id
```

### 热键触发流程

```
键盘按键事件
  │
  ▼
派生类事件处理函数
  │
  │ 更新修饰键状态 (m_ctrl_pressed, m_alt_pressed)
  │
  │ triggerHotkey(...)
  ▼
OtoolKeyboard::triggerHotkey()
  │
  │ 遍历 m_hotkey_registrations
  │ 匹配 key_id + 修饰键组合
  │
  ├─ 匹配成功 ─▶ 调用 callback()
  │                  │
  │                  ├─ callback 返回 true ─▶ 拦截事件
  │                  └─ callback 返回 false ─▶ 继续处理
  │
  └─ 无匹配 ─▶ 不拦截，继续默认处理
```

### 热键事件类型

```cpp
typedef enum {
    HOTKEY_EVENT_PRESSED = 0,   // 按键按下时触发
    HOTKEY_EVENT_RELEASED,      // 按键释放时触发
    HOTKEY_EVENT_CLICKED,       // 按键点击时触发
    HOTKEY_EVENT_BEFORE_INPUT   // 字符输入前触发
} hotkey_event_type_t;
```

**使用场景**:
- `PRESSED`: 快捷键开始时执行操作（如 Ctrl+S 保存）
- `RELEASED`: 释放时执行操作
- `CLICKED`: 完整点击后执行操作
- `BEFORE_INPUT`: 在字符输入到文本区域之前拦截（如过滤输入）

## 扩展机制

### 添加新键盘的步骤

#### 1. 创建键盘目录

```bash
mkdir -p keyboards/my_new_keyboard
```

#### 2. 创建头文件 `my_new_keyboard.h`

```cpp
#ifndef MY_NEW_KEYBOARD_H
#define MY_NEW_KEYBOARD_H

#include "otool_lvgl_keyboard.h"

#ifdef __cplusplus

class MyNewKeyboard : public OtoolKeyboard {
public:
    MyNewKeyboard();
    virtual ~MyNewKeyboard();

    // 实现必要接口
    virtual lv_obj_t* create(lv_obj_t* parent) override;
    virtual void setKeyDisabled(uint16_t key_id, bool disabled) override;
    virtual void simulateCharacter(const char* key_char) override;
    virtual void simulateString(const char* str, uint32_t interval_ms) override;

    // 可选：覆盖布局约束
    virtual float getPreferredAspectRatio() const override { return 3.5f; }

protected:
    // 键盘特定成员
    lv_obj_t* m_key_buttons[MAX_KEYS];
    // ...
};

#endif // __cplusplus
#endif // MY_NEW_KEYBOARD_H
```

#### 3. 创建实现文件 `my_new_keyboard.cpp`

```cpp
#include "my_new_keyboard.h"
#include <esp_log.h>

static const char* TAG = "MyNewKeyboard";

MyNewKeyboard::MyNewKeyboard() {
    // 初始化
}

MyNewKeyboard::~MyNewKeyboard() {
    // 清理
}

lv_obj_t* MyNewKeyboard::create(lv_obj_t* parent) {
    m_kb_cont = lv_obj_create(parent);
    // 创建键盘 UI
    return m_kb_cont;
}

// 实现其他必要方法...
```

#### 4. 更新 `CMakeLists.txt`

```cmake
# 新键盘源文件
list(APPEND SRCS "keyboards/my_new_keyboard/my_new_keyboard.cpp")

# 更新 INCLUDE_DIRS
idf_component_register(
    SRCS ${SRCS}
    INCLUDE_DIRS 
        "include"
        "keyboards/m5tab5_keyboard"
        "keyboards/factorytest_keyboard"
        "keyboards/my_new_keyboard"        # 添加这一行
    REQUIRES lvgl
    PRIV_REQUIRES log
)
```

#### 5. 在应用中使用

```cpp
#include "my_new_keyboard.h"

MyNewKeyboard kb;
lv_obj_t* kb_obj = kb.create(screen);
kb.setTextArea(textarea);
```

### 键盘实现最佳实践

#### 1. 事件处理

**推荐**: 使用静态回调 + 实例方法模式

```cpp
// 头文件
protected:
    static void eventHandlerStatic(lv_event_t* e);
    void handleKeyEvent(lv_event_t* e);

// 实现文件
void MyKeyboard::eventHandlerStatic(lv_event_t* e) {
    MyKeyboard* kb = (MyKeyboard*)lv_event_get_user_data(e);
    kb->handleKeyEvent(e);
}

void MyKeyboard::handleKeyEvent(lv_event_t* e) {
    // 实例方法，可以访问成员变量
    lv_event_code_t code = lv_event_get_code(e);
    // ...
}
```

#### 2. 修饰键管理

**推荐**: 在基类中维护修饰键状态

```cpp
// 基类已提供
protected:
    bool m_ctrl_pressed;
    bool m_alt_pressed;

// 派生类在事件处理中更新
void MyKeyboard::handleKeyEvent(lv_event_t* e) {
    if (key_id == KEY_CTRL) {
        m_ctrl_pressed = (code == LV_EVENT_PRESSED);
    }
    // 触发热键
    if (!triggerHotkey(...)) {
        // 执行默认处理
    }
}
```

#### 3. 资源管理

**原则**: LVGL 对象由 LVGL 管理，无需手动 delete

```cpp
// 正确 ✓
lv_obj_t* btn = lv_btn_create(parent);

// 错误 ✗ - 不要手动删除 LVGL 对象
// lv_obj_del(btn);  // LVGL 会在父对象删除时自动处理
```

#### 4. 日志记录

**推荐**: 使用 ESP_LOG 系列宏

```cpp
static const char* TAG = "MyKeyboard";

ESP_LOGI(TAG, "Keyboard created");
ESP_LOGW(TAG, "Key %d is disabled", key_id);
ESP_LOGE(TAG, "Failed to create button");
```

## 设计考量

### 为什么使用继承而非组合？

**继承优势**:
1. 强制接口一致性（纯虚函数）
2. 代码复用（热键系统在基类实现）
3. 多态性（应用层可以使用 `OtoolKeyboard*` 指针）
4. 清晰的类型关系

**适用场景**: 所有键盘都是"一种"键盘，共享核心行为

### 为什么将键盘分离到子目录？

**优势**:
1. **独立性**: 每个键盘独立开发和维护
2. **可扩展性**: 添加新键盘无需修改现有代码
3. **清晰性**: 相关文件集中在一起
4. **模块化**: 便于团队协作和代码审查

### 为什么热键系统在基类？

**理由**:
1. 热键是通用功能，所有键盘都可能需要
2. 避免在每个派生类中重复实现
3. 统一的热键 API，降低学习成本
4. 修饰键状态管理集中化

## 未来扩展

### 可能的新功能

1. **主题系统**: 动态切换键盘外观
2. **国际化**: 支持多语言键盘布局
3. **输入法集成**: 支持拼音、手写等输入法
4. **手势识别**: 滑动输入等高级功能
5. **自定义布局**: 运行时动态生成键盘布局

### 扩展点

- 基类可以添加新的虚函数（保持向后兼容）
- 热键系统可以扩展支持更多事件类型
- 可以添加键盘工厂类用于动态创建键盘实例

## 总结

本架构的核心思想是：
- **抽象与具体分离**: 基类定义接口，派生类实现细节
- **通用与特定平衡**: 热键等通用功能在基类，键盘特定功能在派生类
- **开放与封闭**: 对扩展开放（添加新键盘），对修改封闭（无需改基类）

遵循这些原则，可以轻松添加新的键盘实现，同时保持代码的清晰和可维护性。
