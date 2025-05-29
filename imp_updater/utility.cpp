#include "stdafx.h"
#include "utility.h"

extern void LogInfo(const std::string& str);
extern void LogInfo(const char* fmt, ...);
extern void LogWarn(const std::string& str);
extern void LogWarn(const char* fmt, ...);
extern void LogError(const std::string& str);
extern void LogError(const char* fmt, ...);
extern void ChangeStatusText(const char* fmt, ...);
extern HWND hWnd;
extern BOOL bIsUpgradeFinish;
extern UINT uInstallProgress;

//命令行选项
static const char* cmdline_option_arr[] = {
	"-old_version_process_id",//1
	"-old_version_process_name",//2
	"-source_path",//3
	"-destination_path",//4
	"-is_create_shortcut",//5
	"-is_recursion_replace_shortcut",//6
	"-is_delete_update_file",//7
	"-start_process_command_line",//8
	"-is_hide_window",//9
	"-is_show_topmost",//10
	"-is_backup_file",//11
	"-window_title",//12
	"-window_width",//13
	"-window_height",//14
	"-is_compress_backup_file",//15
	"-sensitive_dll",//16
	"-latest_version",//17
	"-is_only_start_process",//18
	"-start_process_file_path",//19
	"-start_process_working_directory",//20
	"-is_prompt_if_error",//21
	"-server_address",//22
	"-server_port",//23
	"-upload_update_record_flag",//24
	"-window_style",//25
	"-is_delete_old_version_backup_file",//26
	"-is_wait_old_version_exit",//27
	"-wait_old_version_exit_timeout",//28
	"-kill_old_version_process_flag",//29
	"-upload_update_log_max_size",//30
	nullptr//N - 1
};

//命令行选项注释
static const char* cmdline_option_comment_arr[] = {
	"旧版本进程ID",//1
	"旧版本进程名",//2
	"源路径（更新的文件夹路径）",//3
	"目标路径（需要更新的文件夹路径）",//4
	"是否创建快捷方式",//5
	"是否递归替换快捷方式（遍历整个桌面，替换所有快捷方式）",//6
	"是否删除更新文件",//7
	"启动进程命令行参数",//8
	"是否隐藏窗口",//9
	"是否将窗口置顶",//10
	"是否备份文件",//11
	"窗口标题（仅窗口样式0时生效）",//12
	"窗口宽度（仅窗口样式0时生效）",//13
	"窗口高度（仅窗口样式0时生效）",//14
	"是否压缩备份文件（仅备份文件时生效）",//15
	"敏感的动态链接库（将会绕过杀毒软件拦截，格式（a.dll;b.dll））",//16
	"最新版本号（备份文件时会使用到）",//17
	"是否只启动进程（如果为true，窗口将会被强制隐藏）",//18
	"启动进程文件路径（仅在开启只启动进程时生效）",//19
	"启动进程工作路径（仅在开启只启动进程时生效）",//20
	"如果错误，是否弹窗提示（仅在隐藏窗口和窗口样式为1时生效）",//21
	"服务器地址（用于上传更新记录）",//22
	"服务器端口（用于上传更新记录）",//23
	"上传更新记录标志（0：不上传，1：不包含更新日志，2：如果失败包含更新日志，3：包含更新日志）",//24
	"窗口样式（0：常规窗口样式，1：动画样式）",//25
	"是否删除旧版本备份文件",//26
	"是否等待旧版本退出",//27
	"等待旧版本退出超时",//28
	"杀死旧版本进程标志（0：根据进程ID进行关闭，1：根据进程名进行关闭）",//29
	"上传更新日志最大大小（单位：MB，默认5MB）",//30
	nullptr//N - 1
};

//经常过滤的文件后缀
//static const char* often_filter_file_suffix[] = {
//	".log", //1
//	".txt", //2
//	".csv", //3
//	".tmp", //4
//	".dump", //5
//	".cache", //6
//	".bak", //7
//	".jpg", //8
//	".jpeg", //9
//	".bmp", //10
//	".png", //11
//	".gif", //12
//	".tiff", //13
//	".svg", //14
//	".ico", //15
//	".raw", //16
//	".pcx", //17
//	".ppm", //18
//	".psd", //19
//	".mp3", //20
//	".wav", //21
//	".mp4", //22
//	".avi", //23
//	nullptr //N -1
//};

//经常拦截的dll名字
static const char* often_intercepted_dll_name[] = {
	//INVO
	"invo_mes_plugin.dll",
	"invo_mes_plugin_x64.dll",
	"libcrypto-3.dll",
	"libssl-3.dll",
	"libqui.dll",
	//QT
	"qsvgicon.dll",
	"qgif.dll",
	"qicns.dll",
	"qico.dll",
	"qjpeg.dll",
	"qsvg.dll",
	"qtga.dll",
	"qtiff.dll",
	"qwbmp.dll",
	"qwebp.dll",
	"qwindows.dll",
	"qwindowsvistastyle.dll",
	"Qt5Core.dll",
	"Qt5Gui.dll",
	"Qt5Svg.dll",
	"Qt5Widgets.dll",
	"vcruntime140.dll",
	"libGLESv2.dll",
	"libEGL.dll",
	"opengl32sw.dll",
	"D3Dcompiler_47.dll",
	"ucrtbase.dll",
	//MSVC
	"api-ms-win-core-console-l1-1-0.dll",
	"api-ms-win-core-datetime-l1-1-0.dll",
	"api-ms-win-core-debug-l1-1-0.dll",
	"api-ms-win-core-errorhandling-l1-1-0.dll",
	"api-ms-win-core-file-l1-1-0.dll",
	"api-ms-win-core-file-l1-2-0.dll",
	"api-ms-win-core-file-l2-1-0.dll",
	"api-ms-win-core-handle-l1-1-0.dll",
	"api-ms-win-core-heap-l1-1-0.dll",
	"api-ms-win-core-heap-l2-1-0.dll",
	"api-ms-win-core-interlocked-l1-1-0.dll",
	"api-ms-win-core-libraryloader-l1-1-0.dll",
	"api-ms-win-core-libraryloader-l1-2-0.dll",
	"api-ms-win-core-localization-l1-2-0.dll",
	"api-ms-win-core-memory-l1-1-0.dll",
	"api-ms-win-core-namedpipe-l1-1-0.dll",
	"api-ms-win-core-processenvironment-l1-1-0.dll",
	"api-ms-win-core-processthreads-l1-1-0.dll",
	"api-ms-win-core-processthreads-l1-1-1.dll",
	"api-ms-win-core-processtopology-obsolete-l1-1-0.dll",
	"api-ms-win-core-profile-l1-1-0.dll",
	"api-ms-win-core-rtlsupport-l1-1-0.dll",
	"api-ms-win-core-shlwapi-legacy-l1-1-0.dll",
	"api-ms-win-core-string-l1-1-0.dll",
	"api-ms-win-core-synch-l1-1-0.dll",
	"api-ms-win-core-synch-l1-2-0.dll",
	"api-ms-win-core-sysinfo-l1-1-0.dll",
	"api-ms-win-core-timezone-l1-1-0.dll",
	"api-ms-win-core-util-l1-1-0.dll",
	"api-ms-win-crt-conio-l1-1-0.dll",
	"api-ms-win-crt-convert-l1-1-0.dll",
	"api-ms-win-crt-environment-l1-1-0.dll",
	"api-ms-win-crt-filesystem-l1-1-0.dll",
	"api-ms-win-crt-heap-l1-1-0.dll",
	"api-ms-win-crt-locale-l1-1-0.dll",
	"api-ms-win-crt-math-l1-1-0.dll",
	"api-ms-win-crt-multibyte-l1-1-0.dll",
	"api-ms-win-crt-runtime-l1-1-0.dll",
	"api-ms-win-crt-stdio-l1-1-0.dll",
	"api-ms-win-crt-string-l1-1-0.dll",
	"api-ms-win-crt-time-l1-1-0.dll",
	"api-ms-win-crt-utility-l1-1-0.dll",
	nullptr
};

//#define MAX_UPLOAD_UPDATE_LOG_SIZE (1024 * 1024 * 5)

std::string last_error;
static FILE* log_fileptr = nullptr;
static std::string log_filepath;
static std::string log_filetext;

_cmdline_option::_cmdline_option() {
	is_create_shortcut = true;
	is_recursion_replace_shortcut = false;
	is_delete_update_file = true;
	is_hide_window = false;
	is_show_topmost = false;
	is_backup_file = true;
	is_compress_backup_file = false;
	is_only_start_process = false;
	is_prompt_if_error = true;

	old_version_process_id = 0;
	window_width = 500;
	window_height = 350;

	latest_version = "0.0.0.0";
	server_address = "www.invomes.com";
	server_port = 60682;
	upload_update_record_flag = 1;
	window_style = 1;
	is_delete_old_version_backup_file = false;
	is_wait_old_version_exit = false;
	wait_old_version_exit_timeout = 3000;
	kill_old_version_process_flag = 0;
	upload_update_log_max_size = 5;
}

_cmdline_option::~_cmdline_option() {

}

cmdline_option_t cmdline_option;
std::set<std::string> sensitive_dll_set;

static bool replace_with_windows_path_symbol_cb(char ch)
{
	return ch == '/';
}

void replace_with_windows_path_symbol(std::string& filepath)
{
	std::replace_if(filepath.begin(), filepath.end(), replace_with_windows_path_symbol_cb, '\\');
}

static bool replace_with_linux_path_symbol_cb(char ch) {
	return ch == '\\';
}

void replace_with_linux_path_symbol(std::string& filepath)
{
	std::replace_if(filepath.begin(), filepath.end(), replace_with_linux_path_symbol_cb, '/');
}

std::string format_error(uint32_t error_code) 
{
	const int bufferSize = 8192; // 可以根据需要调整缓冲区大小
	std::vector<char> buffer(bufferSize);

	// FormatMessage() 函数用于格式化一个消息字符串
	// FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS 用于指定从系统消息表中获取字符串，并忽略插入序列
	DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
	if (FormatMessage(flags, nullptr, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		&buffer.front(), bufferSize, nullptr) == 0) {
			// 如果 FormatMessage() 失败，则返回默认的错误消息
			return "Unknown error";
	}

	// 去除结尾的换行符
	if (!buffer.empty() && buffer.back() == '\n') {
		buffer.pop_back();
	}

	if (!buffer.empty() && buffer.back() == '\r') {
		buffer.pop_back();
	}

	return std::string(&buffer.front());
}

void trim_str(std::string& str) {
	if (str.empty()) return;
	str.erase(0, str.find_first_not_of(' '));
	str.erase(str.find_last_not_of(' ') + 1);
}

std::list<std::string>::iterator find_str_list(std::list<std::string>& ls, const std::string& str, bool case_insensitive)
{
	std::list<std::string>::iterator iter = ls.end();
	for (iter = ls.begin(); iter != ls.end(); ++iter) {
		if (case_insensitive) {
			if (!_stricmp((*iter).c_str(), str.c_str())) {
				break;
			}
		}
		else {
			if (!strcmp((*iter).c_str(), str.c_str())) {
				break;
			}
		}
	}
	return iter;
}

std::set<std::string>::iterator find_str_set(std::set<std::string>& ss, const std::string& str, bool case_insensitive)
{
	std::set<std::string>::iterator iter = ss.end();
	for (iter = ss.begin(); iter != ss.end(); ++iter) {
		if (case_insensitive) {
			if (!_stricmp((*iter).c_str(), str.c_str())) {
				break;
			}
		}
		else {
			if (!strcmp((*iter).c_str(), str.c_str())) {
				break;
			}
		}
	}
	return iter;
}

std::vector<std::string>::iterator find_str_vector(std::vector<std::string>& sv, const std::string& str, bool case_insensitive)
{
	std::vector<std::string>::iterator iter = sv.end();
	for (iter = sv.begin(); iter != sv.end(); ++iter) {
		if (case_insensitive) {
			if (!_stricmp((*iter).c_str(), str.c_str())) {
				break;
			}
		}
		else {
			if (!strcmp((*iter).c_str(), str.c_str())) {
				break;
			}
		}
	}
	return iter;
}

time_t get_time_stamp_s()
{
	return time(nullptr);
}

time_t get_time_stamp_ms()
{
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	return time_t(nullptr) * 1000 + st.wMilliseconds;
}

std::vector<std::string> get_cmdline_option_arr()
{
	std::vector<std::string> cmdline_option_list;
	for (int i = 0; i < sizeof(cmdline_option_arr) / sizeof(*cmdline_option_arr); ++i) {
		if (cmdline_option_arr[i]) {
			cmdline_option_list.push_back(cmdline_option_arr[i]);
		}
	}
	return cmdline_option_list;
}

std::vector<std::string> get_cmdline_option_comment_arr()
{
	std::vector<std::string> cmdline_option_comment_list;
	for (int i = 0; i < sizeof(cmdline_option_comment_arr) / sizeof(*cmdline_option_comment_arr); ++i) {
		if (cmdline_option_comment_arr[i]) {
			cmdline_option_comment_list.push_back(cmdline_option_comment_arr[i]);
		}
	}
	return cmdline_option_comment_list;
}

std::vector<std::string> get_often_intercepted_dll_name()
{
	std::vector<std::string> result;
	for (int i = 0; often_intercepted_dll_name[i]; ++i) {
		result.push_back(often_intercepted_dll_name[i]);
	}
	return result;
}

