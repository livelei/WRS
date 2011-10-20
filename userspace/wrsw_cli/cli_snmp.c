/*
 * White Rabbit Switch CLI (Command Line Interface)
 * Copyright (C) 2011, CERN.
 *
 * Authors:     Juan Luis Manas (juan.manas@integrasys.es)
 *
 * Description: SNMP utility functions to simplify handling SNMP stuff from CLI
 *              modules.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <string.h>
#include <errno.h>

#include "cli_snmp.h"

#define LOCALHOST "127.0.0.1"

static netsnmp_session session, *ss;


/**
 * Initializes SNMP operational parameters at the client side.
 * @param username authentication string associated to the principal.
 * @param password must be at least 8 characters long.
 * @return 0 if snmp session was opened. -1 in case of error.
 */
int cli_snmp_init(char *username, char *password)
{
    init_snmp("cli");

    snmp_sess_init( &session );

    session.peername = strdup(LOCALHOST);
    session.version=SNMP_VERSION_3;
    session.securityName = username;
    session.securityNameLen = strlen(session.securityName);
    // the security level is authenticated, but not encrypted,
    // since local access does not require it
    session.securityLevel = SNMP_SEC_LEVEL_AUTHNOPRIV;
    session.securityAuthProto = usmHMACMD5AuthProtocol;
    session.securityAuthProtoLen = sizeof(usmHMACMD5AuthProtocol)/sizeof(oid);
    session.securityAuthKeyLen = USM_AUTH_KU_LEN;

    if (generate_Ku(session.securityAuthProto,
                    session.securityAuthProtoLen,
                    (u_char *) password, strlen(password),
                    session.securityAuthKey,
                    &session.securityAuthKeyLen) != SNMPERR_SUCCESS) {
        snmp_log(LOG_ERR, "Error generating Ku from password.\n");
        return -1;
    }

    ss = snmp_open(&session);
    if (!ss) {
        snmp_sess_perror("cli", &session);
        return -1;
    }

    return 0;
}

/**
 * The callee is responsible for freeing the PDU response returned from
 * this method
 */
static netsnmp_pdu *cli_snmp_get(oid _oid[MAX_OID_LEN], size_t oid_len)
{
    netsnmp_pdu *pdu;
    netsnmp_pdu *response;
    int status;

    pdu = snmp_pdu_create(SNMP_MSG_GET);
    snmp_add_null_var(pdu, _oid, oid_len);

    status = snmp_synch_response(ss, pdu, &response);

    if (status != STAT_SUCCESS)
        errno = status;
    else
        if (response)
           errno = response->errstat;

    return response;
}

int cli_snmp_int(netsnmp_variable_list *vars)
{
    if (vars) {
        switch(vars->type) {
        case ASN_TIMETICKS:
        case ASN_GAUGE:
        case ASN_COUNTER:
        case ASN_INTEGER:
            return *(vars->val).integer;
        default:
            errno = -EINVAL;
        }
    }
    return 0;
}

char *cli_snmp_string(netsnmp_variable_list *vars)
{
    char *sp;

    if (vars) {
        switch(vars->type) {
        case ASN_OCTET_STR:
            sp = (char *)malloc(1 + vars->val_len);
            memcpy(sp, vars->val.string, vars->val_len);
            sp[vars->val_len] = '\0';
            return sp;
        default:
            errno = -EINVAL;
	    }
    }
    return NULL;
}

int cli_snmp_get_int(oid _oid[MAX_OID_LEN], size_t oid_len)
{
    netsnmp_pdu *response;
    int err = 0, value = 0;

    response = cli_snmp_get(_oid, oid_len);
    if (response) {
        value = cli_snmp_int(response->variables);
        err = errno;
        snmp_free_pdu(response);
    }
    errno = err;
    return value;
}

/**
 * The callee is responsible for freeing memory for returned value.
 */
char *cli_snmp_get_string(oid _oid[MAX_OID_LEN], size_t oid_len)
{
    netsnmp_pdu *response;
    char *value = NULL;
    int err = 0;

    response = cli_snmp_get(_oid, oid_len);
    if (response) {
        value = cli_snmp_string(response->variables);
        err = errno;
        snmp_free_pdu(response);
	}
	errno = err;
	return value;
}

