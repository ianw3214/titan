#include "application.h"
#include "resource.h"

#include <wrl.h>
using namespace Microsoft::WRL;

#include <map>
#include <cassert>

#include "commandQueue.h"
#include "game.h"
#include "helpers.h"
#include "window.h"

constexpr wchar_t WINDOW_CLASS_NAME[] = L"DX12RenderWindowClass";

using WindowPtr = std::shared_ptr<Window>;
using WindowMap = std::map<HWND, WindowPtr>;
using WindowNameMap = std::map<std::wstring, WindowPtr>;

static Application* gs_pSingleton = nullptr;
static WindowMap gs_Windows;
static WindowNameMap gs_WindowByName;

static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

struct MakeWindow : public Window
{
    MakeWindow(HWND hWnd, const std::wstring& windowName, int clientWidth, int clientHeight, bool vSync)
        : Window(hWnd, windowName, clientWidth, clientHeight, vSync) {}
};

Application::Application(HINSTANCE hInst)
    : m_hInstance(hInst)
    , m_TearingSupported(false)
{
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

#ifdef _DEBUG
    ComPtr<ID3D12Debug> debugInterface;
    ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
    debugInterface->EnableDebugLayer();
#endif

    WNDCLASSEXW windowClass = { 0 };
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = &WndProc;
    // windowClass.cbClsExtra = 0;
    // windowClass.cbWndExtra = 0;
    windowClass.hInstance = hInst;
    windowClass.hIcon = ::LoadIcon(hInst, MAKEINTRESOURCE(APP_ICON));
    windowClass.hCursor = ::LoadCursorA(NULL, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    windowClass.lpszMenuName = nullptr;
    windowClass.lpszClassName = WINDOW_CLASS_NAME;
    windowClass.hIconSm = ::LoadIcon(hInst, MAKEINTRESOURCE(APP_ICON));

    if (!::RegisterClassExW(&windowClass))
    {
        MessageBoxA(NULL, "Unable to register the window class", "Error", MB_OK | MB_ICONERROR);
    }

    m_dxgiAdapter = GetAdapter(false);
    if (m_dxgiAdapter)
    {
        m_d3d12Device = CreateDevice(m_dxgiAdapter);
    }
    if (m_d3d12Device)
    {
        m_DirectCommandQueue = std::make_shared<CommandQueue>(m_d3d12Device, D3D12_COMMAND_LIST_TYPE_DIRECT);
        m_ComputeCommandQueue = std::make_shared<CommandQueue>(m_d3d12Device, D3D12_COMMAND_LIST_TYPE_COMPUTE);
        m_CopyCommandQueue = std::make_shared<CommandQueue>(m_d3d12Device, D3D12_COMMAND_LIST_TYPE_COPY);

        m_TearingSupported = CheckTearingSupport();
    }
}

void Application::Create(HINSTANCE hInst)
{
    if (!gs_pSingleton)
    {
        gs_pSingleton = new Application(hInst);
    }
}

Application& Application::Get()
{
    assert(gs_pSingleton);
    return *gs_pSingleton;
}

void Application::Destroy()
{
    if (gs_pSingleton)
    {
        assert(gs_Windows.empty() && gs_WindowByName.empty() &&
            "All windows should be destroyed before destroying the application instance");
        delete gs_pSingleton;
        gs_pSingleton = nullptr;
    }
}

Application::~Application()
{
    Flush();
}

Microsoft::WRL::ComPtr<IDXGIAdapter4> Application::GetAdapter(bool bUseWarp)
{
    ComPtr<IDXGIFactory4> dxgiFactory;
    UINT createFactoryFlags = 0;
#ifdef _DEBUG
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

    ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));

    ComPtr<IDXGIAdapter1> dxgiAdapter1;
    ComPtr<IDXGIAdapter4> dxgiAdapter4;

    if (bUseWarp)
    {
        ThrowIfFailed(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1)));
        ThrowIfFailed(dxgiAdapter1.As(&dxgiAdapter4));
    }
    else
    {
        SIZE_T maxDedicatedVideoMemory = 0;
        for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
        {
            DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
            dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

            if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
                SUCCEEDED(D3D12CreateDevice(dxgiAdapter1.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) &&
                dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory)
            {
                maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
                ThrowIfFailed(dxgiAdapter1.As(&dxgiAdapter4));
            }
        }
    }
    return dxgiAdapter4;
}