void open_log_file()
{
	if (cmdline_option.is_only_start_process) {
		return;
	}

	std::string dirpath = get_current_path() + "\\install_log";
	if (_access(dirpath.c_str(), 0) != 0) {
		_mkdir(dirpath.c_str());
	}

	char filename[256]= { 0 };
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(filename, "%04d%02d%02d%02d%02d%02d.log", 
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	std::string filepath = dirpath + "\\" + filename;
	log_fileptr = fopen(filepath.c_str(), "wb");
}

void write_log(const char* fmt, ...)
{
	if (!log_fileptr) {
		return;
	}

	va_list ap;
	va_start(ap, fmt);
	int len = _vscprintf_p(fmt, ap) + 1;
	char* buf = new char[len];
	_vsprintf_p(buf, len, fmt, ap);
	va_end(ap);
	std::string str(buf);
	delete[] buf;

	char temp[256] = { 0 };
	SYSTEMTIME st= { 0 };
	GetLocalTime(&st);
	sprintf_s(temp, "%04d-%02d-%02d %02d:%02d:%02d.%03d ",
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	str.insert(0, temp);
	fprintf(log_fileptr, "%s\n", str.c_str());
	fflush(log_fileptr);

	//"上传更新记录标志（0：不上传，1：不包含更新日志，2：如果失败包含更新日志，3：包含更新日志）",
	if (cmdline_option.upload_update_record_flag > 1) {
		if (log_filetext.size() < (size_t)cmdline_option.upload_update_log_max_size) {
			replace_with_linux_path_symbol(str);
			log_filetext.append(str).append("\n");
		}
	}
}

void write_log_type(const char* type, const std::string& str)
{
	if (!log_fileptr) {
		return;
	}

	char temp[256] = { 0 };
	SYSTEMTIME st= { 0 };
	GetLocalTime(&st);
	sprintf_s(temp, "%04d-%02d-%02d %02d:%02d:%02d.%03d [%s] ",
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute,
		st.wSecond, st.wMilliseconds, type);

	std::string buf(temp);
	buf.append(str);
	fprintf(log_fileptr, "%s\n", buf.c_str());
	fflush(log_fileptr);

	//"上传更新记录标志（0：不上传，1：不包含更新日志，2：如果失败包含更新日志，3：包含更新日志）",
	if (cmdline_option.upload_update_record_flag > 1) {
		if (log_filetext.size() < (size_t)cmdline_option.upload_update_log_max_size) {
			replace_with_linux_path_symbol(buf);
			log_filetext.append(buf).append("\n");
		}
	}
}

void write_log_type_fmt(const char* type, const char* fmt, ...)
{
	if (!log_fileptr) {
		return;
	}

	va_list ap;
	va_start(ap, fmt);
	int len = _vscprintf_p(fmt, ap) + 1;
	char* buf = new char[len];
	_vsprintf_p(buf, len, fmt, ap);
	va_end(ap);
	std::string str(buf);
	delete[] buf;

	char temp[256] = { 0 };
	SYSTEMTIME st= { 0 };
	GetLocalTime(&st);
	sprintf_s(temp, "%04d-%02d-%02d %02d:%02d:%02d.%03d [%s] ",
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, type);
	str.insert(0, temp);
	fprintf(log_fileptr, "%s\n", str.c_str());
	fflush(log_fileptr);

	//"上传更新记录标志（0：不上传，1：不包含更新日志，2：如果失败包含更新日志，3：包含更新日志）",
	if (cmdline_option.upload_update_record_flag > 1) {
		if (log_filetext.size() < (size_t)cmdline_option.upload_update_log_max_size) {
			replace_with_linux_path_symbol(str);
			log_filetext.append(str).append("\n");
		}
	}
}

void close_log_file()
{
	if (log_fileptr) {
		fclose(log_fileptr);
	}
}

std::string format_str(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int len = _vscprintf_p(fmt, ap) + 1;
	char* buf = new char[len];
	_vsprintf_p(buf, len, fmt, ap);
	va_end(ap);
	std::string str(buf);
	delete[] buf;
	return str;
}

void set_last_error(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int len = _vscprintf_p(fmt, ap) + 1;
	char* buf = new char[len];
	_vsprintf_p(buf, len, fmt, ap);
	va_end(ap);
	last_error = buf;
	delete[] buf;
}

void set_last_error(const std::string& str)
{
	last_error = str;
}

std::string get_last_error()
{
	return last_error;
}

char* wideCharToMultiByte(const wchar_t* str, uint32_t code_page)
{
	int size = WideCharToMultiByte(code_page, 0, str, -1, NULL, 0, NULL, FALSE);
	char* buffer = new char[size];
	WideCharToMultiByte(code_page, 0, str, -1, buffer, size, NULL, FALSE);
	return buffer;
}

wchar_t* multiByteToWideChar(const char* str, uint32_t code_page)
{
	int size = MultiByteToWideChar(code_page, 0, str, -1, NULL, 0);
	wchar_t* buffer = new wchar_t[size];
	MultiByteToWideChar(code_page, 0, str, -1, buffer, size);
	return buffer;
}

std::string utf8_to_ansi(const std::string& utf8)
{
	std::string ret;
	if (utf8.empty()) {
		return ret;
	}

	wchar_t* wstr = multiByteToWideChar(utf8.c_str(), CP_UTF8);
	char* str = wideCharToMultiByte(wstr, CP_ACP);
	ret = str;
	delete[] wstr;
	delete[] str;
	return ret;
}

std::string ansi_to_utf8(const std::string& ansi)
{
	std::string ret;
	if (ansi.empty()) {
		return ret;
	}

	wchar_t* wstr = multiByteToWideChar(ansi.c_str(), CP_ACP);
	char* str = wideCharToMultiByte(wstr, CP_UTF8);
	ret = str;
	delete[] wstr;
	delete[] str;
	return ret;
}

bool is_all_digits(const char *str) {
	while (*str) {
		if (!isdigit((unsigned char)*str)) {
			return false;
		}
		str++;
	}
	return true;
}

void traversal_directory(const std::string& path, std::list<std::string>& files, bool recursion,
						filter_file_cb_t filter_file_cb, void* args)
{
	HANDLE hFind;
	WIN32_FIND_DATA ffd;
	std::string fullPath = path + "\\*";

	hFind = FindFirstFile(fullPath.c_str(), &ffd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			// 跳过"."和".."
			if (strcmp(ffd.cFileName, ".") != 0 && strcmp(ffd.cFileName, "..") != 0)
			{
				std::string newPath = path + "\\" + ffd.cFileName;

				if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					// 如果是目录，并且需要遍历子目录，则递归调用
					files.push_back(newPath);
					if (recursion) {
						traversal_directory(newPath, files, recursion, filter_file_cb, args);
					}
				}
				else {
					if (filter_file_cb) {
						if (!filter_file_cb(newPath, args)) {
							files.push_back(newPath);
						}
					}
					else{
						files.push_back(newPath);
					}
				}
			}
		} while (FindNextFile(hFind, &ffd));

		FindClose(hFind);
	}
}

//过滤 return true
//不过滤 return false
static bool filter_file_suffix_cb(const std::string& filepath, void* args)
{
	std::list<std::string>* filter_file = static_cast<std::list<std::string>*>(args);
	if (filter_file->empty()) {
		return false;
	}

	std::string suffix_name;
	size_t npos = filepath.rfind(".");
	if (npos == std::string::npos) {
		return false;
	}

	suffix_name = filepath.substr(npos);

	for (std::list<std::string>::iterator iter = filter_file->begin(); iter != filter_file->end(); ++iter) {
		if (!_stricmp((*iter).c_str(), suffix_name.c_str())) {
			return true;
		}
	}
	return false;
}

static std::string get_filename_by_filepath(const std::string& filepath, bool is_src_path) {
	std::string ret;
	if (is_src_path) {
		size_t npos = filepath.find_first_of(cmdline_option.source_path);
		if (npos != std::string::npos) {
			ret = filepath.substr(npos + cmdline_option.source_path.length() + 1);
		}
	}
	else {
		size_t npos = filepath.find_first_of(cmdline_option.destination_path);
		if (npos != std::string::npos) {
			ret = filepath.substr(npos + cmdline_option.destination_path.length() + 1);
		}
	}
	return ret;
}

static bool filter_file_name_cb(const std::string& filepath, void* args)
{
	std::list<std::string>* filter_file = static_cast<std::list<std::string>*>(args);
	if (filter_file->empty()) {
		return !false;
	}
	std::string fn0 = get_filename_by_filepath(filepath, false);
	for (std::list<std::string>::iterator iter = filter_file->begin(); iter != filter_file->end(); ++iter) {
		std::string fn1 = get_filename_by_filepath(*iter, true);
		if (_stricmp(fn0.c_str(), fn1.c_str()) == 0) {
			//printf("copy %s->%s\n", fn0.c_str(), fn1.c_str());
			return !true;
		}
		//printf("not copy %s->%s\n", fn0.c_str(), fn1.c_str());
	}
	return !false;
}

int miniz_compress_folder(const std::string& folder_path, const std::string& zip_filepath, 
	const std::list<std::string>& filter_file_name, progress_cb_t progress_cb, void* args) {
	if (_access(folder_path.c_str(), 0) == -1) {
		LogError("压缩文件夹%s路径无效。", folder_path.c_str());
		return -1;
	}

	mz_zip_archive zip_archive = { 0 };
	std::string filepath = ansi_to_utf8(zip_filepath);
	if (!mz_zip_writer_init_file(&zip_archive, filepath.c_str(), 0)) {
		LogError("zip文件%s路径无效。", filepath.c_str());
		return -2;
	}

	std::list<std::string> files;
	get_folder_files(folder_path, files, true, &filter_file_name_cb, (void*)&filter_file_name);

	int error_code = 0;
	for (std::list<std::string>::iterator iter = files.begin(); iter != files.end(); ++iter) {
		std::string filepath = *iter;
		do {
			if (progress_cb) {
				progress_cb(filepath, args);
			}
			
			//输出日志
			/*{
				size_t npos = filepath.rfind("\\");
				if (npos != std::string::npos) {
					filename = filepath.substr(npos + 1);
					LogInfo("正在压缩%s。", filename.c_str());
				}
			}*/
			LogInfo("正在压缩%s。", filepath.c_str());
			std::string relpath = filepath.substr(folder_path.length() + 1);
			relpath = ansi_to_utf8(relpath);

			FILE* file = nullptr;
			errno_t err =  fopen_s(&file, filepath.c_str(), "rb");
			if (!file) {
				mz_zip_writer_end(&zip_archive);
				LogError("读取压缩文件%s失败，错误原因%s。", filepath.c_str(), strerror(err));
				error_code = -3;
				break;
			}

			_fseeki64(file, 0, SEEK_END);
			long long file_size = _ftelli64(file);
			_fseeki64(file, 0, SEEK_SET);
			//超过1GB不压缩
			if (file_size > 1024 * 1024 * 1024) {
				fclose(file);
				continue;
			}

			void* file_data = malloc(static_cast<size_t>(file_size));
			if (!file_data) {
				fclose(file);
				mz_zip_writer_end(&zip_archive);
				LogError("内存不足，无法进行压缩文件%s。", filepath.c_str());
				error_code = -4;
				break;
			}
			fread(file_data, 1, static_cast<size_t>(file_size), file);
			fclose(file);

			mz_bool add_result = mz_zip_writer_add_mem(&zip_archive, relpath.c_str(), file_data,
				static_cast<size_t>(file_size), MZ_BEST_COMPRESSION);
			free(file_data);
			if (!add_result) {
				mz_zip_writer_end(&zip_archive);
				LogError("将压缩文件%s合并到内存失败。", filepath.c_str());
				error_code = -5;
				break;
			}
		} while (false);

		//输出日志
		/*{
			if (!filename.empty()) {
				LogInfo("压缩%s%s。", filename.c_str(), (error_code == 0) ? "成功" : "失败");
			}
		}*/
		//LogInfo("压缩%s%s。", filepath.c_str(), (error_code == 0) ? "成功" : "失败");
		if (error_code == 0) {
			LogInfo("压缩%s成功。", filepath.c_str());
		}
		else {
			LogError("压缩%s失败，错误代码：%d。", filepath.c_str(), error_code);
		}

		if (error_code != 0) {
			break;
		}
	}

	mz_zip_writer_finalize_archive(&zip_archive);
	mz_zip_writer_end(&zip_archive);
	return error_code;
}


#if ENABLE_LIBZIP
/*
* @retval 0 成功
* @retval -1 文件路径无效
* @retval -2 打开zip文件失败
* @retval -3 申请zip文件源失败
* @retval -4 添加zip目录源失败
* @retval -5 添加zip文件源失败
* @retval -6 设置文件压缩级别失败
* @retval -7 加密文件失败
* @retval -8 设置文件属性失败
* @retval -9 关闭zip文件失败
*/

static std::string get_libzip_compress_error(int error_code)
{
	std::string str;
	switch (error_code) {
	case 0:
		str = "成功";
		break;
	case -1:
		str = "文件路径无效";
		break;
	case -2:
		str = "打开zip文件失败";
		break;
	case -3:
		str = "申请zip文件源失败";
		break;
	case -4:
		str = "添加zip目录源失败";
		break;
	case -5:
		str = "添加zip文件源失败";
		break;
	case -6:
		str = "设置文件压缩级别失败";
		break;
	case -7:
		str = "加密文件失败";
		break;
	case -8:
		str = "设置文件属性失败";
		break;
	case -9:
		str = "关闭zip文件失败";
		break;
	default:
		str = "未知错误";
		break;
	}

	return str;
}


