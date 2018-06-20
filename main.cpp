#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <time.h>
#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <ctime>
#include <sstream>
#include <fstream>
#include <tchar.h>


#define ID_TRAY1   601
#define CMSG_TRAY1 0x8001

#define NIIF_NONE 0x00000000
#define NIIF_INFO 0x00000001
#define NIIF_WARNING 0x00000002
#define NIIF_ERROR 0x00000003
#define NIIF_USER 0x00000004
#define NIIF_NOSOUND 0x00000010
#define NIIF_LARGE_ICON 0x00000020
#define NIIF_RESPECT_QUIET_TIME 0x00000080

#define _WIN32_IE 0x0500

#define _WIN32_IE 0x0600

#define NIF_INFO 0x00000010

#include <windows.h>

using namespace std;

sf::Color kolor;
bool wylaczyc = false;

sf::RenderWindow okno;

sf::Clock zegar_func;

HMENU menu_tray;
POINT ptCursor;

HWND przyciskp;
HWND przyciskd;

NOTIFYICONDATA nid;

bool zmiana_p;
bool zmiana_d;


sf::Clock zegar_optymalizacja;

LRESULT CALLBACK OnEvent(HWND Handle, UINT Message, WPARAM WParam, LPARAM LParam);

void func()
{
    okno.create( sf::VideoMode( 100, 100, 32 ), "title", sf::Style::None );
    //okno.setFramerateLimit( 1 );

    sf::Vector2i poz;
    poz.x = GetSystemMetrics( SM_CXSCREEN ) - 100;
    poz.y = 0;

    okno.setPosition( poz );

    ShowWindow( okno.getSystemHandle(), WS_EX_TOPMOST );

    SetWindowPos( okno.getSystemHandle(), HWND_TOPMOST , poz.x, poz.y, 100, 100, SWP_NOACTIVATE | SWP_SHOWWINDOW );

    ShowWindow( okno.getSystemHandle(), SW_HIDE );
    sf::Event event;

    while( okno.isOpen() )
    {
        while( okno.pollEvent( event ) )
        {
            if( event.type == sf::Event::Closed )
                okno.close();
        }
        okno.clear( kolor );

        if( zegar_func.getElapsedTime().asSeconds() > 0.5 && wylaczyc == false )
        {
            wylaczyc = true;
            ShowWindow( okno.getSystemHandle(), SW_HIDE );
            //zegar_func.restart();
            cout << "Ukryj" << endl;
            //okno.close();
        }

        okno.display();

        Sleep( 10 );
        //sf::sleep( sf::seconds( 1 ) );
    }
}

sf::Thread thread( &func );

void CaptureScreen(HWND window, const char* filename)
{
RECT windowRect;
GetWindowRect(window, &windowRect);

int bitmap_dx = windowRect.right - windowRect.left - 2;
int bitmap_dy = windowRect.bottom - windowRect.top - 2;

ofstream file(filename, ios::binary);
if(!file) return;

BITMAPFILEHEADER fileHeader;
BITMAPINFOHEADER infoHeader;

fileHeader.bfType      = 0x4d42;
fileHeader.bfSize      = 0;
fileHeader.bfReserved1 = 1;
fileHeader.bfReserved2 = 1;
fileHeader.bfOffBits   = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

infoHeader.biSize          = sizeof(infoHeader);
infoHeader.biWidth         = bitmap_dx;
infoHeader.biHeight        = bitmap_dy;
infoHeader.biPlanes        = 1;
infoHeader.biBitCount      = 24;
infoHeader.biCompression   = BI_RGB;
infoHeader.biSizeImage     = 0;
infoHeader.biXPelsPerMeter = 0;
infoHeader.biYPelsPerMeter = 0;
infoHeader.biClrUsed       = 0;
infoHeader.biClrImportant  = 0;

file.write((char*)&fileHeader, sizeof(fileHeader));
file.write((char*)&infoHeader, sizeof(infoHeader));

BITMAPINFO info;
info.bmiHeader = infoHeader;

HDC winDC = GetWindowDC(window);
HDC memDC = CreateCompatibleDC(winDC);
const char* memory = 0;
HBITMAP bitmap = CreateDIBSection(winDC, &info, DIB_RGB_COLORS, (void**)&memory, 0, 0);
SelectObject(memDC, bitmap);
BitBlt(memDC, 0, 0, bitmap_dx, bitmap_dy, winDC, 0, 0, SRCCOPY);
DeleteDC(memDC);
ReleaseDC(window, winDC);

int bytes = (((24*bitmap_dx + 31) & (~31))/8)*bitmap_dy;
file.write(memory, bytes);

DeleteObject(bitmap);
}

