#include <iostream>
#include "headers/zip.h"
#include "headers/unzip.h"

int main() {
	const char* original_filename = "sources/original.txt";
	const char* ziped_filename = "sources/ziped.txt";
	const char* unziped_filename = "sources/unziped.txt";

	zip(original_filename, ziped_filename);
	unzip(ziped_filename, unziped_filename);

	std::cout << "DONE!" << std::endl;
}