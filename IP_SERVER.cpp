//credit to DarkCrow (Sagi Medina)
#include "stdafx.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <ws2spi.h>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <time.h>  // include time.h header

#pragma comment(lib, "ws2_32.lib")


using namespace std;

#define SERVER_PORT 12345
#define BUF_SIZE 4096  // block transfer size  
#define QUEUE_SIZE 100 
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080


extern void ReadInis ( void );
extern void GetMACaddress(void);
extern void InitLogFile (void);
extern void PrintLog( char HostNameData[20] ,	char RemoteIP[17] , unsigned char MACData[6] , char User_Name[20] , char ban_status , char add_ban ) ;
extern void Print_Ban( char HostNameData[20] ,	char RemoteIP[17] , unsigned char MACData[6] , char User_Name[20] , char ban_status , char add_ban , char BanReason[30] ) ;
extern void Print_Ban_Logs( char HostNameData[20] ,	char RemoteIP[17] , unsigned char MACData[6] , char User_Name[20] , char ban_status , char add_ban , char BanReason[30] ) ;
extern char Check_Ban( char HostNameData[20] ,	char RemoteIP[17] , unsigned char MACData[6] , char User_Name[20] , char ban_status , char add_ban ) ;
int AutoBanTime=2;
int didbanname='1';
int didlogtxt='1';
int didlogonscr='1';
extern int MacProtect;
char ServerVers[3]={0};

