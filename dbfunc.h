#ifndef DBFUNC_H
#define DBFUNC_H



#include <libpq-fe.h>
#include "xtypes.h"



#define PREPARE_CHECK_SUCCESS(name, retval)                          \
if (PQresultStatus(res) != PGRES_COMMAND_OK)                         \
{                                                                    \
	fprintf(stderr, "[" #name "]: Query preparation failed\xa"); \
	PQclear(res);                                                \
	FINISH_CONN_IFSTARTED                                        \
	return retval;                                               \
}


#define EXEC_PREPARED_CHECK_SUCCESS(name, retval, type)       \
if (PQresultStatus(res) != PGRES_ ## type ## _OK)             \
{                                                             \
	fprintf(stderr,  "[" #name "]: Query failed: %s\xa",  \
		ExecStatusType_to_cstr(PQresultStatus(res))); \
	PQclear(res);                                         \
	FINISH_CONN_IFSTARTED                                 \
	return retval;                                        \
}


#define START_CONN_IFNOT(retval) \
int __new_conn_created = 0;      \
if (!conn)                       \
{                                \
	__new_conn_created = 1;  \
	conn = new_conn();       \
	if (!conn)               \
		return retval;   \
}


#define FINISH_CONN_IFSTARTED \
if (__new_conn_created)       \
	PQfinish(conn);



PGconn *
new_conn();



char const *
ConnStatusType_to_cstr(
	ConnStatusType status
);



char const *
ExecStatusType_to_cstr(
	ExecStatusType status
);



#endif /* DBFUNC_H */
