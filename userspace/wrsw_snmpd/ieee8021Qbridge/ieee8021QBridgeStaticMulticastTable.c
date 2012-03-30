/*
 * White Rabbit Switch Management
 * Copyright (C) 2010, CERN.
 *
 * Version:     wrsw_snmpd v1.0
 *
 * Authors:     Juan Luis Manas (juan.manas@integrasys.es)
 *
 * Description: Handles requests for ieee8021QBridgeStaticMulticastTable table.
 *              Provides support for inserting/deleting static multicast
 *              and broadcast entries into the RTU filtering database.
 *              Note: this file originally auto-generated by mib2c using
 *              : mib2c.raw-table.conf 17436 2009-03-31 15:12:19Z dts12 $
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


#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include "ieee8021QBridgeStaticMulticastTable.h"
#include "rtu_fd_proxy.h"
#include "mac.h"
#include "utils.h"

#define MIBMOD  "8021Q"
#define ALL_PORTS 0

/* column number definitions for table ieee8021QBridgeStaticMulticastTable */
#define COLUMN_IEEE8021QBRIDGESTATICMULTICASTADDRESS                1
#define COLUMN_IEEE8021QBRIDGESTATICMULTICASTRECEIVEPORT            2
#define COLUMN_IEEE8021QBRIDGESTATICMULTICASTSTATICEGRESSPORTS      3
#define COLUMN_IEEE8021QBRIDGESTATICMULTICASTFORBIDDENEGRESSPORTS   4
#define COLUMN_IEEE8021QBRIDGESTATICMULTICASTSTORAGETYPE            5
#define COLUMN_IEEE8021QBRIDGESTATICMULTICASTROWSTATUS              6

// Row entry
struct static_multicast_table_entry {
    // indexes
    u_long   cid;
    uint16_t vid;
    uint8_t  mac[ETH_ALEN];
    u_long   rx_port;

    // Columns
    char egress_ports[NUM_PORTS];
    char forbidden_ports[NUM_PORTS];
    int  type;
    int  row_status;

    // Aux fields
    netsnmp_request_info    *req;                   // associated SNMP request
    struct static_multicast_table_entry *next;
};

/**
 * Stores information related to SET actions that must be handled atomically
 */
static struct static_multicast_table_entry *cache = NULL;

/**
 * Create a new row in the cache table
 */
static struct static_multicast_table_entry *cache_create(
    u_long  cid,
    u_long  vid,
    char    *mac,
    u_long  rx_port)
{
    struct static_multicast_table_entry *ent;

    ent = SNMP_MALLOC_TYPEDEF(struct static_multicast_table_entry);
    if (!ent)
        return NULL;

    ent->cid        = cid;
    ent->vid        = vid;
    ent->rx_port    = rx_port;
    ent->type       = ST_OTHER;
    ent->row_status = RS_NOTINSERVICE;

    mac_copy(ent->mac, mac);

    memset(ent->egress_ports,    0, NUM_PORTS);
    memset(ent->forbidden_ports, 0, NUM_PORTS);

    // Insert into cache
    ent->next = cache;
    cache     = ent;
    return ent;
}

/**
 * Determine the appropriate row for an exact request
 */
static struct static_multicast_table_entry *cache_get(
    struct static_multicast_table_entry *idx)
{
    struct static_multicast_table_entry *ent;

    for (ent = cache; ent; ent = ent->next)
        if ((ent->vid == idx->vid) && mac_equal(ent->mac, idx->mac))
            return ent;
    return NULL;
}

/**
 * Remove all entries from cache.
 */
static void cache_clean()
{
    struct static_multicast_table_entry *ptr;

    while(cache) {
        ptr = cache;
        cache = cache->next;
        SNMP_FREE(ptr);
    }
}

/**
 * Get indexes for an entry.
 * @param tinfo table information that contains the indexes (in raw format)
 * @param ent (OUT) used to return the retrieved indexes
 */
static int get_indexes(netsnmp_request_info           *req,
                       netsnmp_handler_registration   *reginfo,
                       netsnmp_table_request_info     *tinfo,
                       struct static_multicast_table_entry *ent)
{
    int oid_len, rootoid_len;
    netsnmp_variable_list *idx;

