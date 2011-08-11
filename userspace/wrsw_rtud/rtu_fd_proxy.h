/*
 * White Rabbit RTU (Routing Table Unit)
 * Copyright (C) 2010, CERN.
 *
 * Version:     wrsw_rtud v2.0
 *
 * Authors:     Juan Luis Manas (juan.manas@integrasys.es)
 *
 * Description: RTU Filtering Database Proxy.
 *              Provides IPC access to the Filtering Database.
 *              Based on Alessandro Rubini & Tomasz Wlostowski mini_ipc framework.
 *
 * Fixes:
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


#ifndef __WHITERABBIT_RTU_FD_PROXY_H
#define __WHITERABBIT_RTU_FD_PROXY_H

#include "minipc.h"

#include "rtu.h"

#define MILLISEC_TIMEOUT 1000

int  rtu_fdb_proxy_create_static_entry(
            uint8_t mac[ETH_ALEN],
            uint16_t vid,
            enum filtering_control port_map[NUM_PORTS],
            enum storage_type type,
            int active
     ) __attribute__((warn_unused_result));

int rtu_fdb_proxy_delete_static_entry(
            uint8_t mac[ETH_ALEN],
            uint16_t vid
     ) __attribute__((warn_unused_result));

int rtu_fdb_proxy_read_entry(
           uint8_t mac[ETH_ALEN],
           uint8_t fid,
           uint32_t *port_map,
           int *entry_type
     ) __attribute__((warn_unused_result));

int rtu_fdb_proxy_read_next_entry(
           uint8_t (*mac)[ETH_ALEN],                            // inout
           uint8_t *fid,                                        // inout
           uint32_t *port_map,                                  // out
           int *entry_type                                      // out
     ) __attribute__((warn_unused_result));

int rtu_fdb_proxy_read_static_entry(
            uint8_t mac[ETH_ALEN],
            uint16_t vid,
            enum filtering_control (*port_map)[NUM_PORTS],      // out
            enum storage_type *type,                            // out
            int *active                                         // out
     ) __attribute__((warn_unused_result));

int rtu_fdb_proxy_read_next_static_entry(
            uint8_t (*mac)[ETH_ALEN],                           // inout
            uint16_t *vid,                                      // inout
            enum filtering_control (*port_map)[NUM_PORTS],      // out
            enum storage_type *type,                            // out
            int *active                                         // out
     ) __attribute__((warn_unused_result));

int  rtu_fdb_proxy_set_aging_time(
            uint8_t fid,
            unsigned long t
    ) __attribute__((warn_unused_result));

unsigned long rtu_fd_proxy_get_aging_time(uint8_t fid);

uint16_t rtu_fdb_proxy_get_num_dynamic_entries(uint8_t fid);
uint32_t rtu_fdb_proxy_get_num_learned_entry_discards(uint8_t fid);
uint16_t rtu_fdb_proxy_get_num_vlans(void);
uint16_t rtu_fdb_proxy_get_max_supported_vlans(void);
uint16_t rtu_fdb_proxy_get_max_vid(void);
uint64_t rtu_fdb_proxy_get_num_vlan_deletes(void);
uint8_t  rtu_fdb_proxy_get_next_fid(uint8_t fid);

struct minipc_ch *rtu_fdb_proxy_create(char *name);

#endif /*__WHITERABBIT_RTU_FD_PROXY_H*/
