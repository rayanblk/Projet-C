//
// Created by mathi on 20/01/2018.
//

#ifndef PROJET_C_DATABASE_H
#define PROJET_C_DATABASE_H

#include <libpq-fe.h>

/*
 * All parameters for connection to database
 */
typedef struct DatabaseConnector{
    PGconn * connector;
    char * connectionParam;
    int error;
} DatabaseConnector;

/*
 * We will put the result of query request
 * To obtain the result we will have to use the fetch function
 */
typedef struct QueryStatement{
    PGresult * PGResult;
    int numberOfrow;
    int numberOfcolumn;
    int activeRow;
    int error;
} QueryStatement;


typedef struct Result{
    char *** finalData;
    int row;
    int column;
} Result;

DatabaseConnector * connectToDatabase(char * connectionParam);

void endConnectionToDatabase(DatabaseConnector * databaseConnector);

QueryStatement * query(DatabaseConnector * databaseConnector, char * statement);

void closeQuery(QueryStatement * queryData, char *** data);

void fetchResult(QueryStatement * query, char *** dataArray);

void fetchAllResult(QueryStatement * query, char **** dataArray);

#endif //PROJET_C_DATABASE_H
