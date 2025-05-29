// imp_updater.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "imp_updater.h"
#include "utility.h"

#define MAX_LOADSTRING 100
#define IDC_STATIC_STATUS 1
#define IDC_LISTBOX_LOG 2
#define TIMER_ID 1
#define PI 3.14159265
#define CONSOLE_DEBUG 0

HINSTANCE hInst;
HWND hWnd, hStatic, hListBox;
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];
BOOL bIsUpgradeFinish = TRUE;
UINT uInstallProgress = 0;

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void LogInfo(const std::string& str)
{
	if (cmdline_option.is_hide_window || cmdline_option.window_style > 0) {
		write_log_type("info", str);
		return;
	}

	std::string item;
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	char buffer[8192] = { 0 };
	sprintf_s(buffer, "%02d:%02d:%02d.%03d %s", 
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, str.c_str());
	SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)buffer);
	int itemCount = SendMessage(hListBox, LB_GETCOUNT, 0, 0);
	SendMessage(hListBox, LB_SETCURSEL, itemCount - 1, 0);
	write_log_type("info", str);
}

void LogInfo(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int len = _vscprintf_p(fmt, ap) + 1;
	char* buf = new char[len];
	_vsprintf_p(buf, len, fmt, ap);
	va_end(ap);
	LogInfo(std::string(buf));
	delete[] buf;
}

void LogWarn(const std::string& str)
{
	if (cmdline_option.is_hide_window || cmdline_option.window_style > 0) {
		write_log_type("warn", str);
		return;
	}

	std::string item;
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	char buffer[8192] = { 0 };
	sprintf_s(buffer, "%02d:%02d:%02d.%03d %s", 
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, str.c_str());
	SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)buffer);
	int itemCount = SendMessage(hListBox, LB_GETCOUNT, 0, 0);
	SendMessage(hListBox, LB_SETCURSEL, itemCount - 1, 0);
	write_log_type("warn", str);
}

void LogWarn(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int len = _vscprintf_p(fmt, ap) + 1;
	char* buf = new char[len];
	_vsprintf_p(buf, len, fmt, ap);
	va_end(ap);
	LogWarn(std::string(buf));
	delete[] buf;
}

void LogError(const std::string& str)
{
	last_error = str;
	if (cmdline_option.is_hide_window || cmdline_option.window_style > 0) {
		write_log_type("error", str);
		return;
	}

	std::string item;
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	char buffer[8192] = { 0 };
	sprintf_s(buffer, "%02d:%02d:%02d.%03d %s", 
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, str.c_str());
	SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)buffer);
	int itemCount = SendMessage(hListBox, LB_GETCOUNT, 0, 0);
	SendMessage(hListBox, LB_SETCURSEL, itemCount - 1, 0);
	write_log_type("error", str);
}

void LogError(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int len = _vscprintf_p(fmt, ap) + 1;
	char* buf = new char[len];
	_vsprintf_p(buf, len, fmt, ap);
	va_end(ap);
	LogError(std::string(buf));
	delete[] buf;
}

