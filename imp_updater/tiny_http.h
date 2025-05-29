#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <map>

class tiny_http {
public:
	typedef struct _request {
		_request() {
			connect_timeout = 5000;
		}
		std::string url;
		std::map<std::string, std::string> headers;
		std::string body;
		int connect_timeout;
	} request_t;

	typedef struct _response {
		_response() {
			status_code = -1;
			content_length = 0;
		}
		int status_code;
		std::string status_message;
		std::map<std::string, std::string> headers;
		std::string body;
		int content_length;
	} response_t;

	tiny_http();
	~tiny_http();
	tiny_http::response_t post(const tiny_http::request_t& request);
	tiny_http::response_t get(const tiny_http::request_t& request);
	std::string get_last_error() const;

public:
	std::string utf8_to_ansi(const std::string& utf8) const;
	std::string ansi_to_utf8(const std::string& ansi) const;
	std::string unicode_to_ansi(const std::wstring& wstr, unsigned int codepage = 0) const;
	std::wstring ansi_to_unicode(const std::string& str, unsigned int codepage = 0) const;

protected:
	void set_last_error(const char* fmt, ...);

private:
	std::string str_trim(const std::string& str) const;
	std::vector<std::string> str_split(const std::string& str, const char* delimiter) const;
	bool parse_http_url(const std::string& url, std::string& protocol, 
		std::string& domain, int& port, std::string& uri) const;
	std::map<std::string, std::string> parse_http_headers_str(const std::string& header_str,
		int& status_code, std::string& status_message) const;
	bool get_ipaddr(const std::string& domain, std::string& addr);
	bool is_valid_ipaddr(const std::string& ip) const;

private:
	std::string last_error_;
};
