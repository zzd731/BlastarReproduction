// OpenGL capability probe: does this session have a real GL context, and which version?
#include <windows.h>
#include <GL/gl.h>
#include <cstdio>

int main() {
    WNDCLASSA wc{};
    wc.lpfnWndProc = DefWindowProcA;
    wc.hInstance = GetModuleHandleA(nullptr);
    wc.lpszClassName = "glprobe";
    RegisterClassA(&wc);

    HWND h = CreateWindowExA(0, "glprobe", "glprobe", WS_OVERLAPPEDWINDOW,
                             0, 0, 320, 240, nullptr, nullptr, wc.hInstance, nullptr);
    if (!h) { std::printf("CreateWindow failed: %lu\n", GetLastError()); return 1; }

    HDC dc = GetDC(h);
    PIXELFORMATDESCRIPTOR pfd{};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;

    int pf = ChoosePixelFormat(dc, &pfd);
    std::printf("pixelformat = %d\n", pf);
    if (!pf || !SetPixelFormat(dc, pf, &pfd)) {
        std::printf("SetPixelFormat failed: %lu\n", GetLastError());
        return 2;
    }

    HGLRC rc = wglCreateContext(dc);
    std::printf("context = %p\n", (void *)rc);
    if (!rc) { std::printf("wglCreateContext failed: %lu\n", GetLastError()); return 3; }

    wglMakeCurrent(dc, rc);
    const char *ver = (const char *)glGetString(GL_VERSION);
    const char *ren = (const char *)glGetString(GL_RENDERER);
    std::printf("GL_VERSION  = %s\n", ver ? ver : "(null)");
    std::printf("GL_RENDERER = %s\n", ren ? ren : "(null)");
    return 0;
}
