// Blastar - M0 skeleton
// 规则来源：SPEC.md（由 1984 原版 BASIC 的 JS 移植反推）
// 占位图：方块/三角，M2 再换 8x8 位图与音效。

#include "raylib.h"

#include <cmath>
#include <cstdio>
#include <cstring>
#include <random>

namespace {

constexpr float kTick     = 0.15f;   // 主循环 150ms
constexpr int   kScreenW  = 750;
constexpr int   kScreenH  = 600;
constexpr float kScale    = 3.0f;    // 逻辑 250x200 -> 屏幕 750x600

struct Input {
    bool left = false, right = false, up = false, down = false, fire = false;
};

enum class State { Title, Instructions, Play, Over };

float Clamp(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

struct Game {
    std::mt19937 rng{20251984u};

    State state = State::Title;
    int   score = 0;
    int   ships = 5;

    float px = 120, py = 180;   // 玩家（左上角，8x8 逻辑单位）
    float fx = 30,  fy = 15;    // 外星货船
    bool  rocket = false;
    float rx = 0, ry = 0;
    bool  beam = false;
    float bx = 0, by = 0;

    int  pauseTicks = 0;        // 爆炸/死亡停顿
    bool exploding = false;
    bool playerDead = false;
    bool justHit = false;       // 本 tick 打中货船，用于闪一下

    void NewGame() {
        score = 0;
        ships = 5;
        state = State::Play;
        SpawnRound();
    }

    void SpawnRound() {
        px = 120; py = 180;
        rocket = false;
        beam = false;
        exploding = false;
        playerDead = false;
        pauseTicks = 0;
        fx = 10;
        fy = 20 + float(rng() % 150);
    }

    void RespawnFreighter() {
        fx = 10;
        fy = 20 + float(rng() % 150);
    }

    void StartBeam() {
        beam = true;
        bx = fx;
        by = fy + 5;
    }

    void LoseShip() {
        --ships;
        playerDead = true;
        pauseTicks = 4;
    }
};

void UpdateBeam(Game &g) {
    g.by += 3.0f;
    if (std::fabs(g.bx - g.px) < 8.0f && std::fabs(g.by - g.py) < 8.0f) {
        g.beam = false;
        g.LoseShip();
        return;
    }
    if (g.by > 205.0f) g.beam = false;
}

void UpdatePlay(Game &g, const Input &in) {
    if (g.pauseTicks > 0) {
        --g.pauseTicks;
        if (g.pauseTicks == 0) {
            if (g.playerDead) {
                g.playerDead = false;
                if (g.ships < 0) g.state = State::Over;
                else g.SpawnRound();
            } else if (g.exploding) {
                g.exploding = false;
                g.RespawnFreighter();
            }
        }
        return;
    }

    if (g.beam) {
        UpdateBeam(g);
        return;
    }

    // 玩家移动：每 tick 4 单位
    float dx = (in.right ? 4.0f : 0.0f) - (in.left ? 4.0f : 0.0f);
    float dy = (in.down  ? 4.0f : 0.0f) - (in.up   ? 4.0f : 0.0f);
    g.px = Clamp(g.px + dx, 10.0f, 240.0f);
    g.py = Clamp(g.py + dy, 10.0f, 180.0f);

    // 货船与玩家同列 -> 状态光束
    if (std::fabs(g.fx - g.px) <= 1.0f) {
        g.StartBeam();
        return;
    }

    if (g.rocket) {
        g.ry -= 6.0f;
        g.fx += 4.0f;
        if (g.fx > 250.0f) g.fx = 10.0f;

        if (std::fabs(g.rx - g.fx) < 8.0f && std::fabs(g.ry - g.fy) < 8.0f) {
            g.score += 80;
            g.rocket = false;
            g.exploding = true;
            g.pauseTicks = 2;
            g.justHit = true;
            return;
        }
        if (std::fabs(g.fx - g.px) <= 1.0f) {
            g.rocket = false;
            g.StartBeam();
            return;
        }
        if (g.ry < -10.0f) g.rocket = false;
    } else {
        g.fx += 3.0f;
        if (g.fx > 250.0f) g.fx = 10.0f;

        if (in.fire) {
            g.rocket = true;
            g.rx = g.px;
            g.ry = g.py - 5.0f;
        }
    }
}

void Update(Game &g, const Input &in) {
    if (g.state != State::Play) return;
    g.justHit = false;
    UpdatePlay(g, in);
}

void DrawShip(const Game &g) {
    Vector2 c{g.px * kScale + 12, g.py * kScale + 12};
    DrawTriangle({c.x, c.y - 20}, {c.x - 18, c.y + 18}, {c.x + 18, c.y + 18}, RAYWHITE);
    DrawTriangle({c.x, c.y + 12}, {c.x - 8, c.y + 18}, {c.x + 8, c.y + 18}, BLACK);
}

void DrawFreighter(const Game &g) {
    float x = g.fx * kScale, y = g.fy * kScale;
    DrawRectangle(int(x), int(y), 24, 24, DARKGREEN);
    DrawRectangleLines(int(x), int(y), 24, 24, GREEN);
    DrawRectangle(int(x) + 6, int(y) + 6, 12, 12, GREEN);
}

void DrawGame(const Game &g) {
    ClearBackground(BLACK);

    if (g.state == State::Title) {
        DrawText("BLASTAR", 250, 140, 60, RAYWHITE);
        DrawText("BY E.R.MUSK", 300, 215, 20, GRAY);
        DrawText("DO YOU NEED INSTRUCTIONS (Y/N)", 195, 330, 20, LIGHTGRAY);
        DrawText("arrows = move    space = fire", 240, 500, 16, DARKGRAY);
        return;
    }

    if (g.state == State::Instructions) {
        DrawText("BLASTAR", 300, 60, 40, RAYWHITE);
        DrawText("ARROWS  - MOVE YOUR SHIP", 190, 170, 20, LIGHTGRAY);
        DrawText("SPACE   - FIRE ROCKETS", 190, 210, 20, LIGHTGRAY);
        DrawText("MISSION: DESTROY THE ALIEN FREIGHTER", 140, 290, 20, LIGHTGRAY);
        DrawText("CARRYING DEADLY HYDROGEN BOMBS", 170, 330, 20, LIGHTGRAY);
        DrawText("AND STATUS BEAM MACHINES", 195, 370, 20, LIGHTGRAY);
        DrawText("PRESS ANY KEY TO START", 250, 480, 20, GRAY);
        return;
    }

    DrawText(TextFormat("SCORE %d          SHIPS %d", g.score, g.ships < 0 ? 0 : g.ships), 20, 12, 22, GRAY);

    if (g.state == State::Over) {
        DrawText("BLASTAR", 300, 150, 40, RAYWHITE);
        DrawText("FLEET DESTROYED", 255, 230, 28, RED);
        DrawText("ANOTHER GAME (Y/N)", 250, 330, 22, LIGHTGRAY);
        return;
    }

    if (g.beam) {
        DrawRectangle(int(g.bx * kScale), int(g.by * kScale), 4, 30, RED);
        DrawText("STATUS BEAM", 270, 40, 18, RED);
    }
    if (g.rocket) {
        DrawRectangle(int(g.rx * kScale) + 3, int(g.ry * kScale), 6, 18, ORANGE);
    }
    if (g.pauseTicks > 0 && g.exploding) {
        DrawCircle(int(g.fx * kScale) + 12, int(g.fy * kScale) + 12, 22, YELLOW);
    } else {
        DrawFreighter(g);
    }
    DrawShip(g);
}

Input ReadInput() {
    Input in;
    in.left  = IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A);
    in.right = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);
    in.up    = IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W);
    in.down  = IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S);
    in.fire  = IsKeyDown(KEY_SPACE);
    return in;
}

