#include <iostream>
#include <Windows.h>

using namespace std;

HHOOK kbHook = NULL, msHook = NULL;
HWND hWnd = NULL;

LRESULT CALLBACK LowLevelKeyboardProc(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    if (nCode < 0)
        return CallNextHookEx(kbHook, nCode, wParam, lParam);

    if (wParam == WM_KEYDOWN || wParam == WM_KEYUP)
    {
        KBDLLHOOKSTRUCT* kbdllHookStruct = (KBDLLHOOKSTRUCT*)lParam;
        if ((kbdllHookStruct->flags & 0x00000010U) != 0)
            cout << "WH_KEYBOARD_LL: LLKHF_INJECTED detected!" << endl;

        if ((kbdllHookStruct->flags & 0x00000002U) != 0)
            cout << "WH_KEYBOARD_LL: LLKHF_LOWER_IL_INJECTED detected!" << endl;
    }

    return CallNextHookEx(kbHook, nCode, wParam, lParam);
}

LRESULT CALLBACK LowLevelMouseProc(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    if (nCode < 0)
        return CallNextHookEx(msHook, nCode, wParam, lParam);

    if (wParam == WM_LBUTTONDOWN || wParam == WM_LBUTTONUP || wParam == WM_MOUSEMOVE || wParam == WM_RBUTTONDOWN || wParam == WM_RBUTTONUP)
    {
        MSLLHOOKSTRUCT* msllHookStruct = (MSLLHOOKSTRUCT*)lParam;
        if ((msllHookStruct->flags & 0x00000001U) != 0)
            cout << "WH_MOUSE_LL: LLMHF_INJECTED detected!" << endl;

        if ((msllHookStruct->flags & 0x00000002U) != 0)
            cout << "WH_MOUSE_LL: LLMHF_LOWER_IL_INJECTED detected!" << endl;
    }

    return CallNextHookEx(msHook, nCode, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
    // GetCurrentInputMessageSource
    INPUT_MESSAGE_SOURCE inputMessageSource;
    if (GetCurrentInputMessageSource(&inputMessageSource))
        if (inputMessageSource.originId == IMO_INJECTED)
            cout << "GetCurrentInputMessageSource: IMO_INJECTED detected!" << endl;
        else
            cout << "Failed to get current input message source." << endl;

    switch (uMsg)
    {
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    case WM_CREATE:
    {
        RAWINPUTDEVICE rawInputDevices[2] = { 0 };

        // Mouse
        rawInputDevices[0].usUsagePage = 0x0001;
        rawInputDevices[0].usUsage = 0x0002;
        rawInputDevices[0].dwFlags = RIDEV_INPUTSINK;
        rawInputDevices[0].hwndTarget = hWnd;

        // Keyboard
        rawInputDevices[1].usUsagePage = 0x0001;
        rawInputDevices[1].usUsage = 0x0006;
        rawInputDevices[1].dwFlags = RIDEV_INPUTSINK;
        rawInputDevices[1].hwndTarget = hWnd;

        //Register for Raw Input
        if (!RegisterRawInputDevices(rawInputDevices, 2, sizeof(rawInputDevices[0])))
            cout << "Failed to register raw input devices." << endl;

        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_INPUT:
        // RawInput


        return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int main()
{
    // Set up Low Level Keyboard Hook
    //kbHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);

    // Set up Low Level Mouse Hook
    //msHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, 0, 0);

    // Set up Window
    WNDCLASS wndClass = {};
    wndClass.hInstance = NULL;
    wndClass.lpszClassName = "InputTester";
    wndClass.lpfnWndProc = WndProc;

    if (RegisterClassA(&wndClass))
    {
        hWnd = CreateWindowEx(NULL, wndClass.lpszClassName, NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, NULL);
        if (hWnd == NULL)
            cout << "Failed to create window." << endl;
    }
    else
        cout << "Failed to register class." << endl;

    MSG message;
    while (GetMessage(&message, NULL, NULL, NULL) > 0)
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    // Tear down Low Level Keyboard Hook
    if (kbHook != NULL)
        UnhookWindowsHookEx(kbHook);

    // Tear down Low Level Mouse Hook
    if (msHook != NULL)
        UnhookWindowsHookEx(msHook);

    // Tear down Window
    if (hWnd != NULL)
        DestroyWindow(hWnd);
}