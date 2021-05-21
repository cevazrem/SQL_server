#include "sql.h"



/* ========  Syntax  ======== */

// SQL_command -> SQL_select | SQL_insert | SQL_update | SQL_delete | SQL_create | SQL_drop

// SQL_select -> SELECT List FROM table_name W

// SQL_insert -> INSERT INTO table_name (field_value {, field_value})

// SQL_update -> UPDATE table_name SET field_name = expr W

// SQL_delete -> DELETE FROM table_name W

// SQL_create -> CREATE TABLE table_name (field_list)

// SQL_drop -> DROP table_name



/* ========  Lexemes  ======== */

// table_name -> iden
// List -> iden {, iden} | *
// field_name -> iden		//a.k.a. text_field_name, long_field_name
// field_list -> field_desc {, field_desc}
// field_desc -> field_name field_type
// field_type -> TEXT (N) | LONG
// field_value -> string | long_int


// W -> text_field_name [NOT] LIKE string_example |
//      expr [NOT] IN (const_list) |
//      logic_expr |
//      ALL

// expr -> Long_expr | Text_expr
// Long_expr -> Long_summ {+ Long_summ | - Long_summ}
// Long_summ -> Long_mult {* Long_mult | / Long_mult | % Long_mult}
// Long_mult -> (Long_expr) | long_field_name | long_int

// Text_expr -> text_field_name | string

// logic_expr -> logic_summ {OR logic_summ}
// logic_summ -> logic_mult {AND logic_mult}
// logic_mult -> NOT logic_mult | (logic_expr) | (ratio)

// ratio -> Text_ratio | Long_ratio
// Text_ratio -> Text_expr R Text_expr
// Long_ratio -> Long_expr R Long_expr

// R -> = | > | < | >= | <= | !=
// const_list -> string {, string} | long_int {, long_int}
// string -> 'char {char}'
// char -> 0-255 / '\''
// long_int -> -N | N
// N -> 0N | 1N | 2N | 3N | 4N | 5N | 6N | 7N | 8N | 9N | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9

//string_example -> string // BUT looking for % _ [] [^]


/* ==========  Govnokod Atention  ========== */

void SQL::SQL_command() {
	SQL_lex_analyse();

	if (state != MAIN) {
		message = " > Unknown command: " + lexeme;
		state = ERR;
	}
	else 
		while (state == MAIN) {
			if (lexeme == SELECT_word)
				SQL_select();
			else if (lexeme == INSERT_word)
				SQL_insert();
			else if (lexeme == UPDATE_word)
				SQL_update();
			else if (lexeme == DELETE_word)
				SQL_delete();
			else if (lexeme == CREATE_word)
				SQL_create();
			else if (lexeme == DROP_word)
				SQL_drop();
		}

	while (!scin.eof())
		scin.get();
	scin.clear();
	state = INIT;
	WHERE_poliz.clear();
	tmp_poliz.clear();
	poliz.clear();

	return;
}

void SQL::SQL_init() {
	do
		c = scin.get();
	while (isspace(c) && !scin.eof());

	if (isspace(c) || scin.eof())
		state = ERR;
	else
		state = INIT;
}

void SQL::SQL_analyse(const std::string &mes) {
	message.clear();
	scin.clear();
	scin << mes;

	SQL_init();
	SQL_command();
}

void SQL::SQL_lex_analyse() {
	if (state == ERR || state == END)
		return;

	while (isspace(c) && !scin.eof()) {
		c = scin.get();
	}

	if (c == '+') {
		state = PLUS;
		lexeme = '+';
		c = scin.get();
		//std::cout << "after plus" << std::endl;
	}
	else if (c == '-') {
		state = SUBTR;
		lexeme = '-';
		c = scin.get();
		//std::cout << "after minus" << std::endl;
	}
	else if (c == '*') {
		state = MULT;
		lexeme = '*';
		c = scin.get();
		//std::cout << "after mult" << std::endl;
	}
	else if (c == '/') {
		state = DIV;
		lexeme = '/';
		c = scin.get();
		//std::cout << "after div" << std::endl;
	}
	else if (c == '%') {
		state = MOD;
		lexeme = '%';
		c = scin.get();
		//std::cout << "after mod" << std::endl;
	}
	else if (c == '(') {
		state = OP_PAR;
		lexeme = '(';
		c = scin.get();
		//std::cout << "after op par" << std::endl;
	}
	else if (c == ')') {
		state = CL_PAR;
		lexeme = ')';
		c = scin.get();
		//std::cout << "after cl par" << std::endl;
	}
	else if (c == '!') {
		c = scin.get();
		if (c != '=') {
			state = ERR;
			lexeme = "";
		}
		else {
			state = N_EQU;
			lexeme = "!=";
		}
		c = scin.get();
		//std::cout << "after not equal" << std::endl;
	}
	else if (c == '=') {
		state = EQU;
		lexeme = '=';
		c = scin.get();
		//std::cout << "after equal" << std::endl;
	}
	else if (c == '<') {
		c = scin.peek();
		if (c != '=') {
			state = LESS;
			lexeme = "<";
		}
		else {
			state = L_EQU;
			lexeme = "<=";
			c = scin.get();
		}
		c = scin.get();
		//std::cout << "after less or equal.. or not equal" << std::endl;
	}
	else if (c == '>') {
		c = scin.peek();
		if (c != '=') {
			state = MORE;
			lexeme = ">";
		}
		else {
			state = M_EQU;
			lexeme = ">=";
			c = scin.get();
		}
		c = scin.get();
		//std::cout << "after more or equal.. or not equal" << std::endl;
	}
	else if (c == ',') {
		state = COMMA;
		lexeme = ',';
		c = scin.get();
		//std::cout << "after comma" << std::endl;
	}
	else if (isdigit(c)) {
		state = NUM;
		lexeme = c;

		do {
			c = scin.get();
			lexeme += c;
		} while (isdigit(c) && !scin.eof());

		if (isalpha(c)) {//When have something like this:  62fr - this is clearly an error 
			state = ERR;
			lexeme = "";
			return;
		}

		lexeme.pop_back();
		//std::cout << "after digit" << std::endl;
	}
	else if (isalpha(c)) {
		state = IDEN;
		lexeme = c;

		do {
			c = scin.get();
			lexeme += c;
		} while (isalnum(c) && !scin.eof());
		lexeme.pop_back();
		
		SQL_check_words(lexeme);
		//std::cout << "after iden" << std::endl;
	}
	else if (c == '\'') {
		state = STR;
		lexeme = c;

		c = scin.get();
		while (c != '\'' && !scin.eof()) {
			lexeme += c;
			c = scin.get();
		}

		if (lexeme == "\'" || c != '\'') {
			state = ERR;
			lexeme = "";
			return;
		}
		lexeme += "\'";
		c = scin.get();
		//std::cout << "after string: " << lexeme << std::endl;
	}
	else if (scin.eof()) {
		//std::cout << "state = " << state << std::endl;
		if (state == INIT)
			state = ERR;
		else
			state = END;
		lexeme = "";
		//std::cout << "after all" << state << std::endl;
	}
	else {
		state = ERR;
		lexeme = "";
		//std::cout << "after error" << std::endl;
	}
	//std::cout << "Read lexeme: <" << lexeme << '>' << state << std::endl;
}


