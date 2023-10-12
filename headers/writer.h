#pragma once
#pragma warning(disable : 6031)
#include <stdio.h>
#include <exception>
#include <string>

//std::string bin(unsigned char n) {
//	if (n == 0) {
//		return "00000000";
//	}
//
//	std::string res = "";
//	while (n > 0) {
//		res += std::to_string(n % 2);
//		n >>= 1;
//	}
//	while (res.size() < 8) {
//		res += '0';
//	}
//
//	std::reverse(res.begin(), res.end());
//	return res;
//}

class FileWriter {
private:
	unsigned char _buffer = 0;
	unsigned char _buffer_size = 0;
	const char* _file_name;
	FILE* _file;

	void _update_buffer(bool flush = false) {
		if (_buffer_size >= 8 || (flush && _buffer_size > 0)) {
			_buffer <<= (8 - _buffer_size);
			fwrite(&_buffer, sizeof(_buffer), 1, _file);
			//std::cout << bin(_buffer)/* << std::endl*/;
			_buffer_size = 0;
			_buffer = 0;
		}
	}

public:
	FileWriter(const char* file_name) {
		_file_name = file_name;
		_file = fopen(file_name, "wb");
	}

	~FileWriter() {
		_update_buffer(true);
		fclose(_file);
	}

	void reopen_file() {
		_update_buffer(true);
		fclose(_file);
		_file = fopen(_file_name, "wb");
	}

	void write_bites(unsigned short out, unsigned short bites = 1, bool reversed = false) {
		for (unsigned char i = 0; i < bites; ++i) {
			_buffer <<= 1;
			if (reversed) {
				_buffer += (out >> i) % 2;
			}
			else {
				_buffer += (out >> (bites - i - 1)) % 2;
			}
			_buffer_size += 1;
			_update_buffer();
		}
	}
};