int RunTest() {
    Game g;
    g.NewGame();
    g.px = 120; g.py = 180;
    g.fx = 112; g.fy = 140;
    g.rocket = true; g.rx = 120; g.ry = 150;
    Input in;

    Update(g, in);
    bool hit = (g.score == 80 && g.exploding && g.pauseTicks == 2);
    Update(g, in);
    Update(g, in);
    bool respawned = (!g.exploding && g.pauseTicks == 0 && g.score == 80);

    std::printf("test: hit=%d score=%d exploding=%d pause=%d respawn=%d\n",
                int(hit), g.score, int(g.exploding), g.pauseTicks, int(respawned));
    return (hit && respawned) ? 0 : 1;
}

int RunShot(const char *path) {
    InitWindow(kScreenW, kScreenH, "Blastar - shot");
    if (!IsWindowReady()) {
        std::printf("shot: window not ready\n");
        return 1;
    }
    Game g;
    g.NewGame();
    for (int i = 0; i < 3; ++i) {
        BeginDrawing();
        DrawGame(g);
        EndDrawing();
    }
    TakeScreenshot(path);
    CloseWindow();
    std::printf("shot: %s\n", path);
    return 0;
}

int RunSim() {
    Game g;
    g.NewGame();
    Input in;
    int ticks = 0;
    for (int i = 0; i < 2000 && g.state == State::Play; ++i) {
        in.right = (i / 20) % 2 == 0;
        in.left  = !in.right;
        in.fire  = (i % 9) == 0;
        Update(g, in);
        ++ticks;
    }
    std::printf("sim: state=%d score=%d ships=%d ticks=%d\n",
                int(g.state), g.score, g.ships, ticks);
    return g.state == State::Over || ticks > 100 ? 0 : 1;
}

} // namespace

int main(int argc, char **argv) {
    if (argc > 1 && std::strcmp(argv[1], "--sim") == 0) return RunSim();
    if (argc > 1 && std::strcmp(argv[1], "--test") == 0) return RunTest();
    if (argc > 2 && std::strcmp(argv[1], "--shot") == 0) return RunShot(argv[2]);

    InitWindow(kScreenW, kScreenH, "Blastar - M0");
    SetTargetFPS(60);

    Game g;
    float acc = 0.0f;

    while (!WindowShouldClose()) {
        Input in = ReadInput();

        if (g.state == State::Title) {
            if (IsKeyPressed(KEY_Y)) g.state = State::Instructions;
            else if (IsKeyPressed(KEY_N)) g.NewGame();
        } else if (g.state == State::Instructions) {
            if (GetKeyPressed() != 0) g.NewGame();
        } else if (g.state == State::Over) {
            if (IsKeyPressed(KEY_Y)) g.NewGame();
        } else {
            acc += GetFrameTime();
            while (acc >= kTick) {
                Update(g, in);
                acc -= kTick;
            }
        }

        BeginDrawing();
        DrawGame(g);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
