/** \file       Viewers/ModelViewer.cpp
 *  \brief      Contains the definition of the model viewer class.
 *  \author     Rhoot
 */

/*	Copyright (C) 2012 Rhoot <https://github.com/rhoot>

    This file is part of Gw2Browser.

    Gw2Browser is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdafx.h"
#include "ModelViewer.h"

namespace gw2b
{

uint32 VertexDefinition::sFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2);

ModelViewer::ModelViewer(wxWindow* pParent, const wxPoint& pPos, const wxSize& pSize)
    : Viewer(pParent, pPos, pSize)
{
    ::memset(&mPresentParams, 0, sizeof(mPresentParams));
    mPresentParams.PresentationInterval    = D3DPRESENT_INTERVAL_IMMEDIATE;
    mPresentParams.SwapEffect              = D3DSWAPEFFECT_DISCARD;
    mPresentParams.Windowed                = true;
    mPresentParams.BackBufferCount         = 1;
    mPresentParams.BackBufferWidth         = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
    mPresentParams.BackBufferHeight        = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
    mPresentParams.BackBufferFormat        = D3DFMT_A8R8G8B8;
    mPresentParams.EnableAutoDepthStencil  = true;
    mPresentParams.AutoDepthStencilFormat  = D3DFMT_D16;

    // Init Direct3D
    IDirect3DDevice9* device = NULL;
    mD3D = ::Direct3DCreate9(D3D_SDK_VERSION);
    mD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, this->GetHandle(), D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE, &mPresentParams, &device);
    mDevice = device;

    // Load shader
    ID3DXEffect* effect = NULL;
    ID3DXBuffer* errors = NULL;
    uint32 shaderFlags  = IfDebug(D3DXSHADER_DEBUG, 0);
    HRESULT result = ::D3DXCreateEffectFromFileW(mDevice, L"data/model_view.hlsl", NULL, NULL, shaderFlags, NULL, &effect, &errors);
    mEffect = effect;

    if (FAILED(result)) {
        wxMessageBox(wxString::Format(wxT("Error 0x%08x while loading shader: %s"), result, static_cast<const char*>(errors->GetBufferPointer())));
    }

    // Hook up events
    this->Connect(wxEVT_PAINT, wxPaintEventHandler(ModelViewer::OnPaintEvt));
}

ModelViewer::~ModelViewer()
{
    for (uint i = 0; i < mMeshCache.GetSize(); i++) {
        if (mMeshCache[i].mIndexBuffer)  { mMeshCache[i].mIndexBuffer->Release();  }
        if (mMeshCache[i].mVertexBuffer) { mMeshCache[i].mVertexBuffer->Release(); }
    }
}

void ModelViewer::Clear()
{
    for (uint i = 0; i < mMeshCache.GetSize(); i++) {
        if (mMeshCache[i].mIndexBuffer)  { mMeshCache[i].mIndexBuffer->Release();  }
        if (mMeshCache[i].mVertexBuffer) { mMeshCache[i].mVertexBuffer->Release(); }
    }
    mMeshCache.Clear();
    mModel = Model();
    Viewer::Clear();
}

void ModelViewer::SetReader(FileReader* pReader)
{
    Ensure::IsOfType<ModelReader>(pReader);
    Viewer::SetReader(pReader);

    // Load model
    ModelReader* reader = this->GetModelReader();
    mModel = reader->GetModel();

    // Create DX mesh cache
    mMeshCache.SetSize(mModel.GetNumMeshes());

    // Load meshes
    for (uint i = 0; i < mModel.GetNumMeshes(); i++) {
        const Mesh& mesh = mModel.GetMesh(i);
        MeshCache& cache = mMeshCache[i];

        // Create and populate the buffers
        uint vertexCount = mesh.mVertices.GetSize();
        uint vertexSize  = sizeof(VertexDefinition);
        uint indexCount  = mesh.mTriangles.GetSize() * 3;
        uint indexSize   = sizeof(uint16);

        if (!this->CreateBuffers(cache, vertexCount, vertexSize, indexCount, indexSize)) { continue; }
        if (!this->PopulateBuffers(mesh, cache)) { 
            ReleasePointer(cache.mIndexBuffer);
            ReleasePointer(cache.mVertexBuffer);
            continue;
        }
    }
}

bool ModelViewer::CreateBuffers(MeshCache& pCache, uint pVertexCount, uint pVertexSize, uint pIndexCount, uint pIndexSize)
{
    pCache.mIndexBuffer  = NULL;
    pCache.mVertexBuffer = NULL;

    // 0 indices or 0 vertices, either is an empty mesh
    if (!pVertexCount || !pIndexCount) {
        return false;
    }

    // Allocate vertex buffer and bail if it fails
    if (FAILED(mDevice->CreateVertexBuffer(pVertexCount * pVertexSize, D3DUSAGE_WRITEONLY, VertexDefinition::sFVF,
        D3DPOOL_DEFAULT, &pCache.mVertexBuffer, NULL))) 
    {
        return false;
    }

    // Allocate index buffer and bail if it fails
    if (FAILED(mDevice->CreateIndexBuffer(pIndexCount * pIndexSize, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
        D3DPOOL_DEFAULT, &pCache.mIndexBuffer, NULL))) 
    {
        pCache.mVertexBuffer->Release();
        pCache.mVertexBuffer = NULL;
        pCache.mIndexBuffer  = NULL;
        return false;
    }

    return true;
}

bool ModelViewer::PopulateBuffers(const Mesh& pMesh, MeshCache& pCache)
{
    uint vertexCount = pMesh.mVertices.GetSize();
    uint vertexSize  = sizeof(VertexDefinition);
    uint indexCount  = pMesh.mTriangles.GetSize() * 3;
    uint indexSize   = sizeof(uint16);

    // Lock vertex buffer
    VertexDefinition* vertices;
    if (FAILED(pCache.mVertexBuffer->Lock(0, vertexCount * vertexSize, reinterpret_cast<void**>(&vertices), 0))){
        return false;
    }

    // Populate vertex buffer
    for (uint j = 0; j < vertexCount; j++) {
        vertices[j].mPosition = pMesh.mVertices[j].mPosition;

        // Normal
        if (pMesh.mVertices[j].mHasNormal) {
            vertices[j].mNormal = pMesh.mVertices[j].mNormal;
        } else {
            vertices[j].mNormal.x = 0;
            vertices[j].mNormal.y = 0;
            vertices[j].mNormal.z = 1;
        }

        // Color
        if (pMesh.mVertices[j].mHasColor) {
            vertices[j].mDiffuse = pMesh.mVertices[j].mColor;
        } else {
            ::memset(&vertices[j].mDiffuse, 0xff, sizeof(uint32));
        }

        // UV1
        if (pMesh.mVertices[j].mHasUV > 0) {
            vertices[j].mUV[0] = pMesh.mVertices[j].mUV[0];
        } else {
            ::memset(&vertices[j].mUV[0], 0, sizeof(XMFLOAT2));
        }

        // UV2
        if (pMesh.mVertices[j].mHasUV > 1) {
            vertices[j].mUV[1] = pMesh.mVertices[j].mUV[1];
        } else {
            ::memset(&vertices[j].mUV[1], 0, sizeof(XMFLOAT2));
        }
    }
    wxASSERT(SUCCEEDED(pCache.mVertexBuffer->Unlock()));

    // Lock index buffer
    uint16* indices;
    if (FAILED(pCache.mIndexBuffer->Lock(0, indexCount * indexSize, reinterpret_cast<void**>(&indices), 0))) {
        return false;
    }
    // Copy index buffer
    ::memcpy(indices, pMesh.mTriangles.GetPointer(), indexCount * indexSize);
    wxASSERT(SUCCEEDED(pCache.mIndexBuffer->Unlock()));

    return true;
}

void ModelViewer::BeginFrame(uint32 pClearColor)
{
    wxSize clientSize = this->GetClientSize();

    D3DVIEWPORT9 viewport;
    ::memset(&viewport, 0, sizeof(viewport));
    viewport.Width  = clientSize.x;
    viewport.Height = clientSize.y;
    viewport.MaxZ   = 1;
    viewport.MinZ   = 0;

    mDevice->SetViewport(&viewport);
    mDevice->BeginScene();
    mDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, pClearColor, 1, 0);
}

void ModelViewer::EndFrame()
{
    D3DVIEWPORT9 viewport;
    mDevice->GetViewport(&viewport);

    RECT sourceRect;
    ::memset(&sourceRect, 0, sizeof(sourceRect));
    sourceRect.right = viewport.Width;
    sourceRect.bottom = viewport.Height;

    mDevice->EndScene();
    mDevice->Present(&sourceRect, NULL, NULL, NULL);
}

void ModelViewer::Render()
{
    mDevice->SetRenderState(D3DRS_LIGHTING, 0);
    mDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    this->UpdateMatrices();

    this->BeginFrame(0x353535);
    for (uint i = 0; i < mModel.GetNumMeshes(); i++) {
        this->DrawMesh(i);
    }
    this->EndFrame();
}

void ModelViewer::OnPaintEvt(wxPaintEvent& pEvent)
{
    this->Render();
}

void ModelViewer::DrawMesh(uint pMeshIndex)
{
    const MeshCache& mesh = mMeshCache[pMeshIndex];

    // No mesh to draw?
    if (mesh.mIndexBuffer == NULL || mesh.mVertexBuffer == NULL) {
        return;
    }

    // Count vertices / primitives
    uint vertexCount    = mModel.GetMesh(pMeshIndex).mVertices.GetSize();
    uint primitiveCount = mModel.GetMesh(pMeshIndex).mTriangles.GetSize();
    
    // Set buffers
    if (FAILED(mDevice->SetFVF(VertexDefinition::sFVF))) { return; }
    if (FAILED(mDevice->SetStreamSource(0, mesh.mVertexBuffer, 0, sizeof(VertexDefinition)))) { return; }
    if (FAILED(mDevice->SetIndices(mesh.mIndexBuffer))) { return; }

    // Begin drawing
    uint numPasses;
    mEffect->SetTechnique("RenderScene");
    mEffect->Begin(&numPasses, 0);

    // Draw each shader pass
    for (uint i = 0; i < numPasses; i++) {
        mEffect->BeginPass(i);
        mDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, vertexCount, 0, primitiveCount);
        mEffect->EndPass();
    }

    // End
    mEffect->End();
}

void ModelViewer::UpdateMatrices()
{
    // All models are located at 0,0,0 with no rotation, so no world matrix is needed

    XMMATRIX viewMatrix;
    XMMATRIX projectionMatrix;

    // View matrix
    D3DXVECTOR3 eyeLocation;
    eyeLocation.x = 0;
    eyeLocation.y = -100;
    eyeLocation.z = 0;
    D3DXVECTOR3 eyeLookAt;
    ::memset(&eyeLookAt, 0, sizeof(eyeLookAt));
    D3DXVECTOR3 eyeUp;
    eyeUp.x = 0;
    eyeUp.y = 0;
    eyeUp.z = -1;
    ::D3DXMatrixLookAtLH(reinterpret_cast<D3DXMATRIX*>(&viewMatrix), &eyeLocation, &eyeLookAt, &eyeUp);

    // Projection matrix
    wxSize clientSize = this->GetClientSize();
    float aspectRatio = (static_cast<float>(clientSize.y) / static_cast<float>(clientSize.x));
    ::D3DXMatrixPerspectiveLH(reinterpret_cast<D3DXMATRIX*>(&projectionMatrix), XM_PIDIV4, aspectRatio, 0.1f, 2000);

    // WorldViewProjection matrix
    XMMATRIX worldViewProjMatrix = ::XMMatrixMultiply(viewMatrix, projectionMatrix);
    mEffect->SetMatrix("g_WorldViewProjMatrix", reinterpret_cast<D3DXMATRIX*>(&worldViewProjMatrix));
}

}; // namespace gw2b
