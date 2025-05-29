#include "stdafx.h"
#include "tiny_http.h"


tiny_http::tiny_http()
{

}

tiny_http::~tiny_http()
{

}

tiny_http::response_t tiny_http::post(const tiny_http::request_t& request)
{
	tiny_http::response_t response;
	HINTERNET hSession = nullptr, hConnect = nullptr, hRequest = nullptr;
	do
	{
		std::string protocol, domain, uri, addr;
		int port = 0;
		if (!parse_http_url(request.url, protocol, domain, port, uri)) {
			set_last_error("解析URL失败");
			break;
		}

		if (!is_valid_ipaddr(domain)) {
			if (!get_ipaddr(domain, addr)) {
				break;
			}
		}
		else {
			addr = domain;
		}

		if (!(hSession = WinHttpOpen(L"WinHTTP Example/1.0",
			WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
			WINHTTP_NO_PROXY_NAME,
			WINHTTP_NO_PROXY_BYPASS, 0))) {
			set_last_error("WinHttpOpen失败，错误代码：%lu。", GetLastError());
			break;
		}

		DWORD connectTimeout = request.connect_timeout;
		if (!WinHttpSetOption(hSession, WINHTTP_OPTION_CONNECT_TIMEOUT, &connectTimeout, sizeof(connectTimeout))) {
			set_last_error("设置WINHTTP_OPTION_CONNECT_TIMEOUT失败，错误代码：%lu。", GetLastError());
			break;
		}

		DWORD sendTimeout = 5000;
		if (!WinHttpSetOption(hSession, WINHTTP_OPTION_SEND_TIMEOUT, &sendTimeout, sizeof(sendTimeout))) {
			set_last_error("设置WINHTTP_OPTION_SEND_TIMEOUT失败，错误代码：%lu。", GetLastError());
			break;
		}

		DWORD receiveTimeout = 5000;
		if (!WinHttpSetOption(hSession, WINHTTP_OPTION_RECEIVE_TIMEOUT, &receiveTimeout, sizeof(receiveTimeout))) {
			set_last_error("设置WINHTTP_OPTION_RECEIVE_TIMEOUT失败，错误代码：%lu。", GetLastError());
			break;
		}

		if (!(hConnect = WinHttpConnect(hSession, ansi_to_unicode(addr).c_str(), port, 0))) {
			set_last_error("WinHttpConnect失败，错误代码：%lu。", GetLastError());
			break;
		}

		std::wstring object_name = ansi_to_unicode(uri);
		if (!(hRequest = WinHttpOpenRequest(hConnect,
			L"POST",
			object_name.empty() ?
			nullptr :
			object_name.c_str(),
			NULL, WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			WINHTTP_FLAG_SECURE))) {
			set_last_error("WinHttpOpenRequest失败，错误代码：%lu。", GetLastError());
			break;
		}

		if (_stricmp(protocol.c_str(), "https") == 0) {
			DWORD dwFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA |
				SECURITY_FLAG_IGNORE_CERT_DATE_INVALID |
				SECURITY_FLAG_IGNORE_CERT_CN_INVALID;
			if (!WinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(dwFlags))) {
				set_last_error("设置WINHTTP_OPTION_SECURITY_FLAGS失败，错误代码：%lu。", GetLastError());
				break;
			}
		}

		std::wstring headers_str;
		for (std::map<std::string, std::string>::const_iterator it = request.headers.begin();
			it != request.headers.end(); ++it) {
			std::wstring header;
			header.append(ansi_to_unicode(it->first)).
				append(L": ").
				append(ansi_to_unicode(it->second)).
				append(L"\r\n");
			headers_str.append(header);
		}

		if (!WinHttpAddRequestHeaders(hRequest, headers_str.c_str(), headers_str.length(), WINHTTP_ADDREQ_FLAG_ADD)) {
			set_last_error("WinHttpAddRequestHeaders失败，错误代码：%lu。", GetLastError());
			break;
		}

		if (!WinHttpSendRequest(hRequest,
			WINHTTP_NO_ADDITIONAL_HEADERS,
			-1L,
			request.body.empty() ?
			nullptr :
			const_cast<char*>(request.body.c_str()),
			request.body.length(),
			request.body.length(),
			0)) {
			set_last_error("WinHttpSendRequest失败，错误代码：%lu。", GetLastError());
			break;
		}

		if (!WinHttpReceiveResponse(hRequest, nullptr)) {
			set_last_error("WinHttpReceiveResponse失败，错误代码：%lu。", GetLastError());
			break;
		}

		DWORD headers_size;
		if (!WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF,
			WINHTTP_HEADER_NAME_BY_INDEX, NULL, &headers_size, WINHTTP_NO_HEADER_INDEX)) {
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
				wchar_t* headers_buffer = new wchar_t[headers_size / sizeof(wchar_t)];
				memset(headers_buffer, 0, headers_size);
				if (!WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF,
					WINHTTP_HEADER_NAME_BY_INDEX, headers_buffer, &headers_size, WINHTTP_NO_HEADER_INDEX)) {
					delete[] headers_buffer;
					set_last_error("WinHttpQueryHeaders失败，错误代码：%lu。", GetLastError());
					break;
				}
				std::string headers_str(unicode_to_ansi(headers_buffer));
				response.headers = parse_http_headers_str(headers_str, response.status_code, response.status_message);
				delete[] headers_buffer;

				for (std::map<std::string, std::string>::iterator it = response.headers.begin();
					it != response.headers.end(); ++it) {
					if (!_stricmp(it->first.c_str(), "Content-Length")) {
						response.content_length = std::atoi(it->second.c_str());
						break;
					}
				}
			}
		}

		DWORD size = 0;
		char buffer[8192] = { 0 };
		bool success = true;
		do {
			memset(buffer, 0, sizeof(buffer));
			if (!WinHttpQueryDataAvailable(hRequest, &size)) {
				success = false;
				set_last_error("WinHttpQueryDataAvailable失败，错误代码：%lu。", GetLastError());
				break;
			}

			if (size > 0) {
				DWORD numberOfBytes = 0;
				if (!WinHttpReadData(hRequest, (LPVOID)buffer, size, &numberOfBytes)) {
					success = false;
					set_last_error("WinHttpReadData失败，错误代码：%lu。", GetLastError());
					break;
				}
				response.body.append(buffer, numberOfBytes);
			}
		} while (size > 0);

		if (!success) {
			break;
		}
	} while (false);
	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);
	return response;
}

