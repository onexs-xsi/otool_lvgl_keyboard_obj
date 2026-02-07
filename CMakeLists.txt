# 基类源文件
set(SRCS "otool_lvgl_keyboard.cpp")

# M5Tab5 键盘源文件
list(APPEND SRCS "keyboards/m5tab5_keyboard/m5tab5_keyboard.cpp")

# 工厂测试键盘源文件
list(APPEND SRCS "keyboards/factorytest_keyboard/factorytest_keyboard.cpp")

# 注册组件
idf_component_register(
    SRCS ${SRCS}
    INCLUDE_DIRS 
        "include"                                   # 基类公共头文件
        "keyboards/m5tab5_keyboard"                 # M5Tab5 键盘头文件
        "keyboards/factorytest_keyboard"            # 工厂测试键盘头文件
    REQUIRES lvgl
    PRIV_REQUIRES log
)
