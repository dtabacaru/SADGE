// GameBoyGui.cpp : Defines the entry point for the application.
//

#include "GameBoy.h"
#include "framework.h"
#include <commdlg.h>
#include "Sadge.Ui.h"
#include <filesystem>
#include <shellscalingapi.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

GameBoy game_boy;
std::thread game_boy_thread;
std::filesystem::path rom;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                LoadRom(HWND hWnd);
void                CloseGameboy();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR    lpCmdLine,
                      _In_ int       nCmdShow)
{
  // Allocate a new console window
  AllocConsole();

  FILE* stream;
  freopen_s(&stream, "CONOUT$", "w", stdout);

  SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  // TODO: Place code here.

  // Initialize global strings
  LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
  LoadStringW(hInstance, IDC_SADGEUI, szWindowClass, MAX_LOADSTRING);
  MyRegisterClass(hInstance);

  // Perform application initialization:
  if (!InitInstance(hInstance, nCmdShow))
  {
    return FALSE;
  }

  HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SADGEUI));

  MSG msg;

  // Main message loop:
  while (GetMessage(&msg, nullptr, 0, 0))
  {
    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  return (int)msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
  WNDCLASSEXW wcex{};

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SADGEUI));
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = CreateSolidBrush(RGB(232, 252, 204));;
  wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SADGEUI);
  wcex.lpszClassName = szWindowClass;
  wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

  return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
  hInst = hInstance; // Store instance handle in our global variable

  RECT rc = {0, 0, 800, 720};
  DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
  AdjustWindowRect(&rc, style, TRUE);
  int windowWidth = rc.right - rc.left;
  int windowHeight = rc.bottom - rc.top;

  HWND hWnd = CreateWindowW(
    szWindowClass,
    szTitle,
    style,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    windowWidth,
    windowHeight,
    nullptr,
    nullptr,
    hInstance,
    nullptr
  );

  if (!hWnd)
  {
    return FALSE;
  }

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  return TRUE;
}

std::filesystem::path OpenFileDialog(HWND hWnd)
{
  std::filesystem::path out;

  TCHAR szFileName[MAX_PATH] = {0};

  OPENFILENAME ofn = {0};
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = hWnd;
  ofn.lpstrFile = szFileName;
  ofn.nMaxFile = sizeof(szFileName) / sizeof(szFileName[0]);

  ofn.lpstrFilter = TEXT("GameBoy ROM Files (*.gb)\0*.gb\0All Files (*.*)\0*.*\0");
  ofn.nFilterIndex = 1;

  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

  if (GetOpenFileName(&ofn))
  {
    return ofn.lpstrFile;
  }

  return out;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

void LoadRom(HWND hWnd)
{
  rom = OpenFileDialog(hWnd);

  if (rom.empty())
    return;

  game_boy_thread = std::thread(
  [hWnd]()
  {
    game_boy.InitScreen();

    HWND raylibHwnd = (HWND)game_boy.GetHandle();
    int ray_style = GetWindowLong(raylibHwnd, GWL_STYLE);
    ray_style &= ~WS_OVERLAPPEDWINDOW;
    ray_style |= WS_CHILD;
    SetWindowLong(raylibHwnd, GWL_STYLE, ray_style);

    SetParent(raylibHwnd, hWnd);

    UINT dpi = GetDpiForWindow(hWnd);
    float scaleFactor = dpi / 96.0f;  // 96 DPI is the baseline
    int adjustedX = static_cast<int>(0 * scaleFactor);
    int adjustedY = static_cast<int>(0 * scaleFactor);
    SetWindowPos(raylibHwnd, nullptr, adjustedX, adjustedY, 0, 0, SWP_NOSIZE);

    //SetWindowPos(raylibHwnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE);
    ShowWindow(raylibHwnd, 1);

    game_boy.InsertRom(rom);
    game_boy.TurnOn();
  });
}

void CloseGameboy()
{
  game_boy.TurnOff();
  if (game_boy_thread.joinable())
    game_boy_thread.join();
}

void ScaleWindow(HWND hWnd, int scale)
{
  game_boy.SetScale(scale);

  int width = scale * 160;
  int height = scale * 144;

  RECT rc = {0, 0, width, height};
  DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
  AdjustWindowRect(&rc, style, TRUE);
  int windowWidth = rc.right - rc.left;
  int windowHeight = rc.bottom - rc.top;

  SetWindowPos(hWnd, nullptr, 0, 0, windowWidth, windowHeight, SWP_NOMOVE | SWP_NOZORDER);
}

bool fullscreen = false;

struct MonitorSize
{
  uint32_t width{};
  uint32_t height{};
};

MonitorSize GetLogicalMonitorSize(HWND hWnd) {
  HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
  MonitorSize size;
  UINT x_dpi;
  UINT y_dpi;
  HRESULT hr = GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &x_dpi, &y_dpi);
  if (SUCCEEDED(hr)) {
    double x_scale = ((double)x_dpi / 96);
    double y_scale = ((double)y_dpi / 96);
    size.width = GetSystemMetrics(SM_CXSCREEN) / x_scale;
    size.height = GetSystemMetrics(SM_CYSCREEN) / y_scale;
  }
  else
  {
    size.width = GetSystemMetrics(SM_CXSCREEN);
    size.height = GetSystemMetrics(SM_CYSCREEN);
  }
  return size;
}

