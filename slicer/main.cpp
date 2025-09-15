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

#include "process.hpp"

using namespace std;

string srcDB;
string srcHost;
string srcUsr;
string srcPwd;
string srcPort;
string dstDB;
string dstHost;
string dstUsr;
string dstPwd;
string dstPort;
nrows percentage;

int main(int argc, char *argv[])
{
	if (argc != 23) {
		cerr << "Invalid number of parameters!!!";
		return 1;
	}

	nrows = 0;
	for (int pid = 1; pid < argc; pid+=2) {
		if (argv[pid] == "-SD" || argv[pid] == "--source-database") {
			srcDB = argv[(pid + 1)];
		}
		if (argv[pid] == "-SH" || argv[pid] == "--source-host") {
			srcHost = argv[(pid + 1)];
		}
		if (argv[pid] == "-SU" || argv[pid] == "--source-user") {
			srcUsr = argv[(pid + 1)];
		}
		if (argv[pid] == "-SW" || argv[pid] == "--source-password") {
			srcPwd = argv[(pid + 1)];
		}
		if (argv[pid] == "-SP" || argv[pid] == "--source-port") {
			srcPort = argv[(pid + 1)];
		}
		if (argv[pid] == "-DD" || argv[pid] == "--destiny-database") {
			dstDB = argv[(pid + 1)];
		}
		if (argv[pid] == "-DH" || argv[pid] == "--destiny-host") {
			dstHost = argv[(pid + 1)];
		}
		if (argv[pid] == "-DU" || argv[pid] == "--destiny-user") {
			dstUsr = argv[(pid + 1)];
		}
		if (argv[pid] == "-DW" || argv[pid] == "--destiny-password") {
			dstPwd = argv[(pid + 1)];
		}
		if (argv[pid] == "-DP" || argv[pid] == "--destiny-port") {
			dstPort = argv[(pid + 1)];
		}
		if (argv[pid] == "-N" || argv[pid] == "--number-of-rows") {
			nrows = stoi(argv[(pid + 1)]);
		}

	}

	sql::Driver* srcDriver;
	sql::Connection* srcCon;
	sql::Driver* dstDriver;
	sql::Connection* dstCon;

	srcDriver = get_driver_instance();
	srcCon = srcDriver->connect("tcp://" + srcHost + ":" + srcPort, srcUsr, srcPwd);
	srcCon->setSchema(srcDB);
		
	dstDriver = get_driver_instance();
	dstCon = dstDriver->connect("tcp://" + dstHost + ":" + dstPort, dstUsr, dstPwd);
	dstCon->setSchema(dstDB);
	
	Process pc;
	pc.process(srcCon, srcDB, dstCon, dstDB, nrows);

	delete srcCon;
	delete dstCon;
	return 0;
}