int libzip_compress(const std::string& compress_path, 
	const std::string& zip_path, 
	const std::string& password,
	const std::list<std::string>& filter_file_suffix,
	progress_cb_t progress_cb, 
	void* args) {
	int result = 0;
	zip_t* archive = nullptr;
	do
	{
		DWORD attrs = GetFileAttributes(compress_path.c_str());
		if (attrs == INVALID_FILE_ATTRIBUTES) {
			result = -1;
			break;
		}

		if (attrs & FILE_ATTRIBUTE_DIRECTORY) {
			std::string parent_path = compress_path;
			std::replace_if(parent_path.begin(), parent_path.end(), [](char ch) { return ch == '\\'; }, '/');
			if (parent_path.back() == '/') {
				parent_path.pop_back();
			}

			size_t npos = parent_path.find_last_of('/');
			if (npos == std::string::npos) {
				result = -1;
				break;
			}
			parent_path = parent_path.substr(0, npos);

			int error = 0;
			std::string utf8zip_path = ansi_to_utf8(zip_path);
			archive = zip_open(utf8zip_path.c_str(), ZIP_CREATE, &error);
			if (!archive) {
				result = -2;
				break;
			}

			std::list<std::string> files;
			files.push_back(compress_path);
			traversal_directory(compress_path, files, true, &filter_file_cb, (void*)&filter_file_suffix);
			bool success = true;
			for (std::list<std::string>::iterator it = files.begin(); it != files.end(); ++it) {
				std::replace_if(it->begin(), it->end(), [](char ch) { return ch == '\\'; }, '/');
				DWORD attrs = GetFileAttributes(it->c_str());
				zip_int64_t file_index = 0;
				std::string relative_path = ansi_to_utf8(it->substr(parent_path.length() + 1));

				if (attrs & FILE_ATTRIBUTE_DIRECTORY) {
					if (relative_path.back() != '/') {
						relative_path += "/";
					}

					if ((file_index = zip_dir_add(archive, relative_path.c_str(), ZIP_FL_OVERWRITE | ZIP_FL_ENC_UTF_8)) < 0) {
						result = -4;
						success = false;
						break;
					}
				}
				else {
					std::string filepath = ansi_to_utf8(*it);
					zip_source_t* source = zip_source_file(archive, filepath.c_str(), 0, 0);
					if (!source) {
						result = -3;
						success = false;
						break;
					}

					if ((file_index = zip_name_locate(archive, relative_path.c_str(), 0)) >= 0) {
						zip_delete(archive, file_index);
					}

					LogInfo("正在压缩%s……", it->c_str());
					if ((file_index = zip_file_add(archive, relative_path.c_str(), source, ZIP_FL_OVERWRITE | ZIP_FL_ENC_UTF_8)) < 0) {
						LogError("压缩%s失败。", it->c_str());
						result = -5;
						zip_source_free(source);
						success = false;
						break;
					}
					LogInfo("压缩%s成功。", it->c_str());

					if (zip_set_file_compression(archive, file_index, ZIP_CM_DEFLATE, 6) != 0) {
						result = -6;
						success = false;
						break;
					}

					if (!password.empty() && zip_file_set_encryption(archive, file_index, ZIP_EM_AES_256, password.c_str()) != 0) {
						result = -7;
						success = false;
						break;
					}

					if (progress_cb)
						progress_cb(*it, args);
				}

				if (zip_file_set_external_attributes(archive, file_index, 0, ZIP_OPSYS_WINDOWS_NTFS, attrs) != 0) {
					result = -8;
					success = false;
					break;
				}
			}

			if (!success) {
				break;
			}
		}
		else {
			int error = 0;
			std::string utf8dst_filepath = ansi_to_utf8(zip_path);
			archive = zip_open(utf8dst_filepath.c_str(), ZIP_CREATE, &error);
			if (!archive) {
				result = -2;
				break;
			}

			std::string utf8src_filepath = ansi_to_utf8(compress_path);
			zip_source_t* source = zip_source_file(archive, utf8src_filepath.c_str(), 0, 0);
			if (!source) {
				result = -3;
				break;
			}

			zip_uint64_t file_index = 0;
			size_t npos = utf8src_filepath.find_last_of('/');
			if (npos == std::string::npos) {
				result = -1;
				break;
			}

			std::string relative_path = utf8src_filepath.substr(npos + 1);
			if ((file_index = zip_file_add(archive, relative_path.c_str(), source, ZIP_FL_OVERWRITE | ZIP_FL_ENC_UTF_8)) < 0) {
				result = -5;
				zip_source_free(source);
				break;
			}

			if (zip_set_file_compression(archive, file_index, ZIP_CM_DEFLATE, 6) != 0) {
				result = -6;
				break;
			}

			if (!password.empty() && zip_file_set_encryption(archive, file_index, ZIP_EM_AES_256, password.c_str()) != 0) {
				result = -7;
				break;
			}

			if (zip_file_set_external_attributes(archive, file_index, 0, ZIP_OPSYS_WINDOWS_NTFS, attrs) != 0) {
				result = -8;
				break;
			}
		}

	} while (false);
	if (archive && zip_close(archive) != 0) {
		result = -9;
	}

	if (result != 0) {
		LogError(get_zip_compress_error(result));
	}
	return result;
}
#endif

int miniz_decompress_file(const std::string& zip_filepath, const std::string& extract_path)
{
	mz_zip_archive zip_archive = { 0 };
	std::string filepath = ansi_to_utf8(zip_filepath);
	if (!mz_zip_reader_init_file(&zip_archive, filepath.c_str(), 0)) {
		LogError("zip文件%s无效。", zip_filepath.c_str());
		return -1;
	}

	int error_code = 0;
	mz_uint num_files = mz_zip_reader_get_num_files(&zip_archive);
	for (mz_uint i = 0; i < num_files; i++) {
		mz_zip_archive_file_stat file_stat;
		std::string file_path;
		do  {
			file_path.clear();
			if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) {
				LogError("获取解压文件信息失败。");
				error_code = -4;
				break;
			}

			if (mz_zip_reader_is_file_a_directory(&zip_archive, i)) {
				continue;
			}

			// 如果是文件夹，创建相应的目录
			std::string file_name = utf8_to_ansi(file_stat.m_filename);
			file_path = extract_path + "/" + file_name;
			for (size_t i = 0; i < file_path.length(); ++i) {
				if (file_path[i] == '/') {
					file_path[i] = '\\';
				}
			}

			size_t npos = file_path.rfind("\\");
			if (npos != std::string::npos) {
				std::string dir_path = file_path.substr(0, npos);
				if (!create_path(dir_path)) {
					LogError("创建提取文件夹%s失败。", dir_path.c_str());
					error_code = -2;
					break;
				}
			}

			LogInfo("正在解压缩%s。", file_path.c_str());
			// 如果是文件，解压缩到目标路径
			void* p = mz_zip_reader_extract_file_to_heap(&zip_archive, file_stat.m_filename, (size_t*)&file_stat.m_uncomp_size, 0);
			if (!p) {
				LogError("提取解压文件%s到内存失败。", file_path.c_str());
				error_code = -5;
				break;
			}

			bool is_write_try_again = true;
			//DWORD old_attrs;
		WRITE_TRY_AGAIN:
			FILE* file = nullptr;
			errno_t err = fopen_s(&file, file_path.c_str(), "wb");
			if (!file) {
				if (err == EACCES && is_write_try_again) {
					DWORD attrs = GetFileAttributes(file_path.c_str());
					if (attrs != INVALID_FILE_ATTRIBUTES) {
						DWORD set = attrs;
						if (attrs & FILE_ATTRIBUTE_HIDDEN) {
							set &= ~FILE_ATTRIBUTE_HIDDEN;
						}

						if (attrs & FILE_ATTRIBUTE_READONLY) {
							set &= ~FILE_ATTRIBUTE_READONLY;
						}
						SetFileAttributes(file_path.c_str(), set);
					}
					is_write_try_again = false;
					goto WRITE_TRY_AGAIN;
				}

				mz_free(p);
				LogError("写入解压文件%s失败，错误原因%s。", file_path.c_str(), strerror(err));
				error_code = -3;
				break;
			}

			size_t write_ret = fwrite(p, 1, (size_t)file_stat.m_uncomp_size, file);
			if (write_ret != (size_t)file_stat.m_uncomp_size) {
				mz_free(p);
				fclose(file);
				LogError("写入解压文件%s失败，校验大小不一致。", file_path.c_str());
				error_code = -3;
				break;
			}
			fclose(file);
			mz_free(p);
		} while (false);

		if  (!file_path.empty()) {
			if (error_code == 0) {
				LogInfo("解压缩%s成功。", file_path.c_str());
			}
			else {
				LogError("解压缩%s失败，错误代码：%d。", file_path.c_str(), error_code);
			}
		}

		if (error_code != 0) {
			break;
		}
	}
	mz_zip_reader_end(&zip_archive);
	return error_code;
}

#if ENABLE_LIBZIP
/*
* @retval 0 成功
* @retval -1 打开zip文件失败
* @retval -2 解压缩路径无效
* @retval -3 设置解压缩密码失败
* @retval -4 获取zip文件名失败
* @retval -5 创建路径失败
* @retval -6 没有输入密码
* @retval -7 输入密码不正确
* @retval -8 打开zip文件失败
* @retval -9 写入文件失败
* @retval -10 获取zip文件属性失败
* @retval -11 关闭zip文件失败
*/
static std::string get_libzip_decompress_error(int error_code)
{
	std::string str;
	switch (error_code) {
	case 0:
		str = "成功";
		break;
	case -1:
		str = "打开zip文件失败";
		break;
	case -2:
		str = "解压缩路径无效";
		break;
	case -3:
		str = "设置解压缩密码失败";
		break;
	case -4:
		str = "获取zip文件名失败";
		break;
	case -5:
		str = "创建路径失败";
		break;
	case -6:
		str = "没有输入密码";
		break;
	case -7:
		str = "输入密码不正确";
		break;
	case -8:
		str = "打开zip文件失败";
		break;
	case -9:
		str = "写入文件失败";
		break;
	case -10:
		str = "获取zip文件属性失败";
		break;
	case -11:
		str = "关闭zip文件失败";
		break;
	default:
		str = "未知错误";
		break;
	}

	return str;
}

int libzip_decompress(const std::string& zip_path, const std::string& decompress_path, const std::string& password) {
	int result = 0;
	zip_t* archive = nullptr;
	do
	{
		DWORD attrs = GetFileAttributes(decompress_path.c_str());
		if (attrs == INVALID_FILE_ATTRIBUTES) {
			if (!create_path(decompress_path.c_str())) {
				result = -2;
				break;
			}
		}
		else {
			if ((attrs & FILE_ATTRIBUTE_DIRECTORY) == 0) {
				result = -2;
				break;
			}
		}

		int error = 0;
		std::string utf8zip_path = ansi_to_utf8(zip_path);
		archive = zip_open(utf8zip_path.c_str(), 0, &error);
		if (!archive) {
			result = -1;
			break;
		}

		if (!password.empty() && (zip_set_default_password(archive, password.c_str()) != 0)) {
			result = -3;
			break;
		}

		bool success = true;
		zip_int64_t num_entries = zip_get_num_entries(archive, 0);
		for (zip_int64_t i = 0; i < num_entries; ++i) {
			const char* utf8_name = zip_get_name(archive, i, 0);
			if (!utf8_name) {
				result = -4;
				success = false;
				break;
			}

			std::string ansi_name = utf8_to_ansi(utf8_name);
			std::string file_path = decompress_path;
			if (file_path.back() != '/') {
				file_path += "/";
				file_path += ansi_name;
			}
			else {
				file_path += ansi_name;
			}

			if (ansi_name[ansi_name.length() - 1] == '/') {
				if (!create_path(file_path)) {
					result = -5;
					success = false;
					break;
				}
				continue;
			}

			size_t last_slash = file_path.find_last_of("/");
			if (last_slash != std::string::npos) {
				std::string parent_dir = file_path.substr(0, last_slash);
				if (!create_path(parent_dir)) {
					result = -5;
					success = false;
					break;
				}
			}

			zip_file_t* file = zip_fopen_index(archive, i, 0);
			if (!file) {
				zip_error_t* zerr = zip_get_error(archive);
				int zcode = zip_error_code_zip(zerr);
				if (zcode == ZIP_ER_NOPASSWD) {
					result = -6;
					//not input password;
				}
				else if (zcode == ZIP_ER_WRONGPASSWD) {
					//password incorrect
					result = -7;
				}
				else {
					result = -8;
				}
				success = false;
				break;
			}

			{
				DWORD attrs = GetFileAttributes(file_path.c_str());
				if (attrs != INVALID_FILE_ATTRIBUTES) {
					SetFileAttributes(file_path.c_str(), FILE_ATTRIBUTE_NORMAL);
				}
			}

			LogInfo("正在解压%s……", file_path.c_str());
			std::ofstream out(file_path, std::ios::binary | std::ios::trunc);
			if (!out.is_open()) {
				LogError("解压%s失败。", file_path.c_str());
				result = -9;
				success = false;
				zip_fclose(file);
				break;
			}

			char buffer[8192] = { 0 };
			zip_int64_t size = 0;
			while ((size = zip_fread(file, buffer, sizeof(buffer))) > 0) {
				out.write(buffer, size);
			}

			if (size < 0) {
				LogError("解压%s失败。", file_path.c_str());
				result = -9;
				success = false;
				zip_fclose(file);
				break;
			}
			out.close();
			LogInfo("解压%s成功。", file_path.c_str());

			zip_uint8_t opsys = 0;
			zip_uint32_t attrs = 0;
			if (zip_file_get_external_attributes(archive, i, 0, &opsys, &attrs) < 0) {
				result = -10;
				success = false;
				zip_fclose(file);
				break;
			}
			zip_fclose(file);
			SetFileAttributes(file_path.c_str(), attrs);
		}

		if (!success) {
			break;
		}
		result = 0;
	} while (false);
	if (archive && zip_close(archive) != 0) {
		result = -11;
	}

	if (result !=0 ) {
		LogError(get_zip_decompress_error(result));
	}
	return result;
}
#endif

std::string get_current_path()
{
	size_t buf_size = 8192;
	char* buf_data = nullptr;
retry_0:
	buf_data = new char[buf_size];
	memset(buf_data, 0, buf_size);
	if (GetModuleFileName(nullptr, buf_data, buf_size) == buf_size && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
		delete[] buf_data;
		buf_size *= 2;
		goto retry_0;
	}

	std::string file_path(buf_data);
	delete[] buf_data;

	size_t npos = file_path.rfind("\\");
	if (npos != std::string::npos) {
		file_path = file_path.substr(0, npos);
	}
	else 
	{
retry_1:
		buf_data = new char[buf_size];
		memset(buf_data, 0, buf_size);
		DWORD need_bytes = GetCurrentDirectory(buf_size, buf_data);
		if (buf_size < need_bytes) {
			delete[] buf_data;
			buf_size *= 2;
			goto retry_1;
		}
		file_path = buf_data;
		delete[] buf_data;
	}
	return file_path;
}

std::string get_base_name(const std::string& file_name, bool is_special)
{
	std::string base_name;
	size_t npos = file_name.find('[');
	if (npos != std::string::npos && is_special)
	{
		base_name = file_name.substr(0, npos);
	}
	else
	{
		//不是特定文件名去除.exe后缀,得到文件名app
		npos = file_name.rfind('.');
		if (npos != std::string::npos)
		{
			base_name = file_name.substr(0, npos);
		}
	}
	return base_name;
}

std::string get_file_name(const std::string& file_path) {
	size_t npos = file_path.find_last_of('\\');
	if (npos != std::string::npos) {
		return file_path.substr(npos + 1);
	}
	else {
		npos = file_path.find_last_of('/');
		if (npos != std::string::npos) {
			return file_path.substr(npos + 1);
		}
	}
	return std::string();
}

