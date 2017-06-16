/*
    Beginning DirectX 11 Game Programming
    By Allen Sherrod and Wendy Jones
*/

//#define _XM_NO_INTRINSICS_

#include<Windows.h>
#include<memory>
#include"ModelsDemo.h"
#include"VideoPlayer.h"
#include <dshow.h>

/////////////////////movie player globals////////////////

VideoPlayer     *g_pPlayer = NULL;                  // Global player object. 
BOOL        g_bRepaintClient = TRUE;            // Repaint the application client area?

//////////////time globals///////////////////
double countsPerSecond = 0.0;
__int64 CounterStart = 0;

int frameCount = 0;
int fps = 0;

__int64 frameTimeOld = 0;
double frameTime;
////////////////////////////////////////////////

void StartTimer();
double GetTime();
double GetFrameTime();

////////////////////////////////////////////
void PlayMovie(void);

//////////////////////////////////media play movie declarations//////////////////////////
INT_PTR CALLBACK    OpenUrlDialogProc(HWND, UINT, WPARAM, LPARAM);
void                NotifyError(HWND hwnd, const WCHAR *sErrorMessage, HRESULT hr);
void                UpdateUI(HWND hwnd, PlayerState state);
HRESULT             AllocGetWindowText(HWND hwnd, WCHAR **pszText, DWORD *pcchLen);

// Message handlers
LRESULT             OnCreateWindow(HWND hwnd);
void                OnFileOpen(HWND hwnd);
//void                OnOpenURL(HWND hwnd);
void                OnPlayerEvent(HWND hwnd, WPARAM pUnkPtr);
void                OnPaint(HWND hwnd);
void                OnResize(WORD width, WORD height);
void                OnKeyPress(WPARAM key, HWND hwnd);
///////////////////////////////////////////////////////////////////////////////////////////////


ModelsDemo demo;


LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );


int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow )
{

	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);


    UNREFERENCED_PARAMETER( prevInstance );
    UNREFERENCED_PARAMETER( cmdLine );

    WNDCLASSEX wndClass = { 0 };
    wndClass.cbSize = sizeof( WNDCLASSEX ) ;
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.hInstance = hInstance;
    wndClass.hCursor = LoadCursor( NULL, IDC_ARROW );
    wndClass.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = "DX11BookWindowClass";

    if( !RegisterClassEx( &wndClass ) )
        return -1;

    //RECT rc = { 0, 0, 640, 480 };
	RECT rc = { 0, 0, 1024, 768 };
    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );

    HWND hwnd = CreateWindowA( "DX11BookWindowClass", "3D Models Demo", WS_OVERLAPPEDWINDOW,
                                CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
                                NULL, NULL, hInstance, NULL );

    if( !hwnd )
        return -1;

    ShowWindow( hwnd, cmdShow );



	


    // Demo Initialize
    bool result = demo.Initialize( hInstance, hwnd );

    if( result == false )
        return -1;

    MSG msg = { 0 };

	//PlayMovie();
/////////////////////////////////////////////////////////////////////////////////
	    // Initialize the player object.
    HRESULT hr = VideoPlayer::CreateInstance(hwnd, hwnd, &g_pPlayer); 
    if (SUCCEEDED(hr))
    {
        UpdateUI(hwnd, Closed);
        //return 0;   // Success.
    }
    else
    {
        NotifyError(NULL, L"Could not initialize the player object.", hr);
        return -1;  // Destroy the window
    }


	hr = g_pPlayer->OpenURL(L"demo.mp4");
	if (SUCCEEDED(hr))
	{
		UpdateUI(hwnd, OpenPending);
	}

done:
	if (FAILED(hr))
	{
		NotifyError(hwnd, L"Could not open the file.", hr);
		UpdateUI(hwnd, Closed);
	}


	//g_pPlayer->Play();

	//while(g_pPlayer->GetState() != Stopped)
	{
	}