    // Get indexes from request - in case OID contains them!.
    // Otherwise use default values for first row
    oid_len     = req->requestvb->name_length;
    rootoid_len = reginfo->rootoid_len;

    if (oid_len > rootoid_len) {
        if (!tinfo || !tinfo->indexes)
            return SNMP_ERR_GENERR; // protects from effects of malformed PDUs
        idx = tinfo->indexes;
        ent->cid = *idx->val.integer;
    } else
        ent->cid = 0;

    if (oid_len > rootoid_len + 1) {
        idx = idx->next_variable;
        ent->vid = *idx->val.integer;
    } else
        ent->vid = 0;

    if (oid_len > rootoid_len + 2) {
        idx = idx->next_variable;
        memcpy(ent->mac, idx->val.string, idx->val_len);
    } else {
        mac_copy(ent->mac, (uint8_t*)DEFAULT_MAC);
        ent->mac[0] = 0x01; // to refer to multicast addresses when searching
    }

    if (oid_len > rootoid_len + 3) {
        idx = idx->next_variable;
        ent->rx_port = *idx->val.integer;
    } else
        ent->rx_port = ALL_PORTS;

    return SNMP_ERR_NOERROR;
}

/**
 * @param ent should contain appropriate entry indexes
 */
static int get_column(netsnmp_variable_list               *vb,
                      int                                 colnum,
                      struct static_multicast_table_entry *ent)
{
    switch (colnum) {
    case COLUMN_IEEE8021QBRIDGESTATICMULTICASTSTATICEGRESSPORTS:
        snmp_set_var_typed_value(vb, ASN_OCTET_STR,
            ent->egress_ports, NUM_PORTS);
        break;
    case COLUMN_IEEE8021QBRIDGESTATICMULTICASTFORBIDDENEGRESSPORTS:
        snmp_set_var_typed_value(vb, ASN_OCTET_STR,
            ent->forbidden_ports, NUM_PORTS);
        break;
    case COLUMN_IEEE8021QBRIDGESTATICMULTICASTSTORAGETYPE:
        snmp_set_var_typed_integer(vb, ASN_INTEGER, ent->type);
        break;
    case COLUMN_IEEE8021QBRIDGESTATICMULTICASTROWSTATUS:
        snmp_set_var_typed_integer(vb, ASN_INTEGER,
            ent->row_status ? RS_ACTIVE:RS_NOTINSERVICE);
        break;
    default:
        return SNMP_NOSUCHOBJECT;
    }
    return SNMP_ERR_NOERROR;
}

static int get(netsnmp_request_info *req, netsnmp_handler_registration *reginfo)
{
    int err;
    uint32_t ep;    // egress ports
    uint32_t fp;    // forbidden ports
    struct static_multicast_table_entry ent;
    netsnmp_table_request_info *tinfo = netsnmp_extract_table_info(req);

    // Get indexes for entry
    err = get_indexes(req, reginfo, tinfo, &ent);
    if (err)
        return err;

    DEBUGMSGTL((MIBMOD, "cid=%lu vid=%d mac=%s rx_port=%d column=%d\n",
        ent.cid, ent.vid, mac_to_str(ent.mac), ent.rx_port, tinfo->colnum));

    if ((ent.cid != DEFAULT_COMPONENT_ID) ||
        (ent.vid >= NUM_VLANS) ||
        (ent.rx_port != ALL_PORTS))
        return SNMP_NOSUCHINSTANCE;

    // Read entry from RTU FDB.
    errno = 0;
    err = rtu_fdb_proxy_read_static_entry(ent.mac, ent.vid, &ep, &fp, &ent.type,
                                          &ent.row_status);
    if (errno)
        goto minipc_err;
    if (err)
        goto entry_not_found;

    to_octetstr(ep, ent.egress_ports);
    to_octetstr(fp, ent.forbidden_ports);

    // Get column value
    return get_column(req->requestvb, tinfo->colnum, &ent);

entry_not_found:
    DEBUGMSGTL((MIBMOD, "entry vid=%d mac=%s not found in fdb\n",
        ent.vid, mac_to_str(ent.mac)));
    return SNMP_NOSUCHINSTANCE;

minipc_err:
    snmp_log(LOG_ERR, "%s(%s): mini-ipc error [%s]\n", __FILE__, __func__,
        strerror(errno));
    return SNMP_ERR_GENERR;
}

