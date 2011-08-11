/*
 * Note: this file originally auto-generated by mib2c using
 *  : mib2c.raw-table.conf 17436 2009-03-31 15:12:19Z dts12 $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "ieee8021QBridgeVlanCurrentTable.h"


/* column number definitions for table ieee8021QBridgeVlanCurrentTable */
#define COLUMN_IEEE8021QBRIDGEVLANTIMEMARK		1
#define COLUMN_IEEE8021QBRIDGEVLANCURRENTCOMPONENTID		2
#define COLUMN_IEEE8021QBRIDGEVLANINDEX		3
#define COLUMN_IEEE8021QBRIDGEVLANFDBID		4
#define COLUMN_IEEE8021QBRIDGEVLANCURRENTEGRESSPORTS		5
#define COLUMN_IEEE8021QBRIDGEVLANCURRENTUNTAGGEDPORTS		6
#define COLUMN_IEEE8021QBRIDGEVLANSTATUS		7
#define COLUMN_IEEE8021QBRIDGEVLANCREATIONTIME		8

    /* Typical data structure for a row entry */
struct ieee8021QBridgeVlanCurrentTable_entry {
    /* Index values */
    u_long ieee8021QBridgeVlanTimeMark;
    u_long ieee8021QBridgeVlanCurrentComponentId;
    u_long ieee8021QBridgeVlanIndex;

    /* Column values */
    u_long ieee8021QBridgeVlanFdbId;
    char ieee8021QBridgeVlanCurrentEgressPorts[NNN];
    size_t ieee8021QBridgeVlanCurrentEgressPorts_len;
    char ieee8021QBridgeVlanCurrentUntaggedPorts[NNN];
    size_t ieee8021QBridgeVlanCurrentUntaggedPorts_len;
    long ieee8021QBridgeVlanStatus;
    u_long ieee8021QBridgeVlanCreationTime;

    int   valid;
};

/* create a new row in the table */
static struct ieee8021QBridgeVlanCurrentTable_entry *
ieee8021QBridgeVlanCurrentTable_createEntry( int dummy
                 , u_long  ieee8021QBridgeVlanTimeMark
                 , u_long  ieee8021QBridgeVlanCurrentComponentId
                 , u_long  ieee8021QBridgeVlanIndex
                ) {
    struct ieee8021QBridgeVlanCurrentTable_entry *entry;

    entry = SNMP_MALLOC_TYPEDEF(struct ieee8021QBridgeVlanCurrentTable_entry);
    if (!entry)
        return NULL;

    /* XXX - insert entry into local data structure */
    return entry;
}

/* remove a row from the table */
static void
ieee8021QBridgeVlanCurrentTable_removeEntry(struct ieee8021QBridgeVlanCurrentTable_entry *entry) {
    if (!entry)
        return;    /* Nothing to remove */

    /* XXX - remove entry from local data structure */

    if (entry)
        SNMP_FREE( entry );   /* XXX - release any other internal resources */
}


/** determine the appropriate row for an exact request */
static struct ieee8021QBridgeVlanCurrentTable_entry *
ieee8021QBridgeVlanCurrentTable_get_entry( netsnmp_variable_list *indexes ) {
    struct ieee8021QBridgeVlanCurrentTable_entry *row = NULL;

    /* XXX - Use the 'indexes' parameter to retrieve the data
       structure for the requested row, and return this. */
    return row;
}

/** determine the appropriate row for an fuzzy request */
static struct ieee8021QBridgeVlanCurrentTable_entry *
ieee8021QBridgeVlanCurrentTable_get_next_entry( netsnmp_handler_registration *reginfo,
                     netsnmp_request_info         *request,
                     int                           column,
                     netsnmp_variable_list        *indexes ) {
    struct ieee8021QBridgeVlanCurrentTable_entry *row = NULL;
    oid             build_space[MAX_OID_LEN];
    size_t          build_space_len = 0;
    size_t          index_oid_len = 0;

    /* XXX - Use the 'indexes' parameter to identify the
             next row in the table.... */

    /* XXX   .... update the 'indexes' parameter with the
             appropriate index values ... */

    /* ... and update the requested OID to match this instance */
    memcpy(build_space, reginfo->rootoid,   /* registered oid */
                        reginfo->rootoid_len * sizeof(oid));
    build_space_len = reginfo->rootoid_len;
    build_space[build_space_len++] = 1;  /* entry */
    build_space[build_space_len++] = column; /* column */
    build_oid_noalloc(build_space + build_space_len,
                      MAX_OID_LEN - build_space_len, &index_oid_len,
                      NULL, 0, indexes);
    snmp_set_var_objid(request->requestvb, build_space,
                       build_space_len + index_oid_len);

    /*  Finally, return the data structure for this row */
    return row;
}


