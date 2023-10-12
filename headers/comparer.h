#pragma once
#include <stdio.h>
#include "reader.h"

class Comparer {
private:
	FileReader _original_file_reader;
	FileReader _ziped_file_reader;
	FileReader _unziped_file_reader;

	size_t _original_file_size = 0;
	size_t _ziped_file_size = 0;
	size_t _unziped_file_size = 0;

public:
	Comparer(const char* original_filename, const char* ziped_filename, const char* unziped_filename) {
		_original_file_reader = FileReader(original_filename);
		_ziped_file_reader = FileReader(ziped_filename);
		_unziped_file_reader = FileReader(unziped_filename);

		_original_file_size = 0;
		_ziped_file_size = 0;
		_unziped_file_size = 0;

		compute_sizes();
	}

	~Comparer() {
		_original_file_reader.~FileReader();
		_ziped_file_reader.~FileReader();
		_unziped_file_reader.~FileReader();
	}

	void compute_sizes() {
		_original_file_size = 0;
		_ziped_file_size = 0;
		_unziped_file_size = 0;

		unsigned char ch;
		while (_original_file_reader.read_bites(ch, 8)) {
			++_original_file_size;
		}
		while (_ziped_file_reader.read_bites(ch, 8)) {
			++_ziped_file_size;
		}
		while (_unziped_file_reader.read_bites(ch, 8)) {
			++_unziped_file_size;
		}
	}

	void reopen_files() {
		_original_file_reader.reopen_file();
		_ziped_file_reader.reopen_file();
		_unziped_file_reader.reopen_file();
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
		if (_original_file_size != _unziped_file_size) {
			return false;
		}

		_original_file_reader.reopen_file();
		_unziped_file_reader.reopen_file();
		for (size_t i = 0; i < _original_file_size; ++i) {
			unsigned char ch1, ch2;
			_original_file_reader.read_bites(ch1, 8);
			_original_file_reader.read_bites(ch2, 8);

			if (ch1 != ch2) {
				return false;
			}
		}

		return true;
	}
};
