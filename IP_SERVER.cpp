LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void CreateAndShowWindow(HINSTANCE hInstance, LPCSTR className, LPCSTR windowName, int width, int height) {
    // Register the window class
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;
    RegisterClassEx(&wc);

    // Create the window
    HWND hWnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
        className,
        windowName,
        WS_POPUP,
        0, 0, width, height,
        NULL, NULL, hInstance, NULL);

    if (hWnd == NULL) {
        std::cout << "CreateWindowEx failed: " << GetLastError() << std::endl;
        return;
    }

    // Make the window transparent and layered
    SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

    // Show the window
    ShowWindow(hWnd, SW_SHOW);
}

void ReceiveScreenData(SOCKET clientSock, HWND hWnd) {
    // Allocate memory for the screen data buffer
    const int bufferLength = SCREEN_WIDTH * SCREEN_HEIGHT * 3;
    char* buffer = new char[bufferLength];

    // Set up a device context for the server-side window
    HDC hdc = GetDC(hWnd);

    while (true) {
        // Receive the screen data
        int bytesReceived = recv(clientSock, buffer, bufferLength, 0);
        if (bytesReceived == SOCKET_ERROR) {
            std::cout << "recv failed: " << WSAGetLastError() << std::endl;
            break;
        }

        if (bytesReceived == 0) {
            std::cout << "Connection closed" << std::endl;
            break;
        }

        // Iterate over the received screen data and set the pixel color on the server-side window
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            for (int x = 0; x < SCREEN_WIDTH; x++) {
                int i = (y * SCREEN_WIDTH + x) * 3;
                BYTE b = (BYTE)buffer[i];
                BYTE g = (BYTE)buffer[i+1];
                BYTE r = (BYTE)buffer[i+2];
                SetPixel(hdc, x, y, RGB(r, g, b));
            }
        }
    }

    // Release the device context and free the buffer memory
    ReleaseDC(hWnd, hdc);
    delete[] buffer;
}



LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI _tmain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char* className = "WindowClass";
    const char* windowTitle = "Remote Screen";

    CreateAndShowWindow(hInstance, className, windowTitle, SCREEN_WIDTH, SCREEN_HEIGHT);

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup failed" << std::endl;
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cout << "socket failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    SOCKADDR_IN serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "bind failed with error: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    if (listen(sock, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::cout << "Waiting for incoming connections..." << std::endl;

    while (true) {
        SOCKADDR_IN clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        SOCKET clientSock = accept(sock, (SOCKADDR*)&clientAddr, &clientAddrSize);
        if (clientSock == INVALID_SOCKET) {
            std::cout << "accept failed with error: " << WSAGetLastError() << std::endl;
            closesocket(sock);
            WSACleanup();
            return 1;
        }

        std::cout << "Client connected" << std::endl;

        ReceiveScreenData(clientSock, FindWindow(className, windowTitle));

        closesocket(clientSock);
    }

    closesocket(sock);
    WSACleanup();

    return 0;
}
