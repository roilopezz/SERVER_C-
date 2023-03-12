// GameGuardServer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "GameGuardServer.h"
#include <CommCtrl.h>
//#include "IP_SERVER.h"




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

	#define SERVER_PORT 55445
	#define HEARTBeat_PORT 55446

	#define BUF_SIZE 4096  // block transfer size  
	#define QUEUE_SIZE 100 
	extern void ReadInis ( void );
	extern void GetMACaddress(void);
	extern void InitLogFile (void);
	extern void PrintLog( char HostNameData[20] ,	char RemoteIP[17] , unsigned char MACData[6] , char User_Name[20] , char ban_status , char add_ban ) ;
	extern void Print_Ban( char HostNameData[20] ,	char RemoteIP[17] , unsigned char MACData[6] , char User_Name[20] , char ban_status , char add_ban , char BanReason[30] ) ;
	extern void Print_Ban_Logs( char HostNameData[20] ,	char RemoteIP[17] , unsigned char MACData[6] , char User_Name[20] , char ban_status , char add_ban , char BanReason[30] ) ;
	extern char Check_Ban( char HostNameData[20] ,	char RemoteIP[17] , unsigned char MACData[6] , char User_Name[20] , char ban_status , char add_ban ) ;
	extern int checkBan;
	int AutoBanTime=2;
	int didbanname='1';
	int didlogtxt='1';
	int didlogonscr='1';
	extern int MacProtect;
	char ServerVers[3]={0};

	unsigned char MACData[6];
	char HostNameData[20];
	char RemoteIP[17];
	char ban_status;
	char User_Name[20]={0};
	char add_ban;
	char BanReason[30]={0};



	std::vector<std::string> ipAddresses;
	std::vector<std::string> users;
	std::vector<std::string> hardwareIds;



