/*
 * Note: this file originally auto-generated by mib2c using
 *  : mib2c.raw-table.conf 17436 2009-03-31 15:12:19Z dts12 $
 */
#ifndef IEEE8021QBRIDGEPORTVLANTABLE_H
#define IEEE8021QBRIDGEPORTVLANTABLE_H

#define MIN_PVID                0x001
#define MAX_PVID                0xFFE

enum acceptable_frame_types {
    admitAll                   = 1,
    admitUntaggedAndPriority   = 2,
    admitTagged                = 3
};

enum qmode {
    access_port                = 0,
    trunk_port                 = 1,
    unqualified_port           = 3
};

void init_ieee8021QBridgePortVlanTable(void);

#endif /* IEEE8021QBRIDGEPORTVLANTABLE_H */