tiny_http::response_t tiny_http::get(const tiny_http::request_t& request)
{
	tiny_http::response_t response;
	HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;
	do
	{
		std::string protocol, domain, uri, addr;
		int port = 0;
		if (!parse_http_url(request.url, protocol, domain, port, uri)) {
			set_last_error("解析URL失败");
			break;
		}

		if (!is_valid_ipaddr(domain)) {
			if (!get_ipaddr(domain, addr)) {
				break;
			}
		}
		else {
			addr = domain;
		}

		if (!(hSession = WinHttpOpen(L"WinHTTP Example/1.0",
			WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
			WINHTTP_NO_PROXY_NAME,
			WINHTTP_NO_PROXY_BYPASS, 0))) {
			set_last_error("WinHttpOpen失败，错误代码：%lu。", GetLastError());
			break;
		}

		DWORD connectTimeout = request.connect_timeout;
		if (!WinHttpSetOption(hSession, WINHTTP_OPTION_CONNECT_TIMEOUT, &connectTimeout, sizeof(connectTimeout))) {
			set_last_error("设置WINHTTP_OPTION_CONNECT_TIMEOUT失败，错误代码：%lu。", GetLastError());
			break;
		}

		DWORD sendTimeout = 5000;
		if (!WinHttpSetOption(hSession, WINHTTP_OPTION_SEND_TIMEOUT, &sendTimeout, sizeof(sendTimeout))) {
			set_last_error("设置WINHTTP_OPTION_SEND_TIMEOUT失败，错误代码：%lu。", GetLastError());
			break;
		}

		DWORD receiveTimeout = 5000;
		if (!WinHttpSetOption(hSession, WINHTTP_OPTION_RECEIVE_TIMEOUT, &receiveTimeout, sizeof(receiveTimeout))) {
			set_last_error("设置WINHTTP_OPTION_RECEIVE_TIMEOUT失败，错误代码：%lu。", GetLastError());
			break;
		}

		if (!(hConnect = WinHttpConnect(hSession, ansi_to_unicode(addr).c_str(), port, 0))) {
			set_last_error("WinHttpConnect失败，错误代码：%lu。", GetLastError());
			break;
		}

		std::wstring object_name = ansi_to_unicode(uri);
		if (!(hRequest = WinHttpOpenRequest(hConnect, L"GET",
			object_name.empty() ?
			nullptr :
			object_name.c_str(),
			NULL, WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			WINHTTP_FLAG_SECURE))) {
			set_last_error("WinHttpOpenRequest失败，错误代码：%lu。", GetLastError());
			break;
		}

		if (_stricmp(protocol.c_str(), "https") == 0) {
			DWORD dwFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA |
				SECURITY_FLAG_IGNORE_CERT_DATE_INVALID |
				SECURITY_FLAG_IGNORE_CERT_CN_INVALID;

			if (!WinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(dwFlags))) {
				set_last_error("设置WINHTTP_OPTION_SECURITY_FLAGS失败，错误代码：%lu。", GetLastError());
				break;
			}
		}

		std::wstring headers_str;
		for (std::map<std::string, std::string>::const_iterator it = request.headers.begin();
			it != request.headers.end(); ++it) {
			std::wstring header;
			header.append(ansi_to_unicode(it->first)).
				append(L": ").
				append(ansi_to_unicode(it->second)).
				append(L"\r\n");
			headers_str.append(header);
		}

		if (!WinHttpAddRequestHeaders(hRequest, headers_str.c_str(), headers_str.length(), WINHTTP_ADDREQ_FLAG_ADD)) {
			set_last_error("WinHttpAddRequestHeaders失败，错误代码：%lu。", GetLastError());
			break;
		}

		if (!WinHttpSendRequest(hRequest,
			WINHTTP_NO_ADDITIONAL_HEADERS,
			-1L,
			request.body.empty() ?
			nullptr :
			const_cast<char*>(request.body.c_str()),
			request.body.length(),
			request.body.length(),
			0)) {
			set_last_error("WinHttpSendRequest失败，错误代码：%lu。", GetLastError());
			break;
		}

		if (!WinHttpReceiveResponse(hRequest, NULL)) {
			set_last_error("WinHttpReceiveResponse失败，错误代码：%lu。", GetLastError());
			break;
		}

		DWORD headers_size;
		if (!WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF,
			WINHTTP_HEADER_NAME_BY_INDEX, NULL, &headers_size, WINHTTP_NO_HEADER_INDEX)) {
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
				wchar_t* headers_buffer = new wchar_t[headers_size / sizeof(wchar_t)];
				memset(headers_buffer, 0, headers_size);
				if (!WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF,
					WINHTTP_HEADER_NAME_BY_INDEX, headers_buffer, &headers_size, WINHTTP_NO_HEADER_INDEX)) {
					delete[] headers_buffer;
					set_last_error("WinHttpQueryHeaders失败，错误代码：%lu。", GetLastError());
					break;
				}
				std::string headers_str(unicode_to_ansi(headers_buffer));
				response.headers = parse_http_headers_str(headers_str, response.status_code, response.status_message);
				delete[] headers_buffer;

				for (std::map<std::string, std::string>::iterator it = response.headers.begin();
					it != response.headers.end(); ++it) {
					if (!_stricmp(it->first.c_str(), "Content-Length")) {
						response.content_length = std::atoi(it->second.c_str());
						break;
					}
				}
			}
		}

		DWORD size = 0;
		char buffer[8192] = { 0 };
		bool success = true;
		do {
			memset(buffer, 0, sizeof(buffer));
			if (!WinHttpQueryDataAvailable(hRequest, &size)) {
				success = false;
				set_last_error("WinHttpQueryDataAvailable失败，错误代码：%lu。", GetLastError());
				break;
			}

			if (size > 0) {
				DWORD numberOfBytes = 0;
				if (!WinHttpReadData(hRequest, (LPVOID)buffer, size, &numberOfBytes)) {
					success = false;
					set_last_error("WinHttpReadData失败，错误代码：%lu。", GetLastError());
					break;
				}
				response.body.append(buffer, numberOfBytes);
			}
		} while (size > 0);

		if (!success) {
			break;
		}


	} while (false);
	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);
	return response;
}

