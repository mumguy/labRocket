// knopka.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "knopka.h"
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#pragma comment(lib, "winmm.lib")



#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
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

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_KNOPKA, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_KNOPKA));

    MSG msg;

    // Цикл основного сообщения:
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

double MAXCX = 3;
double MINCX = -3;
double MAXCY = 3;
double MINCY = -3;
double propXY = 1;
double propYX = 1;
double itoe = 0;
double deltat=0.01;

double lambda = 0.5;
double p1 = 10;
double m = 1;
double g = 9.81;
double PI = 180;
double BREAK=0;


double alpha1 = 30;
double alpha = alpha1 * (3.1415 / PI);


double v0 = p1 / m;

std::vector<double> xData, yData;
bool drawAxes = false, drawGraph = false;

void LoadDataFromFile(const wchar_t* filename) {
    xData.clear();
    yData.clear();

    std::wifstream file(filename);
    if (!file.is_open()) return;

    std::wstring line;
    while (std::getline(file, line)) {
        std::wstringstream ss(line);
        double t, x, y;
        if (ss >> t >> x >> y) {
            xData.push_back(static_cast<double>(x));
            yData.push_back(static_cast<double>(y));
        }
    }
    file.close();
    drawGraph = true;
}







double GetCXtoCD(double MAXD, double x) {
    double k = (MAXD) / (MAXCX - MINCX);
    double b = (-MAXD * MINCX) / (MAXCX - MINCX);

    double x2 = k * x * propXY + b;
    return x2;
}

double GetCYtoCD(double MAXD, double x) {
    double k = -(MAXD) / (MAXCY - MINCY);
    double b = (MAXD * MAXCY) / (MAXCY - MINCY);

    double x2 = k * x + b;
    return x2;
}




ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_KNOPKA));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_KNOPKA);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; 

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


double podshet(double alpha, double lambda, double v0, double m, double g, double t) {

    MAXCY = v0+2;
    MINCY = -v0 - 2;

    // Ускорение вдоль клина
    double a_forward = -g * (sin(alpha) + lambda * cos(alpha));

    // Время до остановки
    double t_stop = v0 / -a_forward;

    double vtest;


    if (tan(alpha) <= lambda) {

        // Блок остановится и останется на месте


        if (t <= t_stop) {

            double x_stop = v0 * t_stop + 0.5 * a_forward * t_stop * t_stop;
            double a_backward = g * (sin(alpha) - lambda * cos(alpha));
            double tmax = sqrt((2 * x_stop) / a_backward) + t_stop;
            MAXCX = tmax * propXY;
            MINCX = -tmax * propXY;

            // Движение до остановки
            double x = v0 * t + 0.5 * a_forward * t * t;
            double v = a_forward * t + v0;
            vtest = v;
            
            itoe = x;


        }
        else {
            // Блок остановился
            double x_stop = v0 * t_stop + 0.5 * a_forward * t_stop * t_stop;
            double a_backward = g * (sin(alpha) - lambda * cos(alpha));
            double tmax = sqrt((2 * x_stop) / a_backward) + t_stop;
            MAXCX = tmax * propXY;
            MINCX = -tmax * propXY;

            
            
            double v = 0;
            vtest = v;

            BREAK = 1;
            
            itoe = x_stop;


        }
    }
    else {

        // Блок остановится, но затем начнет двигаться обратно


        if (t <= t_stop) {
            // Движение до остановки
            double x_stop = v0 * t_stop + 0.5 * a_forward * t_stop * t_stop;
            double a_backward = g * (sin(alpha) - lambda * cos(alpha));
            double tmax = sqrt((2 * x_stop) / a_backward) + t_stop;
            MAXCX = tmax * propXY;
            MINCX = -tmax * propXY;

            double x = v0 * t + 0.5 * a_forward * t * t;
            double v = a_forward * t + v0;
            itoe = x;
            vtest = v;


        }
        else {
            // Движение в обратном направлении
            double x_stop = v0 * t_stop + 0.5 * a_forward * t_stop * t_stop;
            double a_backward = g * (sin(alpha) - lambda * cos(alpha));
            double tmax = sqrt((2 * x_stop) / a_backward)+t_stop;
            MAXCX = tmax * propXY;
            MINCX = -tmax * propXY;

            
            double t_backward = t - t_stop;
            double x = x_stop - 0.5 * a_backward * t_backward * t_backward;
            
            double v = a_backward * t_backward;
            itoe = x;
            vtest = v;




            if (x < 0)
            {


            }
            else
            {
               

            }



        }
    }

    return vtest;

}




