#include "dbhandler.h"


void DB_handler::add_file(const std::string &key) {
	std::vector <std::pair <std::string, std::fstream>>::
		iterator ptr;

	db_files.emplace_back();
	ptr = db_files.end() - 1;
	ptr->first = key;

	std::string path = "./database/" + key;
	//Create new files
	std::fstream fil(path, std::fstream::in | std::fstream::app);
	fil.close();
	ptr->second.open(path, std::fstream::in | std::fstream::out | std::fstream::ate);

	if (!ptr->second.is_open()) {
		std::cout << " > Could not open/create file: " << path << std::endl;
	}
}

//finds files among the open ones or tries to open it manually ONLY if file exists
std::fstream* DB_handler::find_file(const std::string &key) { 
	std::vector <std::pair <std::string, std::fstream>>::
		iterator ptr;

	for (ptr = db_files.begin(); ptr < db_files.end(); ++ptr) {
		if (ptr->first == key)
			break;
	}

	if (ptr == db_files.end() || !ptr->second.is_open()) {

		//This function must not create new files
		//If there is no file "key", this is an error
		if (ptr == db_files.end()) {
			db_files.emplace_back();
			ptr = db_files.end() - 1;
			ptr->first = key;
		}
		ptr->second.open("./database/" + key);

		if (!ptr->second.is_open()) {
			DB_handler::db_files.pop_back();
			return nullptr;
		}
	}
	return &(ptr->second);
}


void DB_handler::add_to_file(const std::string& key, const std::string& record) {
	std::fstream* ptr = find_file(key);
	
	if (ptr != nullptr) {
		if (!ptr->is_open())
			std::cout << " > Warning: file closed: " << key << std::endl;
		else {
			std::streampos pos;
			std::string buf = "";
			ptr->seekg(std::fstream::beg);
			ptr->seekp(std::fstream::beg);
			do {
				pos = ptr->tellg();
				*ptr >> buf;
			} while (!ptr->eof());
			ptr->clear();
			pos += 3;
			ptr->seekp(pos);
			*ptr << record;
			ptr->flush();
		}
	}
	else {
		std::cout << " > Warning: could not find file: >" << key << '<' << std::endl;
	}
	return;
}


void DB_handler::erase(const std::string &key) {
	for (std::vector <std::pair <std::string, std::fstream>>::
		 iterator iter = db_files.begin(); iter < db_files.end(); ++iter) {

		if (iter->first == key) {
			iter->second.close();
			db_files.erase(iter);
			break;
		}
	}
	return;
}


void DB_handler::replace_in_file(const std::string &key, const std::streampos from,
								 const std::streampos to, const std::string &new_str, const unsigned new_size) {
	std::fstream* ptr = DB_handler::find_file(key);
	std::streampos pos;
	std::string reader;

	ptr->clear();

	ptr->seekg(0, ptr->end);
	pos = ptr->tellg();
	ptr->clear();

	if (pos < to) {
		ptr->seekp(from);
		*ptr << new_str;
		for (unsigned i = 0; i < pos - from - new_size; ++i)
			*ptr << ' ';
		ptr->clear();
		ptr->flush();
		return;
	}

	unsigned last = pos - to + 2;
	char* buf = new char[last];
	for (unsigned i = 0; i < last; ++i)
		buf[i] = ' ';

	ptr->seekg(to);
	ptr->read(buf, last);
	ptr->clear();

	ptr->seekp(from);
	if (new_str.size() > 0)
		*ptr << new_str;
	ptr->write(buf, last);
	ptr->clear();

	if (new_size < (to - from))
		for (unsigned i = 0; i < (to - from) - new_size; ++i) {
			*ptr << ' ';
		}
			
	ptr->clear();
	ptr->flush();

	return;
}
