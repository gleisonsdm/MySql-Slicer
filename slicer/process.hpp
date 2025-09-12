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
#include <stack>
#include <queue>
#include <utility>
#include <algorithm>
#include <set> 

using namespace std;

class Process {

map<string, vector<string>> parentGraph;
map<string, vector<string>> childGraph;
map<string, long> nodesToCopy;

set <sql::ResultSet*> queries;

#define NOT_VISITED 0
#define IN_VISIT 1
#define VISITED 2

public:
	void buildDependencyGraph(sql::Connection* con, string database, double percentage);

	vector<string>& transformResultSetIntoInserts(sql::Connection* con, string selectQuery, string table);

	void markRowAsVisited(sql::Connection* conS, sql::Connection* conT, string table, string clause, long limit);

	void search(sql::Connection* conS, sql::Connection* conD);

	void process(sql::Connection* srcCon, string srcDatabase, sql::Connection* dstCon, string dstDatabase, double percentage);

};