int WINAPI WinMain (HINSTANCE Instance,HINSTANCE hPrevInstance,LPSTR lpszArgument,int nCmdShow)
{
    WNDCLASS WindowClass;
    WindowClass.style         = 0;
    WindowClass.lpfnWndProc   = &OnEvent;
    WindowClass.cbClsExtra    = 0;
    WindowClass.cbWndExtra    = 0;
    WindowClass.hInstance     = Instance;
    WindowClass.hIcon         = LoadIcon( GetModuleHandle( NULL ), MAKEINTRESOURCE( 1284 ) );
    WindowClass.hCursor       = 0;
    WindowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BACKGROUND);
    //WindowClass.lpszMenuName  = "Moje_menu";
    WindowClass.lpszMenuName  = NULL;
    WindowClass.lpszClassName = "SFML App";
    //WindowClass.hIconSm = LoadIcon( NULL, "grafika/ikona/X.png" );
    RegisterClass(&WindowClass);

    HWND hwnd = CreateWindowEx( WS_EX_CLIENTEDGE, "SFML App", "Oto okienko", WS_OVERLAPPEDWINDOW, GetSystemMetrics( SM_CXSCREEN ) / 2 - 200, GetSystemMetrics( SM_CYSCREEN ) / 2 - 200, 400, 400, NULL, NULL, Instance, NULL );

    if( hwnd == NULL )
    {
        MessageBox( NULL, "Okno odmówi³o przyjœcia na œwiat!", "Ale kicha...", MB_ICONEXCLAMATION );
        return 1;
    }

    ShowWindow( hwnd, nCmdShow ); // Poka¿ okienko...
    UpdateWindow( hwnd );

    przyciskp = CreateWindowEx( 0, "STATIC", "Aby pobrac kolor wcisnij CTRL+CAPS LOCK", WS_CHILD | WS_VISIBLE, 100, 100, 150, 30, hwnd, NULL, Instance, NULL );

    ShowWindow( hwnd, SW_HIDE );

    LPSTR sTip = "Color Picker";
    LPSTR sTytul = "Uruchomiono";
    LPSTR sOpis = "Color Picker zosta³ uruchomiony";

    nid.cbSize = sizeof( NOTIFYICONDATA );
    nid.hWnd = hwnd;
    nid.uID = ID_TRAY1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
    nid.uCallbackMessage = CMSG_TRAY1;
    nid.hIcon = LoadIcon( GetModuleHandle( NULL ), MAKEINTRESOURCE( 1284 ) );
    nid.dwInfoFlags = NIIF_USER;
    lstrcpy( nid.szTip, sTip );
    lstrcpy( nid.szInfoTitle, sTytul );
    lstrcpy( nid.szInfo, sOpis );

    Shell_NotifyIcon( NIM_ADD, & nid );

    menu_tray = CreatePopupMenu();
    AppendMenu( menu_tray, MF_STRING , 0, "Otworz aplikacje" );
    AppendMenu( menu_tray, MF_STRING , 1, "Zamknij" );




    sf::Vector2i poz_myszy;

    bool reset = false;
    sf::Clock zegar;

    bool rob = false;

    sf::Image image;
    sf::Texture tekstura;
    sf::Sprite sprite;

    thread.launch();

    MSG Message;
    Message.message = ~WM_QUIT;

    while (Message.message != WM_QUIT)
    {
        if (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }
        else
        {
            if( GetAsyncKeyState( VK_CONTROL ) && GetAsyncKeyState( VK_F1 ) && rob == false )
            {
                rob = true;

                poz_myszy = sf::Mouse::getPosition();
                cout<<poz_myszy.x<<" "<<poz_myszy.y<<endl;
                CaptureScreen( GetDesktopWindow(), "plik.bmp" );

                image.loadFromFile( "plik.bmp" );

                kolor = image.getPixel( poz_myszy.x, poz_myszy.y );

                zegar_func.restart();
                wylaczyc = false;

                int czerwony = kolor.r;
                int zielony = kolor.g;
                int niebieski = kolor.b;

                ostringstream ss;
                ss << "rgba(";
                ss << czerwony << ",";
                ss << zielony << ",";
                ss << niebieski << ",1)";
                cout << ss.str();
                const char* output = ss.str().c_str();
                const size_t len = strlen(output) + 1;
                HGLOBAL hMem =  GlobalAlloc(GMEM_MOVEABLE, len);
                memcpy(GlobalLock(hMem), output, len);
                GlobalUnlock(hMem);
                OpenClipboard(0);
                EmptyClipboard();
                SetClipboardData(CF_TEXT, hMem);
                CloseClipboard();


                reset = true;
                zegar.restart();
                ShowWindow( okno.getSystemHandle(), SW_SHOW );
            }

            if( zegar.getElapsedTime().asSeconds() >= 2 && reset == true )
            {
                reset = false;
                rob = false;
            }
            if( zegar.getElapsedTime().asSeconds() >= 3 )
            {
                reset = false;
                rob = false;
            }

        }
        Sleep( 10 );
    }

    okno.close();

    thread.terminate();

    nid.cbSize = sizeof( NOTIFYICONDATA );
    nid.hWnd = hwnd;
    nid.uID = ID_TRAY1;
    nid.uFlags = 0;

    Shell_NotifyIcon( NIM_DELETE, & nid );

    DestroyWindow(hwnd);
    UnregisterClass("SFML App", Instance);

    DestroyMenu( menu_tray );

    return 0;
}

