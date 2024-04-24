#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <shellapi.h>

#include <wrl.h>
using namespace Microsoft::WRL;

#include <d3d12.h>
#include <dxgi1_6.h>
// link against the d3dcompiler.lib library and copy the D3dcompiler_47.dll to the same folder as the binary executable when distributing your project
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <directx/d3dx12.h>

#include "application.h"
#include "window.h"
#include "game.h"
#include "commandQueue.h"
#include "helpers.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class TestGame : public Game {
    uint64_t m_FenceValues[Window::BufferCount] = {};
public:
    TestGame(const std::wstring& name, int width, int height, bool vSync)
        : Game(name, width, height, vSync) {}

    virtual bool LoadContent() override { return true; }
    virtual void UnloadContent() override {}

    virtual void OnUpdate(UpdateEventArgs& e) override {
        auto commandQueue = Application::Get().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
        auto commandList = commandQueue->GetCommandList();
        auto backBuffer = m_pWindow->GetCurrentBackBuffer();
        UINT currentBackBufferIndex = m_pWindow->GetCurrentBackBufferIndex();

        // Clear the render target
        {
            // Both the before and after states of the transition barrier are specified
            CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                backBuffer.Get(),
                D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
            commandList->ResourceBarrier(1, &barrier);

            FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };
            // CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(gRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), gCurrentBackBufferIndex, gRTVDescriptorSize);
            auto rtv = m_pWindow->GetCurrentRenderTargetView();
            commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
        }

        // Present
        {
            CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                backBuffer.Get(),
                D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
            commandList->ResourceBarrier(1, &barrier);

            m_FenceValues[currentBackBufferIndex] = commandQueue->ExecuteCommandList(commandList);

            currentBackBufferIndex = m_pWindow->Present();

            commandQueue->WaitForFenceValue(m_FenceValues[currentBackBufferIndex]);
        }
    }
};

void ParseCommandLineArguments()
{
    int argc;
    wchar_t** argv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);
    for (size_t i = 0; i < argc; ++i)
    {
        if (::wcscmp(argv[i], L"-w") == 0 || ::wcscmp(argv[i], L"--width") == 0)
        {
            // gClientWidth = ::wcstol(argv[++i], nullptr, 10);
        }
        if (::wcscmp(argv[i], L"=h") == 0 || ::wcscmp(argv[i], L"--height") == 0)
        {
            // gClientHeight = ::wcstol(argv[++i], nullptr, 10);
        }
        if (::wcscmp(argv[i], L"-warp") == 0 || ::wcscmp(argv[i], L"--warp") == 0)
        {
            // gUseWarp = true;
        }

        ::LocalFree(argv);
    }
}

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
    Application::Create(hInstance);

    std::shared_ptr<TestGame> game = std::make_shared<TestGame>(L"Learning DirectX12", 1280, 720, false);
    Application::Get().Run(game);

    ParseCommandLineArguments();

    Application::Destroy();

    return 0;
}