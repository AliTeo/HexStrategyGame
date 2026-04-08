#include <windows.h>
#include <string>
#include <vector>

namespace {
constexpr int ID_EDIT_SERVER = 101;
constexpr int ID_EDIT_PORT = 102;
constexpr int ID_BTN_CONNECT = 103;

HWND g_serverEdit = nullptr;
HWND g_portEdit = nullptr;

std::wstring getModuleDirectory() {
    wchar_t modulePath[MAX_PATH] = {0};
    GetModuleFileNameW(nullptr, modulePath, MAX_PATH);
    std::wstring path(modulePath);
    const std::size_t slash = path.find_last_of(L"\\/");
    if (slash == std::wstring::npos) {
        return L".";
    }
    return path.substr(0, slash);
}

std::wstring readEditText(HWND edit) {
    const int len = GetWindowTextLengthW(edit);
    std::wstring text(static_cast<std::size_t>(len), L'\0');
    if (len > 0) {
        GetWindowTextW(edit, text.data(), len + 1);
    }
    return text;
}

void showError(const wchar_t* title, const wchar_t* message) {
    MessageBoxW(nullptr, message, title, MB_ICONERROR | MB_OK);
}

void launchClient(HWND hwnd) {
    const std::wstring server = readEditText(g_serverEdit);
    const std::wstring port = readEditText(g_portEdit);

    if (server.empty() || port.empty()) {
        showError(L"Missing Input", L"Please enter both server and port.");
        return;
    }

    const std::wstring dir = getModuleDirectory();
    const std::wstring clientExe = dir + L"\\HexGameClient.exe";
    std::wstring cmd = L"\"" + clientExe + L"\" " + server + L" " + port;
    std::vector<wchar_t> cmdBuf(cmd.begin(), cmd.end());
    cmdBuf.push_back(L'\0');

    STARTUPINFOW si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);

    if (!CreateProcessW(
            nullptr,
            cmdBuf.data(),
            nullptr,
            nullptr,
            FALSE,
            0,
            nullptr,
            dir.c_str(),
            &si,
            &pi)) {
        showError(L"Launch Failed", L"Could not start HexGameClient.exe.\nMake sure it is in the same folder.");
        return;
    }

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    DestroyWindow(hwnd);
}
}  // namespace

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    (void)lParam;
    switch (msg) {
        case WM_CREATE: {
            CreateWindowW(L"STATIC", L"Server:", WS_VISIBLE | WS_CHILD,
                          20, 20, 80, 24, hwnd, nullptr, nullptr, nullptr);
            g_serverEdit = CreateWindowW(L"EDIT", L"0.tcp.ngrok.io",
                                         WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                                         100, 20, 260, 24, hwnd,
                                         reinterpret_cast<HMENU>(ID_EDIT_SERVER), nullptr, nullptr);

            CreateWindowW(L"STATIC", L"Port:", WS_VISIBLE | WS_CHILD,
                          20, 60, 80, 24, hwnd, nullptr, nullptr, nullptr);
            g_portEdit = CreateWindowW(L"EDIT", L"12345",
                                       WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                                       100, 60, 260, 24, hwnd,
                                       reinterpret_cast<HMENU>(ID_EDIT_PORT), nullptr, nullptr);

            CreateWindowW(L"BUTTON", L"Connect", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                          140, 105, 110, 32, hwnd,
                          reinterpret_cast<HMENU>(ID_BTN_CONNECT), nullptr, nullptr);
            return 0;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == ID_BTN_CONNECT) {
                launchClient(hwnd);
                return 0;
            }
            break;
        }
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"HexGameLauncherWindow";

    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"HexGame Launcher",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 190,
        nullptr, nullptr, hInstance, nullptr);

    if (!hwnd) {
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}