int ServerOne_tmain()
{
	int		b, l, on = 1;
	char	recvbuf[100], banforever='0'; 
	char	buf[100];
	SOCKET	s, sa;
	struct	sockaddr_in channel;  // holds IP address
	WORD	wVersionRequested;
	WSADATA wsaData;
	int		err;
	int		bytesRecv;

	/*
	unsigned char MACData[6];
	char HostNameData[20];
	char RemoteIP[17];
	char ban_status;
	char User_Name[20]={0};
	char add_ban;
	char BanReason[30]={0};
	*/

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
	printf(" [ == Users Blocked == ] \n");
	printf(" \n");


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
	//sa = accept(s, 0 , 0);                  // block for connection request  
	int clientAddrLen = sizeof(channel);
	sa = accept(s, (SOCKADDR*)&channel, &clientAddrLen);

	if (sa < 0 || sa == INVALID_SOCKET) {
		printf("accept error %ld ", WSAGetLastError() ); 
		WSACleanup();
		//return false;
		continue;
	}



	//------------------------------------------------------------
	// Socket is now set up and bound. Wait for connection and process it. 
	
	//---- RECV bytes --------------------------------------------
	bytesRecv = recv( sa, recvbuf, sizeof(recvbuf), 0 );

	//err = WSAGetLastError( );// 10057 = A request to send or receive data was disallowed because the socket is not connected and (when sending on a datagram socket using a sendto call) 
	if ( bytesRecv == 0 || bytesRecv == WSAECONNRESET  || bytesRecv == INVALID_SOCKET) {
      printf( "Connection Closed.\n");
	  WSACleanup();
	  return false;

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



	// Get the client IP address
	sockaddr_in saClient;
	int nLength = sizeof(saClient);
	getpeername(sa, (sockaddr*)&saClient, &nLength);
	char szIPAddress[16];
	inet_ntop(AF_INET, &saClient.sin_addr, szIPAddress, sizeof(szIPAddress));

	// Get the user and hardware ID
	char szUser[256] = "User1";
	char szHardwareID[256] = "HWID1";

	// Add the data to the vectors
	ipAddresses.push_back(szIPAddress);
	users.push_back(szUser);
	hardwareIds.push_back(szHardwareID);



	//if(didlogonscr==1 || add_ban=='1'){
	if(add_ban=='1' || Check_Ban(HostNameData , RemoteIP , MACData , User_Name , ban_status , add_ban) ==  '1'){


		if(Check_Ban(HostNameData , RemoteIP , MACData , User_Name , ban_status , add_ban) ==  '1'){
		printf(" -User Blocking : (%s), Date: %02d/%02d/%02d, Time: %02d:%02d:%02d, \n",RemoteIP, st.wDay , st.wMonth , st.wYear,st.wHour, st.wMinute, st.wSecond); //, //BanReason//ban_status)
		
		}else if(bytesRecv > 0){
		printf(" -System Block User : (%s), Date: %02d/%02d/%02d, Time: %02d:%02d:%02d, \n",RemoteIP, st.wDay , st.wMonth , st.wYear,st.wHour, st.wMinute, st.wSecond); //, //BanReason//ban_status)
		}


	}else{
	//printf(" -User connected : (%s), Date: %02d/%02d/%02d, Time: %02d:%02d:%02d, \n",RemoteIP, st.wDay , st.wMonth , st.wYear,st.wHour, st.wMinute, st.wSecond); //, //BanReason//ban_status)	
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


DWORD WINAPI runHeartbeatServer(LPVOID lpParam)
{
    // Create a socket for the heartbeat server
    SOCKET heartbeatSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (heartbeatSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket with error: " << WSAGetLastError() << std::endl;
        return 1;
    }

    // Bind the socket to a local address and port for the heartbeat server
    sockaddr_in heartbeatAddress;
    heartbeatAddress.sin_family = AF_INET;
    heartbeatAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    heartbeatAddress.sin_port = htons(HEARTBeat_PORT);


    int result = bind(heartbeatSocket, (SOCKADDR*)&heartbeatAddress, sizeof(heartbeatAddress));

    if (result == SOCKET_ERROR) {
        std::cerr << "Failed to bind socket with error: " << WSAGetLastError() << std::endl;
        closesocket(heartbeatSocket);
        return 1;
    }

    // Listen for incoming connections for the heartbeat server
    result = listen(heartbeatSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        std::cerr << "Failed to listen for incoming connections with error: " << WSAGetLastError() << std::endl;
        closesocket(heartbeatSocket);
        return 1;
    }

    //std::cout << "Heartbeat server is listening for incoming connections..." << std::endl;

    // Accept incoming connections and send heartbeat messages
    while (true) {
        SOCKADDR_IN clientAddress;
        int clientAddressSize = sizeof(clientAddress);
        SOCKET clientSocket = accept(heartbeatSocket, (SOCKADDR*)&clientAddress, &clientAddressSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Failed to accept incoming connection with error: " << WSAGetLastError() << std::endl;
            closesocket(heartbeatSocket);
            return 1;
        }

        //std::cout << "Heartbeat server accepted incoming connection from " << inet_ntoa(clientAddress.sin_addr) << std::endl;

        // Send heartbeat messages to the client
        const char* heartbeatMessage = "heartbeat";
        while (true) {
            result = send(clientSocket, heartbeatMessage, strlen(heartbeatMessage), 0);
            if (result == SOCKET_ERROR) {
           //     std::cerr << "Failed to send heartbeat message with error: " << WSAGetLastError() << std::endl;
			closesocket(clientSocket);
			break;
			}
	//std::cout << "Heartbeat message sent to " << inet_ntoa(clientAddress.sin_addr) << std::endl;
	Sleep(1000); // Wait for 1 second before sending the next heartbeat message
	}

    closesocket(clientSocket);
}

// Close the heartbeat server socket
closesocket(heartbeatSocket);

return 0;
}




DWORD WINAPI RoieLopezzzzz(LPVOID lpParam)
{
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed with error: " << result << std::endl;
        return 1;
    }

    // Create two threads to run the heartbeat server and the other server concurrently
    HANDLE heartbeatThread = CreateThread(NULL, 0, runHeartbeatServer, NULL, 0, NULL);
    
	
	ServerOne_tmain();
	
	//HANDLE otherThread = CreateThread(NULL, 0, runOtherServer, NULL, 0, NULL);

    // Wait for the threads to finish before exiting the program
    WaitForSingleObject(heartbeatThread, INFINITE);
   // WaitForSingleObject(otherThread, INFINITE);

    // Close the thread handles
    CloseHandle(heartbeatThread);
    //CloseHandle(otherThread);

    // Clean up Winsock
    WSACleanup();

    return 0;
}











// Define the table control ID
#define ID_TABLE 1001

// Define the column headers
LPCWSTR columnHeaders[] = {
    L"IP",
    L"Hardware ID",
    L"Status"
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{

		//();
	CreateThread(NULL, 0, RoieLopezzzzz, (LPVOID)1, 0, NULL);


    // Register the window class
    const wchar_t CLASS_NAME[]  = L"MyWindowClass";
    
    WNDCLASS wc = { };

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window
    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"GameGuardServer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 500,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL)
    {
        return 0;
    }



    // Create the table control
    HWND hTable = CreateWindowEx(
        0,
        WC_LISTVIEW,
        L"",
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
        10, 10, 470, 450,
        hwnd,
        (HMENU)ID_TABLE,
        hInstance,
        NULL
    );

    // Set the column headers
    LVCOLUMN lvColumn = { };
    lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
    lvColumn.cx = 100;

    for (int i = 0; i < 3; i++)
    {
        lvColumn.pszText = (LPWSTR)columnHeaders[i];
        SendMessage(hTable, LVM_INSERTCOLUMN, i, (LPARAM)&lvColumn);
    }

    // Add some items to the table
    LVITEM lvItem = { };
    lvItem.mask = LVIF_TEXT;

  // Clear the table
SendMessage(hTable, LVM_DELETEALLITEMS, 0, 0);

// Add the data to the table
//for (int i = 0; i < ipAddresses.size(); i++)

for (int i = 0; i < ipAddresses.size(); i++)

{
    LVITEM lvItem;
    lvItem.mask = LVIF_TEXT;
    lvItem.iItem = i;
    lvItem.iSubItem = 0;

    // Convert narrow character string to wide character string
    wchar_t ipAddressW[256];
    MultiByteToWideChar(CP_ACP, 0, ipAddresses[i].c_str(), -1, ipAddressW, 256);
    lvItem.pszText = ipAddressW;

    SendMessage(hTable, LVM_INSERTITEM, 0, (LPARAM)&lvItem);

    lvItem.iSubItem = 1;

    // Convert narrow character string to wide character string
    wchar_t userW[256];
    MultiByteToWideChar(CP_ACP, 0, users[i].c_str(), -1, userW, 256);
    lvItem.pszText = userW;

    SendMessage(hTable, LVM_SETITEM, 0, (LPARAM)&lvItem);

    lvItem.iSubItem = 2;

    // Convert narrow character string to wide character string
    wchar_t hardwareIdW[256];
    MultiByteToWideChar(CP_ACP, 0, hardwareIds[i].c_str(), -1, hardwareIdW, 256);
    lvItem.pszText = hardwareIdW;

    SendMessage(hTable, LVM_SETITEM, 0, (LPARAM)&lvItem);
}

    // Show the window
    ShowWindow(hwnd, nCmdShow);

    // Run the message loop
    MSG msg = { };

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }




    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}
