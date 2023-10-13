#pragma once
#include <stdio.h>
#include "reader.h"

class Comparer {
private:
	FILE* _original_file;
	FILE* _ziped_file;
	FILE* _unziped_file;

	const char* _original_filename;
	const char* _ziped_filename;
	const char* _unziped_filename;

	size_t _original_file_size = 0;
	size_t _ziped_file_size = 0;
	size_t _unziped_file_size = 0;

	std::string _get_percentage(float fraction) {
		fraction *= 100;
		if (abs(fraction) >= 10) {
			return std::to_string((int)fraction);
		}
		if (abs(fraction) >= 1) {
			return std::to_string((int)fraction) + "." + std::to_string((int)(abs(fraction) * 10) % 10);
		}
		if (abs(fraction) >= .01) {
			return (fraction < 0 ? "-" : "") + std::to_string((int)fraction) + "." + std::to_string((int)(abs(fraction) * 100) % 100);
		}
		return "0";
	}

public:
	Comparer(const char* original_filename, const char* ziped_filename, const char* unziped_filename) {
		_original_filename = original_filename;
		_ziped_filename = ziped_filename;
		_unziped_filename = unziped_filename;

		_original_file = fopen(_original_filename, "rb");
		_ziped_file = fopen(_ziped_filename, "rb");
		_unziped_file = fopen(_unziped_filename, "rb");

		_original_file_size = 0;
		_ziped_file_size = 0;
		_unziped_file_size = 0;

		compute_sizes();
	}

	~Comparer() {
		fclose(_original_file);
		fclose(_ziped_file);
		fclose(_unziped_file);
	}

	void compute_sizes() {
		_original_file_size = 0;
		_ziped_file_size = 0;
		_unziped_file_size = 0;

		while (getc(_original_file) != EOF) {
			++_original_file_size;
		}
		while (getc(_ziped_file) != EOF) {
			++_ziped_file_size;
		}
		while (getc(_unziped_file) != EOF) {
			++_unziped_file_size;
		}
	}

	void reopen_files() {
		fclose(_original_file);
		_original_file = fopen(_original_filename, "rb");

		fclose(_ziped_file);
		_ziped_file = fopen(_ziped_filename, "rb");

		fclose(_unziped_file);
		_unziped_file = fopen(_unziped_filename, "rb");
	}

	size_t get_original_file_size() {
		return _original_file_size;
	}

	size_t get_ziped_file_size() {
		return _ziped_file_size;
	}

	size_t get_unziped_file_size() {
		return _unziped_file_size;
	}

	bool isEqualOriginalAndUnziped() {
		reopen_files();

		if (_original_file_size != _unziped_file_size) {
			return false;
		}

		for (size_t i = 0; i < _original_file_size; ++i) {
			unsigned char ch1 = getc(_original_file);
			unsigned char ch2 = getc(_unziped_file);

			if (ch1 != ch2) {
				return false;
			}
		}

		return true;
	}

	std::string getStatisticInfo() {
		reopen_files();
		compute_sizes();

		return
			"\n  Original size: " + std::to_string(_original_file_size) + " bytes" +
			"\n     Ziped size: " + std::to_string(_ziped_file_size) + " bytes" +
			"\n    Compression: " + _get_percentage((float)((long long)_original_file_size - (long long)_ziped_file_size) / _original_file_size) + "%\n";
	}
};
