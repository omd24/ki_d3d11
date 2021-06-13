#pragma once

#include <DirectXMath.h>
using namespace DirectX;

struct Vertex {
    XMFLOAT3 position;
    XMFLOAT3 normal;
    XMFLOAT3 tangent_u;
    XMFLOAT2 texc;
};

static void
create_box (float width, float height, float depth, Vertex out_vtx [], int out_idx []) {

    // Creating Vertices

    float half_width = 0.5f * width;
    float half_height = 0.5f * height;
    float half_depth = 0.5f * depth;

    // Fill in the front face vertex data.
    out_vtx[0] = {.position = {-half_width, -half_height, -half_depth}, .normal = { 0.0f, 0.0f, -1.0f}, .tangent_u = {1.0f, 0.0f, 0.0f}, .texc = {0.0f, 1.0f}};
    out_vtx[1] = {.position = {-half_width, +half_height, -half_depth}, .normal = { 0.0f, 0.0f, -1.0f}, .tangent_u = {1.0f, 0.0f, 0.0f}, .texc = {0.0f, 0.0f}};
    out_vtx[2] = {.position = {+half_width, +half_height, -half_depth}, .normal = { 0.0f, 0.0f, -1.0f}, .tangent_u = {1.0f, 0.0f, 0.0f}, .texc = {1.0f, 0.0f}};
    out_vtx[3] = {.position = {+half_width, -half_height, -half_depth}, .normal = { 0.0f, 0.0f, -1.0f}, .tangent_u = {1.0f, 0.0f, 0.0f}, .texc = {1.0f, 1.0f}};

    // Fill in the back face vertex data.
    out_vtx[4] = {.position = {-half_width, -half_height, +half_depth}, .normal = { 0.0f, 0.0f, 1.0f}, .tangent_u = {-1.0f, 0.0f, 0.0f}, .texc = {1.0f, 1.0f}};
    out_vtx[5] = {.position = {+half_width, -half_height, +half_depth}, .normal = { 0.0f, 0.0f, 1.0f}, .tangent_u = {-1.0f, 0.0f, 0.0f}, .texc = {0.0f, 1.0f}};
    out_vtx[6] = {.position = {+half_width, +half_height, +half_depth}, .normal = { 0.0f, 0.0f, 1.0f}, .tangent_u = {-1.0f, 0.0f, 0.0f}, .texc = {0.0f, 0.0f}};
    out_vtx[7] = {.position = {-half_width, +half_height, +half_depth}, .normal = { 0.0f, 0.0f, 1.0f}, .tangent_u = {-1.0f, 0.0f, 0.0f}, .texc = {1.0f, 0.0f}};

    // Fill in the top face vertex data.
    out_vtx[8] = {.position = {-half_width, +half_height, -half_depth}, .normal = { 0.0f, 1.0f, 0.0f}, .tangent_u = {1.0f, 0.0f, 0.0f}, .texc = {0.0f, 1.0f}};
    out_vtx[9] = {.position = {-half_width, +half_height, +half_depth}, .normal = { 0.0f, 1.0f, 0.0f}, .tangent_u = {1.0f, 0.0f, 0.0f}, .texc = {0.0f, 0.0f}};
    out_vtx[10] = {.position = {+half_width, +half_height, +half_depth}, .normal = { 0.0f, 1.0f, 0.0f}, .tangent_u = {1.0f, 0.0f, 0.0f}, .texc = {1.0f, 0.0f}};
    out_vtx[11] = {.position = {+half_width, +half_height, -half_depth}, .normal = { 0.0f, 1.0f, 0.0f}, .tangent_u = {1.0f, 0.0f, 0.0f}, .texc = {1.0f, 1.0f}};

    // Fill in the bottom face vertex data.
    out_vtx[12] = {.position = {-half_width, -half_height, -half_depth}, .normal = { 0.0f, -1.0f, 0.0f}, .tangent_u = {-1.0f, 0.0f, 0.0f}, .texc = {1.0f, 1.0f}};
    out_vtx[13] = {.position = {+half_width, -half_height, -half_depth}, .normal = { 0.0f, -1.0f, 0.0f}, .tangent_u = {-1.0f, 0.0f, 0.0f}, .texc = {0.0f, 1.0f}};
    out_vtx[14] = {.position = {+half_width, -half_height, +half_depth}, .normal = { 0.0f, -1.0f, 0.0f}, .tangent_u = {-1.0f, 0.0f, 0.0f}, .texc = {0.0f, 0.0f}};
    out_vtx[15] = {.position = {-half_width, -half_height, +half_depth}, .normal = { 0.0f, -1.0f, 0.0f}, .tangent_u = {-1.0f, 0.0f, 0.0f}, .texc = {1.0f, 0.0f}};

    // Fill in the left face vertex data.
    out_vtx[16] = {.position = {-half_width, -half_height, +half_depth}, .normal = { -1.0f, 0.0f, 0.0f}, .tangent_u = {0.0f, 0.0f, -1.0f}, .texc = {0.0f, 1.0f}};
    out_vtx[17] = {.position = {-half_width, +half_height, +half_depth}, .normal = { -1.0f, 0.0f, 0.0f}, .tangent_u = {0.0f, 0.0f, -1.0f}, .texc = {0.0f, 0.0f}};
    out_vtx[18] = {.position = {-half_width, +half_height, -half_depth}, .normal = { -1.0f, 0.0f, 0.0f}, .tangent_u = {0.0f, 0.0f, -1.0f}, .texc = {1.0f, 0.0f}};
    out_vtx[19] = {.position = {-half_width, -half_height, -half_depth}, .normal = { -1.0f, 0.0f, 0.0f}, .tangent_u = {0.0f, 0.0f, -1.0f}, .texc = {1.0f, 1.0f}};

    // Fill in the right face vertex data.
    out_vtx[20] = {.position = {+half_width, -half_height, -half_depth}, .normal = { 1.0f, 0.0f, 0.0f}, .tangent_u = {0.0f, 0.0f, 1.0f}, .texc = {0.0f, 1.0f}};
    out_vtx[21] = {.position = {+half_width, +half_height, -half_depth}, .normal = { 1.0f, 0.0f, 0.0f}, .tangent_u = {0.0f, 0.0f, 1.0f}, .texc = {0.0f, 0.0f}};
    out_vtx[22] = {.position = {+half_width, +half_height, +half_depth}, .normal = { 1.0f, 0.0f, 0.0f}, .tangent_u = {0.0f, 0.0f, 1.0f}, .texc = {1.0f, 0.0f}};
    out_vtx[23] = {.position = {+half_width, -half_height, +half_depth}, .normal = { 1.0f, 0.0f, 0.0f}, .tangent_u = {0.0f, 0.0f, 1.0f}, .texc = {1.0f, 1.0f}};

    // -- Creating Indices 

    // Fill in the front face index data
    out_idx[0] = 0; out_idx[1] = 1; out_idx[2] = 2;
    out_idx[3] = 0; out_idx[4] = 2; out_idx[5] = 3;

    // Fill in the back face index data
    out_idx[6] = 4; out_idx[7] = 5; out_idx[8] = 6;
    out_idx[9] = 4; out_idx[10] = 6; out_idx[11] = 7;

    // Fill in the top face index data
    out_idx[12] = 8; out_idx[13] = 9; out_idx[14] = 10;
    out_idx[15] = 8; out_idx[16] = 10; out_idx[17] = 11;

    // Fill in the bottom face index data
    out_idx[18] = 12; out_idx[19] = 13; out_idx[20] = 14;
    out_idx[21] = 12; out_idx[22] = 14; out_idx[23] = 15;

    // Fill in the left face index data
    out_idx[24] = 16; out_idx[25] = 17; out_idx[26] = 18;
    out_idx[27] = 16; out_idx[28] = 18; out_idx[29] = 19;

    // Fill in the right face index data
    out_idx[30] = 20; out_idx[31] = 21; out_idx[32] = 22;
    out_idx[33] = 20; out_idx[34] = 22; out_idx[35] = 23;
}
static void
create_sphere (float radius, Vertex out_vtx [], int out_idx []) {

    // TODO(omid): add some validation for array sizes
    /* out_vtx [401], out_idx [2280] */

    // -- Compute the vertices stating at the top pole and moving down the stacks.
    int n_stack = 20;
    int n_slice = 20;
    int n_vtx = n_stack * n_slice;
    float phi_step = XM_PI / n_stack;
    float theta_step = 2.0f * XM_PI / n_slice;

    // Poles: note that there will be texture coordinate distortion as there is
    // not a unique point on the texture map to assign to the pole when mapping
    // a rectangular texture onto a sphere.
    Vertex top = {.position = {0.0f, +radius, 0.0f}, .normal = {0.0f, +1.0f, 0.0f}, .tangent_u = {1.0f, 0.0f, 0.0f}, .texc = {0.0f, 0.0f}};
    Vertex bottom = {.position = {0.0f, -radius, 0.0f}, .normal = {0.0f, -1.0f, 0.0f}, .tangent_u = {1.0f, 0.0f, 0.0f}, .texc = {0.0f, 1.0f}};

    out_vtx[0] = top;
    out_vtx[n_vtx + 1] = bottom;

    // -- Compute vertices for each stack ring (do not count the poles as rings).
    int _curr_idx = 1;
    for (int i = 1; i <= n_stack - 1; ++i) {
        float phi = i * phi_step;

        // Vertices of ring.
        for (int j = 0; j <= n_slice; ++j) {
            float theta = j * theta_step;

            Vertex v = {};

            // spherical to cartesian
            v.position.x = radius * sinf(phi) * cosf(theta);
            v.position.y = radius * cosf(phi);
            v.position.z = radius * sinf(phi) * sinf(theta);

            // Partial derivative of P with respect to theta
            v.tangent_u.x = -radius * sinf(phi) * sinf(theta);
            v.tangent_u.y = 0.0f;
            v.tangent_u.z = +radius * sinf(phi) * cosf(theta);

            XMVECTOR T = XMLoadFloat3(&v.tangent_u);
            XMStoreFloat3(&v.tangent_u, XMVector3Normalize(T));

            XMVECTOR p = XMLoadFloat3(&v.position);
            XMStoreFloat3(&v.normal, XMVector3Normalize(p));

            v.texc.x = theta / XM_2PI;
            v.texc.y = phi / XM_PI;

            out_vtx[_curr_idx++] = v;
        }
    }

    // -- Compute indices for top stack.  The top stack was written first to the vertex buffer and connects the top pole to the first ring.

    int _idx_cnt = 0;
    for (int i = 1; i <= n_slice; ++i) {
        out_idx[_idx_cnt++] = 0;
        out_idx[_idx_cnt++] = i + 1;
        out_idx[_idx_cnt++] = i;
    }

    // -- Compute indices for inner stacks (not connected to poles).

    // -- Offset the indices to the index of the first vertex in the first ring.
    // TODO(omid): fix this shenanigan 
    // This is just skipping the top pole vertex.
    int base_index = 1;
    int ring_vtx_cnt = (int)n_slice + 1;
    for (int i = 0; i < n_stack - 2; ++i) {
        for (int j = 0; j < n_slice; ++j) {
            out_idx[_idx_cnt++] = base_index + i * ring_vtx_cnt + j;
            out_idx[_idx_cnt++] = base_index + i * ring_vtx_cnt + j + 1;
            out_idx[_idx_cnt++] = base_index + (i + 1) * ring_vtx_cnt + j;

            out_idx[_idx_cnt++] = base_index + (i + 1) * ring_vtx_cnt + j;
            out_idx[_idx_cnt++] = base_index + i * ring_vtx_cnt + j + 1;
            out_idx[_idx_cnt++] = base_index + (i + 1) * ring_vtx_cnt + j + 1;
        }
    }

    // -- Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer and connects the bottom pole to the bottom ring.

    // South pole vertex was added last.
    int south_pole_index = (int)n_vtx - 1;

    // offset the indices to the index of the first vertex in the last ring.
    base_index = south_pole_index - ring_vtx_cnt;

    for (int i = 0; i < n_slice; ++i) {
        out_idx[_idx_cnt++] = south_pole_index;
        out_idx[_idx_cnt++] = base_index + i;
        out_idx[_idx_cnt++] = base_index + i + 1;
    }
}
static void
create_cylinder (float bottom_radius, float top_radius, float height, Vertex out_vtx [], int out_idx []) {

    // TODO(omid): add some validation for array sizes
    /* out_vtx [485], out_idx [2520] */

    // -- Build Stacks.
    int n_stack = 20;
    int n_slice = 20;
    float stack_height = height / n_stack;

    // Amount to increment radius as we move up each stack level from bottom to top.
    float radius_step = (top_radius - bottom_radius) / n_stack;
    int ring_cnt = n_stack + 1;

    int _vtx_cnt = 0;
    int _idx_cnt = 0;

    // Compute vertices for each stack ring starting at the bottom and moving up.
    for (int i = 0; i < ring_cnt; ++i) {
        float y = -0.5f * height + i * stack_height;
        float r = bottom_radius + i * radius_step;

        // vertices of ring
        float dtheta = 2.0f * XM_PI / n_slice;
        for (int j = 0; j <= n_slice; ++j) {
            Vertex vertex = {};

            float c = cosf(j * dtheta);
            float s = sinf(j * dtheta);

            vertex.position = XMFLOAT3(r * c, y, r * s);

            vertex.texc.x = (float)j / n_slice;
            vertex.texc.y = 1.0f - (float)i / n_stack;

            // This is unit length.
            vertex.tangent_u = XMFLOAT3(-s, 0.0f, c);

            float dr = bottom_radius - top_radius;
            XMFLOAT3 bitangent(dr * c, -height, dr * s);

            XMVECTOR T = XMLoadFloat3(&vertex.tangent_u);
            XMVECTOR B = XMLoadFloat3(&bitangent);
            XMVECTOR N = XMVector3Normalize(XMVector3Cross(T, B));
            XMStoreFloat3(&vertex.normal, N);

            out_vtx[_vtx_cnt++] = vertex;
        }
    }

    // Add one because we duplicate the first and last vertex per ring
    // since the texture coordinates are different.
    int ring_vertex_count = (int)n_slice + 1;

    // Compute indices for each stack.
    for (int i = 0; i < n_stack; ++i) {
        for (int j = 0; j < n_slice; ++j) {
            out_idx[_idx_cnt++] = i * ring_vertex_count + j;
            out_idx[_idx_cnt++] = (i + 1) * ring_vertex_count + j;
            out_idx[_idx_cnt++] = (i + 1) * ring_vertex_count + j + 1;

            out_idx[_idx_cnt++] = i * ring_vertex_count + j;
            out_idx[_idx_cnt++] = (i + 1) * ring_vertex_count + j + 1;
            out_idx[_idx_cnt++] = i * ring_vertex_count + j + 1;
        }
    }

#pragma region build cylinder top
    int base_index_top = (int)_vtx_cnt;
    float y1 = 0.5f * height;
    float dtheta = 2.0f * XM_PI / n_slice;

    // Duplicate cap ring vertices because the texture coordinates and normals differ.
    for (int i = 0; i <= n_slice; ++i) {
        float x = top_radius * cosf(i * dtheta);
        float z = top_radius * sinf(i * dtheta);

        // Scale down by the height to try and make top cap texture coord area
        // proportional to base.
        float u = x / height + 0.5f;
        float v = z / height + 0.5f;

        out_vtx[_vtx_cnt++] = {.position = {x, y1, z}, .normal = {0.0f, 1.0f, 0.0f}, .tangent_u = {1.0f, 0.0f, 0.0f}, .texc = {u, v}};
    }

    // Cap center vertex.
    out_vtx[_vtx_cnt++] = {.position = {0.0f, y1, 0.0f}, .normal = {0.0f, 1.0f, 0.0f}, .tangent_u = {1.0f, 0.0f, 0.0f}, .texc = {0.5f, 0.5f}};

    // Index of center vertex.
    int center_index_top = (int)_vtx_cnt - 1;

    for (int i = 0; i < n_slice; ++i) {
        out_idx[_idx_cnt++] = center_index_top;
        out_idx[_idx_cnt++] = base_index_top + i + 1;
        out_idx[_idx_cnt++] = base_index_top + i;
    }
#pragma endregion build cylinder top

#pragma region build cylinder bottom
    int base_index_bottom = (int)_vtx_cnt;
    float y2 = -0.5f * height;

    // vertices of ring
    //float dTheta = 2.0f * XM_PI / n_slice; // not used
    for (int i = 0; i <= n_slice; ++i) {
        float x = bottom_radius * cosf(i * dtheta);
        float z = bottom_radius * sinf(i * dtheta);

        // Scale down by the height to try and make top cap texture coord area
        // proportional to base.
        float u = x / height + 0.5f;
        float v = z / height + 0.5f;
        out_vtx[_vtx_cnt++] = {.position = {x, y2, z}, .normal = {0.0f, -1.0f, 0.0f}, .tangent_u = {1.0f, 0.0f, 0.0f}, .texc = {u, v}};
    }

    // Cap center vertex.
    out_vtx[_vtx_cnt++] = {.position = {0.0f, y2, 0.0f}, .normal = {0.0f, -1.0f, 0.0f}, .tangent_u = {1.0f, 0.0f, 0.0f}, .texc = {0.5f, 0.5f}};

    // Cache the index of center vertex.
    int center_index_bottom = (int)_vtx_cnt - 1;

    for (int i = 0; i < n_slice; ++i) {
        out_idx[_idx_cnt++] = center_index_bottom;
        out_idx[_idx_cnt++] = base_index_bottom + i;
        out_idx[_idx_cnt++] = base_index_bottom + i + 1;
    }
#pragma endregion build cylinder bottom

}
static void
create_grid16 (float width, float depth, int m, int n, Vertex out_vtx [], int out_idx []) {

    // -- Create the vertices.

    float half_width = 0.5f * width;
    float half_depth = 0.5f * depth;

    float dx = width / (n - 1);
    float dz = depth / (m - 1);

    float du = 1.0f / (n - 1);
    float dv = 1.0f / (m - 1);

    for (int i = 0; i < m; ++i) {
        float z = half_depth - i * dz;
        for (int j = 0; j < n; ++j) {
            float x = -half_width + j * dx;

            out_vtx[i * n + j].position = XMFLOAT3(x, 0.0f, z);
            out_vtx[i * n + j].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
            out_vtx[i * n + j].tangent_u = XMFLOAT3(1.0f, 0.0f, 0.0f);

            // Stretch texture over grid.
            out_vtx[i * n + j].texc.x = j * du;
            out_vtx[i * n + j].texc.y = i * dv;
        }
    }

    // -- Create the indices.

    // Iterate over each quad and compute indices.
    int k = 0;
    int nn = (int)n; // cast to avoid compiler warnings
    for (int i = 0; i < m - 1; ++i) {
        for (int j = 0; j < n - 1; ++j) {
            out_idx[k] = i * nn + j;
            out_idx[k + 1] = i * nn + j + 1;
            out_idx[k + 2] = (i + 1) * nn + j;

            out_idx[k + 3] = (i + 1) * nn + j;
            out_idx[k + 4] = i * nn + j + 1;
            out_idx[k + 5] = (i + 1) * nn + j + 1;

            k += 6; // next quad
        }
    }
}
static void
create_grid32 (float width, float depth, int m, int n, Vertex out_vtx [], uint32_t out_idx []) {

    // -- Create the vertices.

    float half_width = 0.5f * width;
    float half_depth = 0.5f * depth;

    float dx = width / (n - 1);
    float dz = depth / (m - 1);

    float du = 1.0f / (n - 1);
    float dv = 1.0f / (m - 1);

    for (int i = 0; i < m; ++i) {
        float z = half_depth - i * dz;
        for (int j = 0; j < n; ++j) {
            float x = -half_width + j * dx;

            out_vtx[i * n + j].position = XMFLOAT3(x, 0.0f, z);
            out_vtx[i * n + j].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
            out_vtx[i * n + j].tangent_u = XMFLOAT3(1.0f, 0.0f, 0.0f);

            // Stretch texture over grid.
            out_vtx[i * n + j].texc.x = j * du;
            out_vtx[i * n + j].texc.y = i * dv;
        }
    }

    // -- Create the indices.

    // Iterate over each quad and compute indices.
    int k = 0;
    int nn = n;
    for (int i = 0; i < m - 1; ++i) {
        for (int j = 0; j < n - 1; ++j) {
            out_idx[k] = i * nn + j;
            out_idx[k + 1] = i * nn + j + 1;
            out_idx[k + 2] = (i + 1) * nn + j;

            out_idx[k + 3] = (i + 1) * nn + j;
            out_idx[k + 4] = i * nn + j + 1;
            out_idx[k + 5] = (i + 1) * nn + j + 1;

            k += 6; // next quad
        }
    }
}
