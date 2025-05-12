#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

// Function to connect to the PostgreSQL database
PGconn *connect_db() {
    const char *conninfo = "dbname=ton_nom_de_base_de_donnees user=ton_utilisateur password=ton_mot_de_passe";
    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connexion à la base de données échouée : %s", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }

    return conn;
}

// Function to insert a new user into the database
int insert_user(PGconn *conn, const char *last_name, const char *first_name, const char *nickname, const char *email, const char *password) {
    const char *paramValues[6] = {last_name, first_name, nickname, email, password, NULL};

    const char *sql = "INSERT INTO public.users (last_name, first_name, nickname, email, password) VALUES ($1, $2, $3, $4, $5)";

    PGresult *res = PQexecParams(conn, sql, 5, NULL, paramValues, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Failed to insert : %s", PQerrorMessage(conn));
        PQclear(res);
        return 0;
    }

    PQclear(res);
    return 1;
}
// Function to verify user credentials
int verify_user(PGconn *conn, const char *email, const char *password) {
    const char *paramValues[2] = {email, password};

    const char *sql = "SELECT id FROM public.users WHERE email = $1 AND password = $2";

    PGresult *res = PQexecParams(conn, sql, 2, NULL, paramValues, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Failed to check : %s", PQerrorMessage(conn));
        PQclear(res);
        return 0; 
    }

    int rowCount = PQntuples(res);
    PQclear(res);

    return (rowCount > 0);
}

// Function to close the database connection
void close_db(PGconn *conn) {
    PQfinish(conn);
}
