#pragma once
#include <iostream>
#include<fstream>
#include<string>
#include<vector>
#include<map>
#include <filesystem>
#include <Windows.h>

using std::cout;
using std::string;

namespace fs = std::filesystem;

const std::string _EXIT = "exit";
const std::string _CREATE_CMD = "create"; 
const std::string _INSERT_CMD = "insert"; 
const std::string _UPDATE_CMD = "update";
const std::string _DROP_CMD = "drop"; 
const std::string _DELETE_CMD = "delete";
const std::string _SELECT_CMD = "select"; 
const std::string _WHERE_CMD = "where"; 
const std::string _ALL = "*";

enum QueryIndex
{
	cmd = 0,
	name,
	data_start
};

class MySql {
private:
	using Entry = std::map<std::string, std::string>;  // one obj= one map//row
	using Table = std::vector<Entry>; //one file
	using Query = std::vector<std::string>;

public:
	MySql();
	static bool isQueryOK(const std::string& other);
	void Execute(const std::string& other);
private:
	void create_file(const std::string& name);
	void Insert(const Query& query);
	bool drop_table(const Query& query);
	int delete_all_found(const Query& query);
	int update(const Query& query);
	void show_table(const Query& other);
	std::string get_col_name(std::string other);
	std::string get_col_value(std::string other);
	std::string get_path_of_exe();
	Query parse(std::string other);
	void write_in_file(const Query& vec);
	void update_map(const Query& query);
	bool exist(const std::map<std::string, Table>& table, const std::string& name) const;
	void write_after_operation(const Table& other, const std::string& name);
private:
	std::map<std::string, Table> tables; //name of file, contains
	std::string path;
};