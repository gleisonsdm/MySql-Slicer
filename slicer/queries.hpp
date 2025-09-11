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
	long getNumberOfRows(sql::Connection* con, string table);

	string selectDBTables(string database);

	string selectDBViews(string database);

	string selectFKconstraints(string database);

	vector<string>& selectDBTables(sql::Connection* con, string database);

	vector<string>& selectDBViews(sql::Connection* con, string database);

	vector<vector<string>>& selectFKconstraints(sql::Connection* con, string database);
};
