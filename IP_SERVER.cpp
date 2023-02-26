int _tmain() {
    // Initialize Winsock
    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << "\n";
        return 1;
    }

    // Create socket
    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET) {
        std::cerr<< "Failed to create socket: " << WSAGetLastError() << "\n";
WSACleanup();
return 1;
}
	// Bind socket to port
sockaddr_in server_address;
server_address.sin_family = AF_INET;
server_address.sin_addr.s_addr =  htonl(INADDR_ANY);;
server_address.sin_port = htons(SERVER_PORT);
result = bind(server_socket, (sockaddr*) &server_address, sizeof(server_address));
if (result == SOCKET_ERROR) {
    std::cerr << "Failed to bind socket to port: " << WSAGetLastError() << "\n";
    closesocket(server_socket);
    WSACleanup();
    return 1;
}

// Listen for incoming connections
result = listen(server_socket, SOMAXCONN);
if (result == SOCKET_ERROR) {
    std::cerr << "Failed to listen for incoming connections: " << WSAGetLastError() << "\n";
    closesocket(server_socket);
    WSACleanup();
    return 1;
}

// Loop to accept incoming connections
while (true) {
    // Accept incoming connection
    sockaddr_in client_address;
    int address_size = sizeof(client_address);
    SOCKET client_socket = accept(server_socket, (sockaddr*) &client_address, &address_size);
    if (client_socket == INVALID_SOCKET) {
        std::cerr << "Failed to accept incoming connection: " << WSAGetLastError() << "\n";
        continue;
    }

    // Prompt user to choose which screen to display based on client IP
    char ip_address[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_address.sin_addr, ip_address, INET_ADDRSTRLEN);
    std::cout << "Client connected from IP address: " << ip_address << "\n";
    std::cout << "Enter 1 to view screen: ";
    int choice;
    std::cin >> choice;

    // Receive bitmap data from client
    int buffer_size = SCREEN_WIDTH * SCREEN_HEIGHT * 3;
    char* buffer = new char[buffer_size];
    result = recv(client_socket, buffer, buffer_size, 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "Failed to receive bitmap data: " << WSAGetLastError() << "\n";
        closesocket(client_socket);
        continue;
    }

    // Display received bitmap data
    HDC screen_dc = GetDC(NULL);
    HDC memory_dc = CreateCompatibleDC(screen_dc);
    HBITMAP bitmap = CreateCompatibleBitmap(screen_dc, SCREEN_WIDTH, SCREEN_HEIGHT);
    SelectObject(memory_dc, bitmap);

    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT * 3; i += 3) {
        int x = (i / 3) % SCREEN_WIDTH;
        int y = (i / 3) / SCREEN_WIDTH;
        COLORREF color = RGB(buffer[i + 2], buffer[i + 1], buffer[i]);
        SetPixel(memory_dc, x, y, color);
    }

    HDC window_dc = GetDC(GetDesktopWindow());
    BitBlt(window_dc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, memory_dc, 0, 0, SRCCOPY);

    // Clean up resources
    ReleaseDC(NULL, screen_dc);
    ReleaseDC(GetDesktopWindow(), window_dc);
    DeleteObject(bitmap);
    DeleteDC(memory_dc);
    closesocket(client_socket);
}

// Clean up Winsock
closesocket(server_socket);
WSACleanup();

return 0;
}

