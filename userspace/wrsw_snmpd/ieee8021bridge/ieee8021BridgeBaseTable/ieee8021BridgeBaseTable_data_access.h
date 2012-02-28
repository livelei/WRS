/*
 * Note: this file originally auto-generated by mib2c using
 *       version : 14170 $ of $
 *
 * $Id:$
 */
#ifndef IEEE8021BRIDGEBASETABLE_DATA_ACCESS_H
#define IEEE8021BRIDGEBASETABLE_DATA_ACCESS_H

#ifdef __cplusplus
extern "C" {
#endif


/* *********************************************************************
 * function declarations
 */

/* *********************************************************************
 * Table declarations
 */
/**********************************************************************
 **********************************************************************
 ***
 *** Table ieee8021BridgeBaseTable
 ***
 **********************************************************************
 **********************************************************************/
/*
 * IEEE8021-BRIDGE-MIB::ieee8021BridgeBaseTable is subid 1 of ieee8021BridgeBase.
 * Its status is Current.
 * OID: .1.3.111.2.802.1.1.2.1.1.1, length: 11
*/


    int ieee8021BridgeBaseTable_init_data(ieee8021BridgeBaseTable_registration * ieee8021BridgeBaseTable_reg);


    /*
     * TODO:180:o: Review ieee8021BridgeBaseTable cache timeout.
     * The number of seconds before the cache times out
     */
#define IEEE8021BRIDGEBASETABLE_CACHE_TIMEOUT   60

void ieee8021BridgeBaseTable_container_init(netsnmp_container **container_ptr_ptr,
                             netsnmp_cache *cache);
void ieee8021BridgeBaseTable_container_shutdown(netsnmp_container *container_ptr);

int ieee8021BridgeBaseTable_container_load(netsnmp_container *container);
void ieee8021BridgeBaseTable_container_free(netsnmp_container *container);

int ieee8021BridgeBaseTable_cache_load(netsnmp_container *container);
void ieee8021BridgeBaseTable_cache_free(netsnmp_container *container);

    int ieee8021BridgeBaseTable_row_prep( ieee8021BridgeBaseTable_rowreq_ctx *rowreq_ctx);

int ieee8021BridgeBaseTable_validate_index( ieee8021BridgeBaseTable_registration * ieee8021BridgeBaseTable_reg,
                               ieee8021BridgeBaseTable_rowreq_ctx *rowreq_ctx);
int ieee8021BridgeBaseComponentId_check_index( ieee8021BridgeBaseTable_rowreq_ctx *rowreq_ctx ); /* internal */


#ifdef __cplusplus
}
#endif

#endif /* IEEE8021BRIDGEBASETABLE_DATA_ACCESS_H */