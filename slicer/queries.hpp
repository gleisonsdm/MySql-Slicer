/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>

/*
  Include directly the different
  headers from cppconn/ and mysql_driver.h + mysql_util.h
  (and mysql_connection.h). This will reduce your build time!
*/
#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include <cstdio>
#include <string>
#include <vector>

using namespace std;

class Queries { 

public: 
	static long getNumberOfRows(sql::Connection* con, string table);

	static string selectDBTables(string database);

	static string selectDBViews(string database);

	static string selectFKconstraints(string database);

	static vector<string>& selectDBTables(sql::Connection* con, string database);

	static vector<string>& selectDBViews(sql::Connection* con, string database);

	static vector<vector<string>>& selectFKconstraints(sql::Connection* con, string database);

	static sql::ResultSet* Process::markRowAsVisited(sql::Connection* con, string table, string clause, long limit);
};
