#pragma once
#pragma warning(disable : 6031)
#include <stdio.h>
#include <exception>

class FileReader {
private:
	unsigned char _buffer = 0;
	unsigned char _buffer_size = 0;
	const char* _file_name;
	FILE* _file;

	bool _update_buffer() {
		if (_buffer_size <= 0) {
			if (fscanf(_file, "%c", &_buffer) == -1) {
				return false;
			}
			_buffer_size = 8;
		}

		return true;
	}

public:
	FileReader(const char* file_name) {
		_file_name = file_name;
		_file = fopen(file_name, "rb");
	}

	~FileReader() {
		fclose(_file);
	}

	void reopen_file() {
		fclose(_file);
		_file = fopen(_file_name, "rb");
	}

	bool read_bites(unsigned char& out, unsigned char bites = 1) {
		if (bites > 8) {
			throw new std::exception("Incorrect bites to read");
		}

		out = 0;
		for (unsigned char i = 0; i < bites; ++i) {
			if (!_update_buffer()) {
				return false;
			}
			out <<= 1;
			out += (_buffer >> (_buffer_size - 1)) % 2;
			--_buffer_size;
		}

		return true;
	}
};