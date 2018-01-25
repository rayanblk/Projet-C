//
// Created by mathi on 20/01/2018.
//

#ifndef PROJET_C_DATABASE_H
#define PROJET_C_DATABASE_H

#include <libpq-fe.h>
#include <gtk/gtk.h>

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
    int fetchType;
} QueryStatement;

typedef struct PrepareStatement{
    char * query;
    GList * allParam;
    int error;
    DatabaseConnector * connector;
} PrepareStatement;

/*typedef struct Result{
    char *** finalData;
    int row;
    int column;
} Result;*/

DatabaseConnector * connectToDatabase(char * connectionParam);

void endConnectionToDatabase(DatabaseConnector * databaseConnector);

QueryStatement * query(DatabaseConnector * databaseConnector, char * statement);

PrepareStatement * prepareQuery(DatabaseConnector * databaseConnector, char * statement);

QueryStatement * executePrepareStatement(PrepareStatement * prepareStatement);

void bindParam(PrepareStatement * prepareStatement, char * param, int position);

void fetchResult(QueryStatement * query, char *** dataArray);

void fetchAllResult(QueryStatement * query, char **** dataArray);

void closeQuery(QueryStatement * queryData, char *** data);

void closePrepareStatement(PrepareStatement * prepareData, QueryStatement * queryData, char *** finalData);

#endif //PROJET_C_DATABASE_H
