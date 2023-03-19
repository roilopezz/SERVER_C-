// GameGuardServer.cpp : Defines the entry point for the application.

#include "stdafx.h"
#include "GameGuardServer.h"
#include <CommCtrl.h>
#include "stdafx.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <ws2spi.h>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <time.h>  // include time.h header

#include "Logs.h"
#include "BanList.h"

// Define the menu items
#define ID_FILE_EXIT 9001
#define ID_HELP_ABOUT 9002
#define ID_GET_PROCESS 9003


#pragma comment(lib, "ws2_32.lib")


extern void GetMACaddress(void);

int checkBan;
int AutoBanTime=2;
int didbanname='1';
int didlogtxt='1';
int didlogonscr='1';
extern int MacProtect;
char ServerVers[3]={0};
char HostNameData[20];
char RemoteIP[17];
char ban_status;
char User_Name[20]={0};
char add_ban;
char BanReason[30]={0};

unsigned char MACData[6];






LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);






// Declare global variables
HWND hProcessListButton;
HWND hUserInput;

// Declare function to handle button click
//void OnProcessListButtonClicked(HWND hwnd) {
void OnProcessListButtonClicked(){
    // Get the text from the user input
    char username[256];
    GetWindowTextA(hUserInput, username, 256);

	MessageBoxA(NULL,username,username,MB_OK);


    // Send a message to the client to request the process list for the specified user
    // Code to send message to client goes here
}

// Declare function to create the server window




DWORD WINAPI MainGameGuard(LPVOID lpParam)
{
	/*
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed with error: " << result << std::endl;
        return 1;
    }
	*/


    // Create two threads to run the heartbeat server and the other server concurrently
    HANDLE heartbeatThread = CreateThread(NULL, 0, runHeartbeatServer, NULL, 0, NULL);
    HANDLE MainServerThread = CreateThread(NULL, 0, MainServer, NULL, 0, NULL);

    
	//();
	
	//HANDLE otherThread = CreateThread(NULL, 0, runOtherServer, NULL, 0, NULL);

    // Wait for the threads to finish before exiting the program
    WaitForSingleObject(heartbeatThread, INFINITE);
    WaitForSingleObject(MainServerThread, INFINITE);

   // WaitForSingleObject(otherThread, INFINITE);

    // Close the thread handles
    CloseHandle(heartbeatThread);
    CloseHandle(MainServerThread);

    // Clean up Winsock
    WSACleanup();

    return 0;
}

void OnAddLine(HWND hwndLV, const wchar_t* text)
{
    LVITEM lvItem = { 0 };
    lvItem.mask = LVIF_TEXT;
    lvItem.pszText = const_cast<wchar_t*>(text);

    int index = ListView_InsertItem(hwndLV, &lvItem);

    if (index != -1)
    {
        ListView_EnsureVisible(hwndLV, index, FALSE);

        // Check if a new line has been added
        int numLinesBefore = ListView_GetItemCount(hwndLV);
        int numLinesAfter = numLinesBefore + 1;
        if (numLinesAfter > numLinesBefore)
        {
            ListView_Scroll(hwndLV, 0, ListView_GetItemCount(hwndLV));
        }
    }
}


BOOL CreateServerWindow(HINSTANCE hInstance) {
    // Register window class
    WNDCLASS wc = {0};
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszClassName = TEXT("ServerWindowClass");
    if (!RegisterClass(&wc)) {
        return FALSE;
    }

    // Create window
    HWND hwnd = CreateWindow(
        TEXT("ServerWindowClass"),
        TEXT("Server"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 200,
        NULL, NULL, hInstance, NULL
    );
    if (!hwnd) {
        return FALSE;
    }

    // Create user input field
    hUserInput = CreateWindow(
        TEXT("EDIT"),
        TEXT(""),
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        10, 10, 200, 20,
        hwnd, NULL, hInstance, NULL
    );
    if (!hUserInput) {
        return FALSE;
    }

    // Create process list button
    hProcessListButton = CreateWindow(
        TEXT("BUTTON"),
        TEXT("Get Process List"),
        WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
        10, 40, 150, 25,
        hwnd, (HMENU)ID_GET_PROCESS, hInstance, NULL
    );
    if (!hProcessListButton) {
        return FALSE;
    }

    // Show window
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);




   return TRUE;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	CreateThread(NULL, 0, MainGameGuard,NULL, 0, NULL);
	//DWORD MyId;
	//CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)MainGameGuard,NULL,0,&MyId);



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
        L"GameGuard",
        WS_OVERLAPPEDWINDOW,
        //CW_USEDEFAULT, CW_USEDEFAULT, 800, 400,
        CW_USEDEFAULT, CW_USEDEFAULT, 810, 400,

        NULL, NULL, hInstance, NULL
    );


    if (hwnd == NULL)
    {
        return 0;
    }



	CreateServerWindow(hInstance);

	titleGameGuard(hInstance,hwnd,nCmdShow);

	TableTaskProcess(hInstance,hwnd,nCmdShow);
	//TableUsers(hInstance,hwnd , nCmdShow);


  // Clear the table
	//SendMessage(hTable, LVM_DELETEALLITEMS, 0, 0);




    // Show the window
    ShowWindow(hwnd, nCmdShow);

    // Run the message loop
    MSG msg = { };


    while (GetMessage(&msg, NULL, 0, 0))
    {
	  

		// Ensure that the last item is visible

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }


    return (int)msg.wParam;

    //return 0;
	}

	
	



LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
HFONT hFont = CreateFont(23, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                         CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Tahoma");

int lineHeight = 16; // Change this value to set the height of each line of text
int numVisibleLines = 10; // Change this value to set the number of visible lines in the list view
int clientHeight = numVisibleLines * lineHeight;
int prevScrollPos = 0;

    switch (uMsg)
    {

		// Change Color

/*
    case WM_CTLCOLORSTATIC:
    {
        HDC hdcStatic = (HDC)wParam;
       // SetBkColor(hdcStatic, RGB(255, 255, 255)); // Set the background color of the text
        SetTextColor(hdcStatic, RGB(252, 0, 0)); // Set the text color
        SelectObject(hdcStatic, hFont); // Select the font to use
        return (INT_PTR)hBrush;
    }


	
	case WM_ERASEBKGND:
	{
		MessageBox(hwnd, L"WM_ERASEBKGND received", L"Debug", MB_OK);
		HDC hdc = reinterpret_cast<HDC>(wParam);
		RECT rect;
		GetClientRect(hwnd, &rect);
		HBRUSH hBrush = CreateSolidBrush(RGB(24, 0, 0));
		FillRect(hdc, &rect, hBrush);
		DeleteObject(hBrush);
		return TRUE;
	}
	*/


    case WM_CTLCOLORSTATIC:
    {
        HDC hdcStatic = (HDC)wParam;
       // SetBkColor(hdcStatic, RGB(255, 255, 255)); // Set the background color of the text
        SetTextColor(hdcStatic, RGB(4, 161, 255)); // Set the text color
        SelectObject(hdcStatic, hFont); // Select the font to use
        return (INT_PTR)hBrush;
    }


		case WM_CREATE:
		{
			// Create the menu
			HMENU hMenu = CreateMenu();
			HMENU hFileMenu = CreatePopupMenu();
			HMENU hHelpMenu = CreatePopupMenu();

			// Add items to the file menu
			AppendMenu(hFileMenu, MF_STRING, ID_FILE_EXIT, L"Exit");

			// Add items to the help menu
			AppendMenu(hHelpMenu, MF_STRING, ID_HELP_ABOUT, L"About");

			// Add the submenus to the main menu
			AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");
			AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, L"Help");

			// Set the menu for the window
			SetMenu(hwnd, hMenu);

			break;
		}

		// In your window's message loop, handle the WM_COMMAND message like this:
		case WM_COMMAND:
		{


			if (LOWORD(wParam) == ID_GET_PROCESS && HIWORD(wParam) == BN_CLICKED) {
                // Call the function to handle button click
			//MessageBoxA(NULL,"דגכדג","כדגכדכ",MB_OK);
				 MessageBox(hwnd, L"The button was clicked!", L"Button clicked", MB_OK);
               // OnProcessListButtonClicked();
            }
            break;

			switch (LOWORD(wParam))
			{

				//OnProcessListButtonClicked





				case ID_FILE_EXIT:
					// Handle the "Exit" menu item
					DestroyWindow(hwnd);
					break;

				case ID_HELP_ABOUT:
					// Handle the "About" menu item
					MessageBox(hwnd, L"GameGuard by RoiLopez", L"GameGuard", MB_OK | MB_ICONINFORMATION);
					break;

				default:
					break;
			}

			break;
		}



			// Scroll Bar
			 case WM_VSCROLL:
        switch (LOWORD(wParam))
		  {
            case SB_LINEUP:
                // Scroll up one line
                SetScrollPos(hwnd, SB_VERT, GetScrollPos(hwnd, SB_VERT) - 1, TRUE);
                ScrollWindowEx(hwnd, 0, -lineHeight, NULL, NULL, NULL, NULL, SW_INVALIDATE);
                break;

            case SB_LINEDOWN:
                // Scroll down one line
                SetScrollPos(hwnd, SB_VERT, GetScrollPos(hwnd, SB_VERT) + 1, TRUE);
                ScrollWindowEx(hwnd, 0, lineHeight, NULL, NULL, NULL, NULL, SW_INVALIDATE);
                break;




            case SB_PAGEUP:
                // Scroll up one page
                SetScrollPos(hwnd, SB_VERT, GetScrollPos(hwnd, SB_VERT) - numVisibleLines, TRUE);
                ScrollWindowEx(hwnd, 0, -clientHeight, NULL, NULL, NULL, NULL, SW_INVALIDATE);
                break;

            case SB_PAGEDOWN:
                // Scroll down one page
                SetScrollPos(hwnd, SB_VERT, GetScrollPos(hwnd, SB_VERT) + numVisibleLines, TRUE);
                ScrollWindowEx(hwnd, 0, clientHeight, NULL, NULL, NULL, NULL, SW_INVALIDATE);
                break;



            case SB_THUMBPOSITION:
            case SB_THUMBTRACK:
                // Scroll to the specified position
                int scrollPos = HIWORD(wParam);
                SetScrollPos(hwnd, SB_VERT, scrollPos, TRUE);
                ScrollWindowEx(hwnd, 0, (prevScrollPos - scrollPos) * lineHeight, NULL, NULL, NULL, NULL, SW_INVALIDATE);
                prevScrollPos = scrollPos;
                break;
        }

		/*
        case WM_SIZE:
        {
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);


			  // Resize child windows
            HWND hChildWnd = GetWindow(hwnd, GW_CHILD);
            while (hChildWnd)
            {
                MoveWindow(hChildWnd, 0, 0, width, height, TRUE);
                hChildWnd = GetWindow(hChildWnd, GW_HWNDNEXT);
            }

            // כאן יש להתאים את גודל האלמנט לגודל החדש של החלון
          //  SetWindowPos(hTable, NULL, 0, 15, width, height, SWP_NOZORDER);

            break;
        }

		*/




    case WM_DESTROY:
    {
        DeleteObject(hBrush);
        DeleteObject(hFont);
        PostQuitMessage(0);
        break;
    }



        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}