bool is_occupied(const std::string& name)
{
	bool result = false;
	int handle = _sopen(name.c_str(), _O_RDWR, _SH_DENYRW, _S_IREAD | _S_IWRITE);
	if (-1 == handle) {
		result = true;
	}
	else {
		_close(handle);
	}
	return result;
}

int compare_file(const std::string& file_path1, const std::string& file_path2) {
	int error_code = 0;
	FILE* file1 = nullptr, *file2 = nullptr;
	do 
	{
		fopen_s(&file1, file_path1.c_str(), "rb");
		if (!file1) {
			error_code = -1;
			break;
		}

		fopen_s(&file2, file_path2.c_str(), "rb");
		if (!file2) {
			error_code = -1;
			break;
		}

		_fseeki64(file1, SEEK_SET, SEEK_END);
		long long size1 = _ftelli64(file1);
		_fseeki64(file1, SEEK_SET, SEEK_SET);

		_fseeki64(file2, SEEK_SET, SEEK_END);
		long long size2 = _ftelli64(file2);
		_fseeki64(file2, SEEK_SET, SEEK_SET);

		if (size1 != size2) {
			error_code = 1;
			break;
		}

		//超过1GB直接不进行对比内容
		if (size1 / 1024 / 1024 > 1024) {
			error_code = 1;
			break;
		}

		if (size2 / 1024 / 1024 > 1024) {
			error_code = 1;
			break;
		}

		bool success = true;
		char data1[4096] = { 0 }, data2[4096] = { 0 };
		for (long long i = 0; i < size1; i += 4096) {
			size_t read_once = static_cast<size_t>((size1 - i > 4096) ? 4096 : size1 - i);
			size_t read_size1 = fread(data1, 1, read_once, file1);
			if (read_size1 != read_once) {
				success = false;
				break;
			}

			size_t read_size2 = fread(data2, 1, read_once, file2);
			if (read_size2 != read_once) {
				success = false;
				break;
			}

			if (memcmp(data1, data2, read_size1) != 0) {
				success = false;
				break;
			}
		}

		if (!success) {
			error_code = 1;
			break;
		}
		error_code = 0;
	} while (false);

	if (file1) {
		fclose(file1);
	}

	if (file2) {
		fclose(file2);
	}
	return error_code;
}

bool copy_file(const std::string& source_file, const std::string& dest_file)
{
	bool result = false;
	FILE* file1 = nullptr, *file2 = nullptr;
	do 
	{
		fopen_s(&file1, source_file.c_str(), "rb");
		if (!file1) {
			break;
		}

		fopen_s(&file2, dest_file.c_str(), "wb");
		if (!file2) {
			break;
		}

		_fseeki64(file1, SEEK_SET, SEEK_END);
		long long size1 = _ftelli64(file1);
		_fseeki64(file1, SEEK_SET, SEEK_SET);

		bool success = true;
		char data[8192] = { 0 };
		for (long long i = 0; i < size1; i += 8192) {
			size_t once = static_cast<size_t>((size1 - i > 8192) ? 8192 : size1 - i);
			size_t size = fread(data, 1, once, file1);
			if (size != once) {
				success = false;
				break;
			}

			size = fwrite(data, 1, once, file2);
			if (size != once) {
				success = false;
				break;
			}
		}

		if (!success) {
			break;
		}

		result = true;
	} while (false);

	if (file1) {
		fclose(file1);
	}

	if (file2) {
		fclose(file2);
	}
	return result;
}

size_t get_folder_file_number(const std::string& dir_path, filter_file_cb_t filter_cb, void* args)
{
	std::list<std::string> files;
	get_folder_files(dir_path, files, true, filter_cb, args);
	return files.size();
}

bool copy_folder(const std::string& sourceFolder, const std::string& destFolder,
				 filter_file_cb_t filter_file_cb, void* args)
{
	if (!CreateDirectory(destFolder.c_str(), nullptr)) {
		if (GetLastError() != ERROR_ALREADY_EXISTS) {
			LogError("创建文件夹%s失败，错误代码%lu。", destFolder.c_str(), GetLastError());
			return false;
		}
	}

	std::string searchPath = sourceFolder + "\\*.*";
	WIN32_FIND_DATAA findData;
	HANDLE hFind = FindFirstFile(searchPath.c_str(), &findData);
	if (hFind == INVALID_HANDLE_VALUE) {
		LogError("查找第一个文件失败，错误代码%lu。", GetLastError());
		return false;
	}

	do {
		std::string sourceFile = sourceFolder + "\\" + findData.cFileName;
		std::string destFile = destFolder + "\\" + findData.cFileName;

		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0) {
				if (filter_file_cb && filter_file_cb(sourceFile, args)) {
					//printf("not copy dir %s->%s\n", sourceFile.c_str(), destFile.c_str());
					continue;
				}
				//printf("copy dir %s->%s\n", sourceFile.c_str(), destFile.c_str());

				if (!copy_folder(sourceFile, destFile, filter_file_cb, args)) {
					FindClose(hFind);
					return false;
				}
			}
		}
		else {
			if (filter_file_cb && filter_file_cb(sourceFile, args)) {
				//printf("not copy file %s->%s\n", sourceFile.c_str(), destFile.c_str());
				continue;
			}
			//printf("copy file %s->%s\n", sourceFile.c_str(), destFile.c_str());

			bool is_first_esv = true, is_first_ead = true;
RETRY_COPY_FILE:
			std::string old_dest_file;
			bool find_sensitive_dll = false;
			if (sensitive_dll_set.size()) {
				//找到敏感DLL
				std::string file_name = get_file_name(destFile);
				for (std::set<std::string>::iterator iter = sensitive_dll_set.begin();
					iter != sensitive_dll_set.end(); ++iter) {
						if (!_stricmp((*iter).c_str(), file_name.c_str())) {
							size_t npos = destFile.find_last_of('\\');
							if (npos != std::string::npos) {
								find_sensitive_dll = true;
								old_dest_file = destFile;
								destFile.replace(npos + 1, destFile.length() - npos + 1, *iter + ".log");
							}
							break;
						}
				}
				/*std::set<std::string>::iterator iter = sensitive_dll_set.find(file_name);
				if (iter != sensitive_dll_set.end()) {
					size_t npos = destFile.find_last_of('\\');
					if (npos != std::string::npos) {
						find_sensitive_dll = true;
						old_dest_file = destFile;
						destFile.replace(npos + 1, destFile.length() - npos + 1, *iter + ".log");
					}
				}*/
			}

			BOOL success = CopyFile(sourceFile.c_str(), destFile.c_str(), FALSE);
			if (success && find_sensitive_dll) {
				if (!MoveFileEx(destFile.c_str(), old_dest_file.c_str(), MOVEFILE_REPLACE_EXISTING)) {
					LogError("MoveFileEx返回失败，错误代码：%lu", GetLastError());
					CloseHandle(hFind);
					return false;
				}
				destFile = old_dest_file;
			}

			DWORD error = 0;
			if (success) {
				LogInfo("复制%s到%s成功。", sourceFile.c_str(), destFile.c_str());
			}
			else {
				LogError("复制%s到%s失败，错误代码%lu。", sourceFile.c_str(), destFile.c_str(), error = GetLastError());
			}
			
			if (success == FALSE) {
				if (error == ERROR_SHARING_VIOLATION) {
					if (is_first_esv) {
						is_first_esv = false;
						kill_occupied_file_process(destFile);
						LogWarn("文件%s被占用中，正在进行重试。", destFile.c_str());
						Sleep(1000);
						goto RETRY_COPY_FILE;
					}

					//如果被占用，对比文件是否完全一样
					int ret_compare = compare_file(sourceFile, destFile);
					if (ret_compare != 0) {
						FindClose(hFind);
						return false;
					}
					
					LogWarn("%s与%s内容相同，跳过复制。", sourceFile.c_str(), destFile.c_str());
					//相同直接跳过，不做任何处理。
				}
				else if (error == ERROR_ACCESS_DENIED) {
					if (is_first_ead) {
						is_first_ead = false;
						SetFileAttributes(destFile.c_str(), FILE_ATTRIBUTE_NORMAL);
						kill_occupied_file_process(destFile);
						LogWarn("已修改文件%s属性，正在进行重试操作。", destFile.c_str());
						Sleep(1000);
						goto RETRY_COPY_FILE;
					}

					int ret_compare = compare_file(sourceFile, destFile);
					if (ret_compare != 0) {
						LogError("复制文件%s被拒绝，可能存在被第三方软件拦截。", sourceFile.c_str());
						FindClose(hFind);
						return false;
					}

					LogWarn("%s与%s内容相同，跳过复制。", sourceFile.c_str(), destFile.c_str());
					//相同直接跳过，不做任何处理。
				}
				else {
					FindClose(hFind);
					return false;
				}
			}
		}
	} while (FindNextFile(hFind, &findData) != 0);
	FindClose(hFind);
	return true;
}

bool remove_folder(const std::string& path) 
{
	if (!RemoveDirectory(path.c_str())) {
		// 如果文件夹不为空，则递归删除文件和子文件夹
		if (GetLastError() != ERROR_DIR_NOT_EMPTY) {
			// 出现非"目录不为空"的错误，返回失败
			return false;
		}

		// 遍历文件夹中的所有文件和子文件夹
		WIN32_FIND_DATA findFileData;
		std::string searchPath = path + "\\*";
		HANDLE hFind = FindFirstFile(searchPath.c_str(), &findFileData);
		if (hFind == INVALID_HANDLE_VALUE) {
			// 遍历失败，可能是文件夹不存在或其他错误
			return false;
		}

		do {
			// 跳过当前目录（.）和上级目录（..）
			if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if (strcmp(findFileData.cFileName, ".") != 0 &&
					strcmp(findFileData.cFileName, "..") != 0) {
						std::string subPath = path + "\\" + findFileData.cFileName;
						// 递归删除子文件夹
						if (!remove_folder(subPath)) {
							FindClose(hFind);
							return false;
						}
				}

			} else {
				// 删除文件
				bool is_first_esv = true, is_first_ead = true;
				std::string filePath = path + "\\" + findFileData.cFileName;
			RETRY_DELETE_FILE:
				BOOL success = DeleteFile(filePath.c_str());
				if (success) {
					LogInfo("删除%s成功。", filePath.c_str());
				}
				else{
					DWORD error = GetLastError();
					if (error == ERROR_SHARING_VIOLATION) {
						if (is_first_esv) {
							is_first_esv = false;
							kill_occupied_file_process(filePath);
							Sleep(1000);
							goto RETRY_DELETE_FILE;
						}
						success = true;
						LogWarn("删除%s失败，此文件被另一个进程占用中，稍后请手动删除。", filePath.c_str());
					}
					else if (error == ERROR_ACCESS_DENIED) {
						if (is_first_ead) {
							is_first_ead = false;
							SetFileAttributes(filePath.c_str(), FILE_ATTRIBUTE_NORMAL);
							kill_occupied_file_process(filePath);
							Sleep(1000);
							goto RETRY_DELETE_FILE;
						}
						success = true;
						LogWarn("删除%s失败，拒绝访问。", filePath.c_str());
					}
					else {
						LogError("删除%s失败，错误代码%lu。", filePath.c_str(), error);
					}
				}
				
				if (!success) {
					FindClose(hFind);
					return false;
				}
			}
		} while (FindNextFile(hFind, &findFileData));

		FindClose(hFind);

		// 再次尝试删除文件夹，这次它应该是空的
		if (!RemoveDirectory(path.c_str())) {
			return false;
		}
	}
	return true;
}

void find_file(const char* dir, const char* suffix, std::list<std::string>& files, bool recursion)
{
	WIN32_FIND_DATA wfd = { 0 };
	HANDLE handle = NULL;
	std::string temp = std::string(dir) + "\\*";
	if ((handle = FindFirstFile(temp.c_str(), &wfd)) != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (!recursion)
					continue;

				if (!strcmp(wfd.cFileName, ".") || !strcmp(wfd.cFileName, ".."))
					continue;
				temp = std::string(dir) + "\\" + std::string(wfd.cFileName);
				find_file(temp.c_str(), suffix, files, recursion);
			}
			else
			{
				temp = std::string(dir) + "\\" + std::string(wfd.cFileName);
				char* ptr = strrchr(const_cast<char*>(temp.c_str()), '.');
				if (ptr && !strcmp(ptr, suffix))
					files.push_back(temp);
			}
		} while (FindNextFile(handle, &wfd));
		FindClose(handle);
	}
	return;
}

