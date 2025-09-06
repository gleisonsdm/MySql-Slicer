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

using namespace std;

string database;

int main(int argc, char *argv[]
{
	if (argc % 2 != 1) {
		cerr << "Invalid number of parameters!!!";
		return 1;
	}

	for (int pid = 1; pid < argc; pid+=2) {
		if (argv[pid] == "-D" || argv[pid] == "--database") {
			database = argv[(pid + 1)];
		}
	}

	return 0;
}