/** handles requests for the ieee8021QBridgeVlanCurrentTable table */
static int
ieee8021QBridgeVlanCurrentTable_handler(
    netsnmp_mib_handler               *handler,
    netsnmp_handler_registration      *reginfo,
    netsnmp_agent_request_info        *reqinfo,
    netsnmp_request_info              *requests) {

    netsnmp_request_info       *request;
    netsnmp_table_request_info *table_info;
    struct ieee8021QBridgeVlanCurrentTable_entry          *table_entry;

    switch (reqinfo->mode) {
        /*
         * Read-support
         */
    case MODE_GET:
        for (request=requests; request; request=request->next) {
            table_info  =     netsnmp_extract_table_info( request);
            table_entry =     ieee8021QBridgeVlanCurrentTable_get_entry( table_info->indexes );
    
            switch (table_info->colnum) {
            case COLUMN_IEEE8021QBRIDGEVLANFDBID:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_UNSIGNED,
                                            table_entry->ieee8021QBridgeVlanFdbId);
                break;
            case COLUMN_IEEE8021QBRIDGEVLANCURRENTEGRESSPORTS:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_value( request->requestvb, ASN_OCTET_STR,
                                          table_entry->ieee8021QBridgeVlanCurrentEgressPorts,
                                          table_entry->ieee8021QBridgeVlanCurrentEgressPorts_len);
                break;
            case COLUMN_IEEE8021QBRIDGEVLANCURRENTUNTAGGEDPORTS:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_value( request->requestvb, ASN_OCTET_STR,
                                          table_entry->ieee8021QBridgeVlanCurrentUntaggedPorts,
                                          table_entry->ieee8021QBridgeVlanCurrentUntaggedPorts_len);
                break;
            case COLUMN_IEEE8021QBRIDGEVLANSTATUS:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->ieee8021QBridgeVlanStatus);
                break;
            case COLUMN_IEEE8021QBRIDGEVLANCREATIONTIME:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_TIMETICKS,
                                            table_entry->ieee8021QBridgeVlanCreationTime);
                break;
            default:
                netsnmp_set_request_error(reqinfo, request,
                                          SNMP_NOSUCHOBJECT);
                break;
            }
        }
        break;

    case MODE_GETNEXT:
        for (request=requests; request; request=request->next) {
            table_info  =     netsnmp_extract_table_info( request);
            table_entry =     ieee8021QBridgeVlanCurrentTable_get_next_entry( reginfo, request,
                                   table_info->colnum, table_info->indexes );
    
            switch (table_info->colnum) {
            case COLUMN_IEEE8021QBRIDGEVLANFDBID:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_UNSIGNED,
                                            table_entry->ieee8021QBridgeVlanFdbId);
                break;
            case COLUMN_IEEE8021QBRIDGEVLANCURRENTEGRESSPORTS:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_value( request->requestvb, ASN_OCTET_STR,
                                          table_entry->ieee8021QBridgeVlanCurrentEgressPorts,
                                          table_entry->ieee8021QBridgeVlanCurrentEgressPorts_len);
                break;
            case COLUMN_IEEE8021QBRIDGEVLANCURRENTUNTAGGEDPORTS:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_value( request->requestvb, ASN_OCTET_STR,
                                          table_entry->ieee8021QBridgeVlanCurrentUntaggedPorts,
                                          table_entry->ieee8021QBridgeVlanCurrentUntaggedPorts_len);
                break;
            case COLUMN_IEEE8021QBRIDGEVLANSTATUS:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->ieee8021QBridgeVlanStatus);
                break;
            case COLUMN_IEEE8021QBRIDGEVLANCREATIONTIME:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_TIMETICKS,
                                            table_entry->ieee8021QBridgeVlanCreationTime);
                break;
            default:
                netsnmp_set_request_error(reqinfo, request,
                                          SNMP_NOSUCHOBJECT);
                break;
            }
        }
        break;

    }
    return SNMP_ERR_NOERROR;
}

/** Initialize the ieee8021QBridgeVlanCurrentTable table by defining its contents and how it's structured */
static void
initialize_table_ieee8021QBridgeVlanCurrentTable(void)
{
    const oid ieee8021QBridgeVlanCurrentTable_oid[] = {1,3,111,2,802,1,1,4,1,4,2};
    netsnmp_handler_registration    *reg;
    netsnmp_table_registration_info *table_info;

    reg = netsnmp_create_handler_registration(
              "ieee8021QBridgeVlanCurrentTable",     ieee8021QBridgeVlanCurrentTable_handler,
              ieee8021QBridgeVlanCurrentTable_oid, OID_LENGTH(ieee8021QBridgeVlanCurrentTable_oid),
              HANDLER_CAN_RONLY
              );

    table_info = SNMP_MALLOC_TYPEDEF( netsnmp_table_registration_info );
    netsnmp_table_helper_add_indexes(table_info,
                           ASN_TIMETICKS,  /* index: ieee8021QBridgeVlanTimeMark */
                           ASN_UNSIGNED,  /* index: ieee8021QBridgeVlanCurrentComponentId */
                           ASN_UNSIGNED,  /* index: ieee8021QBridgeVlanIndex */
                           0);

    table_info->min_column = COLUMN_IEEE8021QBRIDGEVLANFDBID;
    table_info->max_column = COLUMN_IEEE8021QBRIDGEVLANCREATIONTIME;

    netsnmp_register_table( reg, table_info );

    /* Initialise the contents of the table here */
}

/** Initializes the ieee8021QBridgeVlanCurrentTable module */
void
init_ieee8021QBridgeVlanCurrentTable(void)
{
  /* here we initialize all the tables we're planning on supporting */
    initialize_table_ieee8021QBridgeVlanCurrentTable();
}
