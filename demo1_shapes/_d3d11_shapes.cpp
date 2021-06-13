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

#include <stdio.h>
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

    float aspect_ratio = (float)g_render_ctx->width / g_render_ctx->height;
    XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * XM_PI, aspect_ratio, 1.0f, 1000.0f);
    XMStoreFloat4x4(&render_ctx->proj, P);
}
static void
update_scene (D3D11RenderContext * render_ctx) {
    // Convert Spherical to Cartesian coordinates.
    float x = render_ctx->radius * sinf(render_ctx->phi) * cosf(render_ctx->theta);
    float z = render_ctx->radius * sinf(render_ctx->phi) * sinf(render_ctx->theta);
    float y = render_ctx->radius * cosf(render_ctx->phi);

    // Build the view matrix.
    XMVECTOR pos    = XMVectorSet(x, y, z, 1.0f);
    XMVECTOR target = XMVectorZero();
    XMVECTOR up     = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
    XMStoreFloat4x4(&render_ctx->view, V);
}
static void
draw_scene (D3D11RenderContext * render_ctx) {
    XMVECTORF32 lightblue = {0.69f, 0.77f, 0.87f, 1.0f};
    render_ctx->d3d_immediate_context->ClearRenderTargetView(render_ctx->rtv, reinterpret_cast<const float*>(&lightblue));
    render_ctx->d3d_immediate_context->ClearDepthStencilView(render_ctx->dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    render_ctx->d3d_immediate_context->IASetInputLayout(render_ctx->input_layout);
    render_ctx->d3d_immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    render_ctx->d3d_immediate_context->RSSetState(render_ctx->wireframe_rs);

    UINT stride = sizeof(DemoVertex);
    UINT offset = 0;
    render_ctx->d3d_immediate_context->IASetVertexBuffers(0, 1, &render_ctx->vb, &stride, &offset);
    render_ctx->d3d_immediate_context->IASetIndexBuffer(render_ctx->ib, DXGI_FORMAT_R32_UINT, 0);

    // Set constants

    XMMATRIX view  = XMLoadFloat4x4(&render_ctx->view);
    XMMATRIX proj  = XMLoadFloat4x4(&render_ctx->proj);
    XMMATRIX view_proj = view * proj;

    D3DX11_TECHNIQUE_DESC techDesc;
    render_ctx->tech->GetDesc(&techDesc);
    for (UINT p = 0; p < techDesc.Passes; ++p) {
        // Draw the grid.
        XMMATRIX world = XMLoadFloat4x4(&render_ctx->grid_world);
        XMMATRIX wvp = world * view_proj;
        render_ctx->fx_wvp->SetMatrix(reinterpret_cast<float*>(&wvp));
        render_ctx->tech->GetPassByIndex(p)->Apply(0, render_ctx->d3d_immediate_context);
        render_ctx->d3d_immediate_context->DrawIndexed(render_ctx->grid_index_count, render_ctx->grid_index_offset, render_ctx->grid_vertex_offset);

        // Draw the box.
        world = XMLoadFloat4x4(&render_ctx->box_world);
        wvp = world * view_proj;
        render_ctx->fx_wvp->SetMatrix(reinterpret_cast<float*>(&wvp));
        render_ctx->tech->GetPassByIndex(p)->Apply(0, render_ctx->d3d_immediate_context);
        render_ctx->d3d_immediate_context->DrawIndexed(render_ctx->box_index_count, render_ctx->box_index_offset, render_ctx->box_vertex_offset);

        // Draw center sphere.
        world = XMLoadFloat4x4(&render_ctx->center_sphere);
        wvp = world * view_proj;
        render_ctx->fx_wvp->SetMatrix(reinterpret_cast<float*>(&wvp));
        render_ctx->tech->GetPassByIndex(p)->Apply(0, render_ctx->d3d_immediate_context);
        render_ctx->d3d_immediate_context->DrawIndexed(render_ctx->sphere_index_count, render_ctx->sphere_index_offset, render_ctx->sphere_vertex_offset);

        // Draw the cylinders.
        for (int i = 0; i < 10; ++i) {
            world = XMLoadFloat4x4(&render_ctx->cylinder_world[i]);
            wvp = world * view_proj;
            render_ctx->fx_wvp->SetMatrix(reinterpret_cast<float*>(&wvp));
            render_ctx->tech->GetPassByIndex(p)->Apply(0, render_ctx->d3d_immediate_context);
            render_ctx->d3d_immediate_context->DrawIndexed(render_ctx->cylinder_index_count, render_ctx->cylinder_index_offset, render_ctx->cylinder_vertex_offset);
        }

        // Draw the spheres.
        for (int i = 0; i < 10; ++i) {
            world = XMLoadFloat4x4(&render_ctx->sphere_world[i]);
            wvp = world * view_proj;
            render_ctx->fx_wvp->SetMatrix(reinterpret_cast<float*>(&wvp));
            render_ctx->tech->GetPassByIndex(p)->Apply(0, render_ctx->d3d_immediate_context);
            render_ctx->d3d_immediate_context->DrawIndexed(render_ctx->sphere_index_count, render_ctx->sphere_index_offset, render_ctx->sphere_vertex_offset);
        }
    }
    render_ctx->swapchain->Present(0, 0);
}
static void
handle_mouse_down (D3D11RenderContext * render_ctx, WPARAM btn_state, int x, int y) {
    render_ctx->last_mouse_pos.x = x;
    render_ctx->last_mouse_pos.y = y;

    ::SetCapture(render_ctx->wnd);
}
static void
handle_mouse_up (WPARAM btn_state, int x, int y) {
    ::ReleaseCapture();
}
static void
handle_mouse_move (D3D11RenderContext * render_ctx, WPARAM btn_state, int x, int y) {
    if ((btn_state & MK_LBUTTON) != 0) {
        // Make each pixel correspond to a quarter of a degree.
        float dx = XMConvertToRadians(0.25f * static_cast<float>(x - render_ctx->last_mouse_pos.x));
        float dy = XMConvertToRadians(0.25f * static_cast<float>(y - render_ctx->last_mouse_pos.y));

        // Update angles based on input to orbit camera around box.
        render_ctx->theta += dx;
        render_ctx->phi   += dy;

        // Restrict the angle mPhi.
        render_ctx->phi = min(max(render_ctx->phi, 0.1f), XM_PI - 0.1f);
    } else if ((btn_state & MK_RBUTTON) != 0) {
        // Make each pixel correspond to 0.01 unit in the scene.
        float dx = 0.01f * static_cast<float>(x - render_ctx->last_mouse_pos.x);
        float dy = 0.01f * static_cast<float>(y - render_ctx->last_mouse_pos.y);

        // Update the camera radius based on input.
        render_ctx->radius += dx - dy;

        // Restrict the radius.
        render_ctx->radius = min(max(render_ctx->radius, 3.0f), 200.0f);
    }

    render_ctx->last_mouse_pos.x = x;
    render_ctx->last_mouse_pos.y = y;
}
#define _BOX_VTX_CNT   24
#define _BOX_IDX_CNT   36

#define _GRID_VTX_CNT   2400
#define _GRID_IDX_CNT   13806

#define _SPHERE_VTX_CNT   401
#define _SPHERE_IDX_CNT   2280

#define _CYLINDER_VTX_CNT   485
#define _CYLINDER_IDX_CNT   2520

#define _TOTAL_VTX_CNT  (_BOX_VTX_CNT + _GRID_VTX_CNT + _SPHERE_VTX_CNT + _CYLINDER_VTX_CNT)
#define _TOTAL_IDX_CNT  (_BOX_IDX_CNT + _GRID_IDX_CNT + _SPHERE_IDX_CNT + _CYLINDER_IDX_CNT)
static void
create_geom_buffers (D3D11RenderContext * render_ctx) {
    DemoVertex *    vertices = (DemoVertex *)::malloc(sizeof(DemoVertex) * _TOTAL_VTX_CNT);
    int *           indices = (int *)::malloc(sizeof(int) * _TOTAL_IDX_CNT);
    BYTE *          scratch = (BYTE *)::malloc(sizeof(Vertex) * _TOTAL_VTX_CNT + sizeof(int) * _TOTAL_IDX_CNT);

    // box
    UINT bsz = sizeof(Vertex) * _BOX_VTX_CNT;
    UINT bsz_id = bsz + sizeof(int) * _BOX_IDX_CNT;
    // grid
    UINT gsz = bsz_id + sizeof(Vertex) * _GRID_VTX_CNT;
    UINT gsz_id = gsz + sizeof(int) * _GRID_IDX_CNT;
    // sphere
    UINT ssz = gsz_id + sizeof(Vertex) * _SPHERE_VTX_CNT;
    UINT ssz_id = ssz + sizeof(int) * _SPHERE_IDX_CNT;
    // cylinder
    UINT csz = ssz_id + sizeof(Vertex) * _CYLINDER_VTX_CNT;
    //UINT csz_id = csz + sizeof(int) * _CYLINDER_IDX_CNT; // not used

    Vertex *    box_vertices = reinterpret_cast<Vertex *>(scratch);
    int *       box_indices = reinterpret_cast<int *>(scratch + bsz);
    Vertex *    grid_vertices = reinterpret_cast<Vertex *>(scratch + bsz_id);
    int *       grid_indices = reinterpret_cast<int *>(scratch + gsz);
    Vertex *    sphere_vertices = reinterpret_cast<Vertex *>(scratch + gsz_id);
    int *       sphere_indices = reinterpret_cast<int *>(scratch + ssz);
    Vertex *    cylinder_vertices = reinterpret_cast<Vertex *>(scratch + ssz_id);
    int *       cylinder_indices = reinterpret_cast<int *>(scratch + csz);

    create_box(1.5f, 0.5f, 1.5f, box_vertices, box_indices);
    create_grid(20.0f, 30.0f, 60, 40, grid_vertices, grid_indices);
    create_sphere(0.5f, sphere_vertices, sphere_indices);
    create_cylinder(0.5f, 0.3f, 3.0f, cylinder_vertices, cylinder_indices);

    // We are concatenating all the geometry into one big vertex/index buffer.  So
    // define the regions in the buffer each submesh covers.

    // Cache the vertex offsets to each object in the concatenated vertex buffer.
    render_ctx->box_vertex_offset = 0;
    render_ctx->grid_vertex_offset = _BOX_VTX_CNT;
    render_ctx->sphere_vertex_offset = render_ctx->grid_vertex_offset + _GRID_VTX_CNT;
    render_ctx->cylinder_vertex_offset = render_ctx->sphere_vertex_offset + _SPHERE_VTX_CNT;

    // Cache the starting index for each object in the concatenated index buffer.
    render_ctx->box_index_offset = 0;
    render_ctx->grid_index_offset = _BOX_IDX_CNT;
    render_ctx->sphere_index_offset = render_ctx->grid_index_offset + _GRID_IDX_CNT;
    render_ctx->cylinder_index_offset = render_ctx->sphere_index_offset + _SPHERE_IDX_CNT;

    // index count
    render_ctx->box_index_count =_BOX_IDX_CNT;
    render_ctx->grid_index_count =_GRID_IDX_CNT;
    render_ctx->sphere_index_count =_SPHERE_IDX_CNT;
    render_ctx->cylinder_index_count =_CYLINDER_IDX_CNT;

    // Extract the vertex elements we are interested in and pack the
    // vertices of all the meshes into one vertex buffer.

    XMFLOAT4 black(0.0f, 0.0f, 0.0f, 1.0f);
    UINT k = 0;
    for (size_t i = 0; i < _BOX_VTX_CNT; ++i, ++k) {
        vertices[k].position = box_vertices[i].position;
        vertices[k].color = black;
    }

    for (size_t i = 0; i < _GRID_VTX_CNT; ++i, ++k) {
        vertices[k].position = grid_vertices[i].position;
        vertices[k].color = black;
    }

    for (size_t i = 0; i < _SPHERE_VTX_CNT; ++i, ++k) {
        vertices[k].position = sphere_vertices[i].position;
        vertices[k].color = black;
    }

    for (size_t i = 0; i < _CYLINDER_VTX_CNT; ++i, ++k) {
        vertices[k].position = cylinder_vertices[i].position;
        vertices[k].color = black;
    }

    // -- pack indices
    k = 0;
    for (size_t i = 0; i < _BOX_IDX_CNT; ++i, ++k) {
        indices[k] = box_indices[i];
    }

    for (size_t i = 0; i < _GRID_IDX_CNT; ++i, ++k) {
        indices[k] = grid_indices[i];
    }

    for (size_t i = 0; i < _SPHERE_IDX_CNT; ++i, ++k) {
        indices[k] = sphere_indices[i];
    }

    for (size_t i = 0; i < _CYLINDER_IDX_CNT; ++i, ++k) {
        indices[k] = cylinder_indices[i];
    }

    UINT vb_byte_size = _TOTAL_VTX_CNT * sizeof(DemoVertex);
    UINT ib_byte_size = _TOTAL_IDX_CNT * sizeof(int);

    // create vertex buffer and index buffer
    D3D11_BUFFER_DESC vb_desc;
    vb_desc.Usage = D3D11_USAGE_IMMUTABLE;
    vb_desc.ByteWidth = vb_byte_size;
    vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vb_desc.CPUAccessFlags = 0;
    vb_desc.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinit_data;
    vinit_data.pSysMem = &vertices[0];
    render_ctx->device->CreateBuffer(&vb_desc, &vinit_data, &render_ctx->vb);

    D3D11_BUFFER_DESC ib_desc;
    ib_desc.Usage = D3D11_USAGE_IMMUTABLE;
    ib_desc.ByteWidth =ib_byte_size;
    ib_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ib_desc.CPUAccessFlags = 0;
    ib_desc.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinit_data;
    iinit_data.pSysMem = &indices[0];
    render_ctx->device->CreateBuffer(&ib_desc, &iinit_data, &render_ctx->ib);

    // -- cleanup
    free(scratch);
    free(indices);
    free(vertices);
}
static void
create_fx (D3D11RenderContext * render_ctx) {
    FILE * f = nullptr;
    errno_t err = fopen_s(&f, "./fx/color.fxo", "rb");
    if (0 == f || err != 0) {
        MessageBox(0, _T("Could not open FX file"), 0, 0);
        return;
    }
    fseek(f, 0, SEEK_END);
    int size = (int)ftell(f);
    size_t byte_size = size * sizeof(char);
    fseek(f, 0, 0);
    char * compiled_shader = (char *)malloc(byte_size);
    fread(compiled_shader, 1, size, f);
    fclose(f);

    D3DX11CreateEffectFromMemory(
        compiled_shader, size,
        0, render_ctx->device, &render_ctx->fx
    );
    free(compiled_shader);

    render_ctx->tech    = render_ctx->fx->GetTechniqueByName("ColorTech");
    render_ctx->fx_wvp = render_ctx->fx->GetVariableByName("gWorldViewProj")->AsMatrix();
}
static void
create_vertex_layout (D3D11RenderContext * render_ctx) {
    // Create the vertex input layout.
    D3D11_INPUT_ELEMENT_DESC vert_desc [] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    // Create the input layout
    D3DX11_PASS_DESC pass_desc;
    render_ctx->tech->GetPassByIndex(0)->GetDesc(&pass_desc);
    render_ctx->device->CreateInputLayout(
        vert_desc, 2, pass_desc.pIAInputSignature,
        pass_desc.IAInputSignatureSize, &render_ctx->input_layout
    );
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
        handle_mouse_down(g_render_ctx, wparam, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
        return 0;
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
        handle_mouse_up(wparam, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
        return 0;
    case WM_MOUSEMOVE:
        handle_mouse_move(g_render_ctx, wparam, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
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

    g_render_ctx->theta = 1.5f * XM_PI;
    g_render_ctx->phi = 0.1f * XM_PI;
    g_render_ctx->radius = 15.0f;

    g_render_ctx->last_mouse_pos.x = 0;
    g_render_ctx->last_mouse_pos.y = 0;

    XMMATRIX I = XMMatrixIdentity();
    XMStoreFloat4x4(&g_render_ctx->grid_world, I);
    XMStoreFloat4x4(&g_render_ctx->view, I);
    XMStoreFloat4x4(&g_render_ctx->proj, I);

    XMMATRIX box_scale = XMMatrixScaling(2.0f, 1.0f, 2.0f);
    XMMATRIX box_offset = XMMatrixTranslation(0.0f, 0.5f, 0.0f);
    XMStoreFloat4x4(&g_render_ctx->box_world, XMMatrixMultiply(box_scale, box_offset));

    XMMATRIX center_sphere_scale = XMMatrixScaling(2.0f, 2.0f, 2.0f);
    XMMATRIX center_sphere_offset = XMMatrixTranslation(0.0f, 2.0f, 0.0f);
    XMStoreFloat4x4(&g_render_ctx->center_sphere, XMMatrixMultiply(center_sphere_scale, center_sphere_offset));

    for (int i = 0; i < 5; ++i) {
        XMStoreFloat4x4(&g_render_ctx->cylinder_world[i * 2 + 0], XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f));
        XMStoreFloat4x4(&g_render_ctx->cylinder_world[i * 2 + 1], XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f));

        XMStoreFloat4x4(&g_render_ctx->sphere_world[i * 2 + 0], XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f));
        XMStoreFloat4x4(&g_render_ctx->sphere_world[i * 2 + 1], XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f));
    }

    create_geom_buffers(g_render_ctx);
    create_fx(g_render_ctx);
    create_vertex_layout(g_render_ctx);

    D3D11_RASTERIZER_DESC wireframe_desc;
    ZeroMemory(&wireframe_desc, sizeof(D3D11_RASTERIZER_DESC));
    wireframe_desc.FillMode = D3D11_FILL_WIREFRAME;
    wireframe_desc.CullMode = D3D11_CULL_BACK;
    wireframe_desc.FrontCounterClockwise = false;
    wireframe_desc.DepthClipEnable = true;

    g_render_ctx->device->CreateRasterizerState(&wireframe_desc, &g_render_ctx->wireframe_rs);

#pragma endregion
#pragma region Main Loop
    MSG msg = {0};
    d3d11_resize(g_render_ctx); // initial resize
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
    g_render_ctx->ib->Release();
    g_render_ctx->vb->Release();
    g_render_ctx->fx->Release();
    g_render_ctx->input_layout->Release();
    g_render_ctx->wireframe_rs->Release();

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

