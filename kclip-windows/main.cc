#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d11.h>

#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "kclip.hh"

static struct
{
  HWND wnd;
  // D3D11 resources
  IDXGISwapChain*         swapchain;
  ID3D11Device*           device;
  ID3D11DeviceContext*    device_context;
  ID3D11RenderTargetView* rtgt;
} A = { };

[[noreturn]]
static void Error(const char* error)
{
  OutputDebugStringA(error);
  MessageBoxA(0, error, "KClip - Error", MB_OK);
  exit(1);
}
#define ErrorF(...) Error(TempFmt(__VA_ARGS__))

// Helper: Win32 error code to string
static const char* TempWin32ErrorStr(DWORD win32_code)
{
  static thread_local char error[1024];
  BOOL ok = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 0, win32_code,
                           MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error, sizeof(error), 0);
  if (!ok) {
    return "invalid error code";
  }
  return error;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
  ImGui_ImplWin32_WndProcHandler(hwnd, umsg, wparam, lparam);
  switch (umsg) {
  case WM_DESTROY: {
    PostQuitMessage(0);
    return 0;
  } break;
  default: {
    return DefWindowProcA(hwnd, umsg, wparam, lparam);
  } break;
  }
}

static void CreateRenderTarget()
{
  HRESULT hr = 0;
  ID3D11Texture2D* bbuf = 0;
  hr = A.swapchain->GetBuffer(0, IID_PPV_ARGS(&bbuf));
  if (FAILED(hr)) {
    ErrorF("Failed to get swapchain back buffer: %s", TempWin32ErrorStr(hr));
  }
  hr = A.device->CreateRenderTargetView(bbuf, 0, &A.rtgt);
  if (FAILED(hr)) {
    ErrorF("Failed to create render target: %s", TempWin32ErrorStr(hr));
  }
  bbuf->Release();
}

int APIENTRY WinMain(_In_ HINSTANCE hinstance, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
  HRESULT hr = S_OK;

  hr = CoInitialize(0);
  if (FAILED(hr)) {
    ErrorF("Failed to initialize COM: %s", TempWin32ErrorStr(GetLastError()));
  }

  KClip_InitBeforeWindow();

  WNDCLASSA wc = { };
  wc.lpfnWndProc   = WndProc;
  wc.lpszClassName = KCLIP_TITLE;
  if (!RegisterClassA(&wc)) {
    ErrorF("Failed to register window class: %s", TempWin32ErrorStr(GetLastError()));
  }

  A.wnd = CreateWindowExA(0, wc.lpszClassName, KCLIP_TITLE, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                          KCLIP_W, KCLIP_H, 0, 0, hinstance, 0);
  if (!A.wnd) {
    ErrorF("Failed to create window: %s", TempWin32ErrorStr(GetLastError()));
  }

  // ================================================================================
  // Initialize D3D11
  // ref: https://gist.github.com/d7samurai/261c69490cce0620d0bfc93003cd1052
  // ref: https://github.com/ocornut/imgui/blob/master/examples/example_win32_directx11/main.cpp
  // ================================================================================

  D3D_FEATURE_LEVEL feature_levels[] =
  {
    D3D_FEATURE_LEVEL_11_0,
  };
  UINT num_feature_levels = (UINT)ArrayLen(feature_levels);

  DXGI_SWAP_CHAIN_DESC swapchaindesc = { };
  swapchaindesc.BufferDesc.Width  = 0;
  swapchaindesc.BufferDesc.Height = 0;
  swapchaindesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  swapchaindesc.SampleDesc.Count  = 1;
  swapchaindesc.BufferUsage       = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swapchaindesc.BufferCount       = 2;
  swapchaindesc.OutputWindow      = A.wnd;
  swapchaindesc.Windowed          = TRUE;
  swapchaindesc.SwapEffect        = DXGI_SWAP_EFFECT_FLIP_DISCARD;

  UINT create_flags = 0;
#ifdef _DEBUG
  create_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  hr = D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, create_flags, feature_levels, num_feature_levels,
                                     D3D11_SDK_VERSION, &swapchaindesc, &A.swapchain, &A.device, 0, &A.device_context);
  if (FAILED(hr)) {
    // @@: try using WARP software driver
    ErrorF("Failed to create D3D11 swapchain: %s", TempWin32ErrorStr(hr));
  }

  CreateRenderTarget();

  ImGui::CreateContext();
  ImGui_ImplWin32_Init(A.wnd);
  ImGui_ImplDX11_Init(A.device, A.device_context);

  // @@ need more robust font loading
  // @@ this should be user-selectable anyway
  // ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\tahoma.ttf", 14.0f);

  ShowWindow(A.wnd, SW_SHOW);

  bool running = true;
  while (running) {
    MSG msg;
    while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessageA(&msg);
      if (msg.message == WM_QUIT) {
        running = false;
      }
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    KClip_DrawUI();

    ImGui::Render();

    A.device_context->OMSetRenderTargets(1, &A.rtgt, 0);
    A.device_context->ClearRenderTargetView(A.rtgt, &G.theme.colors[UI_COLOR_BG].Value.x);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    hr = A.swapchain->Present(1, 0);
    if (FAILED(hr)) {
      ErrorF("Present failed: %s", TempWin32ErrorStr(hr));
    }
  }

  return 0;
}
