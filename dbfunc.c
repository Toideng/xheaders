#include <libpq-fe.h>

#include "dbfunc.h"










PGconn *
new_conn(
	char *dbname,
	char *username
	)
{
	char const *keywords[] =
	{
		"dbname",
		"user",
		0,
	};
	char const *values[] =
	{
		dbname,
		username,
		0,
	};
	PGconn *conn = PQconnectdbParams(keywords, values, 0);
	if (!conn)
	{
		fprintf(stderr, "Error: [PQconnectdbParams] returned 0\xa");
		return 0;
	}
	ConnStatusType status = PQstatus(conn);
	if (status != CONNECTION_OK)
	{
		fprintf(stderr, "DB connection failed (status: [%d]: \"%s\")\xa",
			status,
			ConnStatusType_to_cstr(status));
		PQfinish(conn);
		return 0;
	}
	return conn;
}










char const *
ConnStatusType_to_cstr(
	ConnStatusType status
	)
{
	switch (status)
	{
#define XX(x)\
	case x:\
		return #x ;

XX(CONNECTION_OK)
XX(CONNECTION_BAD)
XX(CONNECTION_STARTED)
XX(CONNECTION_MADE)
XX(CONNECTION_AWAITING_RESPONSE)
XX(CONNECTION_AUTH_OK)
XX(CONNECTION_SETENV)
XX(CONNECTION_SSL_STARTUP)
XX(CONNECTION_NEEDED)
XX(CONNECTION_CHECK_WRITABLE)
XX(CONNECTION_CONSUME)
XX(CONNECTION_GSS_STARTUP)
#undef XX
	}

	return "<unknown status>";
}










char const *
ExecStatusType_to_cstr(
	ExecStatusType status
	)
{
	switch (status)
	{
#define XX(x)\
	case x:\
		return #x ;

XX(PGRES_EMPTY_QUERY)
XX(PGRES_COMMAND_OK)
XX(PGRES_TUPLES_OK)
XX(PGRES_COPY_OUT)
XX(PGRES_COPY_IN)
XX(PGRES_BAD_RESPONSE)
XX(PGRES_NONFATAL_ERROR)
XX(PGRES_FATAL_ERROR)
XX(PGRES_COPY_BOTH)
XX(PGRES_SINGLE_TUPLE)
#undef XX
	}

	return "<unknown status>";
}