Microsoft::WRL::ComPtr<ID3D12Device2> Application::CreateDevice(Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter)
{
    ComPtr<ID3D12Device2> d3d12Device2;
    ThrowIfFailed(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3d12Device2)));

#ifdef _DEBUG
    ComPtr<ID3D12InfoQueue> pInfoQueue;
    if (SUCCEEDED(d3d12Device2.As(&pInfoQueue)))
    {
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

        // D3D12_MESSAGE_CATEGORY Categories[] = {};

        D3D12_MESSAGE_SEVERITY Severities[] = {
            D3D12_MESSAGE_SEVERITY_INFO
        };

        D3D12_MESSAGE_ID DenyIds[] = {
            D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
            D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
            D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE
        };

        D3D12_INFO_QUEUE_FILTER NewFilter = {};
        // NewFilter.DenyList.NumCategories = _countof(Categories);
        // NewFilter.DenyList.pCategoryList = Categories
        NewFilter.DenyList.NumSeverities = _countof(Severities);
        NewFilter.DenyList.pSeverityList = Severities;
        NewFilter.DenyList.NumIDs = _countof(DenyIds);
        NewFilter.DenyList.pIDList = DenyIds;

        ThrowIfFailed(pInfoQueue->PushStorageFilter(&NewFilter));
    }
#endif
    return d3d12Device2;
}

bool Application::CheckTearingSupport()
{
    BOOL allowTearing = FALSE;

    ComPtr<IDXGIFactory4> factory4;
    if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4))))
    {
        ComPtr<IDXGIFactory5> factory5;
        if (SUCCEEDED(factory4.As(&factory5)))
        {
            factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
        }
    }
    return allowTearing == TRUE;
}

bool Application::IsTearingSupported() const
{
    return m_TearingSupported;
}

std::shared_ptr<Window> Application::CreateRenderWindow(const std::wstring& windowName, int clientWidth, int clientHeight, bool vSync)
{
    WindowNameMap::iterator windowIter = gs_WindowByName.find(windowName);
    if (windowIter != gs_WindowByName.end())
    {
        return windowIter->second;
    }

    RECT windowRect = { 0, 0, clientWidth, clientHeight };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hWnd = CreateWindowW(WINDOW_CLASS_NAME, windowName.c_str(),
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr, nullptr, m_hInstance, nullptr);

    if (!hWnd)
    {
        MessageBoxA(NULL, "Could not create render window", "Error", MB_OK | MB_ICONERROR);
        return nullptr;
    }

    WindowPtr pWindow = std::make_shared<MakeWindow>(hWnd, windowName, clientWidth, clientHeight, vSync);

    gs_Windows.insert(WindowMap::value_type(hWnd, pWindow));
    gs_WindowByName.insert(WindowNameMap::value_type(windowName, pWindow));

    return pWindow;
}

void Application::DestroyWindow(std::shared_ptr<Window> window)
{
    if (window) window->Destroy();
}

void Application::DestroyWindow(const std::wstring& windowName)
{
    WindowPtr pWindow = GetWindowByName(windowName);
    if (pWindow)
    {
        DestroyWindow(pWindow);
    }
}

std::shared_ptr<Window> Application::GetWindowByName(const std::wstring& windowName)
{
    std::shared_ptr<Window> window;
    WindowNameMap::iterator iter = gs_WindowByName.find(windowName);
    if (iter != gs_WindowByName.end())
    {
        window = iter->second;
    }

    return window;
}

int Application::Run(std::shared_ptr<Game> pGame)
{
    if (!pGame->Initialize()) return 1;
    if (!pGame->LoadContent()) return 2;

    MSG msg = { 0 };
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    Flush();

    pGame->UnloadContent();
    pGame->Destroy();

    return static_cast<int>(msg.wParam);
}

void Application::Quit(int exitCode)
{
    PostQuitMessage(exitCode);
}

Microsoft::WRL::ComPtr<ID3D12Device2> Application::GetDevice() const
{
    return m_d3d12Device;
}

