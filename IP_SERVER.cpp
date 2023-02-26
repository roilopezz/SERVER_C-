void CreateAndShowWindow(HINSTANCE hInstance, const char* className, const char* windowTitle, int width, int height);
void ReceiveScreenData(SOCKET clientSock, HWND hWnd);

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