/**
 * The callee is responsible for freeing the PDU response returned from
 * this method
 */
netsnmp_pdu *cli_snmp_getnext(oid _oid[MAX_OID_LEN], size_t *oid_len)
{
    netsnmp_pdu *pdu;
    netsnmp_pdu *response;
    int status;

    pdu = snmp_pdu_create(SNMP_MSG_GETBULK);
    pdu->max_repetitions = 1;
    pdu->non_repeaters = 0;
    snmp_add_null_var(pdu, _oid, *oid_len);

    status = snmp_synch_response(ss, pdu, &response);

    if (status != STAT_SUCCESS)
        errno = status;
    else
        if (response)
           errno = response->errstat;

    return response;
}


int cli_snmp_getnext_int(oid _oid[MAX_OID_LEN], size_t *oid_len)
{
    netsnmp_pdu *response;
    int err = 0, value = 0;
    netsnmp_variable_list *vars;

    response = cli_snmp_getnext(_oid, oid_len);
    if (response) {
        vars = response->variables;
        memcpy(_oid, vars->name, vars->name_length * sizeof(oid));
        *oid_len = vars->name_length;
        value = cli_snmp_int(vars);
        err = errno;
        snmp_free_pdu(response);
    }
    errno = err;
    return value;
}

char *cli_snmp_getnext_string(oid _oid[MAX_OID_LEN], size_t *oid_len)
{
    netsnmp_variable_list *vars;
    netsnmp_pdu *response;
    char *value = NULL;
    int err = 0;

    response = cli_snmp_getnext(_oid, oid_len);
    if (response) {
        vars = response->variables;
        memcpy(_oid, vars->name, vars->name_length * sizeof(oid));
        *oid_len = vars->name_length;
        value = cli_snmp_string(response->variables);
        err = errno;
        snmp_free_pdu(response);
    }
    errno = err;
    return value;
}

void cli_snmp_set_int(oid _oid[MAX_OID_LEN], size_t oid_len, char *val, char type)
{
    netsnmp_pdu *pdu;
    netsnmp_pdu *response;
    int err = 0, status;

    pdu = snmp_pdu_create(SNMP_MSG_SET);

    if (snmp_add_var(pdu, _oid, oid_len, type, val)) {
        errno = -EINVAL;
        return;
    }

    status = snmp_synch_response(ss, pdu, &response);

    if (status != STAT_SUCCESS)
        err = status;
    else
        if (response)
           err = response->errstat;

    if (response)
        snmp_free_pdu(response);

    errno = err;
}

void cli_snmp_set_str(oid _oid[MAX_OID_LEN], size_t oid_len, char *val)
{
    netsnmp_pdu *pdu;
    netsnmp_pdu *response;
    int err = 0, status;
    char type;

    type = 's';
    pdu = snmp_pdu_create(SNMP_MSG_GET);
    if (snmp_add_var(pdu, _oid, oid_len, type, val)) {
        errno = -EINVAL;
        return;
    }

    status = snmp_synch_response(ss, pdu, &response);

    if (status != STAT_SUCCESS)
        err = status;
    else
        if (response)
           err = response->errstat;

    if (response)
        snmp_free_pdu(response);

    errno = err;
}

void cli_snmp_set(oid _oid[][MAX_OID_LEN],
                  size_t oid_len[],
                  char *val[],
                  char type[],
                  int nvars)
{
    netsnmp_pdu *pdu;
    netsnmp_pdu *response = NULL;
    int err = 0, status, i;

    pdu = snmp_pdu_create(SNMP_MSG_SET);

    int ret = 0;
    for (i = 0; i < nvars; i++) {
        if ((ret = snmp_add_var(pdu, _oid[i], oid_len[i], type[i], val[i]))) {
            errno = ret;
            return;
        }
    }

    status = snmp_synch_response(ss, pdu, &response);

    if (status != STAT_SUCCESS)
        err = status;
    else
        if (response)
           err = response->errstat;

    if (response)
        snmp_free_pdu(response);

    errno = err;
}


/**
 * Closes SNMP session.
 */
void cli_snmp_close()
{
    snmp_close(ss);
}