std::shared_ptr<CommandQueue> Application::GetCommandQueue(D3D12_COMMAND_LIST_TYPE type) const
{
    std::shared_ptr<CommandQueue> commandQueue;
    switch(type)
    {
        case D3D12_COMMAND_LIST_TYPE_DIRECT: {
            commandQueue = m_DirectCommandQueue;
        } break;
        case D3D12_COMMAND_LIST_TYPE_COMPUTE: {
            commandQueue = m_ComputeCommandQueue;
        } break;
        case D3D12_COMMAND_LIST_TYPE_COPY: {
            commandQueue = m_CopyCommandQueue;
        } break;
        default: {
            assert(false && "Invalid command queue type");
        } break;
    }
    return commandQueue;
}

void Application::Flush()
{
    m_DirectCommandQueue->Flush();
    m_ComputeCommandQueue->Flush();
    m_CopyCommandQueue->Flush();
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Application::CreateDescriptorHeap(UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type)
{
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.Type = type;
    desc.NumDescriptors = numDescriptors;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    desc.NodeMask = 0;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
    ThrowIfFailed(m_d3d12Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap)));

    return descriptorHeap;
}

UINT Application::GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type) const
{
    return m_d3d12Device->GetDescriptorHandleIncrementSize(type);
}

static void RemoveWindow(HWND hWnd)
{
    WindowMap::iterator windowIter = gs_Windows.find(hWnd);
    if (windowIter != gs_Windows.end())
    {
        WindowPtr pWindow = windowIter->second;
        gs_WindowByName.erase(pWindow->GetWindowName());
        gs_Windows.erase(windowIter);
    }
}

