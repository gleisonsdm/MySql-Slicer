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

#include "queries.hpp"

#include <cstdio>
#include <string>
#include <vector>

using namespace std;

long Queries::getNumberOfRows(sql::Connection* con, string table) {
	string sql = "select count(*) from " + table + ";";
	sql::Statement* stmt = con->createStatement();
	sql::ResultSet* res = stmt->executeQuery(sql);
	if (res->next()) {
		return stol(res->getString(1));
	}
	return 0L;
}

string Queries::selectDBTables(string database) {
	string sql = "SELECT table_name FROM information_schema.tables ";
	sql += "WHERE table_schema = '" + database + "' AND table_type = 'BASE TABLE';";
	return sql;
}

string Queries::selectDBViews(string database) {
	string sql = "SELECT table_name FROM information_schema.tables ";
	sql += "WHERE table_schema = '" + database + "' AND table_type = 'VIEW';";
	return sql;
}

string Queries::selectFKconstraints(string database) {
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
	return sql;
}

vector<string>& Queries::selectDBTables(sql::Connection* con, string database) {
	string sql = "SELECT table_name FROM information_schema.tables ";
	sql += "WHERE table_schema = '" + database + "' AND table_type = 'BASE TABLE';";
	sql::Statement* stmt = con->createStatement();
	sql::ResultSet* res = stmt->executeQuery(sql);
	vector<string>* result = new vector<string>();
	while (res->next()) {
		result->push_back(res->getString(1));
	}
	return *result;
}

vector<string>& Queries::selectDBViews(sql::Connection* con, string database) {
	string sql = "SELECT table_name FROM information_schema.tables ";
	sql += "WHERE table_schema = '" + database + "' AND table_type = 'VIEW';";
	sql::Statement* stmt = con->createStatement();
	sql::ResultSet* res = stmt->executeQuery(sql);
	vector<string>* result = new vector<string>();
	while (res->next()) {
		result->push_back(res->getString(1));
	}
	return *result;
}

vector<vector<string>>& Queries::selectFKconstraints(sql::Connection* con, string database) {
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
	vector<vector<string>>* result = new vector<vector<string>>();
	while (res->next()) {
		vector<string> row = vector<string>();
		row.push_back(res->getString(1));
		row.push_back(res->getString(2));
		row.push_back(res->getString(3));
		row.push_back(res->getString(4));
		row.push_back(res->getString(5));
		row.push_back(res->getString(6));
		result->push_back(row);
	}
	return *result;
}
