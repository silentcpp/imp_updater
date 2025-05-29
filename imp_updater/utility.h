#include "stdafx.h"

typedef struct _cmdline_option {
	_cmdline_option();
	~_cmdline_option();
	std::map<std::string, std::string> kv;

	bool is_create_shortcut;//1
	bool is_recursion_replace_shortcut;//2
	bool is_delete_update_file;//3
	bool is_hide_window;//4
	bool is_show_topmost;//5
	bool is_backup_file;//6
	bool is_compress_backup_file;//7
	bool is_only_start_process;//8

	int window_width;//9
	int window_height;//10

	DWORD old_version_process_id;//11
	std::string old_version_process_name;//12
	std::string source_path;//13
	std::string destination_path;//14
	std::string start_process_command_line;//15
	std::string window_title;//16
	std::string sensitive_dll;//17
	std::string latest_version;//18
	std::string start_process_file_path;//19
	std::string start_process_working_directory;//20

	bool is_prompt_if_error;//21
	std::string server_address;//22
	int server_port;//23
	int upload_update_record_flag;//24
	int window_style;//25
	bool is_delete_old_version_backup_file;//26
	bool is_wait_old_version_exit;//27
	int wait_old_version_exit_timeout;//28
	int kill_old_version_process_flag;//29
	int upload_update_log_max_size;//30
} cmdline_option_t;

extern cmdline_option_t cmdline_option;
extern std::string last_error;
extern FILE* logfile_ptr;

typedef bool(*filter_file_cb_t)(const std::string& filepath, void* args);

//兼容filter_file_cb_t
typedef bool(*progress_cb_t)(const std::string& filepath, void* args);

#define WM_USER_UPDATE_STATUS (WM_USER + 1)
#define WM_USER_HIDE_WINDOW (WM_USER + 2)
#define WM_USER_SHOW_WINDOW (WM_USER + 3)

void replace_with_windows_path_symbol(std::string& filepath);

void replace_with_linux_path_symbol(std::string& filepath);

std::string format_error(uint32_t error_code);

void trim_str(std::string& str);

std::list<std::string>::iterator find_str_list(std::list<std::string>& ls, const std::string& str, bool case_insensitive = true);

std::set<std::string>::iterator find_str_set(std::set<std::string>& ss, const std::string& str, bool case_insensitive = true);

std::vector<std::string>::iterator find_str_vector(std::vector<std::string>& sv, const std::string& str, bool case_insensitive = true);

time_t get_time_stamp_s();

time_t get_time_stamp_ms();

std::vector<std::string> get_cmdline_option_arr();

std::vector<std::string> get_cmdline_option_comment_arr();

std::vector<std::string> get_often_intercepted_dll_name();

void open_log_file();

void write_log(const char* fmt, ...);

void write_log_type(const char* type, const std::string& str);

void write_log_type_fmt(const char* type, const char* fmt, ...);

void close_log_file();

std::string format_str(const char* fmt, ...);

void set_last_error(const char* fmt, ...);

std::string get_last_error();

char* wideCharToMultiByte(const wchar_t* str, uint32_t code_page);

wchar_t* multiByteToWideChar(const char* str, uint32_t code_page);

std::string utf8_to_ansi(const std::string& utf8);

std::string ansi_to_utf8(const std::string& ansi);

template<class T> T split_by_delimiter(const std::string& s, char c) 
{
	T tokens;
	size_t start = 0;
	size_t end = 0;

	while ((end = s.find(c, start)) != std::string::npos) {
		tokens.push_back(s.substr(start, end - start));
		start = end + 1; // 移动到下一个分隔符之后
	}

	// 添加最后一个分隔符之后的子字符串
	if (start < s.length()) {
		tokens.push_back(s.substr(start));
	}

	return tokens;
}

bool is_all_digits(const char *str);

void traversal_directory(const std::string& path, std::list<std::string>& files, bool recursion = true,
	filter_file_cb_t filter_file_cb = nullptr,
	void* args = nullptr);

int miniz_compress_folder(const std::string& folder_path, const std::string& zip_filepath, 
						const std::list<std::string>& filter_file_name = std::list<std::string>(),
						progress_cb_t progress_cb = nullptr, void* args = nullptr);

int miniz_decompress_file(const std::string& zip_filepath, const std::string& extract_path);

std::string get_current_path();

