/* ===========================================================
   #File: _d3d11_shapes.cpp #
   #Date: 13 June 2021 #
   #Revision: 1.0 #
   #Creator: Omid Miresmaeili #
   #Description: Shapes scene in Direct3D 11 #
   #from http://www.d3dcoder.net/d3d11.htm #
   #Notice: (C) Copyright 2021 by Omid. All Rights Reserved. #
   =========================================================== */

#pragma warning (disable: 6011)     // dereferencing a potentially null pointer
#pragma warning (disable: 26495)    // not initializing struct members
#pragma warning (disable: 26812)    // unscoped enum
#pragma warning (disable: 6387)     // potentially invalid pointer

#include <windows.h>
#include <WindowsX.h>

#include <d3d11.h>
#include <d3dx11effect.h>

#include <DirectXMath.h>
using namespace DirectX;

#include "geometry.h"

#include <tchar.h>

struct DemoVertex {
    XMFLOAT3 position;
    XMFLOAT4 color;
};

struct D3D11RenderContext {
    ID3D11Device *              device;
    ID3D11DeviceContext *       d3d_immediate_context;
    IDXGISwapChain *            swapchain;
    ID3D11Texture2D *           depth_stencil_buffer;
    ID3D11RenderTargetView *    rtv;
    ID3D11DepthStencilView *    dsv;
    D3D11_VIEWPORT              viewport;
    D3D_DRIVER_TYPE             d3d_driver_type;

    // effects sutff
    ID3DX11Effect *                 fx;
    ID3DX11EffectTechnique *        tech;
    ID3DX11EffectMatrixVariable *   fx_wvp;

    ID3D11InputLayout* input_layout;
    ID3D11RasterizerState* wireframe_rs;

    // Define transformations from local spaces to world space.
    XMFLOAT4X4 sphere_world[10];
    XMFLOAT4X4 cylinder_world[10];
    XMFLOAT4X4 box_world;
    XMFLOAT4X4 grid_world;
    XMFLOAT4X4 center_sphere;

    XMFLOAT4X4 view;
    XMFLOAT4X4 proj;

    ID3D11Buffer * vb;
    ID3D11Buffer * ib;

    // vertex buffer and index buffer data
    int     box_vertex_offset;
    int     grid_vertex_offset;
    int     sphere_vertex_offset;
    int     cylinder_vertex_offset;

    UINT    box_index_offset;
    UINT    grid_index_offset;
    UINT    sphere_index_offset;
    UINT    cylinder_index_offset;

    UINT    box_index_count;
    UINT    grid_index_count;
    UINT    sphere_index_count;
    UINT    cylinder_index_count;

    // camera, window, etc
    HWND    wnd;

    float   theta;
    float   phi;
    float   radius;

    int     width;
    int     height;
    bool    enable_4x_msaa;
    UINT    msaa_quality;

    HINSTANCE   instance;

    POINT   last_mouse_pos;
    bool    paused;
    bool    minimized;
    bool    maximize;
    bool    resizing;

    bool    initialized;
};

D3D11RenderContext * g_render_ctx;

