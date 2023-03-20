#include "stdafx.h"
#include "GameGuardServer.h"
#include <CommCtrl.h>
#include <string>
#include <Winsock2.h>
#include <Ws2tcpip.h>  // for inet_ntop

#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

//int sessionIDCounter = 1; // start with session ID 1 and increment for each new connection
static char uniid;
#define MAX_BUFF_SIZE 1024
SOCKET clientSocket;

std::vector<ClientInfo> clients;

char* heartbeatMessage = "heartbeat";

// A struct to hold information about a connected client


// A vector to store information about all connected clients



void SendDataToClient(char* sessionID) {
		 char recvBuf[1024];
		 int recvBufLen = sizeof(recvBuf);
		for (int i = 0; i < clients.size(); i++) {
		 ClientInfo& client = clients[i];
		
			  if (client.sessionID.compare(sessionID) == 0){
		// if (client.sessionID == sessionID){
				
				// Change the value for take the process List from the user
		 heartbeatMessage = "REQ_PROCESS_LIST";

		
		// Receive the task manager data from the client
        int bytesReceived = recv(client.socket, recvBuf, recvBufLen, 0);
        if (bytesReceived == SOCKET_ERROR) {
            // Handle error
        } else if (bytesReceived == 0) {
            // Connection closed
            break;
        } else {

			
		// Send	recvBuf to the function
			OnTastManager(recvBuf);
        }

			  }
		 
		 else{
			 break;
				 //MessageBox(NULL, TEXT("nottttt  FOUNDDD"), TEXT("nottttt FOUNDDD"), MB_OK);
			  }
			  
	}
}


DWORD WINAPI runHeartbeatServer(LPVOID lpParam)
{
	char recvBuf[1024];
    int recvBufLen = sizeof(recvBuf);
	WSADATA wsaData;
    int result2 = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result2 != 0) {
        std::cerr << "WSAStartup failed with error: " << result2 << std::endl;
        return 1;
    }

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
        clientSocket = accept(heartbeatSocket, (SOCKADDR*)&clientAddress, &clientAddressSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Failed to accept incoming connection with error: " << WSAGetLastError() << std::endl;
			MessageBoxA(NULL,"Failed to accept incoming connection with error","Failed to accept incoming connection with error",MB_OK);
            
			closesocket(heartbeatSocket);
            return 1;
        }


    UUID uuid;
    UuidCreate(&uuid);

    unsigned char* str;
    UuidToStringA(&uuid, &str);
   // printf("UUID: %s\n", str);



	char tt[37];
	sprintf(tt,"%s",str);

	clients.push_back(ClientInfo(clientSocket, tt));


	//MessageBoxA(NULL,tt,tt,MB_OK);
	//sessionIDCounter++; // increment session ID counter for next connection



        //std::cout << "Heartbeat server accepted incoming connection from " << inet_ntoa(clientAddress.sin_addr) << std::endl;

        // Send heartbeat messages to the client
      //  const char* heartbeatMessage = "heartbeat";
        while (true) {
            result = send(clientSocket, heartbeatMessage, strlen(heartbeatMessage), 0);
            if (result == SOCKET_ERROR) {

			//char RemoteIP[128];
			//inet_ntop(heartbeatAddress.sin_family, get_in_addr((struct sockaddr*)&heartbeatAddress), RemoteIP, sizeof RemoteIP);

			char clientIP[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &(clientAddress.sin_addr), clientIP, INET_ADDRSTRLEN);



			wchar_t ipAddressW[256];
			MultiByteToWideChar(CP_ACP, 0, clientIP, -1, ipAddressW, 256);
			DeleteItemByIP(ipAddressW);


			//MessageBoxA(NULL,clientIP,"Client disconnected",MB_OK);


			//SendMessage(hTable, LVM_DELETEALLITEMS, 0, 0);
			//MessageBoxA(NULL,"Client disconnected","Client disconnected",MB_OK);
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
