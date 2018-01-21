//
// Created by mathi on 20/01/2018.
//
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "database.h"

DatabaseConnector * connectToDatabase(char * connectionParam){
    PGconn * pgConnector;
    DatabaseConnector * mainParam = NULL;

    mainParam = (DatabaseConnector *) malloc(sizeof(DatabaseConnector));



    if(mainParam != NULL){

        pgConnector = PQconnectdb(connectionParam);

        if (PQstatus(pgConnector) == CONNECTION_BAD) {
            PQfinish(pgConnector);

            mainParam->error = 1;
            mainParam->connector = NULL;
            mainParam->connectionParam = NULL;
        }else{

            mainParam->connector = pgConnector;

            mainParam->error = 0;

            mainParam->connectionParam = (char *) malloc(strlen(connectionParam) *sizeof(char));
            mainParam->connectionParam = connectionParam;
        }
    }


    return mainParam;
}

void endConnectionToDatabase(DatabaseConnector * databaseConnector){

    if(databaseConnector != NULL){
        PQfinish(databaseConnector->connector);

        free(databaseConnector->connectionParam);

        free(databaseConnector);
    }

}

QueryStatement * query(DatabaseConnector * databaseConnector, char * statement){
    PGresult * queryResult;
    QueryStatement * mainQuery = (QueryStatement *) malloc(sizeof(QueryStatement));
    ExecStatusType status;

    if(databaseConnector != NULL){
        queryResult = PQexec(databaseConnector->connector, statement);
        status = PQresultStatus(queryResult);

        if(mainQuery != NULL){
            if(status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK){
                mainQuery->error = 0;
                mainQuery->numberOfcolumn = PQnfields(queryResult);
                mainQuery->numberOfrow = PQntuples(queryResult);
                mainQuery->activeRow = 0;
                mainQuery->PGResult = queryResult;
            }else{
                mainQuery->error = 1;
                mainQuery->numberOfcolumn = 0;
                mainQuery->numberOfrow = 0;
                mainQuery->activeRow = 0;
                mainQuery->PGResult = queryResult;
            }
        }
    }

    return mainQuery;
}

void closeQuery(QueryStatement * queryData, char *** finalData){

    if(queryData != NULL){
        PQclear(queryData->PGResult);
        free(queryData);
    }

    if(finalData != NULL){
        free(*finalData);
        free(finalData);
    }
}

void fetchResult(QueryStatement * query, char *** dataArray){

    if(query != NULL){
        if(*dataArray != NULL){
            free(*dataArray);
        }

        *dataArray = NULL;

        if(query->activeRow < query->numberOfrow){

            *dataArray = (char **) malloc(query->numberOfcolumn * sizeof(char *));

            if(*dataArray != NULL){
                for (int j = 0; j < query->numberOfcolumn; ++j) {
                    (*dataArray)[j] = PQgetvalue(query->PGResult, query->activeRow, j);
                }

                query->activeRow += 1;
            }

        }

    }
}

void fetchAllResult(QueryStatement * query, char **** dataArray){

    if(query != NULL){
        if(*dataArray != NULL){
            free(*dataArray);
        }

        *dataArray = NULL;

        if(query->activeRow < query->numberOfrow){

            *dataArray = (char ***) malloc(query->numberOfrow * sizeof(char **));

            for (int i = 0; i < query->numberOfrow; ++i) {
                (*dataArray)[i] = (char **) malloc(query->numberOfcolumn * sizeof(char *));

                for (int j = 0; j < query->numberOfcolumn; ++j) {
                    (*dataArray)[i][j] = PQgetvalue(query->PGResult, i, j);
                }
            }

        }

    }
}
