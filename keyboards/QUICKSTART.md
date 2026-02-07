# 快速入门：添加新键盘

本指南将带你快速创建一个新的键盘实现。

## 步骤 1: 准备工作

### 确定键盘名称

选择一个描述性的名称，例如：`NumericKeyboard`（数字键盘）

### 命名规范

- **类名**: `NumericKeyboard` (驼峰命名)
- **文件名**: `numeric_keyboard.h` 和 `numeric_keyboard.cpp` (小写+下划线)
- **目录名**: `numeric_keyboard` (小写+下划线)
- **宏定义**: `NUMERIC_KEYBOARD_H` (大写+下划线)

## 步骤 2: 创建目录和文件

### 2.1 创建键盘目录

```bash
cd components/otool_lvgl_keyboard_obj/keyboards
mkdir numeric_keyboard
cd numeric_keyboard
```

### 2.2 复制模板文件

```bash
# 从模板复制头文件
cp ../TEMPLATE_keyboard.h numeric_keyboard.h

# 从模板复制实现文件
cp ../TEMPLATE_keyboard.cpp numeric_keyboard.cpp
```

### 2.3 批量替换名称

在 `numeric_keyboard.h` 和 `numeric_keyboard.cpp` 中：

- 将 `TEMPLATE` 替换为 `NUMERIC`
- 将 `Template` 替换为 `Numeric`
- 将 `template` 替换为 `numeric`

**VS Code 快捷方式**: `Ctrl+H` (查找替换)

## 步骤 3: 更新 CMakeLists.txt

编辑 `components/otool_lvgl_keyboard_obj/CMakeLists.txt`：

```cmake
# 基类源文件
set(SRCS "otool_lvgl_keyboard.cpp")

# M5Tab5 键盘源文件
list(APPEND SRCS "keyboards/m5tab5_keyboard/m5tab5_keyboard.cpp")

# FactoryTest 键盘源文件
list(APPEND SRCS "keyboards/factorytest_keyboard/factorytest_keyboard.cpp")

# 添加这一行 ↓↓↓
# Numeric 键盘源文件
list(APPEND SRCS "keyboards/numeric_keyboard/numeric_keyboard.cpp")

# 注册组件
idf_component_register(
    SRCS ${SRCS}
    INCLUDE_DIRS 
        "include"
        "keyboards/m5tab5_keyboard"
        "keyboards/factorytest_keyboard"
        "keyboards/numeric_keyboard"         # 添加这一行 ↓↓↓
    REQUIRES lvgl
    PRIV_REQUIRES log
)
```

## 步骤 4: 实现键盘功能

### 4.1 设计键盘布局

在 `numeric_keyboard.h` 中定义按键布局常量：

```cpp
// 在类定义之前添加
#define NUMERIC_KB_ROWS 4
#define NUMERIC_KB_COLS 3
#define NUMERIC_KB_KEY_COUNT (NUMERIC_KB_ROWS * NUMERIC_KB_COLS)

// 按键 ID 枚举
typedef enum {
    NUMKEY_1 = 0, NUMKEY_2, NUMKEY_3,
    NUMKEY_4, NUMKEY_5, NUMKEY_6,
    NUMKEY_7, NUMKEY_8, NUMKEY_9,
    NUMKEY_CLEAR, NUMKEY_0, NUMKEY_ENTER
} numeric_key_id_t;
```

### 4.2 实现 `create()` 方法

在 `numeric_keyboard.cpp` 中：

```cpp
lv_obj_t* NumericKeyboard::create(lv_obj_t* parent)
{
    m_kb_cont = lv_obj_create(parent);
    
    // 使用网格布局
    lv_obj_set_layout(m_kb_cont, LV_LAYOUT_GRID);
    
    // 定义网格
    static lv_coord_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(m_kb_cont, col_dsc, row_dsc);
    
    // 按键标签
    const char* labels[12] = {
        "1", "2", "3",
        "4", "5", "6",
        "7", "8", "9",
        "C", "0", "OK"
    };
    
    // 创建按键
    for (int i = 0; i < 12; i++) {
        lv_obj_t* btn = lv_btn_create(m_kb_cont);
        lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_STRETCH, i % 3, 1,
                                  LV_GRID_ALIGN_STRETCH, i / 3, 1);
        
        lv_obj_t* label = lv_label_create(btn);
        lv_label_set_text(label, labels[i]);
        lv_obj_center(label);
        
        lv_obj_set_user_data(btn, (void*)(uintptr_t)i);
        lv_obj_add_event_cb(btn, eventHandlerStatic, LV_EVENT_ALL, this);
        
        m_key_buttons[i] = btn;
    }
    
    return m_kb_cont;
}
```

### 4.3 实现事件处理