std::string get_base_name(const std::string& file_name, bool is_special = true);

std::string get_file_name(const std::string& file_path);

bool is_occupied(const std::string& name);

int compare_file(const std::string& file_path1, const std::string& file_path2);

bool copy_file(const std::string& source_file, const std::string& dest_file);

template<class T> void get_folder_files(const std::string& path, T& files, bool recursion = true,
	filter_file_cb_t filter_file_cb = nullptr, void* args = nullptr, bool contains_dir = false)
{
	HANDLE hFind;
	WIN32_FIND_DATA ffd;
	std::string fullPath = path + "\\*";
	hFind = FindFirstFile(fullPath.c_str(), &ffd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (strcmp(ffd.cFileName, ".") != 0 && strcmp(ffd.cFileName, "..") != 0)
			{
				std::string newPath = path + "\\" + ffd.cFileName;
				if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					if (recursion) {
						if (contains_dir) {
							files.push_back(newPath);
						}
						get_folder_files(newPath, files, recursion, filter_file_cb, args, contains_dir);
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

size_t get_folder_file_number(const std::string& dir_path, filter_file_cb_t filter_cb = nullptr, void* args = nullptr);

bool copy_folder(const std::string& source_folder, const std::string& dest_folder,
				 filter_file_cb_t filter_file_cb = nullptr,
				 void* args = nullptr);

bool remove_folder(const std::string& path);

void find_file(const char* dir, const char* suffix, std::list<std::string>& files, bool recursion);

bool create_shortcut(const std::string& file_path, bool recursion_replace);

cmdline_option_t parse_command_line_option(const std::string& cmdline);

bool get_all_processes(std::map<DWORD, std::string>& processes);

bool check_path_is_valid(const std::string& source_path, const std::string& destination_path);

bool wait_process_exit(DWORD process_id, const std::string& process_name, int kill_flag, size_t timeout);

bool check_process_is_exit(DWORD process_id, const std::string& process_name, int kill_flag, size_t timeout);

bool check_file_is_occupy(const std::string& destination_path);

bool backup_file(const std::string& process_name, 
				 const std::string& latest_version,
				 const std::string& source_path,
				 const std::string& destination_path,
				 bool is_compress_backup_file,
				 progress_cb_t progress_cb,
				 void* args);

//bool backup_file(const std::string& source_path, const std::string& destination_path,
//				 const std::string& backup_file_filter_suffix, bool is_compress_backup_file);

bool delete_old_version_backup_file(const std::string& process_name, const std::string& latest_version);

std::string match_upgrade_app_file_path(const std::string& process_name, const std::string& source_path,
										const std::string& destination_path);

bool rollback_file(const std::string& process_name, const std::string& latest_version,
				   const std::string& destination_path,
				   bool is_compress_backup_file);

bool start_process(const std::string& file_path,
				   const std::string& command_line,
				   const std::string& working_directory);

void update_finish(bool ret, bool is_hide_window, bool is_backup_file, 
					bool is_copy_file_error,
					const std::string& process_name,
					const std::string& latest_version,
					const std::string& destination_path,
					int window_style);

void update_app();

bool create_path(const std::string& path);

std::map<DWORD, std::string> get_occupied_file_pids(const std::string& file_path);

bool terminate_process(DWORD pid);

std::string get_host_name();

class wsa_lock {
public:
	wsa_lock();
	~wsa_lock();

private:
	SOCKET socket_;
	bool init_;
};

bool get_ipaddr(const std::string& domain, std::string& addr);

bool is_valid_ipaddr(const std::string& ip);

std::vector<std::string> get_localhost_addr();

bool get_guid(std::string& guid);

bool get_cpuid(std::string& cpuid);

std::string get_uuid();

bool upload_update_record(bool success, int elapsed_time);

#if ENABLE_LIBZIP
int libzip_compress(const std::string& compress_path, 
	const std::string &zip_path, 
	const std::string& password = std::string(), 
	const std::list<std::string>& filter_file_suffix = std::list<std::string>(),
	progress_cb_t progress_cb = nullptr, 
	void* args = nullptr);

int libzip_decompress(const std::string& zip_path, const std::string& decompress_path, const std::string& password = std::string());
#endif

void kill_occupied_file_process(const std::string& process);

bool deflate_compress(const char* data, std::size_t size, std::string& compress_data);

std::string get_file_exstension(const std::string& file_name);
