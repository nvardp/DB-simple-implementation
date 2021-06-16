#include "MySQL.h"

MySql::MySql()
{
	path = get_path_of_exe() + "\\";
	for (const auto& entry : fs::directory_iterator(path))
	{
		if (fs::path(entry.path()).extension() == ".txt")
		{
			std::fstream FDB;
			std::string fileName = (entry.path()).string();
			FDB.open(fileName);
			std::string line;
			getline(FDB, line);
			Entry row;
			Table table;
			std::string name;
			std::string value;
			while (!FDB.eof())
			{
				while (line != "^")
				{
					if (line != "")
					{
						name = get_col_name(line);
						value = get_col_value(line);
						row.insert(std::pair<std::string, std::string>(name, value));
					}
					getline(FDB, line);
				}
				table.push_back(row);
				row.clear();
				FDB >> line;
			}
			FDB.close();
			std::string TableName = fileName.substr(fileName.find_last_of("\\/") + 1, fileName.size());
			TableName.erase(TableName.size() - 4, 4);
			tables.insert(std::pair<std::string, Table>(TableName, table));
		}
	}
}

std::string MySql::get_col_name(std::string other)
{
	return other.substr(0, other.find("="));
}
std::string MySql::get_col_value(std::string other)
{
	std::string column = other.substr(other.find("="), other.find(","));
	column.erase(0, 1);
	if (column.back() == ',')
		column.erase(column.size() - 1, 1);
	return column;
}
std::string MySql::get_path_of_exe()
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(buffer).substr(0, pos);
}


bool MySql::isQueryOK(const std::string& query)
{
	std::string firstWord = query.substr(0, query.find(" "));
	if (query.empty())
	{
		return false;
	}
	else if (firstWord != _CREATE_CMD && firstWord != _INSERT_CMD
		&& firstWord != _DROP_CMD && firstWord != _SELECT_CMD && firstWord != _DELETE_CMD && firstWord != _UPDATE_CMD)
	{
		std::cout << "No such command. ";
		return false;
	}
	else if ((query.substr(query.find(" "), -1)) == "")
		return false;
	else return true;

}
void MySql::Execute(const std::string& other)
{
	auto _QUERY = parse(other);
	if (_QUERY[cmd] == _CREATE_CMD)
	{
		if (!exist(tables, (_QUERY)[name]))
		{
			create_file((_QUERY)[name]);
			tables.insert(std::pair<std::string, Table>(_QUERY[name], {}));
			std::cout << "created" << std::endl;
		}
		else std::cout << "already exist" << std::endl; // 
	}
	else if (_QUERY[cmd] == _INSERT_CMD)
	{
		if (exist(tables, (_QUERY)[name]))
		{
			Insert(_QUERY);
			std::cout << "inserted " << _QUERY.size() - 2 << " column(s)." << std::endl;
		}
		else std::cout << "Table does not exist" << std::endl;
	}
	else if (_QUERY[cmd] == _DROP_CMD)
	{
		if (!drop_table(_QUERY))
			std::cout << "Table successfully deleted" << std::endl;
		else std::cout << "Table does not exist" << std::endl;
	}
	else if (_QUERY[cmd] == _DELETE_CMD)
	{
		if (exist(tables, (_QUERY)[name]))
		{
			std::cout << delete_all_found(_QUERY) << " row(s) successfully deleted.\n";
		}
	}
	else if (_QUERY[cmd] == _SELECT_CMD)
	{
		if (exist(tables, (_QUERY)[name]))
		{
			show_table(_QUERY);
		}
	}
	else if (_QUERY[cmd] == _UPDATE_CMD)
	{
		if (exist(tables, (_QUERY)[name]))
		{
			std::cout << update(_QUERY) << " row(s) successfully updated.\n";  ;
		}
	}
}
std::vector<std::string> MySql::parse(std::string other)
{
	Query _QUERY;
	while (!other.empty())
	{
		std::string firstWord = other.substr(0, other.find(" "));
		_QUERY.push_back(firstWord);
		other.erase(0, firstWord.size() + 1);
	}
	return _QUERY;
}

