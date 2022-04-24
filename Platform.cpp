// Platform.cpp : 定义应用程序的入口点。
//
#pragma warning(disable : 4996)
#include "framework.h"
#include "Platform.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


const int width = 800;
const int height = 800;
const std::string modelpath = "./models/spot/spot_triangulated_good.obj";
const std::string texturepath = "./models/spot/spot_texture.tga";
const Eigen::Vector3f eye(0, 0, 2);
const Eigen::Vector3f lookat(0, 0, -1);
const Eigen::Vector3f up(0, 1, 0);
const Light light1({ 20, 0, 20 }, { 500,500,500 });
const Light light2({ -20,0,20 }, { 500,500,500 });
std::vector<Light> lights = { light1,light2 };
float alpha = 0.f, beta = 0.f, gamma = 0.f, angle_step = 5.f;
float scaling = 0.5f;

rasterizer rast(width, height);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。
    /*-------------------------------------------------------------------------------*/
    //开启控制台，重定向输入输出流
    AllocConsole();                         
    FILE* stream;
    freopen_s(&stream, "CON", "r", stdin);
    freopen_s(&stream, "CON", "w", stdout);

    obj::Loader loadedObj = obj::Loader(modelpath);   
    rast.set_loaded_obj(loadedObj);
    rast.set_texture(texturepath);
    rast.set_model(make_model(alpha, beta, gamma, scaling));
    rast.set_view(make_view(eye, lookat, up));
    rast.set_projection(make_projection(45.0, 1, -1, -100));
    rast.set_viewport(make_viewport(width, height));
    std::shared_ptr<IShader> shader = std::make_shared<Blinn_phong_shader>();
    rast.set_shader(shader);
    rast.set_light(lights);

    rast.draw();

    DWORD t_pre = 0, t_now = 0;
    /*-------------------------------------------------------------------------------*/

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PLATFORM, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PLATFORM));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            //
            t_now = GetTickCount();
            if (t_now - t_pre >= 2) {
                HWND hWnd = FindWindow(NULL, TEXT("Render Window"));
                RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
                t_pre = t_now;
            }
        }
    }

    return (int)msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;                                   //窗口类的风格
    wcex.lpfnWndProc = WndProc;                                             //消息处理回调函数
    wcex.cbClsExtra = 0;                                                    //附加参数，通常情况下为0
    wcex.cbWndExtra = 0;                                                    //附加参数，通常情况下为0
    wcex.hInstance = hInstance;                                             //窗口句柄
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PLATFORM));        //窗口图标
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);                          //鼠标图标
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 3);                        //窗口画刷
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PLATFORM);                     //窗口菜单名称
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 将实例句柄存储在全局变量中

    HWND hWnd = CreateWindowW(
        szWindowClass,          //窗口类名
        TEXT("Render Window"),  //窗口标题
        WS_OVERLAPPEDWINDOW,    //窗口风格
        CW_USEDEFAULT,          //窗口左上角x位置
        CW_USEDEFAULT,          //窗口左上角y位置
        800,                    //窗口的宽度
        800,                    //窗口的高度
        nullptr,                //该窗口的父窗口或所有者窗口的句柄，这里不需要，设为NULL
        nullptr,                //窗口菜单句柄，这里没有菜单，设置为NULL
        hInstance,              //窗口句柄
        nullptr                 //传递给窗口WM_CREATE消息的一个参数，这里不需要，设置为NULL
    );

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 分析菜单选择:
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
    case WM_KEYDOWN:
        handle_key_message(wParam);
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 在此处添加使用 hdc 的任何绘图代码...
        rast.draw();
        //rast.draw_wireframe();
        window_draw_buffer(hWnd, hdc, rast);
        std::cout << "update frame" << std::endl;

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

// “关于”框的消息处理程序。
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

//-----------------------------------------------------------------------------
void window_draw_buffer(HWND& hWnd, HDC& hdc, rasterizer& rast) {
    int width = rast.width();
    int height = rast.height();
    int channels = 3;				//rgb三通道	
    std::unique_ptr<unsigned char[]> data(new unsigned char[width * height * channels]);    

    int cnt = 0;
    //构建位图数据
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            unsigned char* color = rast.get_color_from_buffer(x, y);
            if (color[0] != 0 || color[1] != 0 || color[2] != 0)
                cnt++;
                
            unsigned char r = color[0];
            unsigned char g = color[1];
            unsigned char b = color[2];
            data[x * 3 + y * width * 3] = b;
            data[x * 3 + y * width * 3 + 1] = g;
            data[x * 3 + y * width * 3 + 2] = r;
        }
    }
    std::cout << "Draw pixels: " << cnt << std::endl;

    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = width * height * channels;

    StretchDIBits(
        hdc,
        0, 0,
        bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight,
        0, 0,
        bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight,
        data.get(),
        (BITMAPINFO*)&bmi.bmiHeader,
        DIB_RGB_COLORS,
        SRCCOPY
    );
}

void handle_key_message(WPARAM virtualKey) {
    switch (virtualKey) {
    case KEY_A:
        update_rotationAngle(beta, -angle_step);
        break;
    case KEY_D:
        update_rotationAngle(beta, angle_step);
        break;
    case KEY_W:
        update_rotationAngle(alpha, angle_step);
        break;
    case KEY_S:
        update_rotationAngle(alpha, -angle_step);
        break;
    case KEY_Q:
        update_rotationAngle(gamma, angle_step);
        break;
    case KEY_E:
        update_rotationAngle(gamma, -angle_step);
        break;
    case KEY_Z:
        update_scaling(scaling, ZOOM_OUT);
        break;
    case KEY_X:
        update_scaling(scaling, ZOOM_IN);
        break;
    }
}

void update_rotationAngle(float& angle, float step) {
    angle += step;
    std::cout << "Rotation Angle(" << alpha << ", " << beta << ", " << gamma << ")" << std::endl;
    rast.set_model(make_model(alpha, beta, gamma, scaling));
}

void update_scaling(float& scale, int operation) {
    if (operation == ZOOM_IN)
        scale *= 1.1f;
    else if (operation == ZOOM_OUT)
        scale *= 0.9f;
    scale = std::clamp(scale, 0.01f, 2.f);
    std::cout << "Scale: " << scaling << std::endl;
    rast.set_model(make_model(alpha, beta, gamma, scaling));
}