bool create_shortcut(const std::string& file_path, bool recursion_replace)
{
	bool result = false, success = true;
	static_cast<void>(CoInitialize(NULL));
	do
	{
		//获取桌面路径
		LPITEMIDLIST item_list = nullptr;
		if (SHGetSpecialFolderLocation(NULL,
			CSIDL_DESKTOPDIRECTORY,
			&item_list) != S_OK) {
				break;
		}

		char desktop_path[4096] = { 0 };
		if (!SHGetPathFromIDList(item_list, desktop_path)) {
			CoTaskMemFree(item_list);
			break;
		}
		CoTaskMemFree(item_list);

		//查询整个桌面,是否已存在,上一个版本
		std::list<std::string> file_list;
		find_file(desktop_path, ".lnk", file_list, recursion_replace);

		std::string file_name;
		size_t npos = file_path.rfind("\\");
		if (npos != std::string::npos) {
			file_name = file_path.substr(npos + 1);
		}
		else {
			file_name = "未知.exe";
		}
		std::string base_name = get_base_name(file_name);
		//std::string base_name = file_name;
		//size_t npos = base_name.find('[');
		//if (npos != std::string::npos) {
		//	//特定文件名,去掉[x.x.x.x]版本号
		//	base_name = base_name.substr(0, npos);
		//}
		//else {
		//	npos = base_name.rfind('.');
		//	if (npos != std::string::npos) {
		//		base_name = base_name.substr(0, npos);
		//	}
		//	//移除" - 副本"或" - 副本 (1)"..." - 副本 (N)"
		//	//utility::remove_filename_copy_str(base_name);
		//}

		std::list<std::string> name_list;
		bool find_old = false;
		//判断是否存在多个旧快捷方式,如果存在将全部替换
		for (std::list<std::string>::iterator iter = file_list.begin(); iter != file_list.end(); ++iter) {
			std::string dst_name;
			std::string x = *iter;
			if (x.rfind('\\') != std::string::npos) {
				dst_name = x.substr(x.rfind('\\') + 1);
				if (dst_name.find('[') != std::string::npos) {
					dst_name = dst_name.substr(0, dst_name.find('['));
				}
				else {
					//不符合特定文件名格式,去除.lnk后缀名
					size_t npos = dst_name.rfind('.');
					if (npos != std::string::npos) {
						dst_name = dst_name.substr(0, npos);
					}

					//utility::remove_filename_copy_str(dst_name);
				}
			}

			//多个条件判断
			if (base_name.find(dst_name) != std::string::npos ||
				dst_name.find(base_name) != std::string::npos ||
				dst_name == base_name) {
					find_old = true;
					name_list.push_back(x);
			}
		}

		std::list<std::string> lnk_list;
		if (find_old) {
			for (std::list<std::string>::iterator iter = name_list.begin(); iter != name_list.end(); ++iter) {
				std::string x = *iter;
				//删除旧快捷方式
				DeleteFile(x.c_str());
				SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);

				//获取旧快捷方式的路径
				std::string path = x;
				size_t npos = path.rfind('\\');
				if (npos != std::string::npos) {
					path = path.substr(0, npos);
					//添加新快捷方式名
					std::string lnk_name = path + std::string("\\") + base_name + std::string(".lnk");
					lnk_list.push_back(lnk_name);
				}
			}
		}
		else {
			std::string lnk_name = desktop_path + std::string("\\") + base_name + std::string(".lnk");
			lnk_list.push_back(lnk_name);
		}

		for (std::list<std::string>::iterator iter0 = lnk_list.begin(); iter0 != lnk_list.end(); ++iter0) {
			std::string x = *iter0;
			IShellLinkA* link = nullptr;
			HRESULT ret = S_FALSE;
			if ((ret = CoCreateInstance(CLSID_ShellLink,
				NULL, CLSCTX_INPROC_SERVER,
				IID_IShellLink,
				(void**)&link)) != S_OK) {
					success = false;
					break;
			}

			std::string working_directory;
			size_t npos = file_path.rfind('\\');
			if (npos != std::string::npos) {
				working_directory = file_path.substr(0, npos);
			}
			else{
				working_directory = get_current_path();	
			}
			link->SetWorkingDirectory(working_directory.c_str());
			link->SetPath(file_path.c_str());
			link->SetIconLocation(file_path.c_str(), 0);

			IPersistFile* ipf = nullptr;
			if (link->QueryInterface(IID_IPersistFile, (void**)&ipf) != S_OK) {
				success = false;
				if (ipf) {
					ipf->Release();
				}
				break;
			}

			wchar_t* wstr = multiByteToWideChar(x.c_str(), CP_ACP);
			ipf->Save(wstr, TRUE);
			ipf->Release();
			delete[] wstr;
		}

		if (!success) {
			break;
		}
		result = true;
	} while (false);
	CoUninitialize();
	return result;
}

bool get_all_processes(std::map<DWORD, std::string>& processes)
{
	bool result = false;
	do
	{
		PROCESSENTRY32 pe32 = { 0 };
		pe32.dwSize = sizeof(pe32);
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (snapshot == INVALID_HANDLE_VALUE)
		{
			break;
		}

		BOOL next = Process32First(snapshot, &pe32);
		while (next)
		{
			processes.insert(std::make_pair(pe32.th32ProcessID, std::string(pe32.szExeFile)));
			next = Process32Next(snapshot, &pe32);
		}
		CloseHandle(snapshot);
		result = true;
	} while (false);
	return result;
}

static bool stdmap_contains(const std::map<std::string, std::string>& t, const std::string& v)
{
	return t.find(v) != t.end();
}

static bool to_bool(const std::string& str) { return _stricmp(str.c_str(), "true") == 0; }

cmdline_option_t parse_command_line_option(const std::string& cmdline)
{
	std::map<std::string, std::string> ret_map;
	std::vector<std::string> key_list = get_cmdline_option_arr();
	for (std::vector<std::string>::const_iterator iter = key_list.begin();
		iter != key_list.end(); ++iter) {
			size_t start_npos = cmdline.find(*iter);
			if (start_npos != std::string::npos) {
				start_npos += 1;
				size_t tmp_npos = std::string::npos;
TRAY_REFIND:
				size_t end_npos = cmdline.find("-", tmp_npos == std::string::npos ? start_npos : tmp_npos);
				if (end_npos != std::string::npos) {
					std::string temp = cmdline.substr(end_npos);
					bool find = false;
					for (size_t i = 0; i < key_list.size(); ++i) {
						if (strncmp(temp.c_str(), key_list[i].c_str(), key_list[i].length()) == 0) {
							find = true;
							break;
						}
					}

					if (!find) {
						tmp_npos = end_npos + 1;
						goto TRAY_REFIND;
					}
				}
				
				std::string kv = cmdline.substr(start_npos, end_npos - start_npos);
				size_t first_npos = kv.find_first_of("\"");
				size_t last_npos = kv.find_last_of("\"");
				bool exist_double_marks = (first_npos != std::string::npos && last_npos != std::string::npos);

				std::string str = kv;
				if (exist_double_marks) {
					str.erase(first_npos, 1);
					str.erase(last_npos - 1, 1);
				}
				/*else {
					bool find_space = false;
					for (size_t i = 0;i < kv.length(); ++i) {
						if (kv[i] == ' ' && !find_space) {
							str.insert(str.begin() + str.length(), kv[i]);
							find_space = true;
						}
						else if (kv[i] != ' ') {
							str.insert(str.begin() + str.length(), kv[i]);
						}
					}
				}*/

				size_t pos = str.find_first_of(" ");
				if (pos != std::string::npos) {
					std::string key = str.substr(0, pos);
					std::string value = str.substr(pos + 1);
					trim_str(key);
					trim_str(value);
					ret_map.insert(std::make_pair(key, value));
				}
			}
	}

	cmdline_option_t cmdline_option;
	cmdline_option.kv = ret_map;

	std::string option = "is_backup_file";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.is_backup_file = to_bool(ret_map[option]);
	}

	option = "is_create_shortcut";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.is_create_shortcut = to_bool(ret_map[option]);
	}

	option = "is_delete_update_file";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.is_delete_update_file = to_bool(ret_map[option]);
	}

	option = "is_hide_window";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.is_hide_window = to_bool(ret_map[option]);
	}

	option = "is_recursion_replace_shortcut";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.is_recursion_replace_shortcut = to_bool(ret_map[option]);
	}

	option = "is_show_topmost";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.is_show_topmost = to_bool(ret_map[option]);
	}

	option = "old_version_process_id";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.old_version_process_id = atol(ret_map[option].c_str());
	}

	option = "old_version_process_name";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.old_version_process_name = ret_map[option];
	}

	option = "start_process_command_line";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.start_process_command_line = ret_map[option];
	}

	option = "source_path";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.source_path = ret_map[option];
		replace_with_windows_path_symbol(cmdline_option.source_path);
	}

	option = "destination_path";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.destination_path = ret_map[option];
		replace_with_windows_path_symbol(cmdline_option.destination_path);
	}

	option = "window_height";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.window_height = atoi(ret_map[option].c_str());
	}

	option = "window_title";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.window_title = ret_map[option];
	}

	option = "window_width";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.window_width = atoi(ret_map[option].c_str());
	}

	option = "is_compress_backup_file";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.is_compress_backup_file = to_bool(ret_map[option]);
	}

	option = "sensitive_dll";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.sensitive_dll = ret_map[option];
	}

	option = "latest_version";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.latest_version = ret_map[option];
	}

	option = "is_only_start_process";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.is_only_start_process = to_bool(ret_map[option]);
	}

	option = "is_prompt_if_error";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.is_prompt_if_error = to_bool(ret_map[option]);
	}

	option = "start_process_file_path";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.start_process_file_path = ret_map[option];
		replace_with_windows_path_symbol(cmdline_option.start_process_file_path);
	}

	option = "start_process_working_directory";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.start_process_working_directory = ret_map[option];
		replace_with_windows_path_symbol(cmdline_option.start_process_working_directory);
	}

	option = "server_address";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.server_address = ret_map[option];
	}

	option = "server_port";
	if (stdmap_contains(ret_map, option)) {
		int port = atoi(ret_map[option].c_str());
		cmdline_option.server_port = port == 0 ? 60682 : port;
	}

	option = "upload_update_record_flag";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.upload_update_record_flag = atoi(ret_map[option].c_str());
	}

	option = "window_style";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.window_style = atoi(ret_map[option].c_str());
	}

	option = "is_delete_old_version_backup_file";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.is_delete_old_version_backup_file = to_bool(ret_map[option]);
	}

	option = "is_wait_old_version_exit";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.is_wait_old_version_exit = to_bool(ret_map[option]);
	}

	option = "wait_old_version_exit_timeout";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.wait_old_version_exit_timeout = atoi(ret_map[option].c_str());
	}

	option = "kill_old_version_process_flag";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.kill_old_version_process_flag = atoi(ret_map[option].c_str());
	}

	option = "upload_update_log_max_size";
	if (stdmap_contains(ret_map, option)) {
		cmdline_option.upload_update_log_max_size = atoi(ret_map[option].c_str()) * 1024 * 1024;
		if (cmdline_option.upload_update_log_max_size == 0) {
			cmdline_option.upload_update_log_max_size = 5 * 1024 * 1024;
		}
	}

	return cmdline_option;
}

bool check_path_is_valid(const std::string& source_path, const std::string& destination_path)
{
	LogInfo("正在检查路径是否有效，请耐心等待……");
	bool result = false;
	do 
	{
		//检查源路径
		int ret = _access(source_path.c_str(), 0);
		if (ret == 0) {
			LogInfo("源路径%s有效！", source_path.c_str());
		}
		else {
			LogError("源路径%s无效！", source_path.c_str());
			break;
		}

		//检查目标路径
		ret = _access(destination_path.c_str(), 0);
		if (ret == 0) {
			LogInfo("目标路径%s有效！", destination_path.c_str());
		}
		else {
			LogError("目标路径%s无效！", destination_path.c_str());
			break;
		}

		LogInfo("检查路径是否有效完成！");
		result = true;
	} while (false);
	return result;
}

bool wait_process_exit(DWORD process_id, const std::string& process_name, int kill_flag, size_t timeout)
{
	LogInfo("等待进程是否退出中，请耐心等待……");
	bool result = false;
	do 
	{
		size_t start = GetTickCount();
		while(true) {
			std::map<DWORD, std::string> pid_map;
			get_all_processes(pid_map);
			bool find = false;
			for (std::map<DWORD, std::string>::iterator iter = pid_map.begin(); iter != pid_map.end(); ++iter) {
				if (kill_flag == 0 && process_id != 0) {
					if (iter->first == process_id) {
						LogWarn("等待%s退出中……", process_name.c_str());
						find = true;
						break;
					}
				}
				else {
					if (iter->second.substr(0, process_name.length()) == process_name) {
						LogWarn("等待%s退出中……", process_name.c_str());
						find = true;
						break;
					}
				}
			}

			if (!find) {
				LogInfo("进程%s已退出。", process_name.c_str());
				break;
			}

			if (GetTickCount() - start > timeout) {
				LogInfo("进程%s未退出。", process_name.c_str());
				break;
			}
			Sleep(100);
		}

		result = true;
	} while (false);
	LogInfo("等待进程是否退出完成！");
	return result;
}

bool check_process_is_exit(DWORD process_id, const std::string& process_name, int kill_flag, size_t timeout)
{
	LogInfo("检查进程是否退出中，请耐心等待……");
	bool result = false, success = true, is_first_check = true;

	do 
	{
		size_t start_time = GetTickCount();
		while(true) {
			bool is_app_run = false;
			std::map<DWORD, std::string> pid_map;
			get_all_processes(pid_map);

			std::list<DWORD> pid_list;
			for (std::map<DWORD, std::string>::iterator iter = pid_map.begin(); iter != pid_map.end(); ++iter) {
				if (kill_flag == 0 && process_id !=0) {
					if (iter->first == process_id) {
						pid_list.push_back(iter->first);
						LogWarn("等待%s退出中……", process_name.c_str());
						is_app_run = true;
						break;
					}
				}
				else {
					if (iter->second.substr(0, process_name.length()) == process_name) {
						pid_list.push_back(iter->first);
						LogWarn("等待%s退出中……", process_name.c_str());
						is_app_run = true;
					}
				}
			}

			if (is_first_check && !is_app_run) {
				LogInfo("进程已完全退出。");
				break;
			}

			is_first_check = false;

			if (!is_app_run) {
				break;
			}

			int exit_num = 0;
			for (std::list<DWORD>::iterator iter = pid_list.begin(); iter != pid_list.end(); ++iter) {
				if (terminate_process(*iter)) {
					LogInfo("终止进程%s成功，其PID为%lu。", process_name.c_str(), *iter);
					++exit_num;
				}
				else {
					LogError("终止进程%s失败，错误代码%lu。", process_name.c_str(), GetLastError());
				}
			}

			if (pid_list.size() == exit_num) {
				break;
			}

			if (GetTickCount() - start_time > timeout) {
				success = false;
				LogError("检查进程是否退出超时。");
				break;
			}
			Sleep(100);
		}

		if (!success) {
			break;
		}

		LogInfo("检查进程是否退出完成！");
		result = true;
	} while (false);
	return result;
}