static void
d3d11_resize (D3D11RenderContext * render_ctx) {
    // Release the old views, as they hold references to the buffers we
    // will be destroying.  Also release the old depth/stencil buffer.
    if (render_ctx->rtv)
        render_ctx->rtv->Release();
    if (render_ctx->dsv)
        render_ctx->dsv->Release();
    if (render_ctx->depth_stencil_buffer)
        render_ctx->depth_stencil_buffer->Release();

        // Resize the swap chain and recreate the render target view.
    render_ctx->swapchain->ResizeBuffers(
        1, render_ctx->width, render_ctx->height,
        DXGI_FORMAT_R8G8B8A8_UNORM, 0
    );
    ID3D11Texture2D * backbuffer;
    render_ctx->swapchain->GetBuffer(0, IID_PPV_ARGS(&backbuffer));
    render_ctx->device->CreateRenderTargetView(backbuffer, 0, &render_ctx->rtv);
    backbuffer->Release();

    // Create the depth/stencil buffer and view.

    D3D11_TEXTURE2D_DESC depth_stencil_desc;

    depth_stencil_desc.Width     = render_ctx->width;
    depth_stencil_desc.Height    = render_ctx->height;
    depth_stencil_desc.MipLevels = 1;
    depth_stencil_desc.ArraySize = 1;
    depth_stencil_desc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;

    // Use 4X MSAA? --must match swap chain MSAA values.
    if (render_ctx->enable_4x_msaa) {
        depth_stencil_desc.SampleDesc.Count   = 4;
        depth_stencil_desc.SampleDesc.Quality = render_ctx->msaa_quality - 1;
    }
    // No MSAA
    else {
        depth_stencil_desc.SampleDesc.Count   = 1;
        depth_stencil_desc.SampleDesc.Quality = 0;
    }

    depth_stencil_desc.Usage          = D3D11_USAGE_DEFAULT;
    depth_stencil_desc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
    depth_stencil_desc.CPUAccessFlags = 0;
    depth_stencil_desc.MiscFlags      = 0;

    render_ctx->device->CreateTexture2D(&depth_stencil_desc, 0, &render_ctx->depth_stencil_buffer);
    render_ctx->device->CreateDepthStencilView(render_ctx->depth_stencil_buffer, 0, &render_ctx->dsv);

    // Bind the render target view and depth/stencil view to the pipeline.
    render_ctx->d3d_immediate_context->OMSetRenderTargets(1, &render_ctx->rtv, render_ctx->dsv);

    // Set the viewport transform.

    render_ctx->viewport.TopLeftX = 0;
    render_ctx->viewport.TopLeftY = 0;
    render_ctx->viewport.Width    = static_cast<float>(render_ctx->width);
    render_ctx->viewport.Height   = static_cast<float>(render_ctx->height);
    render_ctx->viewport.MinDepth = 0.0f;
    render_ctx->viewport.MaxDepth = 1.0f;

    render_ctx->d3d_immediate_context->RSSetViewports(1, &render_ctx->viewport);
}
static void
update_scene (D3D11RenderContext * render_ctx) {

}
static void
draw_scene (D3D11RenderContext * render_ctx) {

}
static void
handle_mouse_down (WPARAM btn_state, int x, int y) {

}
static void
handle_mouse_up (WPARAM btn_state, int x, int y) {

}
static void
handle_mouse_move (WPARAM btn_state, int x, int y) {

}

static void
create_geom_buffers (D3D11RenderContext * render_ctx) {

}
static void
create_fx (D3D11RenderContext * render_ctx) {

}
static void
create_vertex_layout (D3D11RenderContext * render_ctx) {

}


