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
#include <cmath>

#include "queries.hpp"
#include "process.hpp"

using namespace std;

void Process::buildDependencyGraph(sql::Connection* con, string database, double percentage) {
	// create one vector for each vertex
	vector<string> tables = Queries::selectDBTables(con, database);  
	for(int i = 0, ie = tables.size(); i < ie; i++) {
		parentGraph[tables[i]] = vector<string>();
		childGraph[tables[i]] = vector<string>();
		nodesToCopy[tables[i]] = ceil(Queries::getNumberOfRows(con, tables[i]) * (percentage / 100.0));
	} 

	vector<string> views = Queries::selectDBViews(con, database);  
	for(int i = 0, ie = tables.size(); i < ie; i++) {
		parentGraph[views[i]] = vector<string>();
		childGraph[views[i]] = vector<string>();
		nodesToCopy[views[i]] = 0;
	} 

	// Add edges 
	vector<vector<string>> dependencies = Queries::selectFKconstraints(con, database);
	for (int i = 0, ie = dependencies.size(); i < ie; i++) {
		parentGraph[dependencies[i][3]].push_back(dependencies[i][1]);
		childGraph[dependencies[i][1]].push_back(dependencies[i][3]); 
	}
}

vector<string>& Process::transformResultSetIntoInserts(sql::Connection* con, string selectQuery, string table) {
	vector<string>* inserts = new vector<string>();
	try {
		sql::Statement* stmt = con->createStatement();
		sql::ResultSet* res = stmt->executeQuery(selectQuery);
		sql::ResultSetMetaData* res_meta = res->getMetaData();

		string datatype;  
		int columncount = res_meta->getColumnCount();

		while (res->next()) {
			string query = "INSERT INTO " + table;
			string header = "(";
			string values = "(";
			for (int i = 0; i < columncount; i++) {                 
				if (i > 0) {
					header += ",";
					values += ",";
				}
				header += datatype = res_meta->getColumnTypeName(i + 1);
				values += res->getString(i);
			}
			header += ")";
			values += ")";
			query += " " + header + " VALUES " + values + ";";
			inserts->push_back(query);
		}
	} catch(sql::SQLException &e){
		cerr << e.what() << endl;	
	}
	return *inserts;
}

void Process::markRowAsVisited(sql::Connection* conS, sql::Connection* conT, string table, string clause, long limit) {
	string sql = "SELECT * FROM  " + table + " ";
	if (clause != "") {
		sql += "WHERE " + clause + " ";
	}
	sql += "limit = " + to_string(limit) + ";";
	sql::Statement* stmt = conS->createStatement();
	sql::ResultSet* res = stmt->executeQuery(sql);
	while (res->next()) {
		queries.insert(res);
	}
	// Finish
}

void Process::search(sql::Connection* conS, sql::Connection* conD) {
	vector<pair<long, string>> nodesByDependencies;
	map<string, int> visited;
	for (auto I = parentGraph.begin(), IE = parentGraph.end(); I != IE; I++) {
		visited[I->first] = NOT_VISITED;
	}

	stack<string> nodesInVisit;
	for (auto I = parentGraph.begin(), IE = parentGraph.end(); I != IE; I++) {
		nodesByDependencies.push_back(make_pair(I->second.size(), I->first));

		string table = I->first;
		if (visited[table] == VISITED) { 
			continue;
		} else if (visited[table] == IN_VISIT) {
			cerr << "[Error] Unexpeted cycle found!" << endl;
			// Cycle found
			continue;
		} else { 
			visited[table] = IN_VISIT;
			nodesInVisit.push(table);
		}

		vector<string> toMarkAsVisited;
		while (!nodesInVisit.empty()) {
			table = nodesInVisit.top();
			nodesInVisit.pop();
			toMarkAsVisited.push_back(table);

			// Insert Rows
			markRowAsVisited(conS, conD, table, "", nodesToCopy[table]);

			if (visited[table] == VISITED) { 
				continue;
			} else if (visited[table] == IN_VISIT) {
				// Cycle found
				cerr << "[Warning] Cycle found in table: " << table << endl;
				cerr << "Keeping on track by adding missing dependencies." << endl;
				continue;
			} else { 
				visited[table] = IN_VISIT;

				// Insert Rows

				// Adicionar codigo para copiar dependencias
				markRowAsVisited(conS, conD, table, "", nodesToCopy[table]);
				nodesInVisit.push(table);
			}		
		}

		for (int i = 0, ie = toMarkAsVisited.size(); i < ie; i++) {
			visited[toMarkAsVisited[i]] = VISITED;
		}
	}
}

void Process::process(sql::Connection* srcCon, string srcDatabase, sql::Connection* dstCon, string dstDatabase, double percentage) {
	queries = set<sql::ResultSet*>();
	parentGraph = map<string, vector<string>>();
	childGraph = map<string, vector<string>>();
	nodesToCopy = map<string, long>();

	buildDependencyGraph(srcCon, srcDatabase, percentage); 
	search(srcCon, dstCon);
}
