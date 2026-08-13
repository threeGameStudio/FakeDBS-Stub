#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef _MSC_VER
#include <malloc.h>
#endif
#include <stdlib.h>
#include <string.h>

#define MAILSLOT_NAME "\\\\.\\mailslot\\FakeDBS_MS"
#define LISTBOX_ID 1001

/* Recovered with DWARF names from WndProc() at 0x004013c0. */
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND hLB;
    HINSTANCE hInstance;

    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_SIZE:
        hLB = GetDlgItem(hwnd, LISTBOX_ID);
        MoveWindow(hLB, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        return 0;

    case WM_CREATE:
        hInstance = (HINSTANCE)GetWindowLongA(hwnd, GWL_HINSTANCE);
        CreateWindowExA(
            0,
            "LISTBOX",
            "",
            WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
                LBS_HASSTRINGS | LBS_DISABLENOSCROLL,
            0, 0, 100, 100,
            hwnd,
            (HMENU)LISTBOX_ID,
            hInstance,
            NULL);
        return 0;
    }

    return DefWindowProcA(hwnd, message, wParam, lParam);
}

/* Recovered with DWARF names from WinMain() at 0x00401503. */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEXA wc;
    HWND hwnd;
    HWND hLB;
    MSG msg;
    HANDLE hMailSlot;
    BOOL bQuitRecieved = FALSE;

    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;

    hMailSlot = CreateMailslotA(MAILSLOT_NAME, 8192, MAILSLOT_WAIT_FOREVER, NULL);
    if (hMailSlot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursorA(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "WindowClass";
    wc.hIcon = LoadIconA(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIconA(NULL, IDI_APPLICATION);

    if (!RegisterClassExA(&wc)) {
        MessageBoxA(NULL, "Window Registration Failed!", "Error!",
                    MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowExA(
        WS_EX_CLIENTEDGE,
        "WindowClass",
        "FakeDBSView",
        WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        640, 480,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        MessageBoxA(NULL, "Window Creation Failed!", "Error!",
                    MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    while (!bQuitRecieved) {
        int next_msg_size = 0;
        int n_read = 0;

        GetMailslotInfo(hMailSlot, NULL, (LPDWORD)&next_msg_size, NULL, NULL);
        if ((DWORD)next_msg_size == MAILSLOT_NO_MESSAGE) {
            Sleep(1);
        } else {
            /* GNU C89 extension. DWARF describes this as a variable array. */
#ifdef _MSC_VER
            char *msg_buffer = (char *)_alloca((size_t)next_msg_size);
#else
            char msg_buffer[next_msg_size];
#endif
            int idx;

            hLB = GetDlgItem(hwnd, LISTBOX_ID);
            ReadFile(hMailSlot, msg_buffer, next_msg_size,
                     (LPDWORD)&n_read, NULL);
            msg_buffer[n_read] = '\0';

            idx = (int)SendMessageA(hLB, LB_ADDSTRING, 0, (LPARAM)msg_buffer);
            if (idx != LB_ERR) {
                SendMessageA(hLB, LB_SETTOPINDEX, idx, 0);
            }
        }

        while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE) > 0) {
            if (msg.message == WM_QUIT) {
                bQuitRecieved = TRUE;
            }
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
    }

    return (int)msg.wParam;
}
