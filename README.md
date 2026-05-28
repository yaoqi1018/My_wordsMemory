# 背单词小程序

英语词汇学习助手，支持小学、初中、高中三个级别的词汇背诵与测试。

## 功能

- **控制台登录/注册** — 用户名密码管理，数据持久化存储
- **三种难度级别** — 小学词汇、初中词汇、高中词汇
- **两种背诵模式** — 看英文选中文 / 看中文选英文
- **两种出题顺序** — 随机出现 / 字典顺序
- **选择题测验** — 每题 4 个选项，即时反馈对错
- **成绩统计** — 答题完成后显示正确率与进度条
- **错题本** — 自动收录错题，支持单独复习

## 使用说明

1. 运行程序，在控制台完成登录或注册
2. 在主菜单选择词汇范围、出题顺序、背诵模式和答题数量
3. 点击"开始背诵"进入答题
4. 选择答案后自动显示对错并进入下一题
5. 答题结束后查看成绩，可选择返回主菜单或查看错题本

## 项目结构

```
My_words_memory/
├── main.cpp          # 程序入口，控制台登录 + EasyX 页面路由
├── common.h          # 公共头文件（常量、结构体、工具函数）
├── ui.h / ui.cpp     # EasyX 图形界面（菜单、答题、成绩、错题本）
├── user.h / user.cpp # 用户管理（登录、注册）
├── word.h / word.cpp # 词汇加载与缓存
├── quiz_engine.h /
│   └── quiz_engine.cpp   # 答题引擎（出题、判卷、计分）
├── wrong_book.h /
│   └── wrong_book.cpp    # 错题本管理
└── data/             # 词汇数据文件
```

## 环境要求

- **操作系统**：Windows（EasyX 仅支持 Windows）
- **编译器**：Visual Studio 2022（或 2019 / 2017 / 2015）
- **图形库**：EasyX 2024 版

## 安装 EasyX 图形库

EasyX 是一个 Windows 下的 C++ 图形库，替代了传统的 Turbo C 图形接口，无需额外配置复杂的图形环境。

### 步骤一：下载 EasyX 安装包

1. 打开 EasyX 官网：**https://easyx.cn/**
2. 点击下载按钮，获取 `EasyX_2024.exe`（或类似名称的安装程序）

### 步骤二：安装 EasyX

1. 关闭正在运行的 Visual Studio
2. 以管理员身份运行下载的安装程序（右键 → 以管理员身份运行）
3. 安装程序会自动检测系统中已安装的 Visual Studio 版本
4. 在弹出的列表中，勾选你使用的 Visual Studio 版本（如 "Visual Studio 2022"）
5. 点击"安装"按钮，等待完成

安装程序会自动将以下文件复制到 Visual Studio 的对应目录：

| 文件 | 安装位置 | 说明 |
|------|---------|------|
| `easyx.h` / `graphics.h` | `VC\Auxiliary\VS\include\` | 头文件 |
| `libeasyx.a` | `VC\Auxiliary\VS\lib\x64\` | 64 位库文件 |
| `libeasyxa.a` | `VC\Auxiliary\VS\lib\x86\` | 32 位库文件 |

安装完成后，在代码中 `#include <graphics.h>` 即可使用 EasyX 的全部功能。

### 验证安装

在 Visual Studio 中新建一个空项目，创建 `main.cpp` 并写入以下代码：

```cpp
#include <graphics.h>

int main()
{
    initgraph(640, 480);     // 创建 640x480 的绘图窗口
    circle(320, 240, 100);   // 画一个圆
    getchar();               // 暂停
    closegraph();            // 关闭绘图窗口
    return 0;
}
```

如果能成功编译并运行（显示一个带圆形的窗口），说明 EasyX 安装成功。

### 常见问题

**Q：安装程序没有检测到我的 Visual Studio？**

A：确保已正确安装 Visual Studio 并至少运行过一次。如果仍未检测到，可以手动配置：

   - 安装程序底部通常有"设置"或"浏览"按钮，可手动指定 VS 安装路径
   - 或者从安装目录手动复制头文件和库文件到 VS 的对应 `include` 和 `lib` 目录

**Q：链接时提示找不到 `libeasyx.a`？**

A：确认编译目标架构（x86/x64）与库文件匹配。64 位项目使用 `libeasyx.a`，32 位项目使用 `libeasyxa.a`。Visual Studio 默认新项目为 32 位（x86），检查项目属性 → 配置管理器 → 活动解决方案平台是否匹配。

**Q：编译报错 `cannot open source file "graphics.h"`？**

A：EasyX 未正确安装。重复安装步骤，或手动将 `graphics.h` 和 `easyx.h` 复制到 `VC\Auxiliary\VS\include\` 目录下。

## 编译运行

### 使用 Visual Studio

1. 双击打开 `My_words_memory.sln`
2. 确认已按上述步骤安装 EasyX
3. 按 `F5` 或点击"本地 Windows 调试器"运行

### 数据文件说明

程序运行时会在 `data/` 目录下自动创建所需文件。如果遇到文件读取错误，请确保 `data/` 文件夹与 `main.cpp` 在同一目录下。

## 数据文件

- `data/users.txt` — 用户信息（UTF-8 编码，Tab 分隔）
- `data/wrong.txt` — 错题记录（UTF-8 编码）
- `data/words_*.txt` — 各级别词汇表（Tab 分隔：英文\t音标\t中文）