void ChangeStatusText(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int len = _vscprintf_p(fmt, ap) + 1;
	char* buf = new char[len];
	_vsprintf_p(buf, len, fmt, ap);
	va_end(ap);
	SendMessage(hWnd, WM_USER_UPDATE_STATUS, 0, (LPARAM)buf);
	delete[] buf;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
#ifdef _DEBUG
	lpCmdLine = 
		"-old_version_process_name 基恩士激光雕刻.exe"
		"-source_path C:/Users/user/Desktop/基恩士激光雕刻"
		"-destination_path d:/update_test/old"
		"-is_create_shortcut true"
		"-is_delete_update_file false"
		"-is_hide_window false"
		"-is_backup_file true"
		"-window_title imp_updater[更新器]"
		"-window_width 800"
		"-window_height 600"
		"-is_compress_backup_file true"
		"-latest_version 18.635.6280.13"
		"-is_prompt_if_error true"
		"-server_address 111.229.98.137"
		"-server_port 60682"
		"-upload_update_record_flag 0"
		"-window_style 1"
		"-is_delete_old_version_backup_file false"
		"-upload_update_log_max_size 1"
		/*"-is_wait_old_version_exit true"
		"-wait_old_version_exit_timeout 10000"*/
		;
	

	//lpCmdLine = 
	//	"-kill_old_version_process_flag 1"
	//	"-is_wait_old_version_exit true"
	//	"-wait_old_version_exit_timeout 10000"
	//	"-old_version_process_id 0"
	//	"-old_version_process_name notepad.exe"
	//	"-is_only_start_process true"
	//	//"-start_process_file_path c:\\users\\user\\desktop\\MES查询端-Setup.exe"
	//	"-start_process_file_path c:/windows/system32/notepad.exe"
	//	"-start_process_command_line c:/users/user/desktop/新建文本文档.txt"
	//	"-is_prompt_if_error true"
	//	"-is_hide_window true";
#endif
	if (!strcmp(lpCmdLine, "")) {
		return FALSE;
	}

	if (!strncmp(lpCmdLine, "-help", strlen("-help"))) {
		MessageBox(nullptr, 
			"-option [查看选项]\n"
			"-default_sensitive_dll [查看默认敏感的DLL，可绕过杀毒软件拦截]\n"
			"-delete_all_backup_file [删除所有备份文件]\n",
			"提示",
			MB_OK | MB_ICONINFORMATION);
		return TRUE;
	}
	else if (!strncmp(lpCmdLine, "-option", strlen("-option"))) {
		std::string str;
		std::vector<std::string> option_arr =  get_cmdline_option_arr();
		std::vector<std::string> option_comment_arr = get_cmdline_option_comment_arr();
		char temp[256] = { 0 };
		if (option_arr.size() == option_comment_arr.size()) {
			for (size_t i = 0; i < option_arr.size(); ++i) {
				sprintf_s(temp, "选项：%s，注解：%s。\n", option_arr[i].c_str(), option_comment_arr[i].c_str());
				str.append(temp);
			}
		}
		else {
			for (size_t i = 0; i < option_arr.size(); ++i) {
				sprintf_s(temp, "选项：%s。\n", option_arr[i].c_str());
				str.append(temp);
			}
		}
		MessageBox(nullptr, str.c_str(), "提示", MB_OK | MB_ICONINFORMATION);
		return TRUE;
	}
	else if (!strncmp(lpCmdLine, "-default_sensitive_dll", strlen("-default_sensitive_dll"))) {
		std::string str("默认敏感的DLL：\n");
		std::vector<std::string> result = get_often_intercepted_dll_name();
		for (size_t i = 0; i < result.size(); ++i) {
			str.append(format_str("%s\t", result[i].c_str()));
		}
		MessageBox(nullptr, str.c_str(), "提示", MB_OK | MB_ICONINFORMATION);
		return TRUE;
	}
	else if (!strncmp(lpCmdLine, "-delete_all_backup_file", strlen("-delete_all_backup_file"))) {
		char localappdata_path[8192] = { 0 };
		if (SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, localappdata_path) != S_OK) {
			return FALSE;
		}
		std::string dir_path = format_str("%s\\imp_updater_backup", localappdata_path);
		if (_access(dir_path.c_str(), 0) != -1) {
			remove_folder(dir_path);
		}
		return TRUE;
	}
	else {
		cmdline_option = parse_command_line_option(lpCmdLine);
		if (!cmdline_option.is_only_start_process) {
			if (cmdline_option.old_version_process_name.empty() ||
				cmdline_option.source_path.empty() ||
				cmdline_option.destination_path.empty()) {
					return TRUE;
			}
		}
		else {
			if (cmdline_option.start_process_file_path.empty()) {
				return TRUE;
			}
			//只启动进程，将强制隐藏窗口
			cmdline_option.is_hide_window = true;
		}
	}

	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_IMP_UPDATER, szWindowClass, MAX_LOADSTRING);
	if (!cmdline_option.window_title.empty()) {
		// 避免缓冲区溢出，导致程序崩溃。
		if (cmdline_option.window_title.length() <= MAX_LOADSTRING -1) {
			strcpy_s(szTitle, cmdline_option.window_title.c_str());
		}
	}
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow)) {
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IMP_UPDATER));

	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	if (cmdline_option.window_style == 0) {
		WNDCLASSEX wcex= { 0 };

		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= WndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= hInstance;
		wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IMP_UPDATER));
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		//wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_IMP_UPDATER);
		wcex.lpszClassName	= szWindowClass;
		wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

		return RegisterClassEx(&wcex);
	}
	else {
		WNDCLASS wc = { 0 };
		wc.lpfnWndProc = WndProc;
		wc.hInstance = hInstance;
		wc.lpszClassName = szWindowClass;
		wc.hbrBackground = NULL;  // No background brush

		return RegisterClass(&wc);
	}
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中

	if (cmdline_option.window_style == 0) {
		// 获取屏幕工作区的尺寸
		RECT screenRect;
		GetWindowRect(GetDesktopWindow(), &screenRect);
		int screenWidth = screenRect.right - screenRect.left;
		int screenHeight = screenRect.bottom - screenRect.top;
		int windowW = cmdline_option.window_width;
		int windowH = cmdline_option.window_height;
		// 计算窗口的左上角位置，使其位于屏幕中央
		int windowX = (screenWidth - windowW) / 2;
		int windowY = (screenHeight - windowH) / 2;
		hWnd = CreateWindowEx(0, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX,
			windowX, windowY, windowW, windowH, NULL, NULL, hInstance, NULL);
		if (!hWnd) {
			return FALSE;
		}

		hStatic = CreateWindowEx(
			WS_EX_STATICEDGE,                       // 扩展窗口样式
			"STATIC",                               // 预定义的“STATIC”类
			"正在更新中，请耐心等待……",             // 控件的文本
			WS_CHILD | WS_VISIBLE | SS_CENTER,        // 子窗口、可见、左对齐的文本
			10, 10, windowW - 40, 20,                         // 控件的位置和大小
			hWnd,                                   // 父窗口句柄
			(HMENU)IDC_STATIC_STATUS,               // 无ID，可以使用GetDlgItem获取
			hInstance,                              // 应用程序实例句柄
			NULL                                   // 无创建数据
			);
		if  (!hStatic) {
			return FALSE;
		}

		hListBox = CreateWindowEx(
			WS_EX_STATICEDGE,           // 扩展样式
			"LISTBOX",                  // 预定义类名
			"",                         // 控件名
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | LBS_NOTIFY, // 样式
			10, 40, windowW - 40, windowH - 90,           // 位置和大小
			hWnd,                       // 父窗口句柄
			(HMENU)IDC_LISTBOX_LOG,     // 控件 ID
			hInstance, // 实例句柄
			NULL// 附加数据
			);
		if (!hListBox) {
			return FALSE;
		}
	}
	else {
		// Get screen dimensions
		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);

		// Calculate window position (center of the screen)
		int windowWidth = 200;
		int windowHeight = 250;  // Increased height to accommodate text
		int xPos = (screenWidth - windowWidth) / 2;
		int yPos = (screenHeight - windowHeight) / 2;

		hWnd = CreateWindowEx(
			WS_EX_LAYERED,          // Extended window style for layered window
			szWindowClass,             // Window class
			szTitle,              // Window title
			WS_POPUP,               // Window style
			xPos, yPos, windowWidth, windowHeight, // Size and position
			NULL,                   // Parent window    
			NULL,                   // Menu
			hInstance,              // Instance handle
			NULL                    // Additional application data
			);
		// Set the window transparency
		SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
		SetTimer(hWnd, TIMER_ID, 100, NULL);
	}
	

	// 获取窗口的当前扩展样式
	DWORD dwExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);

	// 清除窗口的 WS_EX_APPWINDOW 样式，并设置 WS_EX_TOOLWINDOW 样式
	dwExStyle &= ~(WS_EX_APPWINDOW);
	dwExStyle |= WS_EX_TOOLWINDOW;
	// 更新窗口的扩展样式
	SetWindowLong(hWnd, GWL_EXSTYLE, dwExStyle);

	ShowWindow(hWnd, SW_HIDE);

	UpdateWindow(hWnd);
	SetWindowLongPtr(hWnd, GWL_STYLE, GetWindowLongPtr(hWnd, GWL_STYLE) & ~WS_THICKFRAME);
	if (cmdline_option.is_show_topmost) {
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
	update_app();
	return TRUE;
}