std::string tiny_http::str_trim(const std::string& str) const
{
	size_t first = str.find_first_not_of(" \t\n\r\f\v");
	size_t last = str.find_last_not_of(" \t\n\r\f\v");
	if (first == std::string::npos) {
		return "";
	}
	return str.substr(first, last - first + 1);
}

std::vector<std::string> tiny_http::str_split(const std::string& str, const char* delimiter) const
{
	std::vector<std::string> ret;
	size_t npos = 0, offset = 0;
	while ((npos = str.find(delimiter, offset)) != std::string::npos) {
		ret.push_back(str.substr(offset, npos - offset));
		offset = npos + strlen(delimiter);
	}

	std::string last = str.substr(offset);
	if (!last.empty()) {
		ret.push_back(last);
	}
	return ret;
}

std::map<std::string, std::string> tiny_http::parse_http_headers_str(const std::string& header_str, int& status_code, std::string& status_message) const
{
	std::map<std::string, std::string> ret;
	std::vector<std::string> headers_vec = str_split(header_str, "\r\n");
	for (size_t i = 0; i < headers_vec.size(); ++i) {
		if (headers_vec[i].empty()) {
			continue;
		}

		std::vector<std::string> kv = str_split(headers_vec[i], ":");
		if (kv.size() != 2) {
			kv = str_split(headers_vec[i], " ");
			if (kv.size() == 3) {
				status_code = std::atoi(kv[1].c_str());
				status_message = kv[2];
			}
			continue;
		}
		std::string key = str_trim(kv[0]);
		std::string value = str_trim(kv[1]);
		ret[key] = value;
	}
	return ret;
}