int _tmainOriginal(int argc, _TCHAR* argv[])
{
	int		b, l, on	= 1;
	char	recvbuf[100],banforever='0'; 
	char	buf[100];
	SOCKET	s, sa;
	struct	sockaddr_in channel;  // holds IP address
	WORD	wVersionRequested;
	WSADATA wsaData;
	int		err;
	int		bytesRecv;
	unsigned char MACData[6];
	char HostNameData[20];
	char RemoteIP[17];
	char ban_status;
	char User_Name[20]={0};
	char add_ban;
	char BanReason[30]={0};
	int i;
	SYSTEMTIME st;


	printf(" \n");
	printf("-------------------------------------------------------------- \n");
	printf(" \n");
	printf(" === Anti-Hack Server by RoiLopez === \n");
	printf(" \n");
	//GetMACaddress();
	ReadInis(); // Load Config
	printf(" \n");
	printf(" Users Connections : \n");



	InitLogFile();

	wVersionRequested = MAKEWORD( 1, 1 );
	err = WSAStartup( wVersionRequested, &wsaData );

	if ( err != 0 ) {
		printf("WSAStartup error %ld", WSAGetLastError() );
		WSACleanup();
		return false;
	}
	//------------------------------------------------------

	                

	//---- Build address structure to bind to socket.--------  
	memset(&channel, 0, sizeof(channel));// zerochannel 
	channel.sin_family = AF_INET; 
	channel.sin_addr.s_addr = htonl(INADDR_ANY); 
	channel.sin_port = htons(SERVER_PORT); 
	//--------------------------------------------------------

	// keep track of the number of connections
	int numConnections = 0;

	// ---- create SOCKET--------------------------------------
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);    
	if (s < 0) {
		printf("socket error %ld",WSAGetLastError() );
		WSACleanup();
		return false;
	}

	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on)); 
	//---------------------------------------------------------

	//---- BIND socket ----------------------------------------
	b = bind(s, (struct sockaddr *) &channel, sizeof(channel)); 

	if (b < 0) {
		printf("bind error %ld", WSAGetLastError() ); 
		WSACleanup();
		return false;
	}
	//----------------------------------------------------------


	//---- LISTEN socket ----------------------------------------
	l = listen(s, QUEUE_SIZE);                 // specify queue size 
	if (l < 0) {
		printf("listen error %ld",WSAGetLastError() );
		WSACleanup();
		return false;
	}
	//-----------------------------------------------------------
	while (1) {
	//---- ACCEPT connection ------------------------------------
	sa = accept(s, 0 , 0);                  // block for connection request  
	if (sa < 0) {
		printf("accept error %ld ", WSAGetLastError() ); 
		WSACleanup();
		return false;
	}
	//------------------------------------------------------------
	// Socket is now set up and bound. Wait for connection and process it. 
	
	//---- RECV bytes --------------------------------------------
	bytesRecv = recv( sa, recvbuf, sizeof(recvbuf), 0 );
	err = WSAGetLastError( );// 10057 = A request to send or receive data was disallowed because the socket is not connected and (when sending on a datagram socket using a sendto call) 
	if ( bytesRecv == 0 || bytesRecv == WSAECONNRESET ) {
      printf( "Connection Closed.\n");
	  WSACleanup();
    }


	//------------------------------------------------------------
	// START OF DEFINE DATA RECV!!!!!!!!!!!
	//---------------------------------------------------------------


	for(i=0;i<=5;i++){
		MACData[i]=recvbuf[i];
	}
	for(i=6;i <= 20 ; i++){
		HostNameData[i-6]=recvbuf[i];	
	}
	for(i=21;i<=37;i++){
		RemoteIP[i-21]=recvbuf[i];
		ban_status=recvbuf[38];				//SAVE BAN STATUS
	}


	for(i=39; i <= 59 ; i++){
		User_Name[i-39]=recvbuf[i];	//SAVE USERNAME
	}

	if (strcmp(User_Name,"") == 0){
		strcpy(User_Name,"No_Name");
	}


	for(i=61; i <= 90 ; i++){
		BanReason[i-61]=recvbuf[i]; //SAVE USERNAME
	}				

	//BanReason = 1 = Memory_scan , 2 = Title Scan , 3 = Wrond Client

	add_ban=recvbuf[60];	//SAVE ADD BAN
	GetLocalTime(&st);


	//if(didlogonscr==1 || add_ban=='1'){
	if(add_ban=='1'){

		//ban_status
	if (bytesRecv > 0) {
		//printf(" \n");
		printf("User Ban : (%s), Date: %02d/%02d/%02d, Time: %02d:%02d:%02d, \n",RemoteIP, st.wDay , st.wMonth , st.wYear,st.wHour, st.wMinute, st.wSecond); //, //BanReason//ban_status)
      }
	}else{
	printf("User connected : (%s), Date: %02d/%02d/%02d, Time: %02d:%02d:%02d, \n",RemoteIP, st.wDay , st.wMonth , st.wYear,st.wHour, st.wMinute, st.wSecond); //, //BanReason//ban_status)	
	}

	if(didlogtxt == 1 ){
	PrintLog(HostNameData , RemoteIP , MACData , User_Name , ban_status , add_ban);
	}

	if (add_ban=='1'){
		Print_Ban_Logs(HostNameData , RemoteIP , MACData , User_Name , ban_status , add_ban,BanReason);

		if(BanReason[0] != '2' || didbanname == 1 ){
			Print_Ban(HostNameData , RemoteIP , MACData , User_Name , ban_status , add_ban,BanReason);
			buf[0]='3';	
		}

		else{
			buf[0]='5';	
		}
	}

	else{
		buf[0]=Check_Ban(HostNameData , RemoteIP , MACData , User_Name , ban_status , add_ban);
	}
	

	
	if (recvbuf[99]=='9'){
		exit(0);
	}

	if (recvbuf[98]=='9'){
		banforever='1';
	}


	buf[1]=ServerVers[0];
	buf[2]=ServerVers[1];
	buf[3]=AutoBanTime;

	if (banforever=='1')
	buf[0]='1'; 

	bytesRecv = send( sa, buf, 100, 0 ); 
	closesocket( sa );
	}




	closesocket( s );
	WSACleanup();
	return 0;
}





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
        std::cerr << "Failed to create socket: " << WSAGetLastError() << "\n";
        WSACleanup();
        return 1;
    }

    // Bind socket to port
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);;
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

    // Create window to display screen
    HWND hwnd = CreateWindowW(L"STATIC", NULL, WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, NULL, NULL);

    // Create device context and bitmap to display screen
    HDC hdc_screen = GetDC(NULL);
    HDC hdc_bitmap = CreateCompatibleDC(hdc_screen);
    BITMAPINFO bitmap_info = { 0 };
    bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmap_info.bmiHeader.biWidth = SCREEN_WIDTH;
    bitmap_info.bmiHeader.biHeight = -SCREEN_HEIGHT;  // negative height for top-down bitmap
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = 24;
    bitmap_info.bmiHeader.biCompression = BI_RGB;
    HBITMAP hbitmap = CreateDIBSection(hdc_bitmap, &bitmap_info, DIB_RGB_COLORS, NULL, NULL, 0);
    SelectObject(hdc_bitmap, hbitmap);

    // Loop to accept incoming connections
    while (true) {
        // Accept incoming connection
        sockaddr_in client_address;
        int address_size = sizeof(client_address);
        SOCKET client_socket = accept(server_socket, (sockaddr*) &client_address, &address_size);
        if (client_socket == INVALID_SOCKET) {
            std::cerr << "Failed to accept incoming connection: " << WSAGetLastError() << "\n";
            continue;
        }   // Prompt user to choose which screen to display based on client IP
        std::cout << "Enter the number of the screen you want to display:\n";
        for (int i = 0; i < screen_ips.size(); i++) {
            std::cout << i + 1 << ". " << screen_ips[i] << "\n";
        }
        int choice;
        std::cin >> choice;

        // Get the screen dimensions
        int screen_width = GetSystemMetrics(SM_CXSCREEN);
        int screen_height = GetSystemMetrics(SM_CYSCREEN);

        // Initialize window
        HWND hwnd = CreateWindowEx(
            WS_EX_TOPMOST,              // Extended style
            className,                 // Class name
            windowName,                // Window name
            WS_POPUP | WS_VISIBLE,     // Style
            0, 0,                      // X, Y
            screen_width, screen_height,  // Width, height
            NULL,                      // Parent window
            NULL,                      // Menu
            hInstance,                 // Instance handle
            NULL);                     // Additional application data

        if (hwnd == NULL) {
            std::cerr << "Failed to create window: " << GetLastError() << std::endl;
            return 1;
        }

        // Set the window as the foreground window
        SetForegroundWindow(hwnd);
        SetFocus(hwnd);

        // Draw the bitmap to the window
        HDC hdc = GetDC(hwnd);
        HDC mem_dc = CreateCompatibleDC(hdc);
        HBITMAP bitmap = CreateCompatibleBitmap(hdc, width, height);
        SelectObject(mem_dc, bitmap);
        BITMAPINFO bitmap_info = { 0 };
        bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
        bitmap_info.bmiHeader.biWidth = width;
        bitmap_info.bmiHeader.biHeight = -height;
        bitmap_info.bmiHeader.biPlanes = 1;
        bitmap_info.bmiHeader.biBitCount = 32;
        bitmap_info.bmiHeader.biCompression = BI_RGB;
        bitmap_info.bmiHeader.biSizeImage = width * height * 4;

        while (true) {
            // Receive the screen data from the client
            int bytes_received = recv(client_socket, (char*)screen_data.data(), screen_data.size(), 0);
            if (bytes_received == SOCKET_ERROR) {
                std::cerr << "Failed to receive screen data: " << WSAGetLastError() << std::endl;
                return 1;
            }

            // Convert the screen data to a bitmap
            int result = SetDIBits(mem_dc, bitmap, 0, height, screen_data.data(), &bitmap_info, DIB_RGB_COLORS);
            if (result == 0) {
                std::cerr << "Failed to convert screen data to bitmap: " << GetLastError() << std::endl;
                return 1;
            }

            // Draw the bitmap to the window
            StretchBlt(hdc, 0, 0, screen_width, screen_height, mem_dc, 0, 0, width, height, SRCCOPY);
        }

        // Release resources
        ReleaseDC(hwnd, hdc);
        DeleteDC(mem_dc);
        DeleteObject(bitmap);
        DestroyWindow(hwnd);
        UnregisterClass(className, hInstance);

        // Clean up Winsock
        WSACleanup();

        return 0;
}
	}