LRESULT
msg_proc (D3D11RenderContext * render_ctx, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
     // WM_ACTIVATE is sent when the window is activated or deactivated.  
     // We pause the game when the window is deactivated and unpause it 
     // when it becomes active.  
    case WM_ACTIVATE:
        if (LOWORD(wparam) == WA_INACTIVE)
            render_ctx->paused = true;
        else
            render_ctx->paused = false;
        return 0;

    // WM_SIZE is sent when the user resizes the window.  
    case WM_SIZE:
        // Save the new client area dimensions.
        render_ctx->width  = LOWORD(lparam);
        render_ctx->height = HIWORD(lparam);
        if (render_ctx->device) {
            if (wparam == SIZE_MINIMIZED) {
                render_ctx->paused = true;
                render_ctx->minimized = true;
                render_ctx->maximize = false;
            } else if (wparam == SIZE_MAXIMIZED) {
                render_ctx->paused = false;
                render_ctx->minimized = false;
                render_ctx->maximize = true;
                d3d11_resize(render_ctx);
            } else if (wparam == SIZE_RESTORED) {

                // Restoring from minimized state?
                if (render_ctx->minimized) {
                    render_ctx->paused = false;
                    render_ctx->minimized = false;
                    d3d11_resize(render_ctx);
                }

                // Restoring from maximized state?
                else if (render_ctx->maximize) {
                    render_ctx->paused = false;
                    render_ctx->maximize = false;
                    d3d11_resize(render_ctx);
                } else if (render_ctx->resizing) {
                    // If user is dragging the resize bars, we do not resize 
                    // the buffers here because as the user continuously 
                    // drags the resize bars, a stream of WM_SIZE messages are
                    // sent to the window, and it would be pointless (and slow)
                    // to resize for each WM_SIZE message received from dragging
                    // the resize bars.  So instead, we reset after the user is 
                    // done resizing the window and releases the resize bars, which 
                    // sends a WM_EXITSIZEMOVE message.
                } else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
                {
                    d3d11_resize(render_ctx);
                }
            }
        }
        return 0;

    // WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
    case WM_ENTERSIZEMOVE:
        render_ctx->paused = true;
        render_ctx->resizing  = true;
        return 0;

    // WM_EXITSIZEMOVE is sent when the user releases the resize bars.
    // Here we reset everything based on the new window dimensions.
    case WM_EXITSIZEMOVE:
        render_ctx->paused = false;
        render_ctx->resizing  = false;
        d3d11_resize(render_ctx);
        return 0;

    // WM_DESTROY is sent when the window is being destroyed.
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    // The WM_MENUCHAR message is sent when a menu is active and the user presses 
    // a key that does not correspond to any mnemonic or accelerator key. 
    case WM_MENUCHAR:
        // Don't beep when we alt-enter.
        return MAKELRESULT(0, MNC_CLOSE);

    // Catch this message so to prevent the window from becoming too small.
    case WM_GETMINMAXINFO:
        ((MINMAXINFO*)lparam)->ptMinTrackSize.x = 200;
        ((MINMAXINFO*)lparam)->ptMinTrackSize.y = 200;
        return 0;

    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
        handle_mouse_down(wparam, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
        return 0;
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
        handle_mouse_up(wparam, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
        return 0;
    case WM_MOUSEMOVE:
        handle_mouse_move(wparam, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
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
    RECT R = {0, 0, render_ctx->width, render_ctx->height};
    ::AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
    render_ctx->wnd = CreateWindow(
        _T("D3DWndClassName"),
        _T("D3D11 Demo"),
        WS_OVERLAPPEDWINDOW, 100, 100, R.right, R.bottom,
        0, 0, render_ctx->instance, 0
    );

    ::ShowWindow(render_ctx->wnd, SW_SHOW);
    ::UpdateWindow(render_ctx->wnd);
}
static void
init_direct3d (D3D11RenderContext * render_ctx) {
    // Create the device and device context.

    UINT create_device_flags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
    create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevel;
    HRESULT hr = D3D11CreateDevice(
            0,                 // default adapter
            render_ctx->d3d_driver_type,
            0,                 // no software device
            create_device_flags,
            0, 0,              // default feature level array
            D3D11_SDK_VERSION,
            &render_ctx->device,
            &featureLevel,
            &render_ctx->d3d_immediate_context
    );

    if (FAILED(hr))
        MessageBox(0, _T("D3D11CreateDevice Failed."), 0, 0);

    if (featureLevel != D3D_FEATURE_LEVEL_11_0)
        MessageBox(0, _T("Direct3D Feature Level 11 unsupported."), 0, 0);

    // Check 4X MSAA quality support for our back buffer format.
    // All Direct3D 11 capable devices support 4X MSAA for all render 
    // target formats, so we only need to check quality support.

    render_ctx->device->CheckMultisampleQualityLevels(
        DXGI_FORMAT_R8G8B8A8_UNORM, 4, &render_ctx->msaa_quality
    );

    // Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.

    DXGI_SWAP_CHAIN_DESC swapchain_desc;
    swapchain_desc.BufferDesc.Width  = render_ctx->width;
    swapchain_desc.BufferDesc.Height = render_ctx->height;
    swapchain_desc.BufferDesc.RefreshRate.Numerator = 60;
    swapchain_desc.BufferDesc.RefreshRate.Denominator = 1;
    swapchain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapchain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapchain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    // Use 4X MSAA? 
    if (render_ctx->enable_4x_msaa) {
        swapchain_desc.SampleDesc.Count   = 4;
        swapchain_desc.SampleDesc.Quality = render_ctx->msaa_quality - 1;
    }
    // No MSAA
    else {
        swapchain_desc.SampleDesc.Count   = 1;
        swapchain_desc.SampleDesc.Quality = 0;
    }

    swapchain_desc.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchain_desc.BufferCount  = 1;
    swapchain_desc.OutputWindow = render_ctx->wnd;
    swapchain_desc.Windowed     = true;
    swapchain_desc.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
    swapchain_desc.Flags        = 0;

    // To correctly create the swap chain, we must use the IDXGIFactory that was
    // used to create the device.  If we tried to use a different IDXGIFactory instance
    // (by calling CreateDXGIFactory), we get an error: "IDXGIFactory::CreateSwapChain: 
    // This function is being called with a device from a different IDXGIFactory."

    IDXGIDevice* dxgi_dev = 0;
    render_ctx->device->QueryInterface(IID_PPV_ARGS(&dxgi_dev));

    IDXGIAdapter* dxgi_adapter = 0;
    dxgi_dev->GetParent(IID_PPV_ARGS(&dxgi_adapter));

    IDXGIFactory* dxgi_factory = 0;
    dxgi_adapter->GetParent(IID_PPV_ARGS(&dxgi_factory));

    if (render_ctx->device)
        dxgi_factory->CreateSwapChain(render_ctx->device, &swapchain_desc, &render_ctx->swapchain);
    else
        MessageBox(0, _T("invalid device"), 0, 0);

    dxgi_dev->Release();
    dxgi_adapter->Release();
    dxgi_factory->Release();

    // The remaining steps that need to be carried out for d3d creation
    // also need to be executed every time the window is resized.  So
    // just call the OnResize method here to avoid code duplication.

    d3d11_resize(render_ctx);
}
static void
init_render_ctx (
    D3D11RenderContext * render_ctx, HINSTANCE inst
) {
    if (render_ctx) {
        memset(render_ctx, 0, sizeof(*render_ctx));
        render_ctx->instance = inst;

        render_ctx->d3d_driver_type = D3D_DRIVER_TYPE_HARDWARE;
        render_ctx->enable_4x_msaa = false;
        render_ctx->width = 800;
        render_ctx->height = 600;

        init_window(render_ctx);
        init_direct3d(render_ctx);

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
#pragma region Main Loop
    MSG msg = {0};

    while (msg.message != WM_QUIT) {
        // If there are Window messages then process them.
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        // Otherwise, do animation/game stuff.
        else {
            if (!g_render_ctx->paused) {
                update_scene(g_render_ctx);
                draw_scene(g_render_ctx);

                // -- display results on window's title bar
                TCHAR buf[50];
                _sntprintf_s(buf, 50, 50, _T("D3D11 shapes demo:   %s"), _T(""));
                ::SetWindowText(g_render_ctx->wnd, buf);
            } else {
                Sleep(100);
            }
        }
    }
#pragma endregion
#pragma region Cleanup
    g_render_ctx->rtv->Release();
    g_render_ctx->dsv->Release();
    g_render_ctx->swapchain->Release();
    g_render_ctx->depth_stencil_buffer->Release();

    // Restore all default settings.
    if (g_render_ctx->d3d_immediate_context)
        g_render_ctx->d3d_immediate_context->ClearState();

    g_render_ctx->d3d_immediate_context->Release();
    g_render_ctx->device->Release();
#pragma endregion
}