bool MySql::exist(const std::map<std::string, Table>& table, const std::string& name) const
{
	if (table.find(name) != table.end())
		return true;
	return false;
}
void MySql::create_file(const std::string& name)
{
	std::ofstream File;

	File.open(path + name + ".txt");
}
void MySql::Insert(const Query& query)
{
	write_in_file(query);
	update_map(query);
}
void MySql::update_map(const Query& query)
{
	std::map<std::string, Table>::iterator itr;
	itr = tables.find(query[name]);

	if (itr != tables.end())
	{
		Entry row;
		for (int i = data_start; i < query.size(); i++)
		{
			row.insert(std::pair<std::string, std::string>(get_col_name(query[i]), get_col_value(query[i])));
		}
		Table table;
		table.push_back(row);
		itr->second = table;
	}
}
void MySql::write_in_file(const Query& vec)
{
	std::ofstream FDB;

	FDB.open(path + vec[name] + ".txt", std::ios_base::app);
	FDB << "\n^\n";
	for (int i = data_start; i < vec.size(); i++)
	{
		FDB << get_col_name(vec[i]) << "=" << get_col_value(vec[i]) << std::endl;
	}
	FDB << "^\n";
}
bool MySql::drop_table(const Query& query)
{
	std::string tmp = path + query[name] + ".txt";
	if (!remove(tmp.c_str())) // remove expectes char*
	{
		tables.erase(query[name]);
		return false;
	}
	else return true;
}
int MySql::delete_all_found(const Query& query)
{
	std::map<std::string, Table>::iterator itr;
	itr = tables.find(query[name]);
	int counter = 0;
	if (itr != tables.end())
	{
		auto table = itr->second; //vector of map
		std::map<std::string, std::string>::iterator itr_table;
		int i = 0;
		for (; i < table.size(); i++)
		{
			itr_table = table[i].find(get_col_name(query[data_start]));
			if (itr_table != table[i].end())
				if (itr_table->second == get_col_value(query[data_start]))
				{
					++counter;  break;
				}
		}
		table.erase(table.begin() + i);
		write_after_operation(table, query[name]);
	}
	return counter;
}
void MySql::write_after_operation(const Table& other, const std::string& name)
{
	std::ofstream FDB;
	FDB.open(path + name + ".txt");

	for (int i = 0; i < other.size(); i++)
	{
		for (std::map<std::string, std::string>::const_iterator it = other[i].begin(); it != other[i].end(); ++it)
		{
			FDB << it->first << "=" << it->second << "\n";
		}
		FDB << "^\n";
	}
}
void MySql::show_table(const Query& other)
{
	if (data_start == other.size())
	{
		std::cout << "chose * or where";
		return;
	}
	auto iter = tables.find(other[name]);
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (other[data_start] == _ALL)
	{
		int counter = 0;
		for (size_t i = 0; i < iter->second.size(); i++)
		{

			for (std::map<std::string, std::string>::const_iterator it = iter->second[i].begin(); it != iter->second[i].end(); ++it)
			{
				if (i % 2 == 0)	SetConsoleTextAttribute(hConsole, 112);
				else SetConsoleTextAttribute(hConsole, 128);
				std::cout << it->first << "=" << it->second << ", ";
			}
			std::cout << std::endl;
		}
	}
	else if (other[data_start] == _WHERE_CMD)
	{
		std::map<std::string, std::string>::iterator itr_table;
		for (size_t i = 0; i < iter->second.size(); i++)
		{
			bool couted = false;
			for (std::map<std::string, std::string>::const_iterator it = iter->second[i].begin(); it != iter->second[i].end(); ++it)
			{
				itr_table = iter->second[i].find(get_col_name(other[data_start + 1]));
				if (itr_table != iter->second[i].end())
					if (itr_table->second == get_col_value(other[data_start + 1]))
					{
						if(i%2==0)	SetConsoleTextAttribute(hConsole, 112);
						else SetConsoleTextAttribute(hConsole, 128);
							std::cout << it->first << "=" << it->second << ", ";
							couted = true;
					}
			}
			if (couted)
				std::cout << std::endl;
		}
	}
	SetConsoleTextAttribute(hConsole, 15);
}
int MySql::update(const Query& query)
{
	std::map<std::string, Table>::iterator itr;
	itr = tables.find(query[name]);
	int counter = 0;
	if (itr != tables.end())
	{
		Table table = itr->second; //vector of map
		std::map<std::string, std::string>::iterator itr_table;
		int i = 0;
		for (; i < table.size(); i++)
		{
			itr_table = table[i].find(get_col_name(query[data_start]));
			if (itr_table != table[i].end())
				if (itr_table->second == get_col_value(query[data_start]))
				{
					itr_table->second = get_col_value(query[data_start + 1]);
						++counter;
				}
		}
		itr->second = table;
		write_after_operation(table, query[name]);
	}
	return counter;
}