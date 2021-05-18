#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define SQL_LONG 0
#define SQL_TEXT 1


class DB_handler {
	std::pair <std::string, std::fstream> file;
	std::vector <std::pair <std::string, std::fstream>> db_files;

public:
	//key == filename from db_files
	std::fstream* find_file(const std::string &key);
	void add_file(const std::string &key);
	void add_to_file(const std::string &key, const std::string &record);
	void erase(const std::string &key);
	void replace_in_file(const std::string  &key, const std::streampos from,
						 const std::streampos to, const std::string &new_str, const unsigned new_size);
};
