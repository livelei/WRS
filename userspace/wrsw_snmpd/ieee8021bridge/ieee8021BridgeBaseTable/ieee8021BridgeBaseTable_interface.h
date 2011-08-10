/*
 * Note: this file originally auto-generated by mib2c using
 *       version : 16927 $ of $
 *
 * $Id:$
 */
/** @ingroup interface: Routines to interface to Net-SNMP
 *
 * \warning This code should not be modified, called directly,
 *          or used to interpret functionality. It is subject to
 *          change at any time.
 * 
 * @{
 */
/*
 * *********************************************************************
 * *********************************************************************
 * *********************************************************************
 * ***                                                               ***
 * ***  NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE  ***
 * ***                                                               ***
 * ***                                                               ***
 * ***       THIS FILE DOES NOT CONTAIN ANY USER EDITABLE CODE.      ***
 * ***                                                               ***
 * ***                                                               ***
 * ***       THE GENERATED CODE IS INTERNAL IMPLEMENTATION, AND      ***
 * ***                                                               ***
 * ***                                                               ***
 * ***    IS SUBJECT TO CHANGE WITHOUT WARNING IN FUTURE RELEASES.   ***
 * ***                                                               ***
 * ***                                                               ***
 * *********************************************************************
 * *********************************************************************
 * *********************************************************************
 */
#ifndef IEEE8021BRIDGEBASETABLE_INTERFACE_H
#define IEEE8021BRIDGEBASETABLE_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif


#include "ieee8021BridgeBaseTable.h"


/* ********************************************************************
 * Table declarations
 */

/* PUBLIC interface initialization routine */
void _ieee8021BridgeBaseTable_initialize_interface(ieee8021BridgeBaseTable_registration * user_ctx,
                                    u_long flags);
void _ieee8021BridgeBaseTable_shutdown_interface(ieee8021BridgeBaseTable_registration * user_ctx);

ieee8021BridgeBaseTable_registration *
ieee8021BridgeBaseTable_registration_get( void );

ieee8021BridgeBaseTable_registration *
ieee8021BridgeBaseTable_registration_set( ieee8021BridgeBaseTable_registration * newreg );

netsnmp_container *ieee8021BridgeBaseTable_container_get( void );
int ieee8021BridgeBaseTable_container_size( void );

u_int ieee8021BridgeBaseTable_dirty_get( void );
void ieee8021BridgeBaseTable_dirty_set( u_int status );

    ieee8021BridgeBaseTable_rowreq_ctx * ieee8021BridgeBaseTable_allocate_rowreq_ctx(void *);
void ieee8021BridgeBaseTable_release_rowreq_ctx(ieee8021BridgeBaseTable_rowreq_ctx *rowreq_ctx);

int ieee8021BridgeBaseTable_index_to_oid(netsnmp_index *oid_idx,
                            ieee8021BridgeBaseTable_mib_index *mib_idx);
int ieee8021BridgeBaseTable_index_from_oid(netsnmp_index *oid_idx,
                              ieee8021BridgeBaseTable_mib_index *mib_idx);

/*
 * access to certain internals. use with caution!
 */
void ieee8021BridgeBaseTable_valid_columns_set(netsnmp_column_info *vc);


#ifdef __cplusplus
}
#endif

#endif /* IEEE8021BRIDGEBASETABLE_INTERFACE_H */
/** @} */
