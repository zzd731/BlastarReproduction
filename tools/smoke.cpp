// 冒烟测试：只验证 raylib 能被 MinGW 正确链接
#include "raylib.h"

int main() {
    InitWindow(320, 200, "Blastar - smoke test");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("BLASTAR", 100, 90, 20, RAYWHITE);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
