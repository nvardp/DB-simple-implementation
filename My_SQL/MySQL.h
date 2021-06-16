#pragma once
#include <iostream>
#include<fstream>
#include<string>
#include<vector>
#include<map>
#include <filesystem>
#include <Windows.h>

namespace fs = std::filesystem;
using namespace std;

const string _EXIT = "exit";
const string _CREATE_CMD = "create"; //+
const string _INSERT_CMD = "insert"; //+
const string _UPDATE_CMD = "update";
const string _DROP_CMD = "drop"; //+
const string _DELETE_CMD = "delete"; //+
const string _SELECT_CMD = "select"; //+
const string _WHERE_CMD = "where"; //+
const string _ALL = "*";

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
	using Query = std::vector<string>;

public:
	MySql();
	static bool isQueryOK(const string& other);
	void Execute(const string& other);
private:
	void create_file(const string& name);
	void Insert(const Query& query);
	bool drop_table(const Query& query);
	int delete_all_found(const Query& query);
	int update(const Query& query);
	void show_table(const Query& other);
	string get_col_name(string other);
	string get_col_value(string other);
	string get_path_of_exe();
	Query parse(string other);
	void write_in_file(const Query& vec);
	void update_map(const Query& query);
	bool exist(const std::map<string, Table>& table, const string& name) const;
	void write_after_operation(const Table& other, const string& name);
private:
	std::map<std::string, Table> tables; //name of file, contains
	std::string path;
};