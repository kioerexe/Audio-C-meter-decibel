// Peak Windows.cpp : Definisce il punto di ingresso dell'applicazione.
//

#include "framework.h"
#include "Peak Windows.h"
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "winmm.lib")

#define MAX_LOADSTRING 100
#define WIDTH_WIN 300
#define HEIGHT_WIN 400
#define WIDTH_RECT 70
#define IDT_TIMER1 10
#define OFFSET 100
const int HEIGHT_RECT = (HEIGHT_WIN / 2) + OFFSET;

// Variabili globali:
HINSTANCE hInst;                                // istanza corrente
WCHAR szTitle[MAX_LOADSTRING];                  // Testo della barra del titolo
WCHAR szWindowClass[MAX_LOADSTRING];            // nome della classe della finestra principale
void* reserved;
IMMDevice* endPointOutputDefault = NULL;
IMMDeviceEnumerator* enumeratore = NULL;
IAudioMeterInformation *meterInfo=NULL;
ID2D1Factory* factoryd2d1 = NULL;
ID2D1HwndRenderTarget* renderTarget=NULL;
ID2D1SolidColorBrush *pennelloRettangolo=NULL;
wchar_t* testoWide = (wchar_t*)malloc(12);

void enumOutputDevices(IMMDeviceEnumerator**);

// Dichiarazioni con prototipo di funzioni incluse in questo modulo di codice:
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

    // TODO: Inserire qui il codice.
    CoInitialize(reserved);
    //enum dei dispositivi e attivazione del dispositivo di render attuale
    enumOutputDevices(&enumeratore);
    enumeratore->GetDefaultAudioEndpoint(eRender, eConsole, &endPointOutputDefault);
    auto hr = endPointOutputDefault->Activate(__uuidof(IAudioMeterInformation), CLSCTX_ALL, NULL, (void**)&meterInfo);


    // Inizializzare le stringhe globali
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PEAKWINDOWS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Eseguire l'inizializzazione dall'applicazione:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PEAKWINDOWS));

    MSG msg;

    // Ciclo di messaggi principale:
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
//  FUNZIONE: MyRegisterClass()
//
//  SCOPO: Registra la classe di finestre.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PEAKWINDOWS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PEAKWINDOWS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNZIONE: InitInstance(HINSTANCE, int)
//
//   SCOPO: Salva l'handle di istanza e crea la finestra principale
//
//   COMMENTI:
//
//        In questa funzione l'handle di istanza viene salvato in una variabile globale e
//        viene creata e visualizzata la finestra principale del programma.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Archivia l'handle di istanza nella variabile globale

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, WIDTH_WIN, HEIGHT_WIN, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNZIONE: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  SCOPO: Elabora i messaggi per la finestra principale.
//
//  WM_COMMAND  - elabora il menu dell'applicazione
//  WM_PAINT    - Disegna la finestra principale
//  WM_DESTROY  - inserisce un messaggio di uscita e restituisce un risultato
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        //creo il contesto per disegnale con d2d1
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factoryd2d1);
        //target
        RECT rgn;
        GetWindowRect(hWnd, &rgn);
        D2D1_SIZE_U size = D2D1::SizeU(
            rgn.right - rgn.left,
            rgn.bottom - rgn.top
        );
        //target (la finestra)
        factoryd2d1->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(hWnd, size),
            &renderTarget
        );

        //pennello
        renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green), &pennelloRettangolo);
        SetTimer(hWnd, IDT_TIMER1, USER_TIMER_MINIMUM, NULL);
    }
    break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Analizzare le selezioni di menu:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_TIMER:
        switch (wParam)
        {
        case IDT_TIMER1:
            //volume picco attuale
            float peak = 0.0f;
            meterInfo->GetPeakValue(&peak);
            float altezzaRettangolo = (peak / 1.0f) * HEIGHT_RECT; //calcolo l'altezza del rettangolo
            int db_peak = 20.0f * log10f(peak); //calcolo decibell
            char testoDb[7] = "-db "; //creo il testo per i decibell
            testoDb[3] = (abs(db_peak) / 10) + 48; //decine
            testoDb[4] = abs(db_peak) - (abs(db_peak) / 10)*10 + 48; //unità
            testoDb[5] = '\0'; //null
            mbstowcs_s(NULL,testoWide,12,testoDb,7);
            SetWindowText(hWnd, testoWide);

            D2D1_RECT_F rettangoloVolume = D2D1::RectF((WIDTH_WIN - WIDTH_RECT) / 2, (HEIGHT_RECT - altezzaRettangolo),
                (WIDTH_WIN - WIDTH_RECT) / 2 + WIDTH_RECT, HEIGHT_RECT);

            //disegno
            renderTarget->BeginDraw();
            renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
            renderTarget->FillRectangle(rettangoloVolume, pennelloRettangolo);
            renderTarget->EndDraw();
            SetTimer(hWnd, IDT_TIMER1, USER_TIMER_MINIMUM, NULL); //reset timer
            break;
        }
        break;
    case WM_DESTROY:
        meterInfo->Release();
        endPointOutputDefault->Release();
        enumeratore->Release();

        factoryd2d1->Release();
        renderTarget->Release();
        pennelloRettangolo->Release();
        KillTimer(hWnd ,IDT_TIMER1);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Gestore di messaggi per la finestra Informazioni su.
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

void enumOutputDevices(IMMDeviceEnumerator **pEnumerator)
{
    const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
    const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
    auto hr = CoCreateInstance(
        CLSID_MMDeviceEnumerator, NULL,
        CLSCTX_ALL, IID_IMMDeviceEnumerator,
        (void**)pEnumerator);
}