/////////////////////////////////////////////////////////////////////////

	result = demo.LoadContent();

	 if( result == false )
        return -1;

    while( msg.message != WM_QUIT )
    {
        if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {

		if((demo.GetGameState() == PLAY_INTRO))
			{
				if((g_pPlayer->GetState() == Paused)||(g_pPlayer->GetState() == Stopped))
				{
					UpdateUI(hwnd, Closed);

					hr = g_pPlayer->OpenURL(L"demo.mp4");
					if (SUCCEEDED(hr))
					{
						UpdateUI(hwnd, OpenPending);
					}

			
					if (FAILED(hr))
					{
						NotifyError(hwnd, L"Could not open the file.", hr);
						UpdateUI(hwnd, Closed);
					}

						g_pPlayer->Play();
					}
			}
			//////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////////
			else
			{
				demo.detect_input();    // update the input data before rendering
			

				/*if(keystate[DIK_ESCAPE] & 0x80)
					PostMessage(hwnd, WM_DESTROY, 0, 0);*/


				////////////////////timer////////////////////////////////
				frameCount++;
				if(GetTime() > 1.0f)
				{
					fps = frameCount;
					frameCount = 0;
					StartTimer();
				}	

				frameTime = GetFrameTime();

				//UpdateScene(frameTime);
				//////////////////////////////////////////////////////////////

				// Update and Draw
            
				//demo.Update( 0.01f );
				demo.fps_ = fps;
				demo.frameTime_ = frameTime;

				demo.Update( frameTime );    
				demo.Render( );
			}
        }
    }
	
	// Clean up.
    if (g_pPlayer)
    {
        g_pPlayer->Shutdown();
        SafeRelease(&g_pPlayer);
    }

	demo.cleanDInput();
    // Demo Shutdown
    demo.Shutdown( );

    return static_cast<int>( msg.wParam );
}


LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT paintStruct;
    HDC hDC;

    switch( message )
    {
		case WM_CREATE:
			if((demo.GetGameState() == PLAY_INTRO))
			{
				return OnCreateWindow(hwnd);
			}
			break;


		case WM_COMMAND:
			//switch (LOWORD(wParam))
			//{
			//	case IDM_EXIT:
			//		DestroyWindow(hwnd);
			//		break;
			//	case ID_FILE_OPENFILE:
			//		OnFileOpen(hwnd);
			//		break;
			//	case ID_FILE_OPENURL:
			//		OnOpenURL(hwnd);
			//		break;

			//	default:
			//		return DefWindowProc(hwnd, message, wParam, lParam);
			//}
			break;

        case WM_PAINT:
			if((demo.GetGameState() == PLAY_INTRO))
			{
				OnPaint(hwnd);
			}
			else
			{
				hDC = BeginPaint( hwnd, &paintStruct );
				EndPaint( hwnd, &paintStruct );
			}
            break;

		case WM_SIZE:
			//if((demo.GetGameState() == PLAY_INTRO)||(demo.GetGameState() == INTRO_MOVIE_REPLAY))
			{
				OnResize(LOWORD(lParam), HIWORD(lParam));
			}
			break;

		case WM_ERASEBKGND:
			if((demo.GetGameState() == PLAY_INTRO))
			{
				// Suppress window erasing, to reduce flickering while the video is playing.
				return 1;
			}
			break;

		case WM_CHAR:
			if((demo.GetGameState() == PLAY_INTRO))
			{
				OnKeyPress(wParam, hwnd);
			}	
			break;

		case WM_APP_PLAYER_EVENT:
			if((demo.GetGameState() == PLAY_INTRO))
			{
				OnPlayerEvent(hwnd, wParam);
			}
			break;

        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

        default:
            return DefWindowProc( hwnd, message, wParam, lParam );
    }

    return 0;
}


////////////////////////////////////////////////

void StartTimer()
{
	LARGE_INTEGER frequencyCount;
	QueryPerformanceFrequency(&frequencyCount);

	countsPerSecond = double(frequencyCount.QuadPart);

	QueryPerformanceCounter(&frequencyCount);
	CounterStart = frequencyCount.QuadPart;
}

