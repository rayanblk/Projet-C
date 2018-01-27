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
                mainQuery->fetchType = -1;
            }else{
                //printf("%s \n", PQresultErrorMessage(queryResult));
                mainQuery->error = 1;
                mainQuery->numberOfcolumn = 0;
                mainQuery->numberOfrow = 0;
                mainQuery->activeRow = 0;
                mainQuery->PGResult = queryResult;
                mainQuery->fetchType = -1;
            }
        }
    }

    return mainQuery;
}

PrepareStatement * prepareQuery(DatabaseConnector * databaseConnector, char * statement){
    PrepareStatement * statementToPrepare = (PrepareStatement *) malloc(sizeof(PrepareStatement));

    if(statementToPrepare != NULL){
        statementToPrepare->error = 0;
        statementToPrepare->query = statement;
        statementToPrepare->connector = databaseConnector;
        statementToPrepare->allParam = NULL;
    }

    return statementToPrepare;
}

void bindParam(PrepareStatement * prepareStatement, char * param, int position){
    if(prepareStatement->allParam == NULL){
        prepareStatement->allParam = g_list_prepend(prepareStatement->allParam, (gpointer) param);
    }else if(g_list_nth(prepareStatement->allParam, (guint) position + 1) == NULL){
        prepareStatement->allParam = g_list_append(prepareStatement->allParam, (gpointer) param);
    }else{
        prepareStatement->allParam = g_list_insert_before(prepareStatement->allParam, g_list_nth(prepareStatement->allParam, (guint) position +1), (gpointer) param);
    }

}

QueryStatement * executePrepareStatement(PrepareStatement * prepareStatement){

    PGresult * queryResult;
    QueryStatement * mainQuery = (QueryStatement *) malloc(sizeof(QueryStatement));
    ExecStatusType status;
    char ** allValue = NULL;
    int j = 0;

    if(prepareStatement != NULL){
        if(g_list_length(prepareStatement->allParam) > 0){
            //prepareStatement->allParam = g_list_reverse(prepareStatement->allParam);
            allValue = (char **) malloc(sizeof(char *) * g_list_length(prepareStatement->allParam));
            
            if(allValue != NULL){
                for (GList * i = prepareStatement->allParam; i != NULL; i = i->next, j++) {
                    allValue[j] = (char *) i->data;
                }
            }
        }

        printf("%s \n", prepareStatement->query);
        
        queryResult = PQexecParams(prepareStatement->connector->connector
                , prepareStatement->query
                , g_list_length(prepareStatement->allParam)
                , NULL
                , (const char * const *) allValue
                , NULL
                , NULL
                , 0);

        status = PQresultStatus(queryResult);

        if(mainQuery != NULL){
            if(status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK){
                mainQuery->error = 0;
                mainQuery->numberOfcolumn = PQnfields(queryResult);
                mainQuery->numberOfrow = PQntuples(queryResult);
                mainQuery->activeRow = 0;
                mainQuery->PGResult = queryResult;
            }else{
                printf("%s \n", PQresultErrorMessage(queryResult));
                mainQuery->error = 1;
                mainQuery->numberOfcolumn = 0;
                mainQuery->numberOfrow = 0;
                mainQuery->activeRow = 0;
                mainQuery->PGResult = queryResult;
            }
        }
    }

    free(allValue);

    return mainQuery;
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

        query->fetchType = 0;

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

        query->fetchType = 1;

    }
}

void closeQuery(QueryStatement * queryData, char *** finalData){

    if(finalData != NULL){
        if(queryData->fetchType == 0){
            /*
             * Double pointer
             */
            free(finalData);
        }else if(queryData->fetchType == 1){
            /*
             * Triple pointer
             */
            for(int i = 0; i < queryData->numberOfrow && i < queryData->activeRow; i++){
                free((finalData)[i]);
            }
            free(finalData);
        }
    }

    if(queryData != NULL){
        PQclear(queryData->PGResult);
        free(queryData);
    }
}

void closePrepareStatement(PrepareStatement * prepareData, QueryStatement * queryData, char *** finalData){

    if(finalData != NULL){
        if(queryData->fetchType == 0){
            /*
             * Double pointer
             */
            free(finalData);
        }else if(queryData->fetchType == 1){
            /*
             * Triple pointer
             */
            for(int i = 0; i < queryData->numberOfrow && i < queryData->activeRow; i++){
                free((finalData)[i]);
            }
            free(finalData);
        }
    }

    if(queryData != NULL){
        PQclear(queryData->PGResult);
        free(queryData);
    }

    if(prepareData != NULL){
        free(prepareData);
    }
}