static int get_next(netsnmp_request_info *req,
                    netsnmp_handler_registration *reginfo,
                    netsnmp_table_request_info *tinfo)
{
    int err;
    uint32_t ep;    // egress ports
    uint32_t fp;    // forbidden ports
    netsnmp_variable_list *idx;
    struct static_multicast_table_entry ent;

    // Get indexes for entry
    err = get_indexes(req, reginfo, tinfo, &ent);
    if (err)
        return err;

    DEBUGMSGTL((MIBMOD, "cid=%d vid=%d mac=%s rx_port=%d column=%d\n",
        ent.cid, ent.vid, mac_to_str(ent.mac), ent.rx_port, tinfo->colnum));

    // Get indexes for next entry - SNMP_ENDOFMIBVIEW informs the handler
    // to proceed with next column.
    if (ent.cid > DEFAULT_COMPONENT_ID)
        return SNMP_ENDOFMIBVIEW;
    if (ent.cid == 0) {
        ent.cid     = DEFAULT_COMPONENT_ID;
        ent.vid     = 0;
        ent.rx_port = ALL_PORTS;
        mac_copy(ent.mac, (uint8_t*)DEFAULT_MAC);
        ent.mac[0] = 0x01;
    }
    if (ent.vid >= NUM_VLANS)
        return SNMP_ENDOFMIBVIEW;

    errno = 0;
    err = rtu_fdb_proxy_read_next_static_entry(&ent.mac, &ent.vid, &ep, &fp,
                                               &ent.type, &ent.row_status);
    if (errno)
        goto minipc_err;
    if (err)
        return SNMP_ENDOFMIBVIEW; // No more entries in static FDB

    to_octetstr(ep, ent.egress_ports);
    to_octetstr(fp, ent.forbidden_ports);

    // Update indexes and OID returned in SNMP response
    idx = tinfo->indexes;
    *idx->val.integer = ent.cid;

    idx = idx->next_variable;
    *idx->val.integer = ent.vid;

    idx = idx->next_variable;
    memcpy(idx->val.string, ent.mac, ETH_ALEN);

    idx = idx->next_variable;
    *idx->val.integer = ent.rx_port;

    update_oid(req, reginfo, tinfo->colnum, tinfo->indexes);
    // Return next entry column value
    return get_column(req->requestvb, tinfo->colnum, &ent);

minipc_err:
    snmp_log(LOG_ERR, "%s(%s): mini-ipc error [%s]\n", __FILE__, __func__,
        strerror(errno));
    return SNMP_ERR_GENERR;
}

/**
 * Checks that the type and size of the value matches the corresponding
 * column type and size.
 */
static int set_reserve1(netsnmp_request_info *req,
                        netsnmp_handler_registration *reginfo)
{
    uint32_t ep, fp;                  // aux fields just to read entry from fdb
    int err, s, t;
    struct static_multicast_table_entry ent;
    netsnmp_variable_list *vb = req->requestvb;
    netsnmp_table_request_info *tinfo = netsnmp_extract_table_info(req);

    // Check indexes
    err = get_indexes(req, reginfo, tinfo, &ent);
    if (err)
        return err;

    if ((ent.cid != DEFAULT_COMPONENT_ID) ||
        (ent.vid >= NUM_VLANS) ||
        (ent.rx_port != ALL_PORTS))
        return SNMP_NOSUCHINSTANCE;

    DEBUGMSGTL((MIBMOD, "cid=%d vid=%d mac=%s rx_port=%d column=%d\n",
        ent.cid, ent.vid, mac_to_str(ent.mac), ent.rx_port, tinfo->colnum));

    switch (tinfo->colnum) {
    case COLUMN_IEEE8021QBRIDGESTATICMULTICASTSTATICEGRESSPORTS:
        err = netsnmp_check_vb_type_and_size(vb, ASN_OCTET_STR, NUM_PORTS);
        break;
    case COLUMN_IEEE8021QBRIDGESTATICMULTICASTFORBIDDENEGRESSPORTS:
        err = netsnmp_check_vb_type_and_size(vb, ASN_OCTET_STR, NUM_PORTS);
        break;
    case COLUMN_IEEE8021QBRIDGESTATICMULTICASTSTORAGETYPE:
        err = netsnmp_check_vb_int_range(vb, ST_OTHER, ST_READONLY);
        break;
    case COLUMN_IEEE8021QBRIDGESTATICMULTICASTROWSTATUS:
        // Get current row status and check transition from current to requested
        errno = 0;
        err = rtu_fdb_proxy_read_static_entry(ent.mac, ent.vid, &ep, &fp, &t, &s);
        if (errno)
            goto minipc_err;
        err = netsnmp_check_vb_rowstatus(vb, err ? RS_NONEXISTENT:RS_ACTIVE);
        break;
    default:
        return SNMP_ERR_NOTWRITABLE;
    }
    return err;

minipc_err:
    snmp_log(LOG_ERR, "%s(%s): mini-ipc error [%s]\n", __FILE__, __func__,
        strerror(errno));
    return SNMP_ERR_GENERR;
}