int _tmainWorkss(int argc, _TCHAR* argv[])
{
	int	on= 1;
	printf("                        Anti-Hack Server Loaded by RoiLopez \n");

    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup failed: " << iResult << std::endl;
        return 1;
    }

    // Create a socket for listening
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET || listenSocket < 0 ) {
        std::cout << "Error creating socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

	setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));


    // Bind the socket to an address and port
    sockaddr_in service;
	memset(&service, 0, sizeof(service));// zerochannel 
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = htonl(INADDR_ANY);
    service.sin_port = htons(SERVER_PORT);

    iResult = bind(listenSocket, (SOCKADDR*)&service, sizeof(service));
    if (iResult == SOCKET_ERROR || iResult < 0) {
        std::cout << "Error binding socket: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Start listening for incoming connections
    iResult = listen(listenSocket, QUEUE_SIZE);
    if (iResult == SOCKET_ERROR || iResult < 0) {
        std::cout << "Error listening: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Accept incoming connections and send a heartbeat message
    while (true) {
        //SOCKET clientSocket = accept(listenSocket, NULL, NULL);

		  sockaddr_in clientAddr;
		 int clientAddrLen = sizeof(clientAddr);
		 SOCKET clientSocket = accept(listenSocket, (SOCKADDR*)&clientAddr, &clientAddrLen);

        if (clientSocket == INVALID_SOCKET) {
            std::cout << "Error accepting connection: " << WSAGetLastError() << std::endl;
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }

        // Send a heartbeat message to the client every 5 seconds
        while (true) {
             char* message = "heartbeat";
            iResult = send(clientSocket, message, sizeof(message), 0);



		//--

			//int iResult = send(clientSocket, sendData, sizeof(sendData), 0);
		if (iResult == SOCKET_ERROR) {
		// std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
	    std::cout << "Error sending message: " << WSAGetLastError() << std::endl;
		closesocket( clientSocket );
		break;

		//} else if (iResult != sizeof(message)) {
		//   std::cerr << "Sent " << iResult << " bytes instead of " << sizeof(message) << std::endl;
		//} 
		
		}else {
			//std::cerr << "Sent " << iResult << " bytes instead of " <<message << std::endl;
		 std::cout << "Data sent successfully" << std::endl;
		}





		//	----------------


			

            // Check if the client is still connected
            char buffer[256];
            iResult = recv(clientSocket, buffer, sizeof(buffer), 0);

			if (iResult > 0) {
                std::cout << "Client is still connected." << std::endl;
				std::cerr <<buffer << std::endl;
            }

			
            if(iResult == 0 ||iResult ==  WSAECONNRESET) {
                std::cout << "Client disconnected." << std::endl;
                break;
            }

			Sleep(1000);

            //std::this_thread::sleep_for(std::chrono::seconds(5));
        }

        closesocket(clientSocket);
    }

    // Clean up Winsock
    closesocket(listenSocket);
    WSACleanup();
    return 0;
}


const int HEARTBEAT_TIMEOUT = 5; // in seconds
const int HEARTBEAT_INTERVAL = 5; // in seconds
const int HEARTBEAT_MESSAGE_LENGTH = 4; // in bytes

int _tmainHeartBeat(int argc, _TCHAR* argv[])
{
	int	on= 1;
	printf("                        Anti-Hack Server Loaded by RoiLopez \n");

    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup failed: " << iResult << std::endl;
        return 1;
    }

    // Create a socket for listening
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET || listenSocket < 0 ) {
        std::cout << "Error creating socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

	setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));


    // Bind the socket to an address and port
    sockaddr_in service;
	memset(&service, 0, sizeof(service));// zerochannel 
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = htonl(INADDR_ANY);
    service.sin_port = htons(SERVER_PORT);

    iResult = bind(listenSocket, (SOCKADDR*)&service, sizeof(service));
    if (iResult == SOCKET_ERROR || iResult < 0) {
        std::cout << "Error binding socket: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Start listening for incoming connections
    iResult = listen(listenSocket, QUEUE_SIZE);
    if (iResult == SOCKET_ERROR || iResult < 0) {
        std::cout << "Error listening: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

	
	//time_t currentTime = time(nullptr);
    // Accept incoming connections and send a heartbeat message

    while (true) {

        //SOCKET clientSocket = accept(listenSocket, NULL, NULL);

		  sockaddr_in clientAddr;
		 int clientAddrLen = sizeof(clientAddr);
		 SOCKET clientSocket = accept(listenSocket, (SOCKADDR*)&clientAddr, &clientAddrLen);

        if (clientSocket == INVALID_SOCKET) {
            std::cout << "Error accepting connection: " << WSAGetLastError() << std::endl;
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }

        // Send a heartbeat message to the client every 5 seconds

        while (true) {


            char* message = "heartbeat";
            iResult = send(clientSocket, message, sizeof(message), 0);



		//--

			//int iResult = send(clientSocket, sendData, sizeof(sendData), 0);
		if (iResult == SOCKET_ERROR) {
		// std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
	    std::cout << "Error sending message: " << WSAGetLastError() << std::endl;
		closesocket( clientSocket );
		break;

		//} else if (iResult != sizeof(message)) {
		//   std::cerr << "Sent " << iResult << " bytes instead of " << sizeof(message) << std::endl;
		//} 
		
		}else {
			//std::cerr << "Sent " << iResult << " bytes instead of " <<message << std::endl;
		 std::cout << "Data sent successfully" << std::endl;
		}





		//	----------------


			

            // Check if the client is still connected
			 time_t currentTime = time(NULL);
            char buffer[256];
            iResult = recv(clientSocket, buffer, sizeof(buffer), 0);

			if (iResult > 0) {
                std::cout << "Client is still connected." << std::endl;
				std::cerr <<buffer << std::endl;
            }

			
            if(iResult ==  0) {
                std::cout << "Client disconnected." << std::endl;
                break;
            }

		
			Sleep(1000);
        }

        //closesocket(clientSocket);
    }

    // Clean up Winsock
    closesocket(listenSocket);
    WSACleanup();
    return 0;
}