bool check_file_is_occupy(const std::string& destination_path)
{
	bool result = true, success = true, wait = false;
	LogInfo("检查文件是否占用中，请耐心等待……");
	std::list<std::string> files;
	std::vector<DWORD> closed_pids;
	traversal_directory(destination_path, files, false);
	for (std::list<std::string>::iterator iter = files.begin(); iter != files.end();) {
		DWORD attrs = GetFileAttributes(iter->c_str());
		if (attrs & FILE_ATTRIBUTE_DIRECTORY) {
			++iter;
			continue;
		}

		std::map<DWORD, std::string> pids = get_occupied_file_pids(*iter);
		std::string filepath = *iter;
		if (pids.empty()) {
			LogInfo("%s未占用。", filepath.c_str());
		}
		else {
			LogWarn("%s占用中。", filepath.c_str());
			for (std::map<DWORD, std::string>::iterator it = pids.begin(); it != pids.end(); ++it) {
				std::vector<DWORD>::iterator pid_it =  std::find(closed_pids.begin(), closed_pids.end(), it->first);
				if (pid_it != closed_pids.end()) {
					wait = true;
					LogWarn("占用进程%s已关闭等待系统回收资源中……", it->second.c_str());
					break;
				}

				success = terminate_process(it->first);
				if (success) {
					LogInfo("关闭占用进程%s成功，其PID：%lu。", it->second.c_str(), it->first);
					closed_pids.push_back(it->first);
				}
				else {
					LogError("关闭占用进程%s失败，其PID：%lu，错误代码：%lu。", it->second.c_str(), 
						it->first, GetLastError());
				}
				
				if (!success) {
					result = false;
					break;
				}
			}

			if (!success) {
				break;
			}
		}

		if (wait) {
			wait = false;
			Sleep(100);
			continue;
		}

		++iter;
	}

	if (result) {
		LogInfo("检查文件是否占用完成！");
	}
	return result;
}

//bool backup_file(const std::string& source_path, const std::string& destination_path,
//				 const std::string& backup_file_filter_suffix,
//				 bool is_compress_backup_file)
//{
//	bool result = false;
//	do 
//	{
//		char localappdata_path[8192] = { 0 };
//		if (SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, localappdata_path) != S_OK) {
//			LogInfo("备份文件失败，获取LOCALAPPDATA目录失败。");
//			break;
//		}
//
//		format_str("%s\\imp_updater_backup\\%s", localappdata_path, "");
//
//		//备份文件
//		LogInfo("正在备份文件，请耐心等待……");
//		size_t source_path_npos = source_path.rfind("\\");
//		if (source_path_npos == std::string::npos) {
//			LogInfo("备份文件失败，获取源路径上级目录失败。");
//			break;
//		}
//
//		std::list<std::string> filter_file_suffix;
//		if (!backup_file_filter_suffix.empty()) {
//			filter_file_suffix = split_by_semicolon(backup_file_filter_suffix, ';');
//		}
//
//		if (is_compress_backup_file) {
//			std::string zip_filepath = source_path.substr(0, source_path_npos) + "\\backup.zip";
//			int ret = zip_compress_folder(destination_path.c_str(), zip_filepath.c_str(), filter_file_suffix);
//
//			if (ret != 0)
//			{
//				std::string error_string;
//				switch (ret) {
//				case -1:
//					error_string = format_str("备份文件失败，压缩文件夹路径无效！");
//					break;
//				case -2:
//					error_string = format_str("备份文件失败，zip文件路径无效！");
//					break;
//				case -3:
//					error_string = format_str("备份文件失败，读取压缩文件无效！");
//					break;
//				case -4:
//					error_string = format_str("备份文件失败，内存不足，无法进行压缩！");
//					break;
//				case -5:
//					error_string = format_str("备份文件失败，压缩文件合并到内存失败！");
//					break;
//				default:
//					error_string = format_str("备份文件失败，未知错误！");
//					break;
//				}
//				LogInfo(error_string);
//				break;
//			}
//		}
//		else {
//			std::string bakcup_filepath = source_path.substr(0, source_path_npos) + "\\backup";
//			if (!copy_folder(destination_path, bakcup_filepath, filter_file_cb, (void*)&filter_file_suffix)) {
//				break;
//			}
//		}
//
//		result = true;
//	} while (false);
//
//	if (result) {
//		LogInfo("备份文件完成。");
//	}
//	return result;
//}

bool backup_file(const std::string& process_name,
				 const std::string& latest_version,
				 const std::string& source_path,
				 const std::string& destination_path,
				 bool is_compress_backup_file,
				 progress_cb_t progress_cb,
				 void* args)
{
	bool result = false;
	do 
	{
		char localappdata_path[8192] = { 0 };
		if (SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, localappdata_path) != S_OK) {
			LogError("备份文件失败，获取LOCALAPPDATA目录失败。");
			break;
		}

		std::string file_path;
		size_t npos = process_name.find_last_of('.');
		if (npos != std::string::npos) {
			file_path = format_str("%s\\imp_updater_backup\\%s\\%s", localappdata_path, 
				process_name.substr(0, npos).c_str(),
				latest_version.c_str());
		}
		else
		{
			file_path = format_str("%s\\imp_updater_backup\\%s\\%s", localappdata_path, 
				process_name.c_str(),
				latest_version.c_str());
		}

		if (!create_path(file_path)) {
			LogError("备份文件失败，创建路径%s失败。", file_path.c_str());
			break;
		}

		//备份文件
		LogInfo("正在备份文件，请耐心等待……");

		std::list<std::string> filter_file_name;
		get_folder_files(source_path, filter_file_name, true);

		if (is_compress_backup_file) {
			std::string zip_filepath = file_path + "\\backup.zip";
#if ENABLE_LIBZIP
			if (zip_compress(destination_path, zip_filepath, std::string(), filter_file_suffix, progress_cb, args) != 0) {
				break;
			}
#else
			int ret = miniz_compress_folder(destination_path.c_str(), zip_filepath.c_str(), filter_file_name, progress_cb, args);

			if (ret != 0)
			{
				std::string error_string;
				switch (ret) {
				case -1:
					error_string = format_str("备份文件失败，压缩文件夹路径无效！");
					break;
				case -2:
					error_string = format_str("备份文件失败，zip文件路径无效！");
					break;
				case -3:
					error_string = format_str("备份文件失败，读取压缩文件无效！");
					break;
				case -4:
					error_string = format_str("备份文件失败，内存不足，无法进行压缩！");
					break;
				case -5:
					error_string = format_str("备份文件失败，压缩文件合并到内存失败！");
					break;
				default:
					error_string = format_str("备份文件失败，未知错误！");
					break;
				}
				LogError(error_string);
				break;
			}
#endif
		}
		else {
			std::string bakcup_filepath = file_path + "\\backup";
			if (!copy_folder(destination_path, bakcup_filepath, progress_cb, args)) {
				break;
			}
		}

		result = true;
	} while (false);

	if (result) {
		LogInfo("备份文件完成。");
	}
	return result;
}

bool delete_old_version_backup_file(const std::string& process_name, const std::string& latest_version)
{
	bool result = false, success = true;
	do 
	{
		char localappdata_path[8192] = { 0 };
		if (SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, localappdata_path) != S_OK) {
			LogError("删除旧版本备份文件失败，获取LOCALAPPDATA目录失败。");
			break;
		}

		std::string dir_path;
		size_t npos = process_name.find_last_of('.');
		if (npos != std::string::npos) {
			dir_path = format_str("%s\\imp_updater_backup\\%s", localappdata_path, process_name.substr(0, npos).c_str());
		}
		else {
			dir_path = format_str("%s\\imp_updater_backup\\%s", localappdata_path, process_name.c_str());
		}

		std::list<std::string> dirs;
		traversal_directory(dir_path, dirs, false);

		for (std::list<std::string>::const_iterator cit = dirs.begin(); cit != dirs.end(); ++cit) {
			size_t npos = cit->find_last_of('\\');
			if (npos == std::string::npos) {
				continue;
			}

			std::string str = cit->substr(npos + 1);
			std::vector<std::string> ver0 = split_by_delimiter<std::vector<std::string>>(latest_version, '.');
			std::vector<std::string> ver1 = split_by_delimiter<std::vector<std::string>>(str, '.');
			size_t max_size = std::max<size_t>(ver0.size(), ver1.size());
			while (ver0.size() < max_size) {
				ver0.push_back("0");
			}

			while( ver1.size() < max_size) {
				ver1.push_back("0");
			}

			int cmp = 0;
			for (size_t i = 0; i < ver0.size(); ++i) {
				if (std::atoi(ver0[i].c_str()) > std::atoi(ver1[i].c_str())) {
					cmp = 1;
					break;
				}

				if (std::atoi(ver0[i].c_str()) < std::atoi(ver1[i].c_str())) {
					cmp = -1;
					break;
				}
			}

			if (cmp != 1) {
				continue;
			}

			DWORD attrs = GetFileAttributes(cit->c_str());
			if (attrs == INVALID_FILE_ATTRIBUTES) {
				continue;
			}

			if (attrs & FILE_ATTRIBUTE_DIRECTORY) {
				if (!remove_folder(*cit)) {
					success = false;
					break;
				}
			}
		}

		if (!success) {
			break;
		}

		result = true;
	} while (false);
	return result;
}

std::string match_upgrade_app_file_path(const std::string& process_name, const std::string& source_path,
										const std::string& destination_path)
{
	std::string file_path;
	std::list<std::string> file_list;
	find_file(destination_path.c_str(), ".exe", file_list, true);	
	for (std::list<std::string>::iterator iter = file_list.begin(); iter != file_list.end(); ++iter) {
		if ((*iter).find(process_name) != std::string::npos) {
			file_path = *iter;
			break;
		}
	}

	if (file_path.empty() && !file_list.empty()) {
		//如果没有通过process_name匹配上，那么就去匹配压缩包的名字
		size_t npos = source_path.rfind("\\");
		if (npos != std::string::npos) {
			std::string dir_name = source_path.substr(npos + 1);
			for (std::list<std::string>::iterator iter = file_list.begin(); iter != file_list.end(); ++iter) {
				if ((*iter).find(dir_name) != std::string::npos) {
					file_path = *iter;
					LogWarn("升级压缩包内部，没有找到相关exe程序，使用压缩包名称匹配成功。");
					break;
				}
			}
		}

		//如果通过压缩包都没有找到，那么就默认第一个为升级的exe程序。
		if (file_path.empty()) {
			file_path = file_list.front();
			LogWarn("升级压缩包内部，没有找到相关exe程序，默认匹配第一个exe程序。");
		}
	}

	if (file_path.empty()) {
		LogError("升级压缩包内部，没有找到任何exe程序。");
	}
	return file_path;
}

bool rollback_file(const std::string& process_name, const std::string& latest_version,
				   const std::string& destination_path,
				   bool is_compress_backup_file)
{
	LogInfo("正在进行回滚文件，请耐心等待……");
	bool result = false;
	do 
	{
		char localappdata_path[8192] = { 0 };
		if (SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, localappdata_path) != S_OK) {
			LogError("回滚文件失败，获取LOCALAPPDATA目录失败。");
			break;
		}

		std::string file_path;
		size_t npos = process_name.find_last_of('.');
		if (npos != std::string::npos) {
			file_path = format_str("%s\\imp_updater_backup\\%s\\%s", localappdata_path, 
				process_name.substr(0, npos).c_str(),
				latest_version.c_str());
		}
		else
		{
			file_path = format_str("%s\\imp_updater_backup\\%s\\%s", localappdata_path, 
				process_name.c_str(),
				latest_version.c_str());
		}

		if (!create_path(file_path)) {
			LogError("回滚文件失败，创建路径%s失败。", file_path.c_str());
			break;
		}

		std::string extract_path = file_path;
		std::string rollback_filepath = extract_path + "\\backup.zip";
		extract_path += "\\backup";

		if (is_compress_backup_file) {
#if ENABLE_LIBZIP
			if (libzip_decompress(rollback_filepath, extract_path) != 0) {
				break;
			}
#else
			int ret = miniz_decompress_file(rollback_filepath.c_str(), extract_path.c_str());
			if (ret != 0)
			{
				std::string error_string;
				switch (ret) {
				case -1:
					error_string = "回滚文件失败，zip文件无效！";
					break;
				case -2:
					error_string = "回滚文件失败，创建提取文件夹失败！";
					break;
				case -3:
					error_string = "回滚文件失败，写入解压文件失败！";
					break;
				case -4:
					error_string = "回滚文件失败，获取解压文件信息失败！";
					break;
				case -5:
					error_string = "回滚文件失败，提取解压文件到内存失败！";
					break;
				default:
					error_string = "回滚文件失败，未知错误！";
					break;

				}
				LogError(error_string);
				break;
			}
#endif
		}
		
		//直接拷贝回去
		if (!copy_folder(extract_path, destination_path)) {
			break;
		}

		remove_folder(extract_path);
		result = true;
	} while (false);
	if (result) {
		LogInfo("回滚文件成功。");
	}
	return result;
}

bool start_process(const std::string& file_path, const std::string& command_line, const std::string& working_directory)
{
	bool result = false;
	do 
	{
		LogInfo("正在启动进程，请耐心等待……");
		SHELLEXECUTEINFOA sei = { sizeof(SHELLEXECUTEINFOA) };
		sei.lpVerb = "open";
		sei.lpFile = file_path.c_str();
		sei.lpParameters = command_line.empty() ? nullptr : command_line.c_str();
		//sei.lpDirectory = working_directory.empty() ? nullptr : working_directory.c_str();
		std::string dir_path;
		if (working_directory.empty()) {
			dir_path = file_path;
			replace_with_windows_path_symbol(dir_path);
			size_t npos = dir_path.find_last_of('\\');
			if (npos != std::string::npos) {
				dir_path = dir_path.substr(0, npos);
				sei.lpDirectory = dir_path.c_str();
			}
			else {
				sei.lpDirectory = nullptr;
			}
		}
		else {
			sei.lpDirectory = working_directory.c_str();
		}
		sei.nShow = SW_SHOW;
		sei.fMask = SEE_MASK_FLAG_NO_UI;
		BOOL ret = ShellExecuteEx(&sei);
		if (ret) {
			LogInfo("启动进程成功！");
		}
		else {
			DWORD error_code = GetLastError();
			if (error_code != ERROR_CANCELLED) {
				LogError("启动进程失败！%s", format_error(error_code).c_str());
				break;
			}
		}

		//STARTUPINFOA startInfo = { sizeof(STARTUPINFOA) };
		//PROCESS_INFORMATION procStruct;
		//BOOL ret = CreateProcess(
		//	file_path.c_str(),
		//	command_line.empty() ? nullptr : (char*)command_line.c_str(),
		//	nullptr,
		//	nullptr,
		//	FALSE,
		//	NORMAL_PRIORITY_CLASS,
		//	nullptr,
		//	working_directory.empty() ? nullptr: working_directory.c_str(),
		//	&startInfo,
		//	&procStruct);
		//if (ret != TRUE && GetLastError() == ERROR_ELEVATION_REQUIRED) {
		//	//需要用到管理员权限
		//	SHELLEXECUTEINFOA sei = { sizeof(SHELLEXECUTEINFOA) };
		//	sei.lpVerb = "runas";
		//	sei.lpFile = file_path.c_str();
		//	sei.lpParameters = command_line.empty() ? nullptr : command_line.c_str();
		//	sei.lpDirectory = working_directory.empty() ? nullptr : working_directory.c_str();
		//	sei.nShow = SW_SHOW;
		//	ret = ShellExecuteEx(&sei);
		//	if (ret) {
		//		LogInfo("启动进程成功！");
		//	}
		//	else {
		//		DWORD error_code = GetLastError();
		//		if (error_code != ERROR_CANCELLED) {
		//			LogInfo("启动进程失败！%s", format_error(error_code).c_str());
		//			break;
		//		}
		//	}
		//}
		//else {
		//	if (ret) {
		//		/*if (procStruct.hProcess) {
		//			CloseHandle(procStruct.hProcess);
		//		}

		//		if (procStruct.hThread) {
		//			CloseHandle(procStruct.hThread);
		//		}*/
		//		LogInfo("启动进程成功！");
		//	}
		//	else {
		//		LogInfo("启动进程失败！%s", format_error(GetLastError()).c_str());
		//		break;
		//	}
		//}
		result = true;
	} while (false);
	return result;
}

