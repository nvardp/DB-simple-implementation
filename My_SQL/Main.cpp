#include "MySQL.h"
string ReadQuery()
{
	string query = "";
	std::getline(std::cin, query);
	cout <<query;
	return query;
}
void rules()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); 
	SetConsoleTextAttribute(hConsole, 8);
	string str = " 1. create table_name \n 2. insert table_name col_name=value, col_name2=value \n 3. update table_name col_name=value =new_value \n 4. delete table_name col_name=value \n 5. drop table_name \n 6. select table_name * OR select table_name where col_name=value\n";
	std::cout << str;
	SetConsoleTextAttribute(hConsole, 15);
}

int main()
{
	MySql my_sql;
	rules();
	while (true) {
		const auto query = ReadQuery();
		if (!MySql::isQueryOK(query)) {
			std::cout << "Bad query.\n";
			continue;
		}
		my_sql.Execute(query);
	}
}