// Convert the message ID into a mousebutton ID
MouseButtonEventArgs::MouseButton DecodeMouseButton(UINT messageID)
{
    MouseButtonEventArgs::MouseButton mouseButton = MouseButtonEventArgs::None;
    switch(messageID)
    {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK: {
            mouseButton = MouseButtonEventArgs::Left;
        } break;
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK: {
            mouseButton = MouseButtonEventArgs::Middle;
        } break;
    }
    return mouseButton;
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WindowPtr pWindow;
    {
        WindowMap::iterator iter = gs_Windows.find(hWnd);
        if (iter != gs_Windows.end())
        {
            pWindow = iter->second;
        }
    }

    if (pWindow)
    {
        switch(message)
        {
            case WM_PAINT: {
                UpdateEventArgs updateEventArgs(0.f, 0.f);
                pWindow->OnUpdate(updateEventArgs);
                RenderEventArgs renderEventArgs(0.f, 0.f);
                pWindow->OnRender(renderEventArgs);
            } break;
            case WM_SYSKEYDOWN:
            case WM_KEYDOWN: {
                MSG charMsg;
                unsigned int c = 0;
                if (PeekMessage(&charMsg, hWnd, 0, 0, PM_NOREMOVE) && charMsg.message == WM_CHAR)
                {
                    GetMessage(&charMsg, hWnd, 0, 0);
                    c = static_cast<unsigned int>(charMsg.wParam);
                }
                bool shift = ( GetAsyncKeyState(VK_SHIFT) & 0x8000 ) != 0;
                bool control = ( GetAsyncKeyState(VK_CONTROL) & 0x8000 ) !=0;
                bool alt = ( GetAsyncKeyState(VK_MENU) & 0x8000 ) != 0;
                KeyCode::Key key = (KeyCode::Key) wParam;
                unsigned int scanCode = (lParam & 0x00FF0000) >> 16;
                KeyEventArgs keyEventArgs(key, c, KeyEventArgs::Pressed, shift, control, alt);
                pWindow->OnKeyPressed(keyEventArgs);
            } break;
            case WM_SYSKEYUP:
            case WM_KEYUP: {
                bool shift = ( GetAsyncKeyState(VK_SHIFT) & 0x8000 ) != 0;
                bool control = ( GetAsyncKeyState(VK_CONTROL) & 0x8000 ) !=0;
                bool alt = ( GetAsyncKeyState(VK_MENU) & 0x8000 ) != 0;
                KeyCode::Key key = (KeyCode::Key) wParam;
                unsigned int c = 0;
                unsigned int scanCode = (lParam & 0x00FF0000) >> 16;

                unsigned char keyboardState[256];
                GetKeyboardState(keyboardState);
                wchar_t translatedCharacters[4];
                if (int result = ToUnicodeEx(static_cast<UINT>(wParam), scanCode, keyboardState, translatedCharacters, 4, 0, NULL) > 0)
                {
                    c = translatedCharacters[0];
                }
                KeyEventArgs keyEventArgs(key, c, KeyEventArgs::Released, shift, control, alt);
                pWindow->OnKeyReleased(keyEventArgs);
            } break;
            case WM_SYSCHAR: break;
            case WM_MOUSEMOVE: {
                bool lButton = (wParam & MK_LBUTTON) != 0;
                bool rButton = (wParam & MK_RBUTTON) != 0;
                bool mButton = (wParam & MK_MBUTTON) != 0;
                bool shift = (wParam & MK_SHIFT) != 0;
                bool control = (wParam & MK_CONTROL) != 0;

                int x = ((int)(short) LOWORD(lParam));
                int y = ((int)(short) HIWORD(lParam));

                MouseMotionEventArgs mouseMotionEventArgs(lButton, mButton, rButton, control, shift, x, y);
                pWindow->OnMouseMoved(mouseMotionEventArgs);
            } break;
            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_MBUTTONDOWN: {
                bool lButton = (wParam & MK_LBUTTON) != 0;
                bool rButton = (wParam & MK_RBUTTON) != 0;
                bool mButton = (wParam & MK_MBUTTON) != 0;
                bool shift = (wParam & MK_SHIFT) != 0;
                bool control = (wParam & MK_CONTROL) != 0;

                int x = ((int)(short) LOWORD(lParam));
                int y = ((int)(short) HIWORD(lParam));

                MouseButtonEventArgs mouseButtonEventArgs(DecodeMouseButton(message), MouseButtonEventArgs::Pressed, lButton, mButton, rButton, control, shift, x, y);
                pWindow->OnMouseButtonPressed(mouseButtonEventArgs);
            } break;
            case WM_LBUTTONUP:
            case WM_RBUTTONUP:
            case WM_MBUTTONUP: {
                bool lButton = (wParam & MK_LBUTTON) != 0;
                bool rButton = (wParam & MK_RBUTTON) != 0;
                bool mButton = (wParam & MK_MBUTTON) != 0;
                bool shift = (wParam & MK_SHIFT) != 0;
                bool control = (wParam & MK_CONTROL) != 0;

                int x = ((int)(short) LOWORD(lParam));
                int y = ((int)(short) HIWORD(lParam));

                MouseButtonEventArgs mouseButtonEventArgs(DecodeMouseButton(message), MouseButtonEventArgs::Release, lButton, mButton, rButton, control, shift, x, y);
                pWindow->OnMouseButtonReleased(mouseButtonEventArgs);
            } break;
            case WM_MOUSEWHEEL: {
                float zDelta = ((int)(short)HIWORD(wParam)) / (float) WHEEL_DELTA;
                short keyStates = (short) LOWORD(wParam);

                bool lButton = (wParam & MK_LBUTTON) != 0;
                bool rButton = (wParam & MK_RBUTTON) != 0;
                bool mButton = (wParam & MK_MBUTTON) != 0;
                bool shift = (wParam & MK_SHIFT) != 0;
                bool control = (wParam & MK_CONTROL) != 0;

                int x = ((int)(short) LOWORD(lParam));
                int y = ((int)(short) HIWORD(lParam));

                POINT clientToScreenPoint;
                clientToScreenPoint.x = x;
                clientToScreenPoint.y = y;
                ScreenToClient(hWnd, &clientToScreenPoint);

                MouseWheelEventArgs mouseWheelEventArgs(zDelta, lButton, mButton, rButton, control, shift, (int)clientToScreenPoint.x, (int)clientToScreenPoint.y);
                pWindow->OnMouseWheel(mouseWheelEventArgs);
            } break;
            case WM_SIZE: {
                int width = ((int)(short) LOWORD(lParam));
                int height = ((int)(short) HIWORD(lParam));

                ResizeEventArgs resizeEventArgs(width, height);
                pWindow->OnResize(resizeEventArgs);
            } break;
            case WM_DESTROY: {
                RemoveWindow(hWnd);
                if (gs_Windows.empty())
                {
                    PostQuitMessage(0);
                }
            } break;
            default: {
                return DefWindowProcW(hWnd, message, wParam, lParam);
            } break;
        }
    }
    else {
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}