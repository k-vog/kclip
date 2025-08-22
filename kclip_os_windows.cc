#include "kclip.hh"

#include <d3d11.h>

#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"

static struct
{
  HWND                    wnd;
  UINT                    resize_w;
  UINT                    resize_h;
  IDXGISwapChain*         swapchain;
  ID3D11Device*           device;
  ID3D11DeviceContext*    devicecontext;
  ID3D11RenderTargetView* rtv;
} WIN = { };

static void check_result(HRESULT hr)
{
  if (FAILED(hr)) {
    LPWSTR error = NULL;
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                   NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                   (LPWSTR)&error, 0, NULL);
    wprintf(L"Error: %ls\n", error);
    LocalFree(error);
  }
}

static void check_last_error()
{
  check_result(HRESULT_FROM_WIN32(GetLastError()));
}

static void create_render_target()
{
  ID3D11Texture2D* framebuffer = NULL;
  HRESULT hr = WIN.swapchain->GetBuffer(0, IID_PPV_ARGS(&framebuffer));
  check_result(hr);

  hr = WIN.device->CreateRenderTargetView(framebuffer, NULL, &WIN.rtv);
  check_result(hr);

  framebuffer->Release();
}

static void destroy_render_target()
{
  if (WIN.rtv) {
    WIN.rtv->Release();
    WIN.rtv = NULL;
  }
}

void select_media_file(void)
{
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg,
                                                             WPARAM wParam,
                                                             LPARAM lParam);

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
    return TRUE;
  }

  switch (msg) {
  case WM_DESTROY: {
    PostQuitMessage(0);
    return 0;
  } break;
  case WM_SIZE: {
    if (wparam == SIZE_MINIMIZED)
        return 0;
    WIN.resize_w = (UINT)LOWORD(lparam);
    WIN.resize_h = (UINT)HIWORD(lparam);
    return 0;
  } break;
  }
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

int WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
  G.argc = __argc;
  G.argv = (const char**)__argv;

  kclip_init1_before_window();

  WNDCLASSA wc = { };
  wc.lpfnWndProc = WndProc;
  wc.lpszClassName = "KCLIP2025";
  if (!RegisterClassA(&wc)) {
    check_last_error();
  }

  WIN.wnd = CreateWindowExA(0, wc.lpszClassName, "kclip", WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            DEFAULT_WINDOW_W, DEFAULT_WINDOW_H,
                            NULL, NULL, NULL, NULL);
  if (!WIN.wnd) {
    check_last_error();
  }

  D3D_FEATURE_LEVEL featurelevels[] = { D3D_FEATURE_LEVEL_11_0 };

  DXGI_SWAP_CHAIN_DESC swapchain_desc = { };
  swapchain_desc.BufferDesc.Width  = DEFAULT_WINDOW_W;
  swapchain_desc.BufferDesc.Height = DEFAULT_WINDOW_H;
  swapchain_desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  swapchain_desc.SampleDesc.Count  = 1;
  swapchain_desc.BufferUsage       = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swapchain_desc.BufferCount       = 2;
  swapchain_desc.OutputWindow      = WIN.wnd;
  swapchain_desc.Windowed          = TRUE;
  swapchain_desc.SwapEffect        = DXGI_SWAP_EFFECT_FLIP_DISCARD;

  // @@ Enable debug layer if available
  DWORD device_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

  HRESULT hr =
    D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, device_flags,
                                  featurelevels, ARRAYSIZE(featurelevels),
                                  D3D11_SDK_VERSION, &swapchain_desc,
                                  &WIN.swapchain, &WIN.device, NULL, &WIN.devicecontext);
  check_result(hr);

  create_render_target();

  kclip_init2_setup_dear_imgui();
  ImGui_ImplWin32_Init(WIN.wnd);
  ImGui_ImplDX11_Init(WIN.device, WIN.devicecontext);

  ShowWindow(WIN.wnd, SW_SHOW);
  UpdateWindow(WIN.wnd);

  static bool running = true;
  while (running) {
    MSG msg = { };
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      running &= (msg.message != WM_QUIT);
    }

    if (WIN.resize_w != 0 || WIN.resize_h) {
      destroy_render_target();
      hr = WIN.swapchain->ResizeBuffers(0, WIN.resize_w, WIN.resize_h,
                                        DXGI_FORMAT_UNKNOWN, 0);
      check_result(hr);
      create_render_target();
      WIN.resize_w = 0;
      WIN.resize_h = 0;
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();

    kclip_frame();

    WIN.devicecontext->OMSetRenderTargets(1, &WIN.rtv, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    hr = WIN.swapchain->Present(1, 0);
  }

  DestroyWindow(WIN.wnd);

  return 0;
}

