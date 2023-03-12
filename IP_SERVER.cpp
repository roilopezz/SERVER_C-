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



	static std::vector<std::string> ipAddresses;
	static std::vector<std::string> users;
	static std::vector<std::string> hardwareIds;


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



	// Get the user and hardware ID
	char szUser[256] = "User1";
	char szHardwareID[256] = "HWID1";


	// Add the data to the vectors
	ipAddresses.push_back(string(RemoteIP));



	users.push_back(szUser);
	hardwareIds.push_back((char*)MACData);



	MessageBoxA(NULL,RemoteIP,RemoteIP,MB_OK);


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


	//MessageBoxA(NULL,"DEBUG TEST 5",Anti_Hack_Title,MB_OK);

// Add the data to the table
for (int i = 0; i < ipAddresses.size(); i++){
	MessageBoxA(NULL,"DEBUG TEST 5","hh",MB_OK);

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
