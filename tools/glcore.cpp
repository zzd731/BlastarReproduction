// Probe: can we create a modern (3.3 core) context via wglCreateContextAttribsARB?
// This is the path GLFW uses; a legacy wglCreateContext may succeed while this fails.
#include <windows.h>
#include <GL/gl.h>
#include <cstdio>

typedef HGLRC(WINAPI *PFN_wglCreateContextAttribsARB)(HDC, HGLRC, const int *);

int main() {
    WNDCLASSA wc{};
    wc.lpfnWndProc = DefWindowProcA;
    wc.hInstance = GetModuleHandleA(nullptr);
    wc.lpszClassName = "glcore";
    RegisterClassA(&wc);

    HWND h = CreateWindowExA(0, "glcore", "glcore", WS_OVERLAPPEDWINDOW,
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
    SetPixelFormat(dc, pf, &pfd);

    HGLRC legacy = wglCreateContext(dc);
    std::printf("legacy context : %p\n", (void *)legacy);
    PFN_wglCreateContextAttribsARB createAttribs = nullptr;
    if (legacy) {
        wglMakeCurrent(dc, legacy);
        std::printf("GL_VERSION     : %s\n", (const char *)glGetString(GL_VERSION));
        // wglGetProcAddress only answers while a context is current.
        createAttribs = (PFN_wglCreateContextAttribsARB)wglGetProcAddress("wglCreateContextAttribsARB");
        std::printf("wglCreateContextAttribsARB : %p\n", (void *)createAttribs);
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(legacy);
    }

    if (!createAttribs) { std::printf("=> extension missing\n"); return 2; }

    const int attribs[] = {
        0x2091, 3,        // WGL_CONTEXT_MAJOR_VERSION_ARB
        0x2092, 3,        // WGL_CONTEXT_MINOR_VERSION_ARB
        0x2094, 0x0001,   // WGL_CONTEXT_PROFILE_MASK_ARB, CORE
        0
    };
    SetLastError(0);
    HGLRC core = createAttribs(dc, nullptr, attribs);
    std::printf("core 3.3 context : %p  (GetLastError=%lu)\n", (void *)core, GetLastError());
    if (core) {
        wglMakeCurrent(dc, core);
        std::printf("core GL_VERSION  : %s\n", (const char *)glGetString(GL_VERSION));
        std::printf("RESULT: CORE_OK\n");
    } else {
        std::printf("RESULT: CORE_FAILED\n");
    }
    return core ? 0 : 3;
}
