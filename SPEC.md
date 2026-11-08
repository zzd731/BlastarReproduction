# Blastar（C++ 复刻）规格草案 v0.1

> 目的：把「复刻」从感觉变成可验收的规格——先定规则，再选引擎。
> 状态：下列参数由 2015 年 HTML5 移植（Ojaswy/Blastar）源码反推，**待与原版杂志影印逐项核对**。

## 1. 可确认的原始事实

- 1984 年，Elon Musk 12 岁，用 BASIC 写成；源码刊登在南非杂志《PC and Office Technology》，稿费 500 美元。
- 游戏目标：摧毁运送「氢弹」和「status beam（状态光束）」的外星货船。
- 原始清单影印收录于 Ashlee Vance《Elon Musk》传记照片页；HTML5 版即据此转写。
- 机型：资料有 Commodore 64 / VIC-20 两种说法——需核对，影响字符集、调色板与 POKE 地址。

## 2. 已解码的运行参数（JS 移植版）

| 项 | 值 |
|---|---|
| 逻辑坐标 | 约 250 × 200；8×8 精灵按 3 倍放大为 24px，画布 750×600 |
| 文本 | 24px Commodore 字体，25 行文本缓冲 |
| 主循环 | 150 ms / tick |
| 火箭循环 | 150 ms / tick |
| 状态光束循环 | 220 ms / tick |
| 玩家移动 | 8 方向，每次 4 单位；x ∈ [10,240]，y ∈ [10,180]（y>190 归 180） |
| 货船移动 | 每 tick x += 3；x > 250 回绕到 10 |
| 货船出生 y | 新货船 20 + rand*150；新一命 20 + 170*rand |
| 火箭 | 起点 (P, C-5)；每 tick y -= 6；同期货船 x += 4 |
| 命中得分 | +80 |
| 初始命数 | 5（SH = 5；SH < 0 结束） |
| 开火键 | 空格（原 BASIC 里是 strig 摇杆扳机） |
| 碰撞 | 8×8 位图逐行移位 AND；包围盒 \|dx\|<8、\|dy\|<8；玩家飞船与火箭之间不判定（友军火力） |
| 光束触发 | 主循环中 \|H-P\| ≤ 1；或火箭飞行中 H == P |
| 光束 | 从 (H, G+5) 向下每 tick +4，y>205 结束；命中则 SH -= 1 |
| 结束 | 显示 "FLEET DESTROYED"，询问是否再来一局 |

## 3. 流程状态机

```
TITLE（BLASTAR / BY E.R.MUSK / 音乐）
  → 需要说明？(Y/N) → INSTRUCTIONS → PLAY
PLAY（主循环）
  ├─ 命中货船 → +80 → 爆炸 → 随机位置重生
  ├─ 被光束击中 → SH-1 → 重生
  └─ SH < 0 → GAMEOVER → 再来一局？
```

## 4. BASIC → C++ 映射

- GOTO/GOSUB 标签 → `enum class State` + `switch`；三类「循环」（主循环 / 火箭 / 光束）→ 各自固定步长的 tick 函数。
- 全局变量 P C H G SH SC F D → `struct GameState`。
- RND → `std::mt19937`，固定种子以便逐项对照复现。
- 原版没有帧率概念（速度 = BASIC 解释器执行速度）→ 复刻用固定 150ms tick + accumulator，逻辑与渲染解耦。
- INKEY$/摇杆 → 输入状态 + 边沿检测（开火取上升沿）。

## 5. 技术选型（Windows）

- 编译器：MSVC 2022 Build Tools（或 MinGW-w64 / clang）
- 构建：CMake ≥ 3.20 + vcpkg（或 raylib 单文件集成）
- 图形/输入/音频：raylib（首选，最快出可玩版）；备选 SDL2 + SDL_image/SDL_mixer、SFML
  - 实测：**本项目固定用 raylib 4.5.0（MinGW 预编译）**。raylib 5.5 的 GLFW 3.4 在本机初始化会报
    `Win32: Failed to create interface`，与显卡无关（Intel Iris Xe / OpenGL 4.6，原生 WGL 建 3.3 core 上下文正常）。
    5.5 已移到 `vendor/raylib-5.5-broken` 备查，4.5 原始包在 `D:\Blastar\libs`。
- 纯字符路线：Windows Console API 或 PDCurses（若做「字符级还原」）
- 精灵：8×8 位图存 `uint8_t[8]`，碰撞用位移 + AND 直接照搬原逻辑
- 音效：方波/脉冲波合成（raylib `Wave`），音乐可留到 M2

## 6. 里程碑

1. **M0（半天）**：文字版骨架——标题、Y/N、飞船左右移动、开火、命中计分（用 `█` 方块即可）。
2. **M1**：完整规则闭环（5 命、状态光束、重生、结束）。
3. **M2**：换 8×8 位图精灵 + 音效 + 标题动画。
4. **M3**：与原版逐帧对照校准（录屏比对时序与手感）。

## 7. 版权

原 BASIC 清单版权状态不明（杂志发表）。个人学习复刻无碍；公开发布建议全部重写、注明「基于 1984 年 Elon Musk 的 Blastar 规则复刻」，不要打包原清单或杂志扫描件。