bool SQL_words::SQL_check_main_words(const std::string &message) {
	if (message == SELECT_word || message == INSERT_word ||
		message == UPDATE_word || message == DELETE_word ||
		message == CREATE_word || message == DROP_word) {

		state = MAIN;
		return true;
	}

	return false;
}

bool SQL_words::SQL_check_other_words(const std::string &message) {
	if (message == FROM_word  || message == INTO_word ||
		message == TABLE_word || message == SET_word  ||
		message == LIKE_word  || message == NOT_word  ||
		message == LONG_word  || message == TEXT_word ||
		message == WHERE_word || message == IN_word   ||
		message == ALL_word) {

		state = OTHER;
		return true;
	}

	return false;
}

bool SQL_words::SQL_check_words(const std::string &message) {
	return SQL_check_main_words(message) || SQL_check_other_words(message);
}


/* ========  SQL Main Commands  ======== */

void SQL::SQL_select() {
	message = " > SELECT did not work well";
	long_e = text_e = ratio = logic = false;
	read_lex = 0;
	
	std::vector <std::string> list;
	do {
		SQL_lex_analyse();
		if (state != IDEN) {

			if (state == MULT)
				break;
			state = ERR;
			return;
		}
		list.push_back(lexeme);
		SQL_lex_analyse();
	}
	while (state == COMMA);

	if (state == MULT) {
		list.push_back(lexeme);
		if (list.size() != 1) {
			state = ERR;
			message = " > Error: wrong use of * parameter";
			return;
		}
		SQL_lex_analyse();
	}

	if (lexeme != FROM_word) {
		state = ERR;
		return;
	}

	SQL_lex_analyse();
	if (state != IDEN) {
		state = ERR;
		return;
	}
	curr_fname = lexeme;
	
	if (list[0] != "*") {
		SQL_check_field_names(curr_fname, list);

		if (state == ERR) {
			message = " > Error checking fields";
			return;
		}
	}
	
	curr_strings.clear();
	SQL_lex_analyse();
	SQL_WHERE_clause(curr_fname);

	message.clear();
	std::vector <std::vector <std::string>> fields;
	std::vector <unsigned> indices;

	if (list[0] == "*")
		for (unsigned i = 0; i < curr_table.size(); ++i) {
			message += curr_table[i].name + "  ";
		}
	else {
		for (unsigned i = 0; i < curr_table.size(); ++i) {
			for (unsigned j = 0; j < list.size(); ++j)
				if (curr_table[i].name == list[j]) {
					message += curr_table[i].name + "  ";
					indices.push_back(i);
				}
		}
	}
		
	message += '\n';
	if (WHERE_poliz.size() == 1 && WHERE_poliz[0].first == ALL_word) {
		unsigned ind = 0;
		state = INIT;
		SQL_get_table_string(curr_fname, ind);
		while (state != END && state != ERR) {
			if (indices.empty()) {
				for (unsigned j = 0; j < curr_string.size(); ++j) {
					message += curr_string[j] + ' ';
				}
			}
			else for (unsigned j = 0; j < indices.size(); ++j) {
				message += curr_string[indices[j]] + ' ';
			}
			++ind;
			SQL_get_table_string(curr_fname, ind);
			message += '\n';
		}
	}
	else
		for (unsigned i = 0; i < curr_strings.size(); ++i) {
			SQL_get_table_string(curr_fname, curr_strings[i]);
			
			if (indices.empty()) {
				for (unsigned j = 0; j < curr_string.size(); ++j) {
					message += curr_string[j] + ' ';
				}
			}
			else for (unsigned j = 0; j < indices.size(); ++j) {
				message += curr_string[indices[j]] + ' ';
			}

			message += '\n';
		}
}


void SQL::SQL_insert() {
	std::vector <std::string> new_field;

	SQL_lex_analyse();
	if (lexeme != INTO_word) {
		state = ERR;
		return;
	}

	SQL_lex_analyse();
	if (state != IDEN) {
		state = ERR;
		return;
	}
	std::string fname = lexeme;

	SQL_lex_analyse();
	if (state != OP_PAR) {
		state = ERR;
		return;
	}
	
	do {
		SQL_lex_analyse();
		if (state == STR || state == NUM) {
			new_field.push_back(lexeme);
		}
		else if (state == SUBTR) {

			SQL_lex_analyse();
			if (state != NUM) {
				state = ERR;
				return;
			}
			new_field.push_back('-' + lexeme);
		}
		else {
			state = ERR;
			return;
		}
		SQL_lex_analyse();
	}
	while (state == COMMA);

	if (state != CL_PAR) {
		state = ERR;
		return;
	}
	SQL_lex_analyse();	

	SQL_check_field_values(fname, new_field);
	if (state != ERR) {
		std::string msg;
		for (unsigned i = 0; i < new_field.size(); ++i)
			msg += new_field[i] + " \n";

		msg += " \n";

		DBTable.add_to_file(fname, msg);
		message = " > Field inserted";
	}
	else {
		message = " > Error: could not INSERT";
	}
	return;
}


