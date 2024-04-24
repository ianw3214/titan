#include "game.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <DirectXMath.h>

#include "application.h"
#include "window.h"

Game::Game(const std::wstring& name, int width, int height, bool vSync)
    : m_Name(name), m_Width(width), m_Height(height), m_vSync(vSync) {}

Game::~Game()
{
    assert(!m_pWindow && "Use Game::Destroy() before destruction");
}

bool Game::Initialize()
{
    if (!DirectX::XMVerifyCPUSupport())
    {
        MessageBoxA(NULL, "Failed to verify DirectX Math library support", "Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_pWindow = Application::Get().CreateRenderWindow(m_Name, m_Width, m_Height, m_vSync);
    m_pWindow->RegisterCallbacks(shared_from_this());
    m_pWindow->Show();

    return true;
}

void Game::Destroy()
{
    Application::Get().DestroyWindow(m_pWindow);
    m_pWindow.reset();
}

void Game::OnUpdate(UpdateEventArgs& e)
{

}

void Game::OnRender(RenderEventArgs& e)
{

}

void Game::OnKeyPressed(KeyEventArgs& e)
{
}

void Game::OnKeyReleased(KeyEventArgs& e)
{
}

void Game::OnMouseMoved(class MouseMotionEventArgs& e)
{
}

void Game::OnMouseButtonPressed(MouseButtonEventArgs& e)
{
}

void Game::OnMouseButtonReleased(MouseButtonEventArgs& e)
{
}

void Game::OnMouseWheel(MouseWheelEventArgs& e)
{
}

void Game::OnResize(ResizeEventArgs& e)
{
    m_Width = e.Width;
    m_Height = e.Height;
}

void Game::OnWindowDestroy()
{
    UnloadContent();
}