void ToggleFullscreen(HWND hWnd)
{
  if (!fullscreen)
  {
    MonitorSize size = GetLogicalMonitorSize(hWnd);

    HWND raylibHwnd = (HWND)game_boy.GetHandle();
    int ray_style = GetWindowLong(raylibHwnd, GWL_STYLE);
    ray_style &= ~WS_CHILD;
    ray_style |= WS_OVERLAPPEDWINDOW;
    SetWindowLong(raylibHwnd, GWL_STYLE, ray_style);
    SetParent(raylibHwnd, nullptr);
    SetWindowPos(raylibHwnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE);
    ShowWindow(raylibHwnd, 1);
    game_boy.FullScreen(true, 1920, 1080);
    fullscreen = true;
  }
  else
  {
    HWND raylibHwnd = (HWND)game_boy.GetHandle();
    int ray_style = GetWindowLong(raylibHwnd, GWL_STYLE);
    ray_style &= ~WS_OVERLAPPEDWINDOW;
    ray_style |= WS_CHILD;
    SetWindowLong(raylibHwnd, GWL_STYLE, ray_style);
    SetParent(raylibHwnd, hWnd);
    SetWindowPos(raylibHwnd, nullptr, 0, -39, 0, 0, SWP_NOSIZE);
    ShowWindow(raylibHwnd, 1);
    game_boy.FullScreen(false, 1920, 1080);
    fullscreen = false;
  }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
    case WM_KEYDOWN:
    if (wParam == VK_ESCAPE)  // Check if the ESC key was pressed
    {
      // Perform your desired action, e.g., close the application
      int a = 1;
    }
    break;

    case WM_COMMAND:
    {
      int wmId = LOWORD(wParam);
      // Parse the menu selections:
      switch (wmId)
      {
        case ID_FILE_LOADROM:
          LoadRom(hWnd);
          break;
        case IDM_ABOUT:
          DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
          break;
        case IDM_EXIT:
          DestroyWindow(hWnd);
          break;
        case ID_VIEW_1X:
          ScaleWindow(hWnd,1);
          break;
        case ID_VIEW_2X:
          ScaleWindow(hWnd,2);
          break;
        case ID_VIEW_3X:
          ScaleWindow(hWnd,3);
          break;
        case ID_VIEW_4X:
          ScaleWindow(hWnd,4);
          break;
        case ID_VIEW_5X:
          ScaleWindow(hWnd,5);
          break;
        case ID_VIEW_6X:
          ScaleWindow(hWnd,6);
          break;
        case ID_VIEW_7X:
          ScaleWindow(hWnd,7);
          break;
        case ID_VIEW_8X:
          ScaleWindow(hWnd,8);
          break;
        case ID_VIEW_9X:
          ScaleWindow(hWnd,9);
          break;
        case ID_VIEW_10X:
          ScaleWindow(hWnd,10);
          break;
        case ID_VIEW_11X:
          ScaleWindow(hWnd,11);
          break;
        case ID_VIEW_12X:
          ScaleWindow(hWnd,12);
          break;
        case ID_VIEW_13X:
          ScaleWindow(hWnd,13);
          break;
        case ID_VIEW_FULLSCREEN:
          ToggleFullscreen(hWnd);
          break;
        default:
          return DefWindowProc(hWnd, message, wParam, lParam);
      }
    }
    break;
    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hWnd, &ps);
      EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
    {
      CloseGameboy();
      PostQuitMessage(0);
      break;
    }

    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  UNREFERENCED_PARAMETER(lParam);
  switch (message)
  {
    case WM_INITDIALOG:
      return (INT_PTR)TRUE;

    case WM_COMMAND:
      if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
      {
        EndDialog(hDlg, LOWORD(wParam));
        return (INT_PTR)TRUE;
      }
      break;
  }
  return (INT_PTR)FALSE;
}