void SQL::SQL_update() {
	message = " > UPDATE works well";
	SQL_lex_analyse();
	if (state != IDEN) {
		state = ERR;
		return;
	}
	std::string fname = lexeme;

	SQL_lex_analyse();
	if (lexeme != SET_word) {
		state = ERR;
		return;
	}

	SQL_lex_analyse();
	if (state != IDEN) {
		state = ERR;
		return;
	}

	std::vector <std::string> field;
	field.push_back(lexeme);
	SQL_get_field_names(fname);
	if (state == ERR) {
		return;
	}

	unsigned ind = 0;
	for (; ind < curr_table.size(); ++ind) {
		if (field[0] == curr_table[ind].name) {
			break;
		}
	}
	if (ind == curr_table.size()) {
		state = ERR;
		return;
	}

	
	std::string type = SQL_get_field_type(fname, field[0]);
	curr_fname = fname;

	SQL_lex_analyse();
	if (state != EQU) {
		state = ERR;
		return;
	}

	text_e = long_e = logic = ratio = false;
	read_lex = 0;
	poliz.clear();
	SQL_expr(poliz);
	if (state == ERR || logic || ratio || ((type == LONG_word) && text_e) || ((type == TEXT_word) && long_e)) {
		state = ERR;
		return;
	}
	WHERE_poliz.clear();
	SQL_WHERE_clause(curr_fname);

	if (state == ERR) {
		message = " > Error with UPDATE WHERE-clause";
		return;
	}
	
	std::streampos from, to;

	if (WHERE_poliz.size() == 1 && WHERE_poliz[0].first == ALL_word) {
		state = INIT;
		unsigned i = 0;

		SQL_get_table_string(curr_fname, i);

		while (state != END && state != ERR) {
			tmp_poliz = poliz;
			SQL_compute_poliz(curr_fname);
			
			if (state == ERR) {
				message = " > Error with UPDATE poliz";
				return;
			}
			curr_string[ind] = tmp_poliz[0].first;
			
			std::string new_str;
			for (unsigned j = 0; j < curr_string.size(); ++j)
				new_str += curr_string[j] + " \n";

			SQL_get_string_pos(curr_fname, i, from, to);
			from -= 1;
			DBTable.replace_in_file(curr_fname, from, to, new_str, new_str.size() + curr_string.size()); // because of \r in Win
			
			++i;
			SQL_get_table_string(curr_fname, i);
		}

		if (state == ERR) {
			message = " > Error with UPDATE get-string";
			return;
		}
	}
	else for (unsigned i = curr_strings.size(); i > 0; --i) {
		SQL_get_table_string(curr_fname, curr_strings[i-1]);
		tmp_poliz = poliz;
		SQL_compute_poliz(curr_fname);

		if (state == ERR) {
			message = " > Error with UPDATE poliz";
			return;
		}

		curr_string[ind] = tmp_poliz[0].first;

		std::string new_str;
		for (unsigned j = 0; j < curr_string.size(); ++j)
			new_str += curr_string[j] + " \n";

		SQL_get_string_pos(curr_fname, curr_strings[i-1], from, to);
		from -= 1;
		to += 2;
		DBTable.replace_in_file(curr_fname, from, to, new_str, new_str.size() + curr_string.size());
		if (state == ERR) {
			message = " > Error with UPDATE replacement";
			return;
		}
	}
	return;
}


void SQL::SQL_delete() {
	SQL_lex_analyse();
	if (lexeme != FROM_word) {
		state = ERR;
		return;
	}

	SQL_lex_analyse();
	if (state != IDEN) {
		state = ERR;
		return;
	}
	curr_fname = lexeme;
	
	SQL_lex_analyse();
	SQL_WHERE_clause(curr_fname);

	if (WHERE_poliz.size() == 1 && WHERE_poliz[0].first == ALL_word) {
		std::fstream* ptr = DBTable.find_file(curr_fname);
		ptr->clear();
		ptr->seekg(0);
		std::string buf;
		do
			*ptr >> buf;
		while (buf != file_headend);
		std::streampos st = ptr->tellg(), fin;
		ptr->seekp(0, ptr->end);
		ptr->clear();
		fin = ptr->tellp();
		ptr->clear();
		ptr->seekp(st, ptr->beg);
		std::cout << ptr->tellp();
		char* buff;
		buff = new char[fin - st];
		for (unsigned i = 0; i < fin - st; ++i) {
			buff[i] = ' ';
		}
		ptr->write(buff, fin - st);
		ptr->flush();
	}
	else for (unsigned i = curr_strings.size(); i > 0; --i) {
		std::streampos from, to;
		SQL_get_string_pos(curr_fname, curr_strings[i-1], from, to);
		from -= 3;
		DBTable.replace_in_file(curr_fname, from, to, "", 0);
	}
	curr_strings.clear();
	message = " > Table strings deleted";
	return;
}


void SQL::SQL_create() {
	std::string fields, number;
	std::stringstream for_num;
	unsigned f_num = 0;
	message.clear();

	SQL_lex_analyse();
	if (lexeme != TABLE_word) {
		state = ERR;
		return;
	}

	SQL_lex_analyse();
	if (state != IDEN) {
		state = ERR;
		return;
	}
	std::string fname = lexeme;
	

	SQL_lex_analyse();
	if (state != OP_PAR) {
		state = ERR;
		return;
	}

	do {
		SQL_lex_analyse();
		if (state != IDEN) {
			state = ERR;
			return;
		}
		fields += lexeme + " \n";

		SQL_lex_analyse();
		if (lexeme == TEXT_word) {
			SQL_lex_analyse();
			if (state != OP_PAR) {
				state = ERR;
				return;
			}

			SQL_lex_analyse();
			if (state != NUM) {
				state = ERR;
				return;
			}
			fields += "TEXT \n" + lexeme + " \n";

			SQL_lex_analyse();
			if (state != CL_PAR) {
				state = ERR;
				return;
			}
		}
		else if (lexeme == LONG_word) {
			fields += "LONG \n";
		}
		
		++f_num;
		SQL_lex_analyse();
		//std::cout << "\n" << state << std::endl;
	}
	while (state == COMMA);

	if (state != CL_PAR) {
		state = ERR;
		return;
	}

	fields += " \n " + file_headend + " \n \n"; //I'll use these symbols to find header end easily
	DBTable.add_file(fname);
	message.clear();
	for_num << f_num;
	for_num >> number;
	
	DBTable.add_to_file(fname, number + " \n \n");
	DBTable.add_to_file(fname, fields); // write twice to test this add_to_file function	
	
	std::fstream* ptr = DBTable.find_file(fname);
	ptr->clear();
	ptr->seekp(ptr->beg);
	for (int i = 0; i < 3; ++i)
		*ptr << ' ';
	std::cout << ptr->tellp();
	SQL_lex_analyse(); //need this because some other functions don't have explicit end lexems
	if (message.empty()) {
		message = " Table " + fname + " created successfully\n";
	}
	return;
}


void SQL::SQL_drop() {

	SQL_lex_analyse();
	if (lexeme != TABLE_word) {
		state = ERR;
		return;
	}

	SQL_lex_analyse();
	if (state != IDEN) {
		state = ERR;
		return;
	}

	std::string path = "./database/" + lexeme;

	message = " > Table removed";
	std::fstream* ptr = SQL::DBTable.find_file(lexeme);
	if (ptr != nullptr) {
		if (ptr->is_open())
			ptr->close();

		SQL::DBTable.erase(lexeme);

		if (remove(path.c_str()) == -1)
			message = " > Warning: could not delete file " + lexeme;
	}
	else {
		message = " > Warning: wrong file name for DROP command";
	}

	SQL_lex_analyse(); //need this because some other functions don't have explicit end lexems
	return;
}



void SQL::SQL_get_field_names(const std::string &key) {
	//std::cout << " > Succsessfully started" << std::endl;
	std::fstream *ptr = DBTable.find_file(key);
	ptr->clear();
	ptr->seekg(0);
	ptr->seekp(0);
	std::string buf;
	Fields new_field;
	unsigned fields_num;

	if (ptr == nullptr) {
		std::cout << " > Error: file header ptr error: " << std::endl;
		state = ERR;
		return;
	}

	curr_table.clear();
	*ptr >> fields_num;
	//std::cout << " > Succsessfully started2: " << fields_num << std::endl;

	for (unsigned i = 0; i < fields_num; ++i) {
		*ptr >> new_field.name;
		*ptr >> buf;

		if (buf == TEXT_word) {
			new_field.type = 1;
			*ptr >> new_field.text_length;
		}
		else if (buf == LONG_word) {
			new_field.type = 0;
			new_field.text_length = 0;
		}
		else {
			state = ERR;
			std::cout << " > Error: file header error: " << key << std::endl;
			return;
		}
		//std::cout << " > New field1: " << new_field.name << std::endl;
		//std::cout << " > New field2: " << new_field.type << std::endl;
		//std::cout << " > New field3: " << new_field.text_length << std::endl;
		curr_table.push_back(new_field);
	}
	//std::cout << " > Succsessfully ended" << std::endl;
	return;
}

