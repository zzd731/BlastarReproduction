# Blastar（C++ 复刻版） / Blastar (C++ remake)

> 1984 年，12 岁的 Elon Musk 用 BASIC 写了《Blastar》，卖给南非杂志《PC and Office Technology》，稿费 500 美元。
> 这是一个用 C++ + raylib 复刻其玩法的学习项目（M0 骨架：占位图形、无音效）。
>
> In 1984, a 12-year-old Elon Musk wrote *Blastar* in BASIC and sold it to the South African magazine
> *PC and Office Technology* for $500. This is a C++/raylib study remake of its rules
> (M0 skeleton: placeholder graphics, no sound yet).

---

## 中文说明

### 运行要求

- Windows 10/11（64 位）
- MinGW-w64 GCC（本机路径 `C:\mingw64\bin\g++.exe`；换机器时改 `build.bat` 里的 `CXX`）
- raylib **4.5.0**（已随项目放在 `vendor\raylib`，无需另装）

> 注意：raylib 5.5 自带 GLFW 3.4，在本机会报 `Win32: Failed to create interface` 无法建窗，因此锁定 4.5。

### 编译与运行

| 操作 | 说明 |
|---|---|
| 双击 `run.bat` | 编译并启动（推荐） |
| 双击 `build.bat` | 只编译，产物 `build\blastar.exe` |
| 直接运行 `build\blastar.exe` | 已编译好的游戏 |

失败时 `run.bat` 会停住并显示 `build\run.log`。

### 键位

| 按键 | 作用 |
|---|---|
| `←` / `A` | 左移 |
| `→` / `D` | 右移 |
| `↑` / `W` | 上移 |
| `↓` / `S` | 下移 |
| `空格` | 发射火箭（同时只能有一发） |
| `Y` | 标题页：查看说明；结束页：再来一局 |
| `N` | 标题页：跳过说明直接开始 |
| 任意键 | 说明页：开始游戏 |
| `ESC` / 关闭窗口 | 退出 |

### 规则

| 项目 | 规则 |
|---|---|
| 目标 | 击毁运送氢弹的外星货船 |
| 计分 | 每艘 +80 |
| 生命 | 显示 5，实际 6 次机会（减到 -1 结束） |
| 货船 | 每 0.15 秒右移 3（你开火后 4），出界回绕 |
| 你的移动 | 每 0.15 秒 4 单位，范围 x 10–240、y 10–180 |
| 火箭 | 每 0.15 秒上移 6 |
| 命中判定 | 8×8 像素块重叠 |
| 状态光束 | 货船与你同列（±1）时发射，命中扣 1 命 |
| 节奏 | 逻辑固定 150ms/步，画面 60FPS |

### 目录结构

```
Blastar\
  README.md          本文件（中英文）
  SPEC.md            规则规格草案
  build.bat          编译脚本
  run.bat            编译 + 启动
  src\main.cpp       全部游戏代码
  tools\             自检/探测小程序（smoke、glprobe、glcore、下载脚本）
  vendor\raylib\     raylib 4.5.0（头文件 + libraylib.a）
  build\             编译产物与运行日志
  docs\screenshot.png 运行截图
```

### 自查命令

```
build\blastar.exe --sim    逻辑模拟 2000 步
build\blastar.exe --test   命中 + 计分断言
build\blastar.exe --shot out.png   渲染一帧并存图
```

### 版权

原版 BASIC 清单版权状态不明。本项目为个人学习用途的**玩法复刻**，代码全部重写；
如需公开发布，请保留署名并注明“基于 1984 年 Elon Musk 的 Blastar 规则复刻”。

---

## English

### Requirements

- Windows 10/11 (64-bit)
- MinGW-w64 GCC (on this machine: `C:\mingw64\bin\g++.exe`; edit `CXX` in `build.bat` elsewhere)
- raylib **4.5.0** (bundled in `vendor\raylib`)

> Note: raylib 5.5 ships GLFW 3.4, which fails to create a window on this machine
> (`Win32: Failed to create interface`). Stay on 4.5.

### Build & run

| Action | Result |
|---|---|
| Double-click `run.bat` | Build and launch (recommended) |
| Double-click `build.bat` | Build only → `build\blastar.exe` |
| Run `build\blastar.exe` | Play the prebuilt binary |

On failure `run.bat` pauses and prints `build\run.log`.

### Controls

| Key | Action |
|---|---|
| `←` / `A` | Move left |
| `→` / `D` | Move right |
| `↑` / `W` | Move up |
| `↓` / `S` | Move down |
| `Space` | Fire a rocket (one at a time) |
| `Y` | Title: show instructions; Game over: play again |
| `N` | Title: start immediately |
| Any key | Instructions screen: start |
| `ESC` / close window | Quit |

### Rules

| Item | Rule |
|---|---|
| Goal | Destroy the alien freighter carrying hydrogen bombs |
| Score | +80 per freighter |
| Lives | Displayed as 5, actually 6 chances (ends below -1… i.e. at -1) |
| Freighter | Moves right 3 units every 0.15 s (4 while your rocket flies), wraps at the edge |
| Player | 4 units per 0.15 s, x 10–240, y 10–180 |
| Rocket | 6 units up per 0.15 s |
| Hit test | 8×8 pixel overlap |
| Status beam | Fired when the freighter lines up with you (±1), costs 1 life |
| Timing | Fixed 150 ms logic step, 60 FPS rendering |

### Layout

```
Blastar\
  README.md          this file (Chinese + English)
  SPEC.md            rules spec draft
  build.bat          build script
  run.bat            build + launch
  src\main.cpp       all game code
  tools\             self-test / probe utilities
  vendor\raylib\     raylib 4.5.0 (headers + libraylib.a)
  build\             binaries and run logs
  docs\screenshot.png screenshot
```

### Self-check commands

```
build\blastar.exe --sim            simulate 2000 logic steps
build\blastar.exe --test           assert hit + scoring path
build\blastar.exe --shot out.png   render one frame to PNG
```

### License / credits

The original BASIC listing's copyright status is unclear. This project is a **rules remake** for
personal study; all code is rewritten. If you publish it, keep the credit:
"based on the rules of Elon Musk's 1984 Blastar".
