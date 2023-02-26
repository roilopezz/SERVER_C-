int _tmain() {
    // Initialize Winsock
    WSADATA wsaData = {};
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cout << "WSAStartup failed: " << result << std::endl;
        return 1;
    }

    // Create a socket to listen for incoming connections
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cout << "socket failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Bind the socket to a local address and port
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(12345);
    result = bind(sock, (sockaddr*)&addr, sizeof(addr));
    if (result == SOCKET_ERROR) {
        std::cout << "bind failed: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    result = listen(sock, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        std::cout << "listen failed: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Accept an incoming connection
    SOCKET clientSock = accept(sock, NULL, NULL);
    if (clientSock == INVALID_SOCKET) {
        std::cout << "accept failed: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }


// Declare a buffer to store the received screen data
char buffer[SCREEN_WIDTH * SCREEN_HEIGHT * 3];

// Receive screen data from the client
int received = 0;
while (received < sizeof(buffer)) {
    int result = recv(clientSock, buffer + received, sizeof(buffer) - received, 0);
    if (result == SOCKET_ERROR) {
        std::cout << "recv failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSock);
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    received += result;
}






    // Register a window class for the window that will display the received screen data
    HINSTANCE hInstance = GetModuleHandle(NULL);
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "WindowClass";
    RegisterClassEx(&wc);

	HWND hWnd = CreateWindowEx(
    WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
    "WindowClass",
    "Remote Screen",
    WS_POPUP,
    0, 0, 800, 600,
    NULL, NULL, hInstance, NULL);
if (hWnd == NULL) {
    std::cout << "CreateWindowEx failed: " << GetLastError() << std::endl;
    closesocket(clientSock);
    closesocket(sock);
    WSACleanup();
    return 1;
}



	// Set up a device context for the server-side window
	HDC hdc = GetDC(hWnd);



// Iterate over the received screen data and set the pixel color on the server-side window
for (int y = 0; y < SCREEN_HEIGHT; y++) {
    HDC hdc = GetDC(hWnd); // define hdc inside the loop
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        int i = (y * SCREEN_WIDTH + x) * 3;
        BYTE b = (BYTE)buffer[i];
        BYTE g = (BYTE)buffer[i+1];
        BYTE r = (BYTE)buffer[i+2];
        SetPixel(hdc, x, y, RGB(r, g, b));
    }
    //ReleaseDC(hWnd, hdc); // release hdc after the inner loop
}




// Release the device context
ReleaseDC(hWnd, hdc);

// Make the window transparent and layered
SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

// Show the window
ShowWindow(hWnd, SW_SHOW);



// Receive screen data from the client
//std::vector<char> buffer(BUF_SIZE);
int bytesReceived = 0;
int totalBytesReceived = 0;
do {
    bytesReceived = recv(clientSock, &buffer[totalBytesReceived], BUF_SIZE - totalBytesReceived, 0);
    if (bytesReceived == SOCKET_ERROR) {
        std::cout << "recv failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSock);
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    totalBytesReceived += bytesReceived;
} while (totalBytesReceived < BUF_SIZE);

// Create a bitmap from the received screen data
BITMAPINFO bmi = {};
bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
bmi.bmiHeader.biWidth = 640;
bmi.bmiHeader.biHeight = 480;
bmi.bmiHeader.biPlanes = 1;
bmi.bmiHeader.biBitCount = 24;
bmi.bmiHeader.biCompression = BI_RGB;
HBITMAP hBitmap = CreateDIBitmap(GetDC(NULL), &(bmi.bmiHeader), CBM_INIT, &buffer[0], &bmi, DIB_RGB_COLORS);

// Display the bitmap in the window
//HDC hdc = GetDC(hWnd);
HDC memDC = CreateCompatibleDC(hdc);
HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);
BitBlt(hdc, 0, 0, 640, 480, memDC, 0, 0, SRCCOPY);
SelectObject(memDC, oldBitmap);
DeleteDC(memDC);
ReleaseDC(hWnd, hdc);

// Cleanup
DeleteObject(hBitmap);
closesocket(clientSock);
closesocket(sock);
WSACleanup();
return 0;
}