/**
 * Reserve space in agent memory to hold values until commited
 */
static int set_reserve2(netsnmp_request_info *req)
{
    netsnmp_table_request_info *tinfo = netsnmp_extract_table_info(req);

    switch (tinfo->colnum) {
    case COLUMN_IEEE8021QBRIDGESTATICMULTICASTROWSTATUS:
        switch (*req->requestvb->val.integer) { // status
        case RS_CREATEANDGO:
        case RS_CREATEANDWAIT:
            if (!cache_create(
                    *tinfo->indexes->val.integer,
                    *tinfo->indexes->next_variable->val.integer,
                     tinfo->indexes->next_variable->next_variable->val.string,
                    *tinfo->indexes->next_variable->next_variable->next_variable->val.integer))
                return SNMP_ERR_RESOURCEUNAVAILABLE;
        }
    }
    return SNMP_ERR_NOERROR;
}

/**
 * Cache active/notInService FDB entries.
 */
static int set_reserve3(netsnmp_request_info         *req,
                        netsnmp_handler_registration *reginfo)
{
    int err;
    uint32_t ep;    // egress ports
    uint32_t fp;    // forbidden ports
    struct static_multicast_table_entry *ent, idx;
    netsnmp_table_request_info *tinfo = netsnmp_extract_table_info(req);


    // Get indexes for entry
    get_indexes(req, reginfo, tinfo, &idx); //err checked at set_reserve1
    // Get entry from cache
    ent = cache_get(&idx);
    if (!ent) {
        // This means we are setting fields for ACTIVE or NOTINSERVICE rows.
        // (set_reserve2 creates rows for CREATE_AND_GO and CREATE_AND_WAIT)
        ent = cache_create(idx.cid, idx.vid, idx.mac, idx.rx_port);
        if (!ent)
            return SNMP_ERR_RESOURCEUNAVAILABLE;

        errno = 0;
        err = rtu_fdb_proxy_read_static_entry(ent->mac, ent->vid, &ep,
            &fp, &ent->type, &ent->row_status);
        if (errno)
            goto minipc_err;
        if (err)
            goto entry_not_found;
        to_octetstr(ep, ent->egress_ports);
        to_octetstr(fp, ent->forbidden_ports);
        ent->row_status = ent->row_status ? RS_ACTIVE:RS_NOTINSERVICE;
    }
    return SNMP_ERR_NOERROR;

entry_not_found:
    DEBUGMSGTL((MIBMOD, "entry vid=%d mac=%s not found\n",
        ent->vid, mac_to_str(ent->mac)));
    return SNMP_NOSUCHINSTANCE;

minipc_err:
    snmp_log(LOG_ERR, "%s(%s): mini-ipc error [%s]\n",
        __FILE__, __func__, strerror(errno));
    return SNMP_ERR_GENERR;
}

/**
 * Sets value using cached entries in the agent memory.
 */
