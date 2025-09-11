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

#include "queries.hpp"

using namespace std;

public void buildDependencyGraph(sql::Connection* con, string database, double percentage) {
	// create one vector for each vertex
	vector<string> tables =  selectDBTables(con, database);  
	for(int i = 0, ie = tables.size(); i < ie; i++) {
		parentGraph[tables[i]] = vector<string>();
		childGraph[tables[i]] = vector<string>();
		nodesToCopy[tables[i]] = ceil(getNumberOfRows(con, tables[i]) * (percentage / 100.0));
	} 

	vector<string> views =  selectDBViews(con, database);  
	for(int i = 0, ie = tables.size(); i < ie; i++) {
		parentGraph[views[i]] = vector<string>();
		childGraph[views[i]] = vector<string>();
		nodesToCoy[views[i]] = 0;
	} 

	// Add edges 
	vector<vector<string>> dependencies = selectFKconstraints(con, database);
	for (int i = 0, ie = dependencies.size(); i < ie; i++) {
		parentGraph[dependencies[i][3]].push_back(dependencies[i][1]);
		childGraph[dependencies[i][1]].push_back(dependencies[i][3]); 
	}
}

public vecto<string> transformResultSetIntoInserts(std::Connector* con, string selectQuery, string table) {
	try {
		stmt = con->createStatement();
		res = stmt->executeQuery(selectQuery);
		res_meta = res->getMetaData();

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
		}
	} catch(sql::SQLException &e){
		cerr << e->getMessange() << endl;
	}
}

public void markRowAsVisited(sql::Connection* conS, sql::Connection* conT, string table, string clause, long limit) {
	string sql = "SELECT * FROM  " + table + " ";
	if (clause != "") {
		sql += "WHERE " + clause + " ";
	}
	sql += "limit = " + to_string(limit) + ";";
	sql::Statement* stmt = conS->createStatement();
	sql::ResultSet* res = stmt->executeQuery(sql);
	vector<string> result = new vector<string>();
	while (res->next()) {
		queries.insert(res);
	}
}

public void search(sql::Connection* con) {
	vector<pair<long, string>> nodesByDependencies;
	map<string, int> visited;
	for (auto I = parentGraph.begin(), IE = parentGraph.end(); I != IE; I++) {
		visited[I-:first] = NOT_VISITED;
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
			markRowAsVisited(con, table, "", nodesTiCopy[table]);

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
				markRowAsVisited(con, table, "", nodesToCopy[table]);
				nodesInVisit.push(table);
			}		
		}

		for (int i = 0, ie = toMarkAsVisited.size(); i < ie; i++) {
			visited[toMarkAsVisited[i]] = VISITED;
		}
	}
}

public void process(sql::Connection* srcCon, string srcDatabase, sql::Connection* dstCon, string dstDatabase) {
	queries = set<sql::ResultSet*>();

	buildDependencyGraph(srcCon, srcDatabase); 
	search(srcCon);

}