void update_finish(bool ret, bool is_hide_window, bool is_backup_file, bool is_copy_file_error,
				   const std::string& process_name,
				   const std::string& latest_version,
				   const std::string& destination_path, int window_style)
{
	if (ret) {
		if (!is_hide_window && cmdline_option.window_style == 0) {
			for (int i = 3; i >= 0; --i) {
				ChangeStatusText("更新成功！%d秒后程序将自动退出。", i);
				Sleep(1000);
			}
		}
	}
	else {
		ChangeStatusText("更新失败！");

		if (is_backup_file && is_copy_file_error) {
			std::string caption = format_str("错误原因：%s\n是否要回滚到上一个版本？", get_last_error().c_str());
			int ret = IDYES;
			if (cmdline_option.is_prompt_if_error) {
				ret = MessageBox(nullptr, caption.c_str(), "错误", MB_YESNO | MB_ICONQUESTION);
			}
			if (ret == IDYES) {
				LogInfo("正在回滚到上一个版本，请耐心等待……");
				bool success = rollback_file(process_name, latest_version, destination_path,
					cmdline_option.is_compress_backup_file);
				if (success) {
					LogInfo("回滚到上一个版本成功！");
				}
				else {
					LogError("回滚到上一个版本失败！%s", get_last_error().c_str());
				}
				
				if (is_hide_window || window_style != 0) {
					if (cmdline_option.is_prompt_if_error) {
						MessageBox(nullptr, get_last_error().c_str(), "错误", MB_ICONERROR | MB_OK);
					}
				}
			}
		}
		else {
			if (is_hide_window || window_style != 0) {
				if (cmdline_option.is_prompt_if_error) {
					MessageBox(nullptr, get_last_error().c_str(), "错误", MB_ICONERROR | MB_OK);
				}
			}
		}
	}
}

typedef struct _thread_copy_file_args {
	_thread_copy_file_args(){
		current_progress = 0;
		current_size = 0;
		total_size = 0;
		rate = 0;
		filter_cb = nullptr;
	}

	size_t current_progress;
	size_t current_size;
	size_t total_size;
	int rate;
	filter_file_cb_t filter_cb;
	std::list<std::string> filter_args;
} thread_copy_file_args_t;

static bool thread_copy_file_cb(const std::string& filepath, void* args)
{
	thread_copy_file_args_t* ptr = static_cast<thread_copy_file_args_t*>(args);
	if (ptr->filter_cb && ptr->filter_cb(filepath, &ptr->filter_args)) {
		return true;
	}

	if (ptr->total_size > 0) {
		float precent = static_cast<float>(++ptr->current_size) / static_cast<float>(ptr->total_size) *  ptr->rate;
		uInstallProgress = static_cast<int>(ptr->current_progress + precent);
	}
	
	return false;
}

static unsigned long __stdcall update_thread(void* args)
{
	size_t start_time = GetTickCount();
	bool upgrade_result = false, is_copy_file_error = false;
	bIsUpgradeFinish = FALSE;
	do 
	{
		LogInfo("命令行参数：");
		for (std::map<std::string, std::string>::iterator iter = cmdline_option.kv.begin();
			iter != cmdline_option.kv.end(); ++iter) {
				LogInfo("选项[%s], 数据[%s]。", iter->first.c_str(), iter->second.c_str());
		}

		std::string file_path;
		if (!cmdline_option.is_only_start_process) {
			ChangeStatusText("正在更新%s，请耐心等待……", cmdline_option.old_version_process_name.c_str());

			std::list<std::string> sensitive_dll_list = split_by_delimiter<std::list<std::string>>(cmdline_option.sensitive_dll, ';');
			for (std::list<std::string>::iterator iter = sensitive_dll_list.begin();
				iter != sensitive_dll_list.end(); ++iter) {
					sensitive_dll_set.insert(*iter);
			}

			if (find_str_set(sensitive_dll_set, "$no_default", true) == sensitive_dll_set.end()) {
				//这些DLL会经常被拦
				for (int i = 0; often_intercepted_dll_name[i]; ++i) {
					std::set<std::string>::iterator iter = find_str_set(sensitive_dll_set, often_intercepted_dll_name[i], true);
					if (iter == sensitive_dll_set.end()) {
						sensitive_dll_set.insert(often_intercepted_dll_name[i]);
					}
				}
			}
			else {
				sensitive_dll_set.erase("$no_default");
			}
			
			//等待进程退出
			if (cmdline_option.is_wait_old_version_exit) {
				wait_process_exit(cmdline_option.old_version_process_id,
					cmdline_option.old_version_process_name, 
					cmdline_option.kill_old_version_process_flag,
					cmdline_option.wait_old_version_exit_timeout);
			}

			if (!cmdline_option.is_hide_window) {
				PostMessage(hWnd, WM_USER_SHOW_WINDOW, 0, 0);
			}

			//检查路径是否有效
			if (!check_path_is_valid(cmdline_option.source_path, cmdline_option.destination_path)) {
				break;
			}

			//检查进程是否退出
			if (!check_process_is_exit(cmdline_option.old_version_process_id,
				cmdline_option.old_version_process_name, 
				cmdline_option.kill_old_version_process_flag,
				cmdline_option.wait_old_version_exit_timeout)) {
				break;
			}
			uInstallProgress += 5;

			//检查文件是否被占用
			if (!check_file_is_occupy(cmdline_option.destination_path)) {
				break;
			}
			uInstallProgress += 5;
			
			thread_copy_file_args_t tcfa;
			
			//备份文件
			if (cmdline_option.is_backup_file) {
				std::list<std::string> filter_file_name;
				get_folder_files(cmdline_option.source_path, filter_file_name, true, nullptr, nullptr, true);
				tcfa.current_size = 0;
				tcfa.current_progress = uInstallProgress;
				tcfa.total_size = get_folder_file_number(cmdline_option.destination_path, &filter_file_name_cb, &filter_file_name);
				tcfa.rate = 40;
				tcfa.filter_cb = cmdline_option.is_compress_backup_file ? nullptr : &filter_file_name_cb;
				tcfa.filter_args = filter_file_name;
				if (!backup_file(cmdline_option.old_version_process_name,
					cmdline_option.latest_version,
					cmdline_option.source_path,
					cmdline_option.destination_path,
					cmdline_option.is_compress_backup_file,
					&thread_copy_file_cb, &tcfa)) {
					break;
				}
				uInstallProgress = 50;
				tcfa.filter_cb = nullptr;
			}
			else {
				tcfa.rate = 80;
			}

			tcfa.current_size = 0;
			tcfa.current_progress = uInstallProgress;
			tcfa.total_size = get_folder_file_number(cmdline_option.source_path);

			//复制文件夹
			LogInfo("复制更新文件中，请耐心等待……");
			bool success = copy_folder(cmdline_option.source_path, cmdline_option.destination_path, &thread_copy_file_cb, &tcfa);
			if (success) {
				LogInfo("复制更新文件成功。");
			}
			else {
				LogError("复制更新文件失败。");
				is_copy_file_error = true;
				break;
			}

			uInstallProgress = 90;

			//删除更新文件
			if (cmdline_option.is_delete_update_file) {
				LogInfo("正在删除更新文件，请耐心等待……");
				bool success = remove_folder(cmdline_option.source_path);
				if (success) {
					LogInfo("删除更新文件成功！");
				}
				else {
					//忽略错误
					LogWarn("删除更新文件失败！请手动删除。");
				}
			}

			//删除旧版本备份文件
			if (cmdline_option.is_delete_old_version_backup_file) {
				LogInfo("正在删除旧版本备份文件，请耐心等待……");
				bool success = delete_old_version_backup_file(cmdline_option.old_version_process_name, cmdline_option.latest_version);
				if (success) {
					LogInfo("删除旧版本备份文件成功！");
				}
				else {
					LogWarn("删除旧版本备份文件失败！请手动删除。");
				}
			}

			uInstallProgress += 5;
			//匹配升级的应用程序
			cmdline_option.start_process_file_path = match_upgrade_app_file_path(cmdline_option.old_version_process_name,
				cmdline_option.source_path, cmdline_option.destination_path);

			if (cmdline_option.start_process_file_path.empty()) {
				LogError("更新文件夹内部，没有找到主程序！");
				break;
			}

			//快捷方式处理
			if (cmdline_option.is_create_shortcut) {
				LogInfo("正在创建快捷方式，请耐心等待……");
				success = create_shortcut(cmdline_option.start_process_file_path, cmdline_option.is_recursion_replace_shortcut);
				
				if (success) {
					LogInfo("创建快捷方式成功！");
				}
				else {
					LogError("创建快捷方式失败！");
					break;
				}
			}

			uInstallProgress += 4;

			LogInfo("更新完成！");
		}
		else {
			ChangeStatusText("正在启动程序中，请耐心等待……");
			//等待进程退出
			if (cmdline_option.is_wait_old_version_exit) {
				wait_process_exit(cmdline_option.old_version_process_id,
					cmdline_option.old_version_process_name,
					cmdline_option.kill_old_version_process_flag,
					cmdline_option.wait_old_version_exit_timeout);

				check_process_is_exit(cmdline_option.old_version_process_id,
					cmdline_option.old_version_process_name,
					cmdline_option.kill_old_version_process_flag,
					cmdline_option.wait_old_version_exit_timeout);
			}
		}
		
		//启动进程
		if (!start_process(cmdline_option.start_process_file_path, 
			cmdline_option.start_process_command_line,
			cmdline_option.start_process_working_directory)) {
			break;
		}

		uInstallProgress += 1;
		upgrade_result = true;
	} while (false);
	bIsUpgradeFinish = TRUE;

	int elapsed_time = static_cast<int>(static_cast<float>(GetTickCount() - start_time) / 1000);
	if (!cmdline_option.is_only_start_process) {
		LogInfo("更新用时%d秒。", elapsed_time);
		update_finish(upgrade_result, cmdline_option.is_hide_window,
			cmdline_option.is_backup_file, is_copy_file_error, 
			cmdline_option.old_version_process_name,
			cmdline_option.latest_version,
			cmdline_option.destination_path,
			cmdline_option.window_style);
	}
	else {
		if (!upgrade_result) {
			if (cmdline_option.is_prompt_if_error) {
				MessageBox(nullptr, get_last_error().c_str(), "错误", MB_ICONERROR | MB_OK);
			}
		}
	}

	if (!cmdline_option.is_only_start_process) {
		//隐藏掉窗口
		SendMessage(hWnd, WM_USER_HIDE_WINDOW, 0, 0);
		upload_update_record(upgrade_result, elapsed_time);
	}
	
	PostMessage(hWnd, WM_CLOSE, 0, 0);
	return 0;
}

void update_app() 
{
	HANDLE handle = CreateThread(nullptr, 0, update_thread, nullptr, 0, nullptr);
	if (handle != INVALID_HANDLE_VALUE) {
		CloseHandle(handle);
	}
}

bool create_path(const std::string& folder)
{
	std::string folder_builder;
	std::string sub;
	sub.reserve(folder.size());
	for (std::string::const_iterator it = folder.begin(); it != folder.end(); ++it) {
		//cout << *(folder.end()-1) << endl;
		const char c = *it;
		sub.push_back(c);
		if (c == '\\' || it == folder.end() - 1) {
			folder_builder.append(sub);
			if (0 != ::_access(folder_builder.c_str(), 0)) {
				// this folder not exist
				if (0 != ::_mkdir(folder_builder.c_str())) {
					// create failed
					return false;
				}
			}
			sub.clear();
		}
	}
	return true;
}

std::map<DWORD, std::string> get_occupied_file_pids(const std::string& file_path)
{
	std::map<DWORD, std::string> ret;
	do 
	{
		DWORD dwSession = 0;
		WCHAR szSessionKey[CCH_RM_SESSION_KEY + 1] = { 0 };
		DWORD dwError = RmStartSession(&dwSession, 0, szSessionKey);
		if (dwError != ERROR_SUCCESS) {
			break;
		}

		const wchar_t* pszFile = multiByteToWideChar(file_path.c_str(), CP_ACP);
		dwError = RmRegisterResources(dwSession, 1, &pszFile, 0, NULL, 0, NULL);
		if (dwError != ERROR_SUCCESS) {
			delete[] pszFile;
			RmEndSession(dwSession);
			break;
		}
		delete[] pszFile;

		UINT nProcInfoNeeded = 0;
		UINT nProcInfo = 0;
		DWORD dwReason;
		dwError = RmGetList(dwSession, &nProcInfoNeeded, &nProcInfo, nullptr, &dwReason);
		if (dwError != ERROR_MORE_DATA) {
			RmEndSession(dwSession);
			break;
		}

		nProcInfo = nProcInfoNeeded;
		RM_PROCESS_INFO* rgpi = new RM_PROCESS_INFO[nProcInfo];
		memset(rgpi, 0, sizeof(rgpi) * nProcInfo);
		dwError = RmGetList(dwSession, &nProcInfoNeeded, &nProcInfo, rgpi, &dwReason);
		if (dwError != ERROR_SUCCESS) {
			delete[] rgpi;
			RmEndSession(dwSession);
			break;
		}

		for (UINT i = 0; i < nProcInfo; i++) {
			DWORD pid = rgpi[i].Process.dwProcessId;
			char* str = wideCharToMultiByte(rgpi[i].strAppName, CP_ACP);
			std::string app(str);
			delete[] str;
			ret.insert(std::make_pair(pid, app));
		}

		delete[] rgpi;
		RmEndSession(dwSession);
	} while (false);
	return ret;
}

