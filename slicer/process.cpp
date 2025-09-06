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
#include <pair>
#include <algorithm>
#include <set> 

#include "queries.cpp"

using namespace std;

map<string, vector<string>> parentGraph;
map<string, vector<string>> childGraph;
map<string, long> nodesToCopy;

#define NOT_VISITED 0
#define IN_VISIT 1
#define VISITED 2

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

set <sql::ResultSet*> queries;

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

public void process(sql::Connection* con, string database) {
  queries = set<sql::ResultSet*>();

	buildDependencyGraph(con, database); 
	search(con);

}

// Deletar

public string selectDBTables(string database) {
	string sql = "SELECT table_name FROM information_schema.tables ";
  sql += "WHERE table_schema = '" + database + "' AND table_type = 'BASE TABLE';";
	return sql;
}

public string selectDBViews(string database) {
	string sql = "SELECT table_name FROM information_schema.tables ";
  sql += "WHERE table_schema = '" + database + "' AND table_type = 'VIEW';";
	return sql;
}

public string selectFKconstraints(string database) {
	string sql = "SELECT rc.TABLE_NAME AS TableWithForeignKey, ";
  sql += "kcu.COLUMN_NAME AS ForeignKeyColumn, ";
  sql += "rc.REFERENCED_TABLE_NAME AS ReferencedTable, ";
  sql += "kcu.REFERENCED_COLUMN_NAME AS ReferencedColumn, ";
  sql += "rc.CONSTRAINT_NAME AS ForeignKeyConstraintName FROM ";
  sql += "INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS rc JOIN" ;
  sql += "INFORMATION_SCHEMA.KEY_COLUMN_USAGE kcu ON ";
  sql += "rc.CONSTRAINT_SCHEMA = kcu.TABLE_SCHEMA AND ";
  sql += "rc.TABLE_NAME = kcu.TABLE_NAME AND ";
  sql += "rc.CONSTRAINT_NAME = kcu.CONSTRAINT_NAME WHERE ";
  sql += "rc.CONSTRAINT_SCHEMA = '" + database + "';";
}

public &vector<string> selectDBTables(sql::Connection* con, string database) {
	string sql = "SELECT table_name FROM information_schema.tables ";
  sql += "WHERE table_schema = '" + database + "' AND table_type = 'BASE TABLE';";
	sql::Statement* stmt = con->createStatement();
  sql::ResultSet* res = stmt->executeQuery(sql);
  vector<string> result = new vector<string>();
  while (res->next()) {
    result.push_back(res->getString(1);
  }
  return result;
}

public &vector<string> selectDBViews(sql::Connection* con, string database) {
	string sql = "SELECT table_name FROM information_schema.tables ";
  sql += "WHERE table_schema = '" + database + "' AND table_type = 'VIEW';";
	sql::Statement* stmt = con->createStatement();
  sql::ResultSet* res = stmt->executeQuery(sql);
  vector<string> result = new vector<string>();
  while (res->next()) {
    result.push_back(res->getString(1);
  }
  return result;
}

public &vector<vector<string>> selectFKconstraints(sql::Connection* con, string database) {
	string sql = "SELECT rc.TABLE_NAME AS TableWithForeignKey, ";
  sql += "kcu.COLUMN_NAME AS ForeignKeyColumn, ";
  sql += "rc.REFERENCED_TABLE_NAME AS ReferencedTable, ";
  sql += "kcu.REFERENCED_COLUMN_NAME AS ReferencedColumn, ";
  sql += "rc.CONSTRAINT_NAME AS ForeignKeyConstraintName FROM ";
  sql += "INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS rc JOIN" ;
  sql += "INFORMATION_SCHEMA.KEY_COLUMN_USAGE kcu ON ";
  sql += "rc.CONSTRAINT_SCHEMA = kcu.TABLE_SCHEMA AND ";
  sql += "rc.TABLE_NAME = kcu.TABLE_NAME AND ";
  sql += "rc.CONSTRAINT_NAME = kcu.CONSTRAINT_NAME WHERE ";
  sql += "rc.CONSTRAINT_SCHEMA = '" + database + "';";
  sql += "WHERE table_schema = '" + database + "' AND table_type = 'VIEW';";
	sql::Statement* stmt = con->createStatement();
  sql::ResultSet* res = stmt->executeQuery(sql);
  vector<vector<string>> result = new vector<string>();
  while (res->next()) {
    vector<string> row = new vector<string>();
    row.push_back(res->getString(1);
    row.push_back(res->getString(2);
    row.push_back(res->getString(3);
    row.push_back(res->getString(4);
    row.push_back(res->getString(5);
    row.push_back(res->getString(6);
		result.push_back(row);
  }
  return result;
}