static int set_action(netsnmp_request_info *req,
                      netsnmp_handler_registration *reginfo)
{
    struct static_multicast_table_entry *ent, idx;
    netsnmp_variable_list *vb = req->requestvb;
    netsnmp_table_request_info *tinfo = netsnmp_extract_table_info(req);

    // Get indexes
    get_indexes(req, reginfo, tinfo, &idx);
    // Get entry from cache
    ent = cache_get(&idx);
    switch (tinfo->colnum) {
    case COLUMN_IEEE8021QBRIDGESTATICMULTICASTSTATICEGRESSPORTS:
        memcpy(ent->egress_ports, vb->val.string, vb->val_len);
        break;
    case COLUMN_IEEE8021QBRIDGESTATICMULTICASTFORBIDDENEGRESSPORTS:
        memcpy(ent->forbidden_ports, vb->val.string, vb->val_len);
        break;
    case COLUMN_IEEE8021QBRIDGESTATICMULTICASTSTORAGETYPE:
        ent->type = *vb->val.integer;
        break;
    case COLUMN_IEEE8021QBRIDGESTATICMULTICASTROWSTATUS:
        ent->row_status = *vb->val.integer;
        break;
    }
    // Keep reference to last request for this entry (to return err later on)
    ent->req = req;
    return SNMP_ERR_NOERROR;
}

/**
 * \brief Check consistency of active rows.
 */
static int check_consistency(struct static_multicast_table_entry *ent)
{
    int i;

    switch(ent->row_status) {
    case RS_ACTIVE:
    case RS_CREATEANDGO:
        // MAC should be multicast (this is static _multcast_ table)
        if (!mac_multicast(ent->mac)) {
            snmp_log(LOG_ERR, "%s(%d): error - mac address is unicast\n",
                __FILE__, __LINE__);
            return SNMP_ERR_INCONSISTENTVALUE;
        }
        // Ports can not be egress and forbidden egress at the same time.
        for (i = 0; i < NUM_PORTS; i++) {
            if((ent->egress_ports[i] == 1) && (ent->forbidden_ports[i] == 1)) {
                snmp_log(LOG_ERR,
                    "%s(%d): inconsistent egress port definition - port %d\n",
                    __FILE__, __LINE__, i);
                return SNMP_ERR_INCONSISTENTVALUE;
            }
        }
    }
    return SNMP_ERR_NOERROR;
}

static int set_commit(struct static_multicast_table_entry *ent)
{
    int err;
    uint32_t ep;
    uint32_t fp;

    switch (ent->row_status) {
    case RS_DESTROY:
        // Remove entry from FDB
        errno = 0;
        err = rtu_fdb_proxy_delete_static_entry(ent->mac, ent->vid);
        if (errno)
            goto minipc_err;
        // Permanent entries can not be removed so an error might be raised...
        if (err)
            goto not_deleted;
        break;
    default:
        from_octetstr(&ep, ent->egress_ports);
        from_octetstr(&fp, ent->forbidden_ports);
        // create/update entry in FDB
        errno = 0;
        err = rtu_fdb_proxy_create_static_entry(ent->mac, ent->vid, ep, fp,
            ent->type, RS_IS_GOING_ACTIVE(ent->row_status));
        if (errno)
            goto minipc_err;
        if (err)
            goto not_created;
        break;
    }
    return SNMP_ERR_NOERROR;

not_deleted:
    snmp_log(LOG_ERR, "%s(%s): delete static entry error [%d]\n",
        __FILE__, __func__, err);
    return SNMP_ERR_GENERR;

not_created:
    snmp_log(LOG_ERR, "%s(%s): create static entry error [%d]\n",
        __FILE__, __func__, err);
    return SNMP_ERR_GENERR;

minipc_err:
    snmp_log(LOG_ERR, "%s(%d): mini-ipc error [%s]\n", __FILE__, __LINE__,
        strerror(errno));
    return SNMP_ERR_GENERR;
}


/**
 * Handles requests for the ieee8021QBridgeStaticMulticastTable table
 */
