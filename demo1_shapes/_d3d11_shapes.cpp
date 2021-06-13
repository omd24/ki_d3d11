/* ===========================================================
   #File: _d3d11_shapes.cpp #
   #Date: 13 June 2021 #
   #Revision: 1.0 #
   #Creator: Omid Miresmaeili #
   #Description: Shapes scene in Direct3D 11 #
   #from http://www.d3dcoder.net/d3d11.htm #
   #Notice: (C) Copyright 2021 by Omid. All Rights Reserved. #
   =========================================================== */

#include <windows.h>

#include <tchar.h>

struct D3D11RenderContext {
    HINSTANCE instance;

    void * device;
    HWND wnd;

    bool paused;
    bool initialized;
};

D3D11RenderContext * g_render_ctx;


LRESULT
msg_proc (D3D11RenderContext * render_ctx, UINT msg, WPARAM wparam, LPARAM lparam) {
    // Is the application in a minimized or maximized state?
    static bool min_maxed = false;

    RECT client_rect = {0, 0, 0, 0};
    switch (msg) {

    // WM_ACTIVE is sent when the window is activated or deactivated.
    // We pause the game when the main window is deactivated and 
    // unpause it when it becomes active.
    case WM_ACTIVATE:
        if (LOWORD(wparam) == WA_INACTIVE)
            render_ctx->paused = true;
        else
            render_ctx->paused = false;
        return 0;


    // WM_SIZE is sent when the user resizes the window.  
    case WM_SIZE:
        if (render_ctx->device) {



            if (wparam == SIZE_MINIMIZED) {
                render_ctx->paused = true;
                min_maxed = true;
            } else if (wparam == SIZE_MAXIMIZED) {
                render_ctx->paused = false;
                min_maxed = true;

            }
            // Restored is any resize that is not a minimize or maximize.
            // For example, restoring the window to its default size
            // after a minimize or maximize, or from dragging the resize
            // bars.
            else if (wparam == SIZE_RESTORED) {
                render_ctx->paused = false;

                // Are we restoring from a mimimized or maximized state, 
                // and are in windowed mode?  Do not execute this code if 
                // we are restoring to full screen mode.
                if (min_maxed && render_ctx) {
                    
                } else {
                    // No, which implies the user is resizing by dragging
                    // the resize bars.  However, we do not reset the device
                    // here because as the user continuously drags the resize
                    // bars, a stream of WM_SIZE messages is sent to the window,
                    // and it would be pointless (and slow) to reset for each
                    // WM_SIZE message received from dragging the resize bars.
                    // So instead, we reset after the user is done resizing the
                    // window and releases the resize bars, which sends a
                    // WM_EXITSIZEMOVE message.
                }
                min_maxed = false;
            }
        }
        return 0;


    // WM_EXITSIZEMOVE is sent when the user releases the resize bars.
    // Here we reset everything based on the new window dimensions.
    case WM_EXITSIZEMOVE:
        GetClientRect(render_ctx->wnd, &client_rect);

        return 0;

    // WM_CLOSE is sent when the user presses the 'X' button in the
    // caption bar menu.
    case WM_CLOSE:
        DestroyWindow(render_ctx->wnd);
        return 0;

    // WM_DESTROY is sent when the window is being destroyed.
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_KEYDOWN:
        if (wparam == VK_ESCAPE)
            (void *)0;
        else if (wparam == 'F')
            (void *)0;
        return 0;
    }
    return DefWindowProc(render_ctx->wnd, msg, wparam, lparam);
}
LRESULT CALLBACK
wnd_proc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    // Don't start processing messages until the application has been created.
    if (g_render_ctx->initialized)
        return msg_proc(g_render_ctx, msg, wparam, lparam);
    else
        return DefWindowProc(hwnd, msg, wparam, lparam);
}
static void
init_window (D3D11RenderContext * render_ctx) {
    WNDCLASS wc;
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = wnd_proc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = render_ctx->instance;
    wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = 0;
    wc.lpszClassName = _T("D3DWndClassName");
    RegisterClass(&wc);

    if (RegisterClass(&wc)) {
        int x = 10;
    }

    // Default to a window with a client area rectangle of 800x600.
    RECT R = {0, 0, 800, 600};
    ::AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
    render_ctx->wnd = CreateWindow(
        _T("D3DWndClassName"),
        _T("D3D9 Demo"),
        WS_OVERLAPPEDWINDOW, 100, 100, R.right, R.bottom,
        0, 0, render_ctx->instance, 0
    );

    ::ShowWindow(render_ctx->wnd, SW_SHOW);
    ::UpdateWindow(render_ctx->wnd);
}
static void
init_render_ctx (
    D3D11RenderContext * render_ctx, HINSTANCE inst
) {
    if (render_ctx) {
        memset(render_ctx, 0, sizeof(*render_ctx));
        render_ctx->instance = inst;

        init_window(render_ctx);

        render_ctx->initialized = true;
    }
}

int WINAPI
WinMain (
    _In_ HINSTANCE instance,
    _In_opt_ HINSTANCE prev_instance,
    _In_ LPSTR cmdline,
    _In_ int showcmd
) {
#pragma region Initialize

    g_render_ctx = (D3D11RenderContext *)::malloc(sizeof(D3D11RenderContext));
    init_render_ctx (
        g_render_ctx,
        instance
    );


#pragma endregion


}