```cpp
void NumericKeyboard::handleKeyEvent(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code != LV_EVENT_CLICKED) return;
    
    lv_obj_t* btn = lv_event_get_target(e);
    uint16_t key_id = (uint16_t)(uintptr_t)lv_obj_get_user_data(btn);
    
    if (!m_textarea) return;
    
    switch (key_id) {
        case NUMKEY_CLEAR:
            lv_textarea_set_text(m_textarea, "");
            break;
        case NUMKEY_ENTER:
            lv_textarea_add_char(m_textarea, '\n');
            break;
        default:
            if (key_id <= NUMKEY_0) {
                char digit = (key_id == NUMKEY_0) ? '0' : ('1' + key_id);
                lv_textarea_add_char(m_textarea, digit);
            }
            break;
    }
}
```

## 步骤 5: 在应用中使用

### 5.1 包含头文件

```cpp
#include "numeric_keyboard.h"
```

### 5.2 创建键盘实例

```cpp
// 创建键盘
NumericKeyboard numeric_kb;
lv_obj_t* kb = numeric_kb.create(lv_scr_act());

// 关联文本区域
lv_obj_t* ta = lv_textarea_create(lv_scr_act());
numeric_kb.setTextArea(ta);
```

### 5.3 注册热键（可选）

```cpp
// ESC 键清空输入
bool clear_callback(const hotkey_event_t* event, void* user_data) {
    NumericKeyboard* kb = (NumericKeyboard*)user_data;
    kb->reset();
    return true;
}

numeric_kb.registerHotkey(NUMKEY_CLEAR, HOTKEY_EVENT_PRESSED, 
                          clear_callback, &numeric_kb);
```

## 步骤 6: 编译和测试

### 6.1 编译项目

```bash
idf.py build
```

### 6.2 烧录和运行

```bash
idf.py flash monitor
```

## 进阶技巧

### 1. 添加按键定义文件

如果键盘布局复杂，创建 `numeric_keyboard_def.h`：

```cpp
#ifndef NUMERIC_KEYBOARD_DEF_H
#define NUMERIC_KEYBOARD_DEF_H

// 布局参数
#define NUMERIC_KB_ROWS 4
#define NUMERIC_KB_COLS 3

// 按键映射表
typedef struct {
    uint16_t id;
    const char* label;
    const char* value;
} numeric_key_t;

static const numeric_key_t numeric_keys[] = {
    {0, "1", "1"}, {1, "2", "2"}, {2, "3", "3"},
    // ...
};

#endif
```

### 2. 支持多语言

```cpp
void NumericKeyboard::setLanguage(const char* lang) {
    if (strcmp(lang, "zh") == 0) {
        // 中文标签
    } else {
        // 英文标签
    }
}
```

### 3. 自定义样式

```cpp
void NumericKeyboard::setTheme(lv_color_t primary_color) {
    for (int i = 0; i < m_key_count; i++) {
        lv_obj_set_style_bg_color(m_key_buttons[i], primary_color, 0);
    }
}
```

### 4. 添加动画效果

```cpp
void NumericKeyboard::handleKeyEvent(lv_event_t* e) {
    if (code == LV_EVENT_PRESSED) {
        // 按下动画
        lv_obj_t* btn = lv_event_get_target(e);
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, btn);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_height);
        lv_anim_set_values(&a, lv_obj_get_height(btn), lv_obj_get_height(btn) - 5);
        lv_anim_set_time(&a, 100);
        lv_anim_start(&a);
    }
}
```

## 常见问题

### Q1: 编译错误 "undefined reference"

**解决**: 检查 `CMakeLists.txt` 是否正确添加了源文件

### Q2: 键盘不显示

**解决**: 检查 `create()` 是否正确创建容器并设置大小

### Q3: 按键事件不响应

**解决**: 检查是否正确添加事件回调 `lv_obj_add_event_cb()`

### Q4: 热键不工作

**解决**: 
1. 确保调用了 `triggerHotkey()` 
2. 检查修饰键状态是否正确更新

## 参考资源

- [LVGL 文档](https://docs.lvgl.io/)
- [keyboards/README.md](README.md) - 详细的键盘添加指南
- [ARCHITECTURE.md](../ARCHITECTURE.md) - 架构设计说明
- [m5tab5_keyboard/](m5tab5_keyboard/) - 完整实现示例
- [factorytest_keyboard/](factorytest_keyboard/) - 简化实现示例

## 检查清单

开发完成后，检查以下项目：

- [ ] 头文件包含了 `#include "otool_lvgl_keyboard.h"`
- [ ] 类继承自 `OtoolKeyboard`
- [ ] 实现了所有纯虚函数
- [ ] 在 `CMakeLists.txt` 中添加了源文件和头文件路径
- [ ] 构造函数中正确初始化成员变量
- [ ] 析构函数中释放了动态分配的内存
- [ ] 事件处理中调用了 `triggerHotkey()` 支持热键系统
- [ ] 代码添加了适当的日志输出
- [ ] 通过编译测试
- [ ] 键盘功能测试正常

完成后，你就拥有了一个全新的键盘实现！🎉