static int _handler(netsnmp_mib_handler          *handler,
                    netsnmp_handler_registration *reginfo,
                    netsnmp_agent_request_info   *reqinfo,
                    netsnmp_request_info         *requests)
{
    int err;
    netsnmp_request_info *req;
    netsnmp_table_request_info *tinfo;
    struct static_multicast_table_entry *ent;

    switch (reqinfo->mode) {
    case MODE_GET:
        for (req = requests; req; req = req->next) {
            err = get(req, reginfo);
            if (err)
                netsnmp_set_request_error(reqinfo, req, err);
        }
        break;
    case MODE_GETNEXT:
        for (req = requests; req; req = req->next) {
            tinfo = netsnmp_extract_table_info(req);
            if (tinfo) {
                err = get_next(req, reginfo, tinfo);
            } else {
                DEBUGMSGTL((MIBMOD, "No table info\n"));
                err = SNMP_ERR_GENERR;
            }
            if (err)
                netsnmp_set_request_error(reqinfo, req, err);
        }
        break;
    case MODE_SET_RESERVE1:
        for (req =requests; req; req =req->next) {
            err = set_reserve1(req, reginfo);
            if (err)
                netsnmp_set_request_error(reqinfo, req, err);
        }
        break;
    case MODE_SET_RESERVE2:
        for (req = requests; req; req = req->next) {
            err = set_reserve2(req);
            if (err)
                netsnmp_set_request_error(reqinfo, req, err);
        }
        for (req = requests; req; req = req->next) {
            err = set_reserve3(req, reginfo);
            if (err)
                netsnmp_set_request_error(reqinfo, req, err);
        }
        break;
    case MODE_SET_FREE:
        cache_clean();
        break;
    case MODE_SET_ACTION:
        for (req = requests; req; req = req->next) {
            err = set_action(req, reginfo);
            if (err)
                netsnmp_set_request_error(reqinfo, req, err);
        }
        for (ent = cache; ent; ent = ent->next) {
            err = check_consistency(ent);
            if (err)
                netsnmp_set_request_error(reqinfo, ent->req, err);
        }
        break;
    case MODE_SET_UNDO:
        cache_clean();
        break;
    case MODE_SET_COMMIT:
        for (ent = cache; ent; ent = ent->next) {
            err = set_commit(ent);
            if (err)
                netsnmp_set_request_error(reqinfo, ent->req, err);
        }
        cache_clean(); // prepare for next atomic set operation
        break;
    }
    return SNMP_ERR_NOERROR;
}

/**
 * Initialize the ieee8021QBridgeStaticMulticastTable table by defining its
 * contents and how it's structured
 */
static void initialize_table(void)
{
    const oid                       _oid[] = {1,3,111,2,802,1,1,4,1,3,2};
    netsnmp_handler_registration    *reg;
    netsnmp_table_registration_info *tinfo;
    netsnmp_variable_list           *idx;

    reg = netsnmp_create_handler_registration(
            "ieee8021QBridgeStaticMulticastTable",
            _handler,
            (oid *)_oid,
            OID_LENGTH(_oid),
            HANDLER_CAN_RWRITE);

    tinfo = SNMP_MALLOC_TYPEDEF( netsnmp_table_registration_info );
    netsnmp_table_helper_add_indexes(
            tinfo,
            ASN_UNSIGNED,               /* index: ComponentId */
            ASN_UNSIGNED,               /* index: VlanIndex */
            ASN_PRIV_IMPLIED_OCTET_STR, /* index: MulticastAddress */
            ASN_UNSIGNED,               /* index: ReceivePort */
            0);

    // Fix the MacAddress Index variable binding lenght
    idx = tinfo->indexes;
    idx = idx->next_variable; // skip componentId
    idx = idx->next_variable; // skip vlanIndex
    idx->val_len = ETH_ALEN;

    tinfo->min_column = COLUMN_IEEE8021QBRIDGESTATICMULTICASTSTATICEGRESSPORTS;
    tinfo->max_column = COLUMN_IEEE8021QBRIDGESTATICMULTICASTROWSTATUS;

    netsnmp_register_table(reg, tinfo);
}

/**
 * Initializes the ieee8021QBridgeStaticMulticastTable module
 */
void init_ieee8021QBridgeStaticMulticastTable(void)
{
    struct minipc_ch *client;

    client = rtu_fdb_proxy_create("rtu_fdb");
    if(client) {
        initialize_table();
        snmp_log(LOG_INFO, "%s: initialised\n", __FILE__);
    } else {
        snmp_log(LOG_ERR, "%s: error creating mini-ipc proxy - %s\n", __FILE__,
            strerror(errno));
    }
}
