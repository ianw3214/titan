#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <wrl.h>
using namespace Microsoft::WRL;
#include <d3d12.h>
#include <dxgi1_5.h>

#include "Events.h"
#include "highResolutionClock.h"

#include <string>
#include <memory>

class Game;

class Window
{
public:
    static const UINT BufferCount  = 3;

    HWND GetWindowHandle() const;
    void Destroy();

    const std::wstring& GetWindowName() const;
    int GetClientWidth() const;
    int GetClientHeight() const;
    bool IsVsync() const;
    bool IsFullscreen() const;

    void SetVSync(bool vSync);
    void ToggleVSync();
    void SetFullscreen(bool fullscreen);
    void ToggleFullscreen();

    void Show();
    void Hide();
    UINT GetCurrentBackBufferIndex() const;
    UINT Present();

    D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRenderTargetView() const;
    Microsoft::WRL::ComPtr<ID3D12Resource> GetCurrentBackBuffer() const;

protected:
    friend LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    friend class Application;
    friend class Game;

    Window() = delete;
    Window(HWND hWnd, const std::wstring& windowName, int clientWidth, int clientHeight, bool vSync);
    virtual ~Window();

    void RegisterCallbacks(std::shared_ptr<Game> pGame);

    virtual void OnUpdate(UpdateEventArgs& e);
    virtual void OnRender(RenderEventArgs& e);

    virtual void OnKeyPressed(KeyEventArgs& e);
    virtual void OnKeyReleased(KeyEventArgs& e);
    virtual void OnMouseMoved(MouseMotionEventArgs& e);
    virtual void OnMouseButtonPressed(MouseButtonEventArgs& e);
    virtual void OnMouseButtonReleased(MouseButtonEventArgs& e);
    virtual void OnMouseWheel(MouseWheelEventArgs& e);
    virtual void OnResize(ResizeEventArgs& e);

    Microsoft::WRL::ComPtr<IDXGISwapChain4> CreateSwapChain();
    void UpdateRenderTargetViews();

private:
    Window(const Window& copy) = delete;
    Window& operator=(const Window& other) = delete;

    HWND m_hWnd;
    std::wstring m_WindowName;

    int m_ClientWidth;
    int m_ClientHeight;
    bool m_VSync;
    bool m_Fullscreen;

    HighResolutionClock m_UpdateClock;
    HighResolutionClock m_RenderClock;
    uint64_t m_FrameCounter;

    std::weak_ptr<Game> m_pGame;

    Microsoft::WRL::ComPtr<IDXGISwapChain4> m_dxgiSwapChain;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_d3d12RTVDescriptorHeap;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_d3d12BackBuffers[BufferCount];

    UINT m_RTVDescriptorSize;
    UINT m_CurrentBackBufferIndex;

    RECT m_WindowRect;
    bool m_IsTearingSupported;
};