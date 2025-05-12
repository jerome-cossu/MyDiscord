#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

// Fonction pour se connecter à la base de données
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

// Fonction pour insérer un nouvel utilisateur
int insert_user(PGconn *conn, const char *last_name, const char *first_name, const char *nickname, const char *email, const char *password) {
    const char *paramValues[6] = {last_name, first_name, nickname, email, password, NULL};

    // Requête SQL pour insérer un utilisateur
    const char *sql = "INSERT INTO public.users (last_name, first_name, nickname, email, password) VALUES ($1, $2, $3, $4, $5)";

    // Exécute la requête
    PGresult *res = PQexecParams(conn, sql, 5, NULL, paramValues, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Erreur d'insertion : %s", PQerrorMessage(conn));
        PQclear(res);
        return 0; // Échec
    }

    PQclear(res);
    return 1; // Succès
}

// Fonction pour vérifier si un utilisateur existe avec un email et mot de passe
int verify_user(PGconn *conn, const char *email, const char *password) {
    const char *paramValues[2] = {email, password};

    // Requête SQL pour vérifier les identifiants
    const char *sql = "SELECT id FROM public.users WHERE email = $1 AND password = $2";

    PGresult *res = PQexecParams(conn, sql, 2, NULL, paramValues, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Erreur de vérification : %s", PQerrorMessage(conn));
        PQclear(res);
        return 0; // Échec
    }

    int rowCount = PQntuples(res);
    PQclear(res);

    return (rowCount > 0); // Retourne 1 si un utilisateur est trouvé, sinon 0
}

// Fonction pour fermer la connexion à la base de données
void close_db(PGconn *conn) {
    PQfinish(conn);
}