LRESULT CALLBACK OnEvent(HWND Handle, UINT Message, WPARAM WParam, LPARAM LParam)
{
    switch (Message)
    {
        case WM_DESTROY:
            okno.close();

            thread.terminate();

            nid.cbSize = sizeof( NOTIFYICONDATA );
            nid.hWnd = Handle;
            nid.uID = ID_TRAY1;
            nid.uFlags = 0;

            Shell_NotifyIcon( NIM_DELETE, & nid );

            DestroyWindow(Handle);

            DestroyMenu( menu_tray );

            return 1;

        case WM_CLOSE:
            if( MessageBox( Handle, "Zamknac aplikacje?", "Zamknac?", MB_YESNO | MB_ICONQUESTION ) == IDNO )
                return 0;
            break;

        case CMSG_TRAY1:
            if( WParam == ID_TRAY1 )
            {
                if( LParam == WM_LBUTTONDBLCLK )
                {
                    ShowWindow( Handle, WS_EX_TOPMOST );
                    SetForegroundWindow(Handle);
                }
                if( LParam == WM_RBUTTONDOWN )
                {
                    SetForegroundWindow(Handle);

                    GetCursorPos(&ptCursor);
                    TrackPopupMenuEx(menu_tray, TPM_RIGHTBUTTON, ptCursor.x, ptCursor.y, Handle,0);


                    PostMessage(Handle, WM_NULL, 0, 0);
                }
            }

            break;
        case WM_COMMAND:
            if( WParam == 0 )
            {
                ShowWindow( Handle, WS_EX_TOPMOST );
                SetForegroundWindow(Handle);
            }
            if( WParam == 1 )
            {
                okno.close();

                thread.terminate();

                nid.cbSize = sizeof( NOTIFYICONDATA );
                nid.hWnd = Handle;
                nid.uID = ID_TRAY1;
                nid.uFlags = 0;

                Shell_NotifyIcon( NIM_DELETE, & nid );

                DestroyWindow(Handle);

                DestroyMenu( menu_tray );

                return 1;
            }
            if( (HWND)LParam == przyciskp )
            {
                MessageBox( Handle, "Wxisnieto", "Hej", MB_ICONERROR );
            }
            break;

        case WM_SIZE:
            {
                if( WParam == SIZE_MINIMIZED )
                    ShowWindow( Handle, SW_HIDE );
            }
            break;
    }

    return DefWindowProc(Handle, Message, WParam, LParam);
}
