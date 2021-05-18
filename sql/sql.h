#pragma once

#include "../dbt/dbhandler.h"
#include <sstream>
#include <vector>
#include <stdlib.h>


struct SQL_words {
protected:
	char c;
	enum lex_state { INIT, PLUS, SUBTR, MULT, DIV, MOD, OP_PAR, CL_PAR, COMMA, NUM, IDEN, STR, EQU, N_EQU, L_EQU, M_EQU, LESS, MORE, MAIN, OTHER, ERR, END } state;
	enum poliz_state { LONG, TEXT, OP, LONG_F, TEXT_F}; //LONG-field, TEXT-field
	const std::string file_headend = "#====#";
	std::string lexeme;

	//Main words
	const std::string SELECT_word = "SELECT";
	const std::string INSERT_word = "INSERT";
	const std::string UPDATE_word = "UPDATE";
	const std::string DELETE_word = "DELETE";
	const std::string CREATE_word = "CREATE";
	const std::string DROP_word   = "DROP"  ;

	//Other words
	const std::string FROM_word   = "FROM"  ;
	const std::string INTO_word   = "INTO"  ;
	const std::string TABLE_word  = "TABLE" ;
	const std::string SET_word    = "SET"   ;
	const std::string LIKE_word   = "LIKE"  ;
	const std::string NOT_word    = "NOT"   ;
	const std::string OR_word     = "OR"    ;
	const std::string AND_word    = "AND"   ;
	const std::string LONG_word   = "LONG"  ;
	const std::string TEXT_word   = "TEXT"  ;
	const std::string WHERE_word  = "WHERE" ;
	const std::string IN_word     = "IN"    ;
	const std::string ALL_word    = "ALL"   ;

	bool SQL_check_main_words(const std::string &message);
	bool SQL_check_other_words(const std::string &message);
	bool SQL_check_words(const std::string &message);
};

class SQL : SQL_words {
	DB_handler DBTable;
	std::string message;
	std::stringstream scin;

	struct Fields {
		std::string name;
		int type = 0; //0 - LONG; 1 - TEXT
		unsigned text_length = 0; //for TEXT field size; for long = 0
	};

	std::vector <Fields> curr_table;
	std::string curr_fname;
	std::vector <unsigned> curr_strings; //for WHERE-clause
	std::vector <unsigned> curr_fields;
	std::vector <std::string> curr_string;
	std::vector <std::pair<std::string, poliz_state>> poliz;
	std::vector <std::pair<std::string, poliz_state>> WHERE_poliz;
	std::vector <std::pair<std::string, poliz_state>> tmp_poliz;

	bool text_e, long_e, logic, ratio;
	unsigned read_lex = 0;

public:
	void SQL_command();
	void SQL_select();
	void SQL_insert();
	void SQL_update();
	void SQL_delete();
	void SQL_create();
	void SQL_drop();

	void SQL_WHERE_clause(const std::string &key);

	void SQL_analyse(const std::string &message);
	void SQL_logic_expr(std::vector <std::pair<std::string, poliz_state>> &poliz);
	void SQL_logic_S(std::vector <std::pair<std::string, poliz_state>> &poliz);
	void SQL_logic_M(std::vector <std::pair<std::string, poliz_state>> &poliz);
	void SQL_expr(std::vector <std::pair<std::string, poliz_state>> &poliz);
	void SQL_S(std::vector <std::pair<std::string, poliz_state>> &poliz);
	void SQL_M(std::vector <std::pair<std::string, poliz_state>> &poliz);

	void SQL_init();
	void SQL_lex_analyse();
	void SQL_compute_poliz(const std::string &key);
	void SQL_get_string_pos(const std::string& key, unsigned number, std::streampos &from, std::streampos &to);
	void SQL_get_field_names(const std::string &key);
	void SQL_get_table_string(const std::string &key, unsigned number);
	void SQL_check_field_values(const std::string &key, const std::vector <std::string> &list); //for INSERT
	void SQL_check_field_names(const std::string &key, const std::vector <std::string> &list); //for SELECT
	std::string SQL_get_field_type(const std::string &key, const std::string &field);
	const std::string & SQL_get_result();
};