bool terminate_process(DWORD pid)
{
	bool result = false;
	do 
	{
		HANDLE hProcess = OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE, FALSE, pid);
		if (hProcess == nullptr) {
			LogError("OpenProcess返回失败，错误代码：%lu。", GetLastError());
			break;
		}

		if (!TerminateProcess(hProcess, 0)) {
			CloseHandle(hProcess);
			LogError("TerminateProcess返回失败，错误代码：%lu。", GetLastError());
			break;
		}

		DWORD object = WaitForSingleObject(hProcess, 30000);
		if (object != WAIT_OBJECT_0) {
			CloseHandle(hProcess);
			LogError("WaitForSingleObject返回失败，错误代码：%lu。", GetLastError());
			break;
		}
		CloseHandle(hProcess);
		result = true;
	} while (false);
	return result;
}

std::string get_host_name()
{
	std::string host_name("unknown");
	//bool clear_wsa = false;
	do 
	{
		/*SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
		if (s == INVALID_SOCKET && WSAGetLastError() == WSANOTINITIALISED) {
			WSADATA wsaData;
			int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
			if (result != 0) {
				break;
			}
			clear_wsa = true;
		}*/

		char hostname[256] = { 0 };
		if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
			break;
		}
		host_name = hostname;
	} while (false);
	
	/*if (clear_wsa) {
		WSACleanup();
	}*/
	return host_name;
}

wsa_lock::wsa_lock() :socket_(INVALID_SOCKET), init_(false)
{
	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET && WSAGetLastError() == WSANOTINITIALISED) {
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0) {
			init_ = true;
		}
	}
}

wsa_lock::~wsa_lock()
{
	if (init_) {
		WSACleanup();
	}
}

static wsa_lock global_wsa_lock;

bool get_ipaddr(const std::string& domain, std::string& addr)
{
	bool result = false;
	do 
	{
		/*WSADATA wsaData;
		int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (result != 0) {
			break;
		}*/

		struct addrinfo hints = { 0 };
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;

		struct addrinfo* res = nullptr;
		if (getaddrinfo(domain.c_str(), nullptr, &hints, &res) != 0) {
			break;
		}

		for (struct addrinfo* ptr = res; ptr != nullptr; ptr = ptr->ai_next) {
			char ipStr[INET_ADDRSTRLEN] = { 0 };
			struct sockaddr_in* sockaddr_ipv4 = reinterpret_cast<struct sockaddr_in*>(ptr->ai_addr);
			inet_ntop(AF_INET, &(sockaddr_ipv4->sin_addr), ipStr, INET_ADDRSTRLEN);
			addr = ipStr;
			break;
		}

		freeaddrinfo(res);
		/*WSACleanup();*/
		result = true;
	} while (false);
	return result;
}

bool is_valid_ipaddr(const std::string& ip)
{
	// 用来记录每个部分的值
	int segments = 0;
	int segmentValue = 0;
	bool hasDigits = false;

	for (size_t i = 0; i < ip.length(); ++i) {
		char c = ip[i];

		if (std::isdigit(c)) {
			// 累计数字部分的值
			segmentValue = segmentValue * 10 + (c - '0');
			if (segmentValue > 255) {
				return false; // 每段值应在 0-255 之间
			}
			hasDigits = true;
		}
		else if (c == '.') {
			// 遇到分段符号，检查当前段是否合法
			if (!hasDigits) {
				return false; // 确保段中有数字
			}
			++segments;
			if (segments > 3) {
				return false; // 最多只能有 3 个点
			}
			segmentValue = 0; // 重置当前段值
			hasDigits = false; // 重置是否有数字的标志
		}
		else {
			return false; // 非法字符
		}
	}

	// 检查最后一个段
	return hasDigits && segments == 3;
}

std::vector<std::string> get_localhost_addr() {
	std::vector<std::string> ips;
	ULONG outBufLen = 0;
	DWORD dwRetVal = 0;
	PIP_ADAPTER_ADDRESSES pAddresses = NULL;
	if (GetAdaptersAddresses(AF_INET, 0, NULL, pAddresses, &outBufLen) == ERROR_BUFFER_OVERFLOW) {
		pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
	}

	if (pAddresses) {
		dwRetVal = GetAdaptersAddresses(AF_INET, 0, NULL, pAddresses, &outBufLen);
		if (dwRetVal == NO_ERROR) {
			PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses;
			while (pCurrAddresses != NULL) {
				if (pCurrAddresses->IfType == IF_TYPE_ETHERNET_CSMACD ||
					pCurrAddresses->IfType == IF_TYPE_IEEE80211) {
						std::string adapterName = pCurrAddresses->AdapterName;
						std::wstring friendlyName = pCurrAddresses->FriendlyName;

						if (adapterName.find("VMware") != std::string::npos ||
							adapterName.find("VirtualBox") != std::string::npos ||
							friendlyName.find(L"VMware") != std::wstring::npos ||
							friendlyName.find(L"VirtualBox") != std::wstring::npos) {
							pCurrAddresses = pCurrAddresses->Next;
							continue;
						}

						for (PIP_ADAPTER_UNICAST_ADDRESS pUnicast = pCurrAddresses->FirstUnicastAddress;
							pUnicast != NULL;
							pUnicast = pUnicast->Next) {
								struct sockaddr_in* sa_in = (struct sockaddr_in*)pUnicast->Address.lpSockaddr;
								std::string addr = inet_ntoa(sa_in->sin_addr);
								if (addr.find("169.254") != std::string::npos) {
									continue;
								}
								ips.push_back(addr);
						}
				}
				pCurrAddresses = pCurrAddresses->Next;
			}
		}
		free(pAddresses);
	}

	return ips;
}

bool get_guid(std::string& guid) {
	bool result = false;
	HKEY hKey = nullptr;
	do 
	{
		CHAR szGuid[256] = { 0 };
		DWORD dwSize = sizeof(szGuid);
		DWORD dwType = REG_SZ;

		if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Cryptography",
			0, KEY_READ | KEY_WOW64_64KEY, &hKey) != ERROR_SUCCESS) {
				break;
		}

		if (RegQueryValueExA(hKey, "MachineGuid", NULL, &dwType, (LPBYTE)szGuid, &dwSize) != ERROR_SUCCESS) {
			break;
		}
		guid = szGuid;
		result = true;
	} while (false);

	if (hKey) {
		RegCloseKey(hKey);
	}
	return result;
}

bool get_cpuid(std::string& cpuid) {
	cpuid.clear();
	HRESULT hres;

	// 初始化 COM
	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres)) {
		return false;
	}

	// 设置安全级别
	hres = CoInitializeSecurity(
		NULL,
		-1,
		NULL,
		NULL,
		RPC_C_AUTHN_LEVEL_DEFAULT,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		EOAC_NONE,
		NULL);

	if (FAILED(hres)) {
		CoUninitialize();
		return false;
	}

	IWbemLocator* pLoc = NULL;
	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator,
		(LPVOID*)&pLoc);

	if (FAILED(hres)) {
		CoUninitialize();
		return false;
	}

	IWbemServices* pSvc = NULL;
	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"),
		NULL,
		NULL,
		0,
		NULL,
		0,
		0,
		&pSvc);

	if (FAILED(hres)) {
		pLoc->Release();
		CoUninitialize();
		return false;
	}

	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT ProcessorId FROM Win32_Processor"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);

	if (FAILED(hres)) {
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return false;
	}

	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;

	while (pEnumerator) {
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
		if (0 == uReturn) {
			break;
		}

		VARIANT vtProp;
		hr = pclsObj->Get(L"ProcessorId", 0, &vtProp, 0, 0);
		if (SUCCEEDED(hr)) {
			cpuid = _bstr_t(vtProp.bstrVal);
			VariantClear(&vtProp);
		}

		pclsObj->Release();
	}

	pEnumerator->Release();
	pSvc->Release();
	pLoc->Release();
	CoUninitialize();

	return !cpuid.empty();
}

std::string get_uuid()
{
	std::string uuid;
	if (!get_guid(uuid)) {
		if (!get_cpuid(uuid)) {
			uuid = "undefined";
		}
	}
	return uuid;
}

#include "picojson.h"
#include "base64.h"

static size_t curl_write_body_cb(void* contents, size_t size, size_t nmemb, void* userp) {
	size_t total = size * nmemb;
	std::string* body = static_cast<std::string*>(userp);
	body->append(static_cast<char*>(contents), total);
	return total;
}

bool upload_update_record(bool success, int elapsed_time)
{
	bool result = false;
	//curl_global_init(CURL_GLOBAL_DEFAULT);
	CURL* curl = nullptr;
	struct curl_slist* headers = nullptr;
	do 
	{	
		//"上传更新记录标志（0：不上传，1：不包含更新日志，2：如果失败包含更新日志，3：包含更新日志）",
		if (cmdline_option.upload_update_record_flag == 0) {
			result = true;
			break;
		}

		std::string update_log;
		if ((cmdline_option.upload_update_record_flag == 2 && !success) ||
			cmdline_option.upload_update_record_flag == 3) {
			update_log = base64_encode(ansi_to_utf8(log_filetext));
		}

		curl = curl_easy_init();
		if (!curl) {
			write_log_type_fmt("error", "curl初始化失败。");
			break;
		}

		picojson::object obj;
		std::vector<std::string> addr = get_localhost_addr();
		std::vector<picojson::value> ip_address;
		for (size_t i = 0; i < addr.size(); ++i) {
			ip_address.push_back(picojson::value(addr[i].c_str()));
		}

		time_t timestamp = get_time_stamp_ms();
		std::string uuid = ansi_to_utf8(get_uuid());
		std::string app_name = ansi_to_utf8(cmdline_option.old_version_process_name);
		std::string app_version = ansi_to_utf8(cmdline_option.latest_version);
		std::string app_secret = utility::sha256(uuid + app_name + app_version + format_str("%lld", ((timestamp + elapsed_time) >> 1)));
		std::string host_name = ansi_to_utf8(get_host_name());
		obj["method"] = picojson::value("upload_update_record");
		obj["uuid"] = picojson::value(uuid);
		obj["app_name"] = picojson::value(app_name);
		obj["app_version"] = picojson::value(app_version);
		obj["app_secret"] = picojson::value(app_secret);
		obj["ip_address"] = picojson::value(ip_address);
		obj["host_name"] = picojson::value(host_name);
		obj["update_result"] = picojson::value(success);
		obj["update_log"] = picojson::value(update_log);
		obj["elapsed_time"] = picojson::value(static_cast<double>(elapsed_time));
		obj["timestamp"] = picojson::value(static_cast<double>(timestamp));

		std::string json_str = picojson::value(obj).serialize();
		std::string req_body;
		if (!deflate_compress(json_str.c_str(), json_str.size(), req_body)) {
			write_log_type_fmt("error", "deflate压缩请求体失败。");
			break;
		}

		char url[1024] = { 0 };
		sprintf(url, "https://%s:%d/post_request", cmdline_option.server_address.c_str(), cmdline_option.server_port);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, req_body.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, req_body.size());

		std::string content_length = format_str("Content-Length: %lu", req_body.length());
		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, "Content-Encoding: deflate");
		headers = curl_slist_append(headers, content_length.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);

		std::string resp_body;
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curl_write_body_cb);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp_body);

		// 执行请求
		CURLcode code = curl_easy_perform(curl);
		if (code != CURLE_OK) {
			write_log_type_fmt("error", "上传更新记录失败！%s。", curl_easy_strerror(code));
			break;
		}

		long status_code = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);

		if (status_code != 200) {
			write_log_type_fmt("error", "上传更新记录失败！服务器响应异常状态码%d。", status_code);
			break;
		}

		picojson::value value;
		picojson::parse(value, resp_body);
		obj = value.get<picojson::object>();

		int error_code =static_cast<int>(obj["error_code"].get<double>());
		std::string error_string = obj["error_string"].get<std::string>();
		error_string = utf8_to_ansi(error_string);

		if (error_code == 0) {
			write_log_type_fmt("info", "上传更新记录成功！");
		}
		else {
			write_log_type_fmt("error", "上传更新记录失败！%s。", error_string.c_str());
			break;
		}

		result = true;
	} while (false);

	if (headers) {
		curl_slist_free_all(headers);
	}

	if (curl) {
		curl_easy_cleanup(curl);
	}

	//curl_global_cleanup();
	return result;
}

void kill_occupied_file_process(const std::string& process)
{
	std::map<DWORD, std::string> pids = get_occupied_file_pids(process);
	for (std::map<DWORD, std::string>::iterator it = pids.begin(); it != pids.end(); ++it) {
		bool ret = terminate_process(it->first);
		if (ret) {
			LogInfo("关闭占用%s文件进程成功！占用进程：%s，PID：%lu。", process.c_str(), it->second.c_str(), it->first);
		}
		else {
			LogError("关闭占用%s文件进程失败！占用进程：%s，PID：%lu，错误代码：%lu。", 
				process.c_str(), it->second.c_str(), it->first, GetLastError());
		}
	}
}

bool deflate_compress(const char* data, std::size_t size, std::string& compress_data)
{
	z_stream zs = { 0 };

	if (deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
		return false;
	}

	zs.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(data));
	zs.avail_in = size;

	int ret;
	char outbuffer[4096] = { 0 };

	do {
		zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
		zs.avail_out = sizeof(outbuffer);

		ret = ::deflate(&zs, Z_FINISH);

		if (compress_data.size() < zs.total_out) {
			compress_data.append(outbuffer, zs.total_out - compress_data.size());
		}
	} while (ret == Z_OK);

	deflateEnd(&zs);

	return ret == Z_STREAM_END;
}

std::string get_file_exstension(const std::string& file_name)
{
	size_t dotPos = file_name.find_last_of(".");
	if (dotPos != std::string::npos) {
		return file_name.substr(dotPos);
	}
	return "";
}