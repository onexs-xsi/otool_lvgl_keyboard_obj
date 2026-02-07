# Keyboards 目录说明

本目录包含所有键盘实现。每个键盘都有自己独立的子目录。

## 目录结构

```
keyboards/
├── README.md                    # 本文件
├── m5tab5_keyboard/             # M5Tab5 键盘实现
│   ├── m5tab5_keyboard.h        # 键盘类声明
│   ├── m5tab5_keyboard.cpp      # 键盘实现
│   └── m5tab5_keyboard_def.h    # 键盘布局定义
└── factorytest_keyboard/        # 工厂测试键盘实现
    ├── factorytest_keyboard.h   # 键盘类声明
    └── factorytest_keyboard.cpp # 键盘实现
```

## 如何添加新键盘

### 1. 创建键盘子目录

在 `keyboards/` 目录下创建新的子目录，例如 `my_new_keyboard/`

```bash
mkdir keyboards/my_new_keyboard
```

### 2. 创建键盘文件

每个键盘至少需要以下文件：

- **`my_new_keyboard.h`** - 键盘类声明
- **`my_new_keyboard.cpp`** - 键盘实现
- *可选* **`my_new_keyboard_def.h`** - 键盘布局定义（如果键盘有复杂的按键定义）

### 3. 键盘类声明模板

```cpp
#ifndef MY_NEW_KEYBOARD_H
#define MY_NEW_KEYBOARD_H

#include "otool_lvgl_keyboard.h"

#ifdef __cplusplus

class MyNewKeyboard : public OtoolKeyboard {
public:
    MyNewKeyboard();
    virtual ~MyNewKeyboard();

    // 必须实现的虚函数
    virtual lv_obj_t* create(lv_obj_t* parent) override;
    virtual void setKeyDisabled(uint16_t key_id, bool disabled) override;
    virtual void simulateCharacter(const char* key_char) override;
    virtual void simulateString(const char* str, uint32_t interval_ms = 100) override;

    // 可选：覆盖布局约束
    virtual float getPreferredAspectRatio() const override { return 3.0f; }
    virtual float getMinAspectRatio() const override { return 2.0f; }
    virtual float getMaxAspectRatio() const override { return 4.0f; }
    virtual float getMaxScreenHeightPct() const override { return 0.60f; }

protected:
    // 键盘特定的成员变量和方法
};

#endif // __cplusplus
#endif // MY_NEW_KEYBOARD_H
```

### 4. 更新 CMakeLists.txt

在组件的 `CMakeLists.txt` 中添加新键盘的源文件和头文件路径：

```cmake
# 新键盘源文件
list(APPEND SRCS "keyboards/my_new_keyboard/my_new_keyboard.cpp")

# 注册组件时添加头文件路径
idf_component_register(
    SRCS ${SRCS}
    INCLUDE_DIRS 
        "include"                                   # 基类公共头文件
        "keyboards/m5tab5_keyboard"                 # M5Tab5 键盘
        "keyboards/factorytest_keyboard"            # FactoryTest 键盘
        "keyboards/my_new_keyboard"                 # 新键盘头文件路径
    REQUIRES lvgl
    PRIV_REQUIRES log
)
```

### 5. 必须实现的接口

所有键盘类都必须继承自 `OtoolKeyboard` 并实现以下虚函数：

#### 5.1 `create()` - 创建键盘 UI

```cpp
virtual lv_obj_t* create(lv_obj_t* parent) override;
```

**功能**: 创建并返回键盘的 LVGL 容器对象  
**参数**: `parent` - 父对象  
**返回**: 键盘容器对象指针

#### 5.2 `setKeyDisabled()` - 设置按键禁用状态

```cpp
virtual void setKeyDisabled(uint16_t key_id, bool disabled) override;
```

**功能**: 启用或禁用特定按键  
**参数**:
- `key_id` - 按键 ID
- `disabled` - true=禁用，false=启用

#### 5.3 `simulateCharacter()` - 模拟单字符输入

```cpp
virtual void simulateCharacter(const char* key_char) override;
```

**功能**: 模拟输入单个字符  
**参数**: `key_char` - 要模拟的字符

#### 5.4 `simulateString()` - 模拟字符串输入

```cpp
virtual void simulateString(const char* str, uint32_t interval_ms = 100) override;
```

**功能**: 模拟输入字符串  
**参数**:
- `str` - 要模拟的字符串
- `interval_ms` - 字符间隔时间（毫秒）

### 6. 可用的基类功能

#### 6.1 文本区域操作

```cpp
// 设置目标文本区域
void setTextArea(lv_obj_t* ta);

// 向文本区域发送文本
void sendTextToTextarea(const char* text);
```

#### 6.2 热键系统

```cpp
// 注册热键
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

// 启用/禁用热键
void setHotkeyEnabled(bool enabled);
```

#### 6.3 布局约束（可选覆盖）

```cpp
virtual float getPreferredAspectRatio() const;  // 首选宽高比
virtual float getMinAspectRatio() const;        // 最小宽高比
virtual float getMaxAspectRatio() const;        // 最大宽高比
virtual float getMaxScreenHeightPct() const;    // 最大屏幕高度占比
```

## 设计原则

1. **独立性**: 每个键盘独立于其他键盘，放在自己的子目录中
2. **可扩展性**: 通过继承 `OtoolKeyboard` 实现新键盘，无需修改基类
3. **一致性**: 所有键盘遵循相同的接口规范
4. **模块化**: 键盘相关的所有文件集中在一个目录中

## 示例参考

- **M5Tab5Keyboard**: 完整功能的物理键盘实现，包含复杂的按键逻辑
- **FactoryTestKeyboard**: 简化的测试键盘实现，适合快速原型

## 注意事项

1. 键盘头文件中必须包含 `#include "otool_lvgl_keyboard.h"` 以继承基类
2. 确保在 `CMakeLists.txt` 中添加源文件和头文件路径
3. 使用 `#ifdef __cplusplus` 保护 C++ 代码
4. 所有 LVGL 对象的生命周期由 LVGL 管理，无需手动释放
5. 可以参考现有键盘实现来了解最佳实践
