#include "stdafx.h"
#include <CommCtrl.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <ws2spi.h>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <time.h>  // include time.h header
#include "GameGuardServer.h"
#include "Globals.h"
#include "CheckConnection.h"


//ClientInfo ClientInfoOO;
extern std::vector<ClientInfo> clients;
//std::vector<ClientInfo> clients;
//ClientInfo ClientInfoOO;

extern unsigned char MACData[6];

extern char g_uuid[37];
LPCWSTR columnHeaders[] = {
		L"ID",
		L"IP",
		L"Hardware ID",
		L"Status",
		L"Time"

};


void TableUsers(HINSTANCE hInstance,HWND hwnd , int nCmdShow){    
	hTable = CreateWindowEx(
        0,
        WC_LISTVIEW,
        L"",
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | LVS_REPORT | LVS_SINGLESEL,
       // 0, 100, 800, 360,
       // 0, 100, 800, 260,
	    0, 105, 800, 260,
        hwnd,
        (HMENU)ID_TABLE,
        hInstance,
        NULL
    );



    if (hTable == NULL || !IsWindow(hTable)) {
        MessageBoxA(NULL, "Invalid table handle from tableeee", "Error", MB_OK | MB_ICONERROR);
        return;
    }



    // Set the column headers
    LVCOLUMN lvColumn = { };
    lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
    lvColumn.cx = 200;

    for (int i = 0; i < 5; i++)
    {
        lvColumn.pszText = (LPWSTR)columnHeaders[i];
        SendMessage(hTable, LVM_INSERTCOLUMN, i, (LPARAM)&lvColumn);
    }

    // Add some items to the table
    LVITEM lvItem = { };
    lvItem.mask = LVIF_TEXT;





	 //ShowWindow(hwnd, SW_SHOW);





}


void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int getSizeClients(int size){

	return 1;
}

void OnClientConnect(SOCKET clientSocket, sockaddr_in clientAddr , char* ban)
{
    char RemoteIP[128];
    inet_ntop(clientAddr.sin_family, get_in_addr((struct sockaddr*)&clientAddr), RemoteIP, sizeof RemoteIP);

    // Add new IP address to the ipAddresses vector
    ipAddresses.push_back(RemoteIP);
	
	// Add client socket to the vector
    clientSockets.push_back(clientSocket);
	

    // Update the list view control with the new IP address
    LVITEM lvItem;
    lvItem.mask = LVIF_TEXT;
    lvItem.iItem = ipAddresses.size() - 1;


	char sesId[37];

	//UUID sesId;
	//char tt[37];

	for (int i = 0; i < clients.size(); i++) {
			ClientInfo& client = clients[i];
			if (client.socket == clientSocket) {
				//c_str();

				//sesId = client.sessionID; 

				strcpy(sesId, client.sessionID.c_str());

				MessageBoxA(NULL,sesId,sesId,MB_OK);
				break;
			}
		}
	



	lvItem.iSubItem = 0;
	wchar_t IDDdd[256];
	MultiByteToWideChar(CP_ACP, 0, sesId, -1, IDDdd, 256);
	lvItem.pszText = IDDdd;
	SendMessage(hTable, LVM_INSERTITEM, 0, (LPARAM)&lvItem);


    // Convert narrow character string to wide character string
	lvItem.iSubItem = 1;
    wchar_t ipAddressW[256];
    MultiByteToWideChar(CP_ACP, 0, RemoteIP, -1, ipAddressW, 256);
    lvItem.pszText = ipAddressW;
    //SendMessage(hTable, LVM_SETITEM, 0, (LPARAM)&lvItem);
    SendMessage(hTable, LVM_SETITEM, 0, (LPARAM)&lvItem);

	char macc[256];
	sprintf(macc,"%02X-%02X-%02X-%02X-%02X-%02X",MACData[0], MACData[1], MACData[2], MACData[3], MACData[4], MACData[5]);

    lvItem.iSubItem = 2;
	wchar_t ipAddressW2[256];
    MultiByteToWideChar(CP_ACP, 0, macc, -1, ipAddressW2, 256);
    lvItem.pszText = ipAddressW2;
    SendMessage(hTable, LVM_SETITEM, 0, (LPARAM)&lvItem);


    lvItem.iSubItem = 3;
	wchar_t ipAddressW3[256];
    MultiByteToWideChar(CP_ACP, 0, ban, -1, ipAddressW3, 256);
    lvItem.pszText = ipAddressW3;
    SendMessage(hTable, LVM_SETITEM, 0, (LPARAM)&lvItem);


	SYSTEMTIME st;
	GetLocalTime(&st);
	char timeNow[70];
	sprintf(timeNow,"Date: %02d/%02d/%02d, Time: %02d:%02d:%02d",st.wDay , st.wMonth , st.wYear,st.wHour, st.wMinute, st.wSecond);
	//Date: %02d/%02d/%02d, Time: %02d:%02d:%02d st.wDay , st.wMonth , st.wYear,st.wHour, st.wMinute, st.wSecond


    lvItem.iSubItem = 4;
	wchar_t ipAddressW4[256];
    MultiByteToWideChar(CP_ACP, 0, timeNow, -1, ipAddressW4, 256);
    lvItem.pszText = ipAddressW4;
    SendMessage(hTable, LVM_SETITEM, 0, (LPARAM)&lvItem);

}

int GetItemIndexByIP(LPCWSTR ipAddress)
{
    if (hTable == NULL || !IsWindow(hTable)) {
       // MessageBoxA(NULL, "Invalid table handle", "Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    int itemCount = ListView_GetItemCount(hTable);
    for (int i = 0; i < itemCount; i++) {
        wchar_t ipAddressW[256];
        LVITEM lvItem;
        lvItem.mask = LVIF_TEXT;
        lvItem.iItem = i;
        lvItem.iSubItem = 1;
        lvItem.pszText = ipAddressW;
        lvItem.cchTextMax = 256;
        SendMessage(hTable, LVM_GETITEMTEXT, i, (LPARAM)&lvItem);

        if (lstrcmpW(ipAddressW, ipAddress) == 0) {
            return i;
        }
    }

    //MessageBoxA(NULL, "Item not found", "Error", MB_OK | MB_ICONERROR);
    return -1;
}


void DeleteItemByIP(LPCWSTR ipAddress) {
    if (hTable == NULL || !IsWindow(hTable)) {
        //MessageBoxA(NULL, "Invalid table handle", "Error", MB_OK | MB_ICONERROR);
        return;
    }

   // int index = SendMessage(hTable, LVM_FINDITEM, -1, (LPARAM)&lvFindInfo);
	int index = GetItemIndexByIP(ipAddress);
    if (index == -1) {
      //  MessageBoxA(NULL, "Item not found", "Error", MB_OK | MB_ICONERROR);
        return;
    }
    
    DWORD result = SendMessageTimeout(hTable, LVM_DELETEITEM, index, 0, SMTO_ABORTIFHUNG, 5000, NULL);
    if (result == 0) {
        DWORD errorCode = GetLastError();
        std::wstring errorMessage = L"Failed to delete item with error code: " + std::to_wstring(static_cast<long long>(errorCode));
        MessageBoxW(NULL, errorMessage.c_str(), L"Error", MB_OK | MB_ICONERROR);
    } else {
		ipAddresses.erase(ipAddresses.begin() + index);
       // MessageBoxA(NULL, "Item deleted successfully", "Success", MB_OK);
    }
}