#include <math.h>
void DrawLoadingAnimation(HDC hdc, int width, int height, int frame) {
	int radius = min(width, height - 50) / 4; // Adjust radius for the text space
	int cx = width / 2;
	int cy = (height - 50) / 2; // Adjust center position for the text space
	int dotRadius = radius / 5;

	static COLORREF colors[12] = {
		RGB(255, 0, 0), RGB(255, 165, 0), RGB(255, 255, 0),
		RGB(0, 128, 0), RGB(0, 0, 255), RGB(75, 0, 130),
		RGB(238, 130, 238), RGB(255, 105, 180), RGB(255, 20, 147),
		RGB(138, 43, 226), RGB(0, 255, 255), RGB(128, 0, 128)
	};

	for (int i = 0; i < 12; i++) {
		double angle = (i * 30 + frame * 30) * PI / 180;
		int x = (int)(cx + radius * cos(angle));
		int y = (int)(cy + radius * sin(angle));

		HBRUSH brush = CreateSolidBrush(colors[i]);
		HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
		Ellipse(hdc, x - dotRadius, y - dotRadius, x + dotRadius, y + dotRadius);
		SelectObject(hdc, oldBrush);
		DeleteObject(brush);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int frame = 0;
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message)
	{
	case WM_CREATE:
#if CONSOLE_DEBUG
		AllocConsole();
		SetConsoleTitle("imp updater debug");
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
#endif
		open_log_file();
		break;
	case WM_USER_UPDATE_STATUS:
		SetWindowText(hStatic, (const char*)lParam);
		break;
	case WM_USER_HIDE_WINDOW:
		ShowWindow(hWnd, SW_HIDE);
		break;
	case WM_USER_SHOW_WINDOW:
		ShowWindow(hWnd, SW_SHOW);
		break;
	case WM_COMMAND:
		break;
	case WM_PAINT:
		if (cmdline_option.window_style == 0) {
			hdc = BeginPaint(hWnd, &ps);
			// TODO: 在此添加任意绘图代码...
			EndPaint(hWnd, &ps);
		}
		else {
			//PAINTSTRUCT ps;
			//HDC hdc = BeginPaint(hWnd, &ps);

			//RECT rect;
			//GetClientRect(hWnd, &rect);
			//int width = rect.right - rect.left;
			//int height = rect.bottom - rect.top;

			//// Draw the loading animation
			//DrawLoadingAnimation(hdc, width, height, frame);

			//// Draw the text
			//char text[64] = { 0 };
			//sprintf_s(text, "更新中(%d%%)", uInstallProgress);
			//SetBkMode(hdc, TRANSPARENT);
			//SetTextColor(hdc, RGB(255, 255, 255));

			//HFONT hFont = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
			//	OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			//	DEFAULT_PITCH | FF_SWISS, "Arial");
			//HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

			//SIZE textSize;
			//GetTextExtentPoint32(hdc, text, strlen(text), &textSize);

			//int textX = (width - textSize.cx) / 2 + 5;
			//int textY = height - textSize.cy - 50; // Position the text near the bottom

			//TextOut(hdc, textX, textY, text, strlen(text));

			//SelectObject(hdc, oldFont);
			//DeleteObject(hFont);

			//EndPaint(hWnd, &ps);

			static int index = 0;
			static COLORREF colors[] = { 
				RGB(255, 0, 0), RGB(255, 165, 0), RGB(255, 255, 0),
				RGB(0, 128, 0), RGB(0, 0, 255), RGB(75, 0, 130),
				RGB(238, 130, 238), RGB(255, 105, 180), RGB(255, 20, 147),
				RGB(138, 43, 226), RGB(0, 255, 255), RGB(128, 0, 128)
			};

			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);

			RECT rect;
			GetClientRect(hWnd, &rect);
			int width = rect.right - rect.left;
			int height = rect.bottom - rect.top;

			// Draw the loading animation
			DrawLoadingAnimation(hdc, width, height, frame);

			// Draw the text
			char text[64] = { 0 };
			sprintf_s(text, "更新中%d%%", uInstallProgress);
			SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, colors[(index%=12)++]);

			HFONT hFont = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
				OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_SWISS, "Arial");
			HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

			SIZE textSize;
			GetTextExtentPoint32(hdc, text, strlen(text), &textSize);

			int textX = (width - textSize.cx) / 2 + 5;
			int textY = height - textSize.cy - 50; // Position the text near the bottom

			// Clear the previous text area
			HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
			rect.left = textX;
			rect.top = textY;
			rect.right = rect.left + width / 2;
			rect.bottom = rect.top + 20;
			FillRect(hdc, &rect, hBrush); // This line clears the previous text with a black rectangle
			// You can change RGB(0, 0, 0) to the color that matches your background

			TextOut(hdc, textX, textY, text, strlen(text));

			SelectObject(hdc, oldFont);
			DeleteObject(hFont);
			DeleteObject(hBrush);

			EndPaint(hWnd, &ps);
		}
		
		break;
	case WM_CLOSE:
		if (!bIsUpgradeFinish) {
			if (MessageBox(nullptr, "当前正在更新中，你是否要终止更新？终止更新将会导致升级失败！",
				"询问", MB_ICONQUESTION | MB_YESNO) == IDYES) {
				LogWarn("用户终止更新。");
				DestroyWindow(hWnd);
			}
		}
		else {
			DestroyWindow(hWnd);
		}
		break;
	case WM_DESTROY:
#if CONSOLE_DEBUG
		FreeConsole();
#endif
		close_log_file();
		PostQuitMessage(0);
		break;
	case WM_TIMER:
		{
		if (wParam == TIMER_ID) {
			frame = (frame + 1) % 12; // Update frame
			InvalidateRect(hWnd, NULL, TRUE); // Redraw the window
		}
		} 
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}