bool tiny_http::parse_http_url(const std::string& url, std::string& protocol, std::string& domain, int& port, std::string& uri) const
{
	size_t protocolEnd = url.find("://");
	if (protocolEnd != std::string::npos) {
		protocol = url.substr(0, protocolEnd);
	}
	else {
		return false;
	}

	size_t hostStart = protocolEnd + 3; // 跳过 "://"
	size_t portStart = url.find(':', hostStart); // 找到冒号（端口的起始位置）

	if (portStart != std::string::npos) {
		// 提取域名部分
		domain = url.substr(hostStart, portStart - hostStart);

		// 提取端口部分
		size_t portEnd = url.find('/', portStart); // 检查端口后是否有路径
		if (portEnd == std::string::npos) {
			port = std::atoi(url.substr(portStart + 1).c_str());
		}
		else {
			port = std::atoi(url.substr(portStart + 1, portEnd - portStart - 1).c_str());
			uri = url.substr(portEnd);
		}
	}
	else {
		size_t uriStart = url.find('/', hostStart);
		if (uriStart != std::string::npos) {
			domain = url.substr(hostStart, uriStart - hostStart);
			uri = url.substr(uriStart);
		}
		else {
			domain = url.substr(hostStart);
		}
		port = (protocol == "http") ? 80 : 443;
	}

	return true;
}

bool tiny_http::get_ipaddr(const std::string& domain, std::string& addr)
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
		int err = 0;
		if ((err = getaddrinfo(domain.c_str(), nullptr, &hints, &res)) != 0) {
			set_last_error("解析域名失败，%s。", gai_strerror(err));
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

std::string tiny_http::utf8_to_ansi(const std::string& utf8) const
{
	std::string str;
	int size = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
	wchar_t* wstr = new wchar_t[size];
	MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, wstr, size);
	size = WideCharToMultiByte(CP_ACP, 0, wstr, -1, nullptr, 0, nullptr, nullptr);
	char* mstr = new char[size];
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, mstr, size, NULL, NULL);
	str = mstr;
	delete[] wstr;
	delete[] mstr;
	return str;
}

std::string tiny_http::ansi_to_utf8(const std::string& ansi) const
{
	std::string str;
	int size = MultiByteToWideChar(CP_ACP, 0, ansi.c_str(), -1, nullptr, 0);
	wchar_t* wstr = new wchar_t[size];
	MultiByteToWideChar(CP_ACP, 0, ansi.c_str(), -1, wstr, size);
	size = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, nullptr, 0, nullptr, nullptr);
	char* mstr = new char[size];
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, mstr, size, NULL, NULL);
	str = mstr;
	delete[] wstr;
	delete[] mstr;
	return str;
}

std::string tiny_http::unicode_to_ansi(const std::wstring& wstr, unsigned int codepage) const
{
	std::string str;
	int size = WideCharToMultiByte(codepage, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	char* mstr = new char[size];
	WideCharToMultiByte(codepage, 0, wstr.c_str(), -1, mstr, size, NULL, NULL);
	str = mstr;
	delete[] mstr;
	return str;
}

std::wstring tiny_http::ansi_to_unicode(const std::string& str, unsigned int codepage) const
{
	std::wstring wstr;
	int size = MultiByteToWideChar(codepage, 0, str.c_str(), -1, nullptr, 0);
	wchar_t* mstr = new wchar_t[size];
	MultiByteToWideChar(codepage, 0, str.c_str(), -1, mstr, size);
	wstr = mstr;
	delete[] mstr;
	return wstr;
}

void tiny_http::set_last_error(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int len = _vscprintf_p(fmt, ap) + 1;
	char* buf = new char[len];
	_vsprintf_p(buf, len, fmt, ap);
	va_end(ap);
	last_error_ = buf;
	delete[] buf;
}

bool tiny_http::is_valid_ipaddr(const std::string& ip) const
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


std::string tiny_http::get_last_error() const
{
	return last_error_;
}