double GetTime()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return double(currentTime.QuadPart-CounterStart)/countsPerSecond;
}

double GetFrameTime()
{
	LARGE_INTEGER currentTime;
	__int64 tickCount;
	QueryPerformanceCounter(&currentTime);

	tickCount = currentTime.QuadPart-frameTimeOld;
	frameTimeOld = currentTime.QuadPart;

	if(tickCount < 0.0f)
		tickCount = 0.0f;

	return float(tickCount)/countsPerSecond;
}

/////////////////////////////////////////////////////////////////////

////////////////////////////////////directshow///////////////////////////

void PlayMovie(void)
{
    IGraphBuilder *pGraph = NULL;
    IMediaControl *pControl = NULL;
    IMediaEvent   *pEvent = NULL;

    // Initialize the COM library.
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
    {
        printf("ERROR - Could not initialize COM library");
        return;
    }

    // Create the filter graph manager and query for interfaces.
    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
                        IID_IGraphBuilder, (void **)&pGraph);
    if (FAILED(hr))
    {
        printf("ERROR - Could not create the Filter Graph Manager.");
        return;
    }

    hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
    hr = pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);

    // Build the graph. IMPORTANT: Change this string to a file on your system.
    //hr = pGraph->RenderFile(L"C:\\Example.avi", NULL);
	hr = pGraph->RenderFile(L"D:\\TerreMars.avi", NULL);
    if (SUCCEEDED(hr))
    {
        // Run the graph.
        hr = pControl->Run();
        if (SUCCEEDED(hr))
        {
            // Wait for completion.
            long evCode;
            //pEvent->WaitForCompletion(INFINITE, &evCode);
			pEvent->WaitForCompletion(10000, &evCode);
            // Note: Do not use INFINITE in a real application, because it
            // can block indefinitely.
        }
    }
    pControl->Release();
    pEvent->Release();
    pGraph->Release();
    CoUninitialize();
}
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////media foudation movie player windows proc helper functions/////////////////


//  Open an audio/video file.
void OnFileOpen(HWND hwnd)
{
    IFileOpenDialog *pFileOpen = NULL;
    IShellItem *pItem = NULL;
    PWSTR pszFilePath = NULL;

    // Create the FileOpenDialog object.
    HRESULT hr = CoCreateInstance(__uuidof(FileOpenDialog), NULL, 
        CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileOpen));
    if (FAILED(hr))
    {
        goto done;
    }

    // Show the Open dialog box.
    hr = pFileOpen->Show(NULL);
    if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
    {
        // The user canceled the dialog. Do not treat as an error.
        hr = S_OK;
        goto done;
    }
    else if (FAILED(hr))
    {
        goto done;
    }

    // Get the file name from the dialog box.
    hr = pFileOpen->GetResult(&pItem);
    if (FAILED(hr))
    {
        goto done;
    }

    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
    if (FAILED(hr))
    {
        goto done;
    }

    // Display the file name to the user.
    hr = g_pPlayer->OpenURL(pszFilePath);
    if (SUCCEEDED(hr))
    {
        UpdateUI(hwnd, OpenPending);
    }

done:
    if (FAILED(hr))
    {
        NotifyError(hwnd, L"Could not open the file.", hr);
        UpdateUI(hwnd, Closed);
    }
    CoTaskMemFree(pszFilePath);
    SafeRelease(&pItem);
    SafeRelease(&pFileOpen);
}

//  Open a media file from a URL.
//void OnOpenURL(HWND hwnd)
//{
//    HRESULT hr = S_OK;
//
//    // Pass in an OpenUrlDialogInfo structure to the dialog. The dialog 
//    // fills in this structure with the URL. The dialog proc allocates
//    // the memory for the string. 
//
//    OpenUrlDialogInfo url;
//    ZeroMemory(&url, sizeof(&url));
//
//    // Show the Open URL dialog.
//    if (IDOK == DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_OPENURL), hwnd,
//        OpenUrlDialogProc, (LPARAM)&url))
//    {
//        // Open the file with the playback object.
//        hr = g_pPlayer->OpenURL(url.pszURL);
//        if (SUCCEEDED(hr))
//        {
//            UpdateUI(hwnd, OpenPending);
//        }
//        else
//        {
//            NotifyError(hwnd, L"Could not open this URL.", hr);
//            UpdateUI(hwnd, Closed);
//        }
//    }
//
//    // The caller must free the URL string.
//    CoTaskMemFree(url.pszURL);
//}

//  Handler for WM_CREATE message.
LRESULT OnCreateWindow(HWND hwnd)
{
    // Initialize the player object.
    HRESULT hr = VideoPlayer::CreateInstance(hwnd, hwnd, &g_pPlayer); 
    if (SUCCEEDED(hr))
    {
        UpdateUI(hwnd, Closed);
        return 0;   // Success.
    }
    else
    {
        NotifyError(NULL, L"Could not initialize the player object.", hr);
        return -1;  // Destroy the window
    }
}

//  Handler for WM_PAINT messages.
void OnPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    if (g_pPlayer && g_pPlayer->HasVideo())
    {
        // Video is playing. Ask the player to repaint.
        g_pPlayer->Repaint();
    }
    else
    {
        // The video is not playing, so we must paint the application window.
        RECT rc;
        GetClientRect(hwnd, &rc);
        FillRect(hdc, &rc, (HBRUSH) COLOR_WINDOW);
    }
    EndPaint(hwnd, &ps);
}

//  Handler for WM_SIZE messages.
void OnResize(WORD width, WORD height)
{
    if (g_pPlayer)
    {
        g_pPlayer->ResizeVideo(width, height);
    }
}


// Handler for WM_CHAR messages. 
void OnKeyPress(WPARAM key, HWND hwnd)
{
	if((demo.GetGameState() == PLAY_INTRO))
	{
		g_pPlayer->Pause();
		demo.SetGameState(START_MENU);	
	}

    switch (key)
    {
    // Space key toggles between running and paused

		
    case VK_SPACE:
							// Display the file name to the user.
	//	HRESULT hr = g_pPlayer->OpenURL(L"Wildlife.wmv");
	//	if (SUCCEEDED(hr))
	//	{
	//		UpdateUI(hwnd, OpenPending);
	//	}

	//done:
	//	if (FAILED(hr))
	//	{
	//		NotifyError(hwnd, L"Could not open the file.", hr);
	//		UpdateUI(hwnd, Closed);
	//	}
	//	g_pPlayer->Play();
		
		//while(g_pPlayer->GetState() != Stopped)
		{
		}
		/*
        if (g_pPlayer->GetState() == Started)
        {
            g_pPlayer->Pause();
        }
        else if (g_pPlayer->GetState() == Paused)
        {
            g_pPlayer->Play();
        }*/
        break;
    }
}

// Handler for Media Session events.
void OnPlayerEvent(HWND hwnd, WPARAM pUnkPtr)
{
    HRESULT hr = g_pPlayer->HandleEvent(pUnkPtr);
    if (FAILED(hr))
    {
        NotifyError(hwnd, L"An error occurred.", hr);
    }
    UpdateUI(hwnd, g_pPlayer->GetState());

	if((demo.GetGameState() == PLAY_INTRO)&&(g_pPlayer->GetState() == Stopped))
	{
		demo.SetGameState(PLAY_INTRO);
	}
	
}


// Update the application UI to reflect the current state.

void UpdateUI(HWND hwnd, PlayerState state)
{
    BOOL bWaiting = FALSE;
    BOOL bPlayback = FALSE;

    assert(g_pPlayer != NULL);

    switch (state)
    {
    case OpenPending:
        bWaiting = TRUE;
        break;

    case Started:
        bPlayback = TRUE;
        break;

    case Paused:
        bPlayback = TRUE;
        break;
    }

    //HMENU hMenu = GetMenu(hwnd);
    //UINT  uEnable = MF_BYCOMMAND | (bWaiting ? MF_GRAYED : MF_ENABLED);

    //EnableMenuItem(hMenu, ID_FILE_OPENFILE, uEnable);
    //EnableMenuItem(hMenu, ID_FILE_OPENURL, uEnable);

    if (bPlayback && g_pPlayer->HasVideo())
    {
        g_bRepaintClient = FALSE;
    }
    else
    {
        g_bRepaintClient = TRUE;
    }
}