std::string SQL::SQL_get_field_type(const std::string &key, const std::string &field) {
	std::fstream *ptr = DBTable.find_file(key);
	std::string buf;
	ptr->clear();
	ptr->seekp(0);
	ptr->seekg(0);

	do {
		*ptr >> buf;
	}
	while (buf != file_headend && buf != field && !ptr->eof());

	if (buf == field) {
		*ptr >> buf;
		return buf;
	}
	else {
		std::cout << " > Error: could not find field: " << field << std::endl;
		state = ERR;
	}
	return "";
}

void SQL::SQL_get_string_pos(const std::string& key, unsigned number, std::streampos& from, std::streampos& to) {
	SQL_get_field_names(key);
	if (state == ERR) {
		std::cout << " > Error with file name: " << key << std::endl;
		return;
	}

	std::fstream* ptr = DBTable.find_file(key);
	std::string buf;
	ptr->clear();
	ptr->seekp(0);
	ptr->seekg(0);

	do {
		*ptr >> buf;
	} while (buf != file_headend);

	for (unsigned i = 0; i < number; ++i) {
		for (unsigned j = 0; j < curr_table.size(); ++j) {
			if (curr_table[j].type == 0)
				*ptr >> buf;
			else {
				char c;
				do
					c = ptr->get();
				while (c != '\'');

				do
					c = ptr->get();
				while (c != '\'');
				c = ptr->get();
			}

			if (ptr->eof()) {
				ptr->clear();
				std::cout << " > Warning: there is no such string" << std::endl;
				from = 0;
				to = 0;
				return;
			}
		}
	}

	for (int i = 0; i < 2; ++i) {
		ptr->get();
	}
	from = ptr->tellg();

	for (unsigned j = 0; j < curr_table.size(); ++j) {
		if (curr_table[j].type == 0) {
			*ptr >> buf;

			if (ptr->eof()) {
				ptr->clear();
				std::cout << " > Warning: there is no such string" << std::endl;
				state = END;
				return;
			}
		}
		else {
			char c;
			do
				c = ptr->get();
			while (c != '\'');

			do
				c = ptr->get();
			while (c != '\'');

			if (ptr->eof()) {
				ptr->clear();
				std::cout << " > Warning: there is no such string" << std::endl;
				state = END;
				return;
			}

			c = ptr->get();
		}
	}
	to = ptr->tellg();
}

void SQL::SQL_get_table_string(const std::string &key, unsigned number) {
	SQL_get_field_names(key);
	if (state == ERR) {
		std::cout << " > Error when try to get table fields' names" << std::endl;
		return;
	}

	std::fstream* ptr = DBTable.find_file(key);
	std::string buf;
	ptr->clear();
	ptr->seekp(0);
	ptr->seekg(0);

	do {
		*ptr >> buf;
	}
	while (buf != file_headend);

	for (unsigned i = 0; i < number; ++i) {
		for (unsigned j = 0; j < curr_table.size(); ++j) {
			if (curr_table[j].type == 0)
				*ptr >> buf;
			else {
				char c;
				do
					c = ptr->get();
				while (c != '\'');

				do
					c = ptr->get();
				while (c != '\'');
				c = ptr->get();
			}

			if (ptr->eof()) {
				ptr->clear();
				std::cout << " > Warning: too big string number: " << number << " for file " << key << std::endl;
				state = END;
				return;
			}
		}
	}
	curr_string.clear();
	for (unsigned j = 0; j < curr_table.size(); ++j) {
		buf.clear();
		if (curr_table[j].type == 0)
			*ptr >> buf;
		else {
			do {
				c = ptr->get();
			}
			while (c != '\'');

			buf = "\'";
			do {
				c = ptr->get();
				buf += c;
			}
			while (c != '\'');
		}
		curr_string.push_back(buf);

		if (ptr->eof()) {
			if (j == 0) {
				state = END;
			}
				
			else
				state = ERR;

			return;
		}
			
	}
	if (curr_string.size() < curr_table.size()) {
		std::cout << " > Error: fields number = " << curr_table.size() << ", table string size = " << curr_string.size() << std::endl;
		state = ERR;
		return;
	}
}

void SQL::SQL_check_field_values(const std::string &key, const std::vector <std::string> &list) {
	SQL_get_field_names(key);
	if (state == ERR) {
		std::cout << " > Error trying to get table fields' names" << std::endl;
		return;
	}
	//std::cout << " > Succsessfully got: " << curr_table.size() << std::endl;
	//unsigned fields_num;
	curr_fields.clear();

	for (unsigned i = 0; i < list.size(); ++i) {
		unsigned j;
		//std::cout << " > New list: " << list[i] << std::endl;
		std::string::const_iterator str_itr = list[i].begin();
		int cur_type = 2;

		if (*str_itr == '-' || isdigit(*str_itr))
			cur_type = 0;
		else if (*str_itr == '\'')
			cur_type = 1;
		else {
			std::cout << " > Error: not a number or string: " << *str_itr << std::endl;
		}

		for (j = 0; j < curr_table.size(); ++j) {

			if (curr_table[j].type == cur_type) {
				if (cur_type == 1) {

					//std::cout << " > New text list leng: " << list[i].size() << std::endl;
					//std::cout << " > New text field leng: " << curr_table[j].text_length << std::endl;
					
					if (list[i].size() > curr_table[j].text_length+2) {
						state = ERR;
						return;
					}
				}
				curr_fields.push_back(j);
				break;
			}
		}
		if (j == curr_table.size()) {
			state = ERR;
			std::cout << " > Error: could not find field >" << list[i] << "< in table >" << key << '<' << std::endl;
			return;
		}
	}
	return;
}

void SQL::SQL_check_field_names(const std::string& key, const std::vector <std::string>& list) {
	SQL_get_field_names(key);
	curr_fields.clear();

	for (unsigned i = 0; i < list.size(); ++i) {
		unsigned j = 0;

		for (j = 0; j < curr_table.size(); ++j) {
			if (curr_table[j].name == list[i]) {
				break;
			}
		}
		if (j == curr_table.size()) {
			state = ERR;
			std::cout << " > Error: could not find field >" << list[i] << "< in table >" << key << '<' << std::endl;
			return;
		}
		else {
			curr_fields.push_back(j);
		}
	}
	return;
}


/* ========  Expression analyser  ======== */

