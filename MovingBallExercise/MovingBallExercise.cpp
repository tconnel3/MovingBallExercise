// MovingBallExercise.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "MovingBallExercise.h"
#include "wingdi.h"
#include <vector>
#include <thread>

#define MAX_LOADSTRING 100
#define BTN_PLAY 101                            //Play button definition
#define BTN_PAUSE 102                           //Pause button definition

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
BOOL animPause = false;                         // A boolean tracking whether or not the animation should be paused
BOOL termFlag = false;                          // A boolean to see if the thread should be terminated
HANDLE wThread;                                 // The thread being declared globally
DWORD threadID;                                 // The thread ID for creating a thread using win32


// Ball coordinates struct
struct BallCoord {
    int left;
    int top;
    int right;
    int bottom;
};

BallCoord make_coord(const int& left, const int& top, const int& right, const int& bottom)
{
    BallCoord bcor;
    bcor.left = left;
    bcor.top = top;
    bcor.right = right;
    bcor.bottom = bottom;

    return bcor;

}

std::vector<BallCoord> ballPositions;    // the vector storing the future positions of the ball

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MOVINGBALLEXERCISE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MOVINGBALLEXERCISE));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MOVINGBALLEXERCISE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MOVINGBALLEXERCISE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}



//   FUNCTION: CalcPositions()
//
//   PURPOSE: Calculate and store future ball positions

DWORD WINAPI CalcPositions(LPVOID lpParam)
{

    //These four variables define the ball's start postion
    //They'll be updated as the loop iterates
    int left = 15;
    int top = 15;
    int right = 30;
    int bottom = 30;

    //Pushing back the initial coordinates
    ballPositions.push_back(make_coord(left, top, right ,bottom));


    //The distance the ball moves each loop iteration
    int yDist = 3;
    int xDist = 3;

    while (1)
    {
        if (termFlag == TRUE)
        {
            ExitThread(threadID); //ExitThread is used because of the thread handle
        }

        //If the ball 'touches' one of the vertical bounds, reverse the vertical trajectory
        if ((top <= 0 && yDist < 0)|| (bottom >= 300 && yDist > 0))
        {
            yDist = yDist * -1;
        }

        //If the ball 'touches' one of the horizontal bounds, reverse the horizontal trajectory
        if ((left <= 0 && xDist < 0)|| (right >= 400 && xDist > 0))
        {
            xDist = xDist * -1; 
        }

        //ballPositions is a vector which stores the next 5 positions
        //ballPosition[0] is the next position
        if (ballPositions.size() < 5)
        {
            left = left + xDist; //Update the left position
            top = top + yDist; //Update the top position
            right = right + xDist; //Update the right position
            bottom = bottom + yDist; //Update the bottom position
            ballPositions.push_back(make_coord(left, top, right, bottom)); //Push back the coordinate struct
        }
    }
    return 0;
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
       CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);


   //Create the 'play' button
   CreateWindowEx(
       0,
       L"Button",
       L"Play",
       WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
       50,
       400,
       100,
       100,
       hWnd,
       (HMENU)BTN_PLAY,
       GetModuleHandle(NULL),
       NULL);

   //Create the 'pause' button
   CreateWindowEx(
       0,
       L"Button",
       L"Pause",
       WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
       160,
       400,
       100,
       100,
       hWnd,
       (HMENU)BTN_PAUSE,
       GetModuleHandle(NULL),
       NULL);

   //RECT rect;
   //GetWindowRect(hWnd, &rect);

   //Set a timer that updates every 1 second
   SetTimer(hWnd, 1, 33, NULL);


   //Create the thread
   wThread = CreateThread(NULL, 0, CalcPositions, NULL, 0, &threadID);
  
   

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu and buttons
//  WM_TIMER    - erase previous painted shape
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                
                break;
            case IDM_EXIT:
                termFlag = true; //On exit, set termFlag to true so the thread will exit and join the main thread
                DestroyWindow(hWnd);
                break;
            case BTN_PLAY:
                animPause = false;
                break;
            case BTN_PAUSE:
                animPause = true;
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    case WM_TIMER:
        {
            if (!animPause)
            {
                InvalidateRect(hWnd, NULL, TRUE);
            }   
            
        }
        
    case WM_PAINT:
        {
            if (!animPause)
            {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            SelectObject(ps.hdc, GetStockObject(BLACK_BRUSH)); //Set the brush to black so the shapes is filled
            Rectangle(ps.hdc, 0, 300, 400, 305); //Paint the bottom rail
            Rectangle(ps.hdc, 400, 0, 405, 305); //Paint the right rail
            Ellipse(ps.hdc, ballPositions[0].left, ballPositions[0].top, ballPositions[0].right, ballPositions[0].bottom); //paint the ball using the coordinates from ballPositions

            ballPositions.erase(ballPositions.begin()); //Remove the first element from ballPositions
            EndPaint(hWnd, &ps);
            }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