//  Show a message box with an error message.
void NotifyError(HWND hwnd, PCWSTR pszErrorMessage, HRESULT hrErr)
{
    const size_t MESSAGE_LEN = 512;
    CHAR message[MESSAGE_LEN];
	LPCTSTR pszFormat = TEXT("%s (HRESULT = 0x%X)");

    if (SUCCEEDED(StringCchPrintf(message, MESSAGE_LEN, pszFormat, 
        pszErrorMessage, hrErr)))
    {
        MessageBox(hwnd, message, NULL, MB_OK | MB_ICONERROR);
    }
}


////  Dialog proc for the "Open URL" dialog.
//INT_PTR CALLBACK OpenUrlDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    static OpenUrlDialogInfo *pUrl = NULL;
//
//    BOOL result = FALSE;
//
//    switch (message)
//    {
//    case WM_INITDIALOG:
//        // The caller sends a pointer to an OpenUrlDialogInfo structure as the 
//        // lParam. This structure stores the URL.
//        pUrl = (OpenUrlDialogInfo*)lParam;
//        return (INT_PTR)TRUE;
//
//    case WM_COMMAND:
//        switch (LOWORD(wParam))
//        {
//        case IDOK:
//            if (pUrl)
//            {
//                // Get the URL from the edit box in the dialog. This function 
//                // allocates memory. The caller must call CoTaskMemAlloc.
//                if (SUCCEEDED(AllocGetWindowText(GetDlgItem(hDlg, IDC_EDIT_URL), 
//                    &pUrl->pszURL, &pUrl->cch)))
//                {
//                    result = TRUE;
//                }
//            }
//            EndDialog(hDlg, result ? IDOK : IDABORT);
//            break;
//
//        case IDCANCEL:
//            EndDialog(hDlg, LOWORD(IDCANCEL));
//            break;
//        }
//        return (INT_PTR)FALSE;
//    }
//    return (INT_PTR)FALSE;
//}

// Helper function to get text from a window.
//
// This function allocates a buffer and returns it in pszText. The caller must
// call CoTaskMemFree on the buffer.
//
// hwnd:     Handle to the window.
// pszText:  Receives a pointer to the string.
// pcchLen:  Receives the length of the string, in characters, not including
//           the terminating NULL character. 

HRESULT AllocGetWindowText(HWND hwnd, WCHAR **pszText, DWORD *pcchLen)
{
    if (pszText == NULL || pcchLen == NULL)
    {
        return E_POINTER;
    }

    *pszText = NULL;  

    int cch = GetWindowTextLength(hwnd);  
    if (cch < 0) 
    {
        return E_UNEXPECTED; 
    }

    PWSTR pszTmp = (PWSTR)CoTaskMemAlloc(sizeof(WCHAR) * (cch + 1)); 
    // Includes room for terminating NULL character

    if (!pszTmp)
    {
        return E_OUTOFMEMORY;
    }

    if (cch == 0)
    {
        pszTmp[0] = L'\0';  // No text.
    }
    else
    {
        int res = GetWindowText(hwnd, (LPSTR)pszTmp, (cch + 1));  
        // Size includes terminating null character.

        // GetWindowText returns 0 if (a) there is no text or (b) it failed.
        // We checked for (a) already, so 0 means failure here.
        if (res == 0)
        {
            CoTaskMemFree(pszTmp);
            return __HRESULT_FROM_WIN32(GetLastError());
        }
    }

    // If we got here, szTmp is valid, so return it to the caller.
    *pszText = pszTmp;

    // Return the length NOT including the '\0'.
    *pcchLen = static_cast<DWORD>(cch);  

     return S_OK;
}
//////////////////////////////////////////////////////////////////////////////////////////