// Something like this:

// <bool logic, ratio, text, long;>

// L -> B { OR <if (!ratio) state = ERROR; ratio = false> <logic = true> B }
// B -> A {AND <if (!ratio) state = ERROR; ratio = false> <logic = true> A }
// A -> NOT <logic = true> A <if (!ratio) state = ERROR;> | 
//      <tmp_logic = logic> <logic = text = long = false> S 
//      [OP < if (logic) state = ERROR; ratio = true> S
//      < if (text && long) state = ERROR; if (logic) state = ERROR;> 
//      <logic = tmp_logic>]
// S -> E { +E | -E }
// E -> M { *M | /M | % M }
// M -> (L) | string <text = true> | long <long = true> | TEXT_FIELD <text = true> | LONG_FIELD <long = true> | ALL <state = END>
// OP ->  = | != | < | > | <= | >=


void SQL::SQL_logic_expr(std::vector <std::pair<std::string, poliz_state>>& poliz) {
	if (state == END)
		return;

	//std::cout << " DOING L_expr" << state << std::endl;

	SQL_logic_S(poliz);

	while (lexeme == OR_word) {
		if (read_lex > 0)
			--read_lex;
		
		//std::cout << " Going OR\n";
		

		if (!ratio) {
			state = ERR;
			return;
		}
		ratio = false;

		//std::cout << " Cont OR\n";
		SQL_logic_S(poliz);
		logic = true;
		//std::cout << " End OR\n";

		poliz.push_back(std::make_pair(OR_word, OP));
	}
}

void SQL::SQL_logic_S(std::vector <std::pair<std::string, poliz_state>>& poliz) {
	if (state == END)
		return;
	//std::cout << " DOING L_S " << state << std::endl;

	SQL_logic_M(poliz);

	while (lexeme == AND_word) {
		//std::cout << " Going AND\n";
		if (read_lex > 0)
			--read_lex;

		
		if (!ratio) {
			//std::cout << " Why not?\n";
			state = ERR;
			return;
		}
		ratio = false;
		
		//std::cout << " Continue AND\n";
		SQL_logic_M(poliz);
		logic = true;

		//std::cout << " End AND\n";
		poliz.push_back(std::make_pair(AND_word, OP));
	}
}

void SQL::SQL_logic_M(std::vector <std::pair<std::string, poliz_state>>& poliz) {
	if (state == END)
		return;

	//std::cout << " DOING L_M " << state << std::endl;

	if (read_lex == 0) {
		SQL_lex_analyse();
		//std::cout << " DOING READ IN L_M " << lexeme << std::endl;
	}
	if (lexeme == NOT_word) {
		
		if (read_lex > 0)
			--read_lex;

		SQL_logic_M(poliz);
		logic = true;
		if (!ratio) {
			//std::cout << "ratio L_M prob\n";
			state = ERR;
			return;
		}

		poliz.push_back(std::make_pair(NOT_word, OP));
		//std::cout << " Read here in L_M end " << state << std::endl;
	}
	else {
		if (read_lex == 0)
			++read_lex;
		bool tmp_logic = logic;
		logic = text_e = long_e = false;
		SQL_expr(poliz);
		

		if (state == N_EQU || state == L_EQU || state == M_EQU || state == MORE || state == LESS || state == EQU) {
			std::string op = lexeme;

			if (logic) {
				//std::cout << "logic prob " << lexeme << std::endl;
				state = ERR;
				return;
			}

			ratio = true;

			SQL_expr(poliz);
			

			if (logic) {
				//std::cout << "logic prob " << lexeme << std::endl;
				state = ERR;
				return;
			}

			if (text_e && long_e) {
				//std::cout << "l_t prob\n";
				state = ERR;
				return;
			}
			poliz.push_back(std::make_pair(op, OP));
		}

		
		logic = tmp_logic;
	}
}

void SQL::SQL_expr(std::vector <std::pair<std::string, poliz_state>>& poliz) {
	if (state == END)
		return;

	//std::cout << " DOING Expr " << state << std::endl;

	SQL_S(poliz);

	while (state == PLUS || state == SUBTR) {
		std::string op = lexeme;

		if (text_e) {
			message = " > Error: operation " + lexeme + " for text expression";
			state = ERR;
			return;
		}

		SQL_S(poliz);

		poliz.push_back(std::make_pair(op, OP));
	}
}

void SQL::SQL_S(std::vector <std::pair<std::string, poliz_state>>& poliz) {
	if (state == END)
		return;

	//std::cout << " DOING S " << state << std::endl;

	SQL_M(poliz);

	while (state == MULT || state == DIV || state == MOD) {
		std::string op = lexeme;

		if (text_e) {
			message = " > Error: operation " + lexeme + " for text expression";
			state = ERR;
			return;
		}

		SQL_M(poliz);

		poliz.push_back(std::make_pair(op, OP));
	}
}

void SQL::SQL_M(std::vector <std::pair<std::string, poliz_state>>& poliz) {

	if (state == ERR)
		return;

	//std::cout << " DOING M " << state << std::endl;

	if (read_lex == 0) {
		SQL_lex_analyse();
		//std::cout << " DOING READ IN M " << lexeme << std::endl;
	}
		
	if (state == OP_PAR) {

		if (read_lex > 0)
			read_lex--;

		SQL_logic_expr(poliz);

		if (state != CL_PAR) {
			state = ERR;
			return;
		}
	}
	else if (state == SUBTR) {
		if (read_lex > 0)
			read_lex--;

		SQL_lex_analyse();
		if (state != NUM) {
			state = ERR;
			return;
		}
		long_e = true;
		poliz.push_back(std::make_pair('-' + lexeme, LONG));
	}
	else if (state == NUM) {
		if (read_lex > 0)
			read_lex--;

		long_e = true;
		poliz.push_back(std::make_pair(lexeme, LONG));
	}
	else if (state == IDEN) {
		if (read_lex > 0)
			read_lex--;
		std::string type = SQL_get_field_type(curr_fname, lexeme);

		if (type == LONG_word) {
			long_e = true;
			if (text_e) {
				state = ERR;
				return;
			}
			poliz.push_back(std::make_pair(lexeme, LONG_F));
		}
		else if (type == TEXT_word) {
			text_e = true;
			if (long_e) {
				state = ERR;
				return;
			}
			poliz.push_back(std::make_pair(lexeme, TEXT_F));
		}
		else {
			std::cout << " > Error: wrong lexeme = >" << lexeme << "< type >" << type << '<' << "file: >" << curr_fname << '<' << std::endl;
			state = ERR;
			return;
		}
	}
	else if (state == STR) {
		if (read_lex > 0)
			read_lex--;
		text_e = true;
		if (long_e) {
			state = ERR;
			return;
		}
		poliz.push_back(std::make_pair(lexeme, TEXT));
	}
	else if (lexeme == ALL_word) {
		if (read_lex > 0)
			read_lex--;

		if (poliz.empty()) {
			poliz.push_back(std::make_pair(lexeme, OP));
		}
		else {
			state = ERR;
		}
	}
	else {
		state = ERR;
		return;
	}
	//std::cout << " Read here in M end " << state << std::endl;
	SQL_lex_analyse();
	return;
}

//This function will be generating vector of fields server should work with
void SQL::SQL_WHERE_clause(const std::string& key) {
	curr_fname = key;
	SQL_get_field_names(curr_fname);

	if (lexeme != WHERE_word) {
		state = ERR;
		return;
	}

	text_e = long_e = logic = ratio = false;
	read_lex = 0;
	SQL_logic_expr(WHERE_poliz);
	bool NOT = false;
	if (state == ERR) {
		std::cout << " > Analyser error";
		poliz.clear();
		WHERE_poliz.clear();
		return;
	}
	else if (logic || ratio) {
		curr_strings.clear();
		// Check logic expression for every string
		bool f_independent = true;
		for (unsigned i = 0; i < WHERE_poliz.size(); ++i) {
			if (WHERE_poliz[i].second == TEXT_F || WHERE_poliz[i].second == LONG_F) {
				f_independent = false;
				break;
			}
		}
		if (f_independent) {
			tmp_poliz = WHERE_poliz;
			SQL_compute_poliz(key);
			if (tmp_poliz.size() != 1) {
				std::cout << " > Error: wrong poliz logic result";
				state = ERR;
				return;
			}
			WHERE_poliz.clear();

			if (tmp_poliz[0].first == "true") {
				WHERE_poliz.push_back(std::make_pair("ALL", OP));
			}
			curr_strings.clear();
			return;
		}
		else {
			unsigned str_ind = 0;
			state = INIT;
			SQL_get_table_string(key, str_ind);

			while (state != END && state != ERR) {
				tmp_poliz = WHERE_poliz;
				SQL_compute_poliz(key);
				if (tmp_poliz[0].first == "true")
					curr_strings.push_back(str_ind);
				else if (tmp_poliz[0].first != "false") {
					state = ERR;
					std::cout << " > Error: wrong result of logic expression == " + tmp_poliz[0].first;
					return;
				}
				++str_ind;
				SQL_get_table_string(key, str_ind);
			}
			if (state == ERR) {
				std::cout << " Error when read file string " << str_ind << std::endl;
			}
			WHERE_poliz.clear();
			
		}
		return;
	}
	else if (WHERE_poliz.size() == 1 && WHERE_poliz[0].first == ALL_word) {
		curr_strings.clear();
		return;
	}
	else {
		if (lexeme == NOT_word) {
			NOT = true;
			SQL_lex_analyse();
		}

		if (lexeme == IN_word) {

			SQL_lex_analyse();
			if (state != OP_PAR) {
				state = ERR;
				return;
			}
			lex_state tmp_st = INIT;
			std::vector <std::string> consts;

			do {
				SQL_lex_analyse();
				if (state != STR && state != SUBTR && state != NUM) {
					state = ERR;
					return;
				}
				if (state == SUBTR) {
					SQL_lex_analyse();
					if (state != NUM) {
						state = ERR;
						return;
					}
					consts.push_back('-' + lexeme);
				}
				else {
					consts.push_back(lexeme);
				}
				if (tmp_st == INIT) {
					tmp_st = state;
				}
				else if (state != tmp_st) {
					state = ERR;
					return;
				}
				SQL_lex_analyse();
			} while (state == COMMA);

			if (state != CL_PAR) {
				state = ERR;
				return;
			}

			bool f_independent = true;
			for (unsigned i = 0; i < WHERE_poliz.size(); ++i) {
				if (WHERE_poliz[i].second == TEXT_F || WHERE_poliz[i].second == LONG_F) {
					f_independent = false;
					break;
				}
			}
			if (f_independent) {
				tmp_poliz = WHERE_poliz;
				SQL_compute_poliz(key);
				if (tmp_poliz.size() != 1) {
					std::cout << " > Error: wrong poliz result\n";
					state = ERR;
					return;
				}
				WHERE_poliz.clear();

				for (unsigned i = 0; i < consts.size(); ++i) {
					if (tmp_poliz[0].first == consts[i]) {
						WHERE_poliz.push_back(std::make_pair("ALL", OP));
						break;
					}
				}
				curr_strings.clear();
				tmp_poliz.clear();
				return;
			}
			else {
				unsigned str_ind = 0;
				state = INIT;
				SQL_get_table_string(key, str_ind);

				while (state != END && state != ERR) {
					tmp_poliz = WHERE_poliz;
					SQL_compute_poliz(key);
					for (unsigned i = 0; i < consts.size(); ++i) {
						if (tmp_poliz[0].first == consts[i]) {
							curr_strings.push_back(str_ind);
							break;
						}
					}
					++str_ind;
					SQL_get_table_string(key, str_ind);
				}
				WHERE_poliz.clear();
				if (state == ERR) {
					std::cout << " Error when read file" << std::endl;
				}
			}
		}
		else if (lexeme == LIKE_word) {
			if (WHERE_poliz.size() != 1 || WHERE_poliz[0].second != TEXT_F) {
				state = ERR;
				return;
			}

			SQL_lex_analyse();
			if (state != STR) {
				state = ERR;
				return;
			}
			std::string curr_templ = lexeme;
			curr_templ.pop_back();
			unsigned ind = curr_table.size();
			for (unsigned i = 0; i < curr_table.size(); ++i) {
				if (WHERE_poliz[0].first == curr_table[i].name) {
					ind = i;
					break;
				}
			}
			if (ind == curr_table.size()) {
				state = ERR;
				std::cout << " > Error: could not find field " + WHERE_poliz[0].first + " in table " + curr_fname;
				return;
			}
			//Check string == lexeme
			unsigned str_ind = 0;
			state = INIT;
			SQL_get_table_string(key, str_ind);
			
			

			while (state != END && state != ERR) {
				curr_string[ind].pop_back();
				bool good = true;
				unsigned i = 1, j = 1;

				for (; i < curr_templ.size() && j < curr_string[ind].size(); ++i, ++j) {
					if (curr_templ[i] == '_') {
						continue;
					}
					else if (curr_templ[i] == '%') {
						++i;
						if (i >= curr_templ.size()) {
							j = curr_string[ind].size();
							break;
						}
							

						while (curr_string[ind][j] != curr_templ[i] && j < curr_string[ind].size()) {
							++j;
						}
						if (j == curr_string[ind].size()) {
							good = false;
							break;
						}
					}
					else if (curr_templ[i] == '[') {
						bool Not = false;
						if (i + 1 >= curr_templ.size()) {
							message = " > Error: wrong template string";
							state = ERR;
							return;
						}
						if (curr_templ[i + 1] == '^') {
							++i;
							Not = true;
						}
						++i;
						if (i + 1 >= curr_templ.size()) {
							message = " > Error: wrong template string";
							state = ERR;
							return;
						}
						char st = curr_templ[i], fin;
						if (curr_templ[i + 1] == '-') {
							i += 2;
							fin = curr_templ[i];
							if (curr_string[ind][j] < st || curr_string[ind][j] > fin) {
								good = false;
								break;
							}
							++i;
							if (curr_templ[i] != ']') {
								message = " > Error: wrong template string";
								state = ERR;
								return;
							}
						}
						else do {
							if (curr_templ[i] != curr_string[ind][j]) {
								++i;
								continue;
							}
							break;
						} while (curr_templ[i] != ']' && i < curr_templ.size());
						
						if (curr_templ[i] == ']' || i >= curr_templ.size())
							good = false;
						else {
							do {
								++i;
							}
							while (curr_templ[i] != ']');
						}
						if (Not)
							good = !good;
						if (!good)
							break;
					}
					else if (curr_templ[i] != curr_string[ind][j]) {
						good = false;
						break;
					}
				}
				if (i < curr_templ.size() || j < curr_string[ind].size())
					good = false;

				if (NOT)
					good = !good;

				if (good)
					curr_strings.push_back(str_ind);

				++str_ind;
				SQL_get_table_string(key, str_ind);
			}
			if (state == ERR) {
				std::cout << " Error in WHERE LIKE\n";
			}
			return;
		}
	}
}


/* ==========  Poliz  ========== */

void SQL::SQL_compute_poliz(const std::string& key) {
	std::vector <std::pair <std::string, poliz_state>> val_stack;

	for (unsigned ind = 0; ind < tmp_poliz.size(); ++ind) {
		if (tmp_poliz[ind].second != OP) {
			//std::cout << " Read num/text\n";

			if (tmp_poliz[ind].second == TEXT_F || tmp_poliz[ind].second == LONG_F) {
				//std::cout << " Read numf/textf\n";
				for (unsigned i = 0; i < curr_table.size(); ++i) {
					if (tmp_poliz[ind].first == curr_table[i].name) {
						if ((tmp_poliz[ind].second == TEXT_F && curr_table[i].type == 0) ||
							(tmp_poliz[ind].second == LONG_F && curr_table[i].type == 1)) {
							message = " > Error: poliz type mismatch " + tmp_poliz[ind].second + curr_table[i].type;
							return;
						}
						tmp_poliz[ind].first = curr_string[i];

						if (tmp_poliz[ind].second == TEXT_F)
							tmp_poliz[ind].second = TEXT;
						else if (tmp_poliz[ind].second == LONG_F)
							tmp_poliz[ind].second = LONG;
						break;
					}
				}
			}

			val_stack.push_back(tmp_poliz[ind]);
		}
		else {
			std::pair <std::string, poliz_state> expr1, expr2;

			if (tmp_poliz[ind].first == ALL_word) {
				state = ERR;
				return;
			}

			if (tmp_poliz[ind].first != NOT_word) {
				expr2 = val_stack[val_stack.size() - 1];
				val_stack.pop_back();
			}
			expr1 = val_stack[val_stack.size() - 1];
			val_stack.pop_back();
			if (tmp_poliz[ind].first != NOT_word) {
				if (expr2.second == TEXT || expr2.second == TEXT_F) {
					if (expr1.second != TEXT && expr1.second != TEXT_F) {
						state = ERR;
						message = " > Error: type mismatch: expr1 = " + expr1.second;
						message += "; expr2 = " + expr2.second;
						return;
					}
				}
				else if (expr2.second == LONG || expr2.second == LONG_F) {
					if (expr1.second != LONG && expr1.second != LONG_F) {
						state = ERR;
						message = " > Error: type mismatch: expr1 = " + expr1.second;
						message += "; expr2 = " + expr2.second;
						return;
					}
				}
				//Find value for fields here
				if (expr2.second == TEXT_F || expr2.second == LONG_F) {
					for (unsigned i = 0; i < curr_table.size(); ++i) {
						if (expr2.first == curr_table[i].name) {
							if ((expr2.second == TEXT_F && curr_table[i].type == 0) ||
								(expr2.second == LONG_F && curr_table[i].type == 1)) {
								message = " > Error: poliz type mismatch " + expr2.second + curr_table[i].type;
								return;
							}
							expr2.first = curr_string[i];

							if (expr2.second == TEXT_F)
								expr2.second = TEXT;
							else if (expr2.second == LONG_F)
								expr2.second = LONG;
							break;
						}
					}
				}
				//std::cout << "Expr2 = " << expr2.first << "\n";
			}
			if (expr1.second == TEXT_F || expr1.second == LONG_F) {
				for (unsigned i = 0; i < curr_table.size(); ++i) {
					if (expr1.first == curr_table[i].name) {
						if ((expr1.second == TEXT_F && curr_table[i].type == 0) ||
							(expr1.second == LONG_F && curr_table[i].type == 1)) {
							message = " > Error: poliz type mismatch " + expr1.second + curr_table[i].type;
							return;
						}
						expr1.first = curr_string[i];

						if (expr1.second == TEXT_F)
							expr1.second = TEXT;
						else if (expr1.second == LONG_F)
							expr1.second = LONG;
						break;
					}
				}
			}
			//std::cout << "Expr1 = " << expr1.first << "\n";

			if (tmp_poliz[ind].first == NOT_word) {
				if (expr1.first == "true" && expr1.second == TEXT) {
					expr1.first = "false";
					val_stack.push_back(expr1);
				}
				else if (expr1.first == "false" && expr1.second == TEXT) {
					expr1.first = "true";
					val_stack.push_back(expr1);
				}
				else {
					state = ERR;
					std::cout << " > Error computing NOT op: " << expr1.first << " === " << expr1.second << std::endl;
				}
			}
			else if (tmp_poliz[ind].first == "=") {
				if (expr1.first == expr2.first) {
					expr2.first = "true";
				}
				else {
					expr2.first = "false";
				}
				expr2.second = TEXT;
				val_stack.push_back(expr2);
			}
			else if (tmp_poliz[ind].first == "!=") {
				if (expr1.first != expr2.first) {
					expr2.first = "true";
				}
				else {
					expr2.first = "false";
				}
				expr2.second = TEXT;
				val_stack.push_back(expr2);
			}
			else if (tmp_poliz[ind].first == ">=") {
				std::stringstream helper;
				long ex1, ex2;

				if (expr1.second == TEXT && expr2.second == TEXT) {
					if (expr1.first.back() == '\'')
						expr1.first.pop_back();
					if (expr2.first.back() == '\'')
						expr2.first.pop_back();

					if (expr1.first >= expr2.first) {
						expr2.first = "true";
					}
					else {
						expr2.first = "false";
					}
				}
				else if (expr1.second == LONG && expr2.second == LONG) {
					helper << expr2.first;
					helper >> ex2;
					helper.clear();
					helper << expr1.first;
					helper >> ex1;
					helper.clear();
					if (ex1 >= ex2) {
						expr2.first = "true";
					}
					else {
						expr2.first = "false";
					}
				}
				expr2.second = TEXT;
				val_stack.push_back(expr2);
			}
			else if (tmp_poliz[ind].first == "<=") {
				std::stringstream helper;
				long ex1, ex2;

				if (expr1.second == TEXT && expr2.second == TEXT) {
					if (expr1.first.back() == '\'')
						expr1.first.pop_back();
					if (expr2.first.back() == '\'')
						expr2.first.pop_back();

					if (expr1.first <= expr2.first) {
						expr2.first = "true";
					}
					else {
						expr2.first = "false";
					}
				}
				else if (expr1.second == LONG && expr2.second == LONG) {
					helper << expr2.first;
					helper >> ex2;
					helper.clear();
					helper << expr1.first;
					helper >> ex1;
					helper.clear();
					if (ex1 <= ex2) {
						expr2.first = "true";
					}
					else {
						expr2.first = "false";
					}
				}
				expr2.second = TEXT;
				val_stack.push_back(expr2);
			}
			else if (tmp_poliz[ind].first == ">") {
			std::stringstream helper;
			long ex1, ex2;

			if (expr1.second == TEXT && expr2.second == TEXT) {
				if (expr1.first.back() == '\'')
					expr1.first.pop_back();
				if (expr2.first.back() == '\'')
					expr2.first.pop_back();

				if (expr1.first > expr2.first) {
					expr2.first = "true";
				}
				else {
					expr2.first = "false";
				}
			}
			else if (expr1.second == LONG && expr2.second == LONG) {
				helper << expr2.first;
				helper >> ex2;
				helper.clear();
				helper << expr1.first;
				helper >> ex1;
				helper.clear();
				if (ex1 > ex2) {
					expr2.first = "true";
				}
				else {
					expr2.first = "false";
				}
			}
			expr2.second = TEXT;
			val_stack.push_back(expr2);
			}
			else if (tmp_poliz[ind].first == "<") {
			std::stringstream helper;
			long ex1, ex2;

			if (expr1.second == TEXT && expr2.second == TEXT) {
				if (expr1.first.back() == '\'')
					expr1.first.pop_back();
				if (expr2.first.back() == '\'')
					expr2.first.pop_back();

				if (expr1.first < expr2.first) {
					expr2.first = "true";
				}
				else {
					expr2.first = "false";
				}
			}
			else if (expr1.second == LONG && expr2.second == LONG) {
				helper << expr2.first;
				helper >> ex2;
				helper.clear();
				helper << expr1.first;
				helper >> ex1;
				helper.clear();
				if (ex1 < ex2) {
					expr2.first = "true";
				}
				else {
					expr2.first = "false";
				}
			}
			expr2.second = TEXT;
			val_stack.push_back(expr2);
			}

			else if (tmp_poliz[ind].first == OR_word) {
				if (expr1.first != "true" && expr1.first != "false" && expr1.second != TEXT &&
					expr2.first != "true" && expr2.first != "false" && expr2.second != TEXT) {
					state = ERR;
					message = " > Error computing OR";
					return;
				}
				if (expr1.first == "true" || expr2.first == "true") {
					expr1.first = "true";
				}
				else {
					expr1.first = "false";
				}
				val_stack.push_back(expr1);
			}
			else if (tmp_poliz[ind].first == AND_word) {
				if (expr1.first != "true" && expr1.first != "false" && expr1.second != TEXT &&
					expr2.first != "true" && expr2.first != "false" && expr2.second != TEXT) {
					state = ERR;
					message = " > Error computing AND";
					return;
				}
				if (expr1.first == "false" || expr2.first == "false") {
					expr1.first = "false";
				}
				else {
					expr1.first = "true";
				}
				val_stack.push_back(expr1);
			}

			else if (tmp_poliz[ind].first == "+") {
				std::stringstream helper;
				long ex1, ex2;

				if (expr1.second != LONG || expr2.second != LONG) {
					state = ERR;
					message = " > Error: wrong parameters for +";
					return;
				}
				
				helper << expr2.first;
				helper >> ex2;
				helper.clear();
				helper << expr1.first;
				helper >> ex1;
				helper.clear();
				
				ex1 += ex2;

				helper << ex1;
				helper >> expr1.first;
				helper.clear();

				val_stack.push_back(expr1);
			}
			else if (tmp_poliz[ind].first == "-") {
				std::stringstream helper;
				long ex1, ex2;

				if (expr1.second != LONG || expr2.second != LONG) {
					state = ERR;
					message = " > Error: wrong parameters for -";
					return;
				}
				helper << expr2.first;
				helper >> ex2;
				helper.clear();
				helper << expr1.first;
				helper >> ex1;
				helper.clear();

				ex1 -= ex2;

				helper << ex1;
				helper >> expr1.first;
				helper.clear();

				val_stack.push_back(expr1);
			}
			else if (tmp_poliz[ind].first == "*") {
				std::stringstream helper;
				long ex1, ex2;

				if (expr1.second != LONG || expr2.second != LONG) {
					state = ERR;
					message = " > Error: wrong parameters for *";
					return;
				}

				helper << expr2.first;
				helper >> ex2;
				helper.clear();
				helper << expr1.first;
				helper >> ex1;
				helper.clear();

				ex1 *= ex2;

				helper << ex1;
				helper >> expr1.first;
				helper.clear();

				val_stack.push_back(expr1);
			}
			else if (tmp_poliz[ind].first == "/") {
				std::stringstream helper;
				long ex1, ex2;

				if (expr1.second != LONG || expr2.second != LONG) {
					state = ERR;
					message = " > Error: wrong parameters for /";
					return;
				}

				helper << expr2.first;
				helper >> ex2;
				helper.clear();

				if (ex2 == 0) {
					state = ERR;
					message = " > Error: division by zero";
					return;
				}

				helper << expr1.first;
				helper >> ex1;
				helper.clear();

				ex1 /= ex2;

				helper << ex1;
				helper >> expr1.first;
				helper.clear();

				val_stack.push_back(expr1);
			}
			else if (tmp_poliz[ind].first == "%") {
				std::stringstream helper;
				long ex1, ex2;

				if (expr1.second != LONG || expr2.second != LONG) {
					state = ERR;
					message = " > Error: wrong parameters for %";
					return;
				}

				helper << expr2.first;
				helper >> ex2;
				helper.clear();

				if (ex2 == 0) {
					state = ERR;
					message = " > Error: mod by zero";
					return;
				}

				helper << expr1.first;
				helper >> ex1;
				helper.clear();

				ex1 %= ex2;

				helper << ex1;
				helper >> expr1.first;
				helper.clear();

				val_stack.push_back(expr1);
			}
		}
	}
	if (val_stack.size() != 1) {
		std::cout << " > Error: wrong poliz result" << std::endl;
		state = ERR;
	}
	tmp_poliz.clear();
	tmp_poliz = val_stack;
}


const std::string& SQL::SQL_get_result() {
	return message;
}