//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    static HWND hwndEdit1;

    switch (message)
    {
        static HWND hwndButton;
        
       
        case WM_CREATE:
        {
            
            // Создание кнопки
            HWND hwndButton = CreateWindow(
                L"BUTTON",                   
                L"Обновить график",                 
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 
                50, 50,                     
                300, 50,                    
                hWnd,                       
                (HMENU)1,                  
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL                        
            ); 

             hwndEdit1 = CreateWindow(
                L"EDIT",                     
                L"Размер единичного отрезка по x = ",                         
                WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_READONLY,  
                50, 120,                     
                400, 25,                     // Размеры (ширина, высота)
                hWnd,                        // Родительское окно
                (HMENU)2,                   // Идентификатор Edit
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL                         // Дополнительные параметры
            );
            

        
        
        
        }






        break;


    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:

            if (LOWORD(wParam) == 1) {
                
                char buffer[256];

                InvalidateRect(hWnd, NULL, TRUE); 
                UpdateWindow(hWnd);



            }




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

        RECT rect;
        GetClientRect(hWnd, &rect);
        double width = rect.right - rect.left;
        double height = rect.bottom - rect.top;

        // Находим центр окна
        int centerX = width / 2;
        int centerY = height / 2;

        LOGBRUSH lb;
        lb.lbStyle = BS_SOLID;
        lb.lbColor = RGB(255, 0, 0);
        lb.lbHatch = 0;


        DWORD dashStyles[] = { 5,8 };

        HPEN hPen = ExtCreatePen(PS_GEOMETRIC | PS_USERSTYLE, 3, &lb, 2, dashStyles);
        SelectObject(hdc, hPen);

        hPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
        SelectObject(hdc, hPen);

        LoadDataFromFile(L"D:\\progVS\\rocket\\rocket\\trajectory.txt");

        double maxX = *std::max_element(xData.begin(), xData.end());

        double maxY = *std::max_element(yData.begin(), yData.end());

        MoveToEx(hdc, centerX, centerY , NULL);


        if (!xData.empty())
        {
           

            for (size_t i = 0; i < xData.size(); i++)
            {
                LineTo(hdc, centerX + (xData[i] / maxX / 2 * width), centerY - (yData[i] / maxY / 2 * height));
            }
        }



        // Рисуем вертикальную линию
        MoveToEx(hdc, centerX, 0, nullptr);
        LineTo(hdc, centerX, height);



        // Рисуем горизонтальную линию
        MoveToEx(hdc, 0, centerY, nullptr);
        LineTo(hdc, width, centerY);



        propXY = width / height;
        propYX = height / width;


        MoveToEx(hdc, centerX, centerY, nullptr);  // для графика скорости

       

        MoveToEx(hdc, centerX, centerY, nullptr);  // для графика скорости

        

        // Рисуем вертикальную линию
       

        MoveToEx(hdc, centerX + ( maxX / 10/ maxX / 2 * width), centerY+10, nullptr);
        LineTo(hdc, centerX + (maxX / 10 / maxX / 2 * width), centerY - 10);



        // Рисуем горизонтальную линию
        MoveToEx(hdc, centerX+10, centerY - (maxY / 10 / maxY / 2 * height), nullptr);
        LineTo(hdc, centerX - 10,centerY-(maxY / 10 / maxY / 2 * height));



        wchar_t buffer[256];
        


        
        swprintf(buffer, 256, L" Размер единичного отрезка По X: %.2f  По Y: %.2f", maxX / 10, maxY / 10);

        SetWindowText(hwndEdit1, buffer);



            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            EndPaint(hWnd, &ps);
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

// Обработчик сообщений для окна "О программе".
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

