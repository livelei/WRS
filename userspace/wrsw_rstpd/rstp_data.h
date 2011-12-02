/*
 * White Rabbit Switch RSTP
 * Copyright (C) 2011, CERN.
 *
 * Authors:     Miguel Baizán (miguel.baizan@integrasys-sa.com)
 *
 * Description: Basic RSTP structures and parameters.
 *
 * Fixes:
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

#ifndef __WHITERABBIT_RSTP_DATA_H
#define __WHITERABBIT_RSTP_DATA_H

#include <stdint.h>
#include <linux/if_ether.h>
#include <time.h>

#include <hw/trace.h>


#define DEFAULT_LOG_FILE "/var/log/rstpd.log" /* Running as daemon, if
                                                 -f option is not specified */
#ifdef VERBOSE
#define TRACEV(...) TRACE(__VA_ARGS__)
#else
#define TRACEV(...)
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 1
#endif

#define MAX_NUM_PORTS 10
#define NUM_STMCH_PER_PORT 9

/* Bridge ID length */
#define BRIDGE_PRIO_LEN 2 /* Number of octets */
#define BRIDGE_ID_LEN   (BRIDGE_PRIO_LEN + ETH_ALEN)

/* Types of BPDU supported */
#define BPDU_TYPE_CONFIG    0x00
#define BPDU_TYPE_TCN       0x80
#define BPDU_TYPE_RSTP      0x02

/* STP is version 0 and RSTP is version 2 */
#define RST_PROTOCOL_VERSION_IDENTIFIER 0x02

/* Masks and offsets for the flags parameter in the BPDUs */
#define PROPOSAL_FLAG_OFFS               1
#define PORT_ROLE_FLAG_OFFS              2
#define LEARNING_FLAG_OFFS               4
#define FORWARDING_FLAG_OFFS             5
#define AGREEMENT_FLAG_OFFS              6
#define TOPOLOGY_CHANGE_ACK_FLAG_OFFS    7
#define TOPOLOGY_CHANGE_FLAG        0x01
#define PROPOSAL_FLAG               (0x01 << PROPOSAL_FLAG_OFFS)
#define PORT_ROLE_FLAG              (0x03 << PORT_ROLE_FLAG_OFFS)
#define LEARNING_FLAG               (0x01 << LEARNING_FLAG_OFFS)
#define FORWARDING_FLAG             (0x01 << FORWARDING_FLAG_OFFS)
#define AGREEMENT_FLAG              (0x01 << AGREEMENT_FLAG_OFFS)
#define TOPOLOGY_CHANGE_ACK_FLAG    (0x01 << TOPOLOGY_CHANGE_ACK_FLAG_OFFS)

/* Port roles as defined in the BPDU */
#define PORT_ROLE_UNKNOWN           0
#define PORT_ROLE_ALTERNATE_BACKUP  1
#define PORT_ROLE_ROOT              2
#define PORT_ROLE_DESIGNATED        3

/* Default values for the RSTP performance parameters. See 802.1D-2004, clause
   17.14 and 9.2.8. Time values in BPDUs are encoded in units of time of
   1/256 of a second */
#define ONE_SECOND  256
#define DEFAULT_MIGRATE_TIME            (3 * ONE_SECOND)
#define DEFAULT_BRIDGE_HELLO_TIME       (2 * ONE_SECOND)
#define DEFAULT_BRIDGE_MAX_AGE          (20 * ONE_SECOND)
#define DEFAULT_BRIDGE_FORWARD_DELAY    (15 * ONE_SECOND)
#define DEFAULT_TRANSMIT_HOLD_COUNT     0x06        /* 6 */
#define DEFAULT_BRIDGE_PRIORITY         0x8000      /* 32768 */
#define DEFAULT_PORT_PRIORITY           0x80        /* 128 */

/* Ranges for the RSTP performance parameters */
#define MIN_BRIDGE_HELLO_TIME           ONE_SECOND
#define MAX_BRIDGE_HELLO_TIME           (2 * ONE_SECOND)
#define MIN_BRIDGE_MAX_AGE              (6 * ONE_SECOND)
#define MAX_BRIDGE_MAX_AGE              (40 * ONE_SECOND)
#define MIN_BRIDGE_FORWARD_DELAY        (4 * ONE_SECOND)
#define MAX_BRIDGE_FORWARD_DELAY        (30 * ONE_SECOND)
#define MIN_TRANSMIT_HOLD_COUNT         0x01
#define MAX_TRANSMIT_HOLD_COUNT         0x0A
#define MIN_BRIDGE_PRIORITY             0
#define MAX_BRIDGE_PRIORITY             0xF000      /* 61440 */
#define MIN_PORT_PRIORITY               0
#define MAX_PORT_PRIORITY               0xF0        /* 240 */
#define BRIDGE_PRIORITY_STEP            0x1000      /* 4096 */
#define PORT_PRIORITY_STEP              0x10        /* 16 */

/* Default Port Path Cost values. See  802.1D-2004, clause 17.14 */
#define DEFAULT_PORT_PATH_COST_100_KBPS     0xBEBC200   /* 200 000 000 */
#define DEFAULT_PORT_PATH_COST_1_MBPS       0x1312D00   /* 20 000 000 */
#define DEFAULT_PORT_PATH_COST_10_MBPS      0x01E8480   /* 2 000 000 */
#define DEFAULT_PORT_PATH_COST_100_MBPS     0x0030D40   /* 200 000 */
#define DEFAULT_PORT_PATH_COST_1_GBPS       0x0004E20   /* 20 000 */
#define DEFAULT_PORT_PATH_COST_10_GBPS      0x00007D0   /* 2 000 */
#define DEFAULT_PORT_PATH_COST_100_GBPS     0x00000C8   /* 200 */
#define DEFAULT_PORT_PATH_COST_1_TBPS       0x0000014   /* 20 */
#define DEFAULT_PORT_PATH_COST_10_TBPS      0x0000002   /* 2 */
#define MAX_PORT_PATH_COST                  0xBEBC200   /* 200000000 */

/* RSTP port flags (rstp_flags). See  802.1D-2004, clause 17.19  */
enum port_flag {
    agree = 0,      /* 17.19.2 */
    agreed,         /* 17.19.3 */
    disputed,       /* 17.19.6 */
    fdbFlush,       /* 17.19.7 */
    forward,        /* 17.19.8 */
    forwarding,     /* 17.19.9 */
    learn,          /* 17.19.11 */
    learning,       /* 17.19.12 */
    mcheck,         /* 17.19.13 */
    newInfo,        /* 17.19.16 */
    operEdge,       /* 17.19.17 */
    portEnabled,    /* 17.19.18 */
    proposed,       /* 17.19.23 */
    proposing,      /* 17.19.24 */
    rcvdBPDU,       /* 17.19.25 */
    rcvdMsg,        /* 17.19.27 */
    rcvdRSTP,       /* 17.19.28 */
    rcvdSTP,        /* 17.19.29 */
    rcvdTc,         /* 17.19.30 */
    rcvdTcAck,      /* 17.19.31 */
    rcvdTcn,        /* 17.19.32 */
    reRoot,         /* 17.19.33 */
    reselect,       /* 17.19.34 */
    selected,       /* 17.19.36 */
    sendRSTP,       /* 17.19.38 */
    _sync,          /* 17.19.39 */ /* renamed because of a previous declaration
                                      in unistd.h */
    synced,         /* 17.19.40 */
    tcAck,          /* 17.19.41 */
    tcProp,         /* 17.19.42 */
    tick,           /* 17.19.43 */
    updtInfo        /* 17.19.45 */
};

/* Indicate the origin/state of the Port's ST information. See 802.1D,
   clause 17.19.10 */
enum port_info {
    Received,
    Mine,
    Aged,
    Disabled
};

enum received_info {
    SuperiorDesignatedInfo,
    RepeatedDesignatedInfo,
    InferiorDesignatedInfo,
    InferiorRootAlternateInfo,
    OtherInfo
};

enum port_role {
    RootPort,
    DesignatedPort,
    AlternatePort,
    BackupPort,
    DisabledPort
};

/* Values that the Force Protocol Version management parameter can take */
enum protocol_version {
    STP_COMPATIBLE = 0,
    RSTP_NORMAL_OPERATION = 2
};

/* State machines IDs. Be sure that the port's state machines start at 0, and
   the bridge's state machine is the last */
enum stmch_id {
    PIM = 0,    /* Port Information Machine */
    PRT,        /* Port Role Transitions Machine */
    PRX,        /* Port Receive Machine */
    PST,        /* Port State Transitions Machine*/
    TCM,        /* Topology Change Machine */
    PPM,        /* Port protocol Migration Machine */
    PTX,        /* Port Transmit Machine */
    PTI,        /* Port Timers Machine */
    BDM,        /* Bridge Detection Machine */
    PRS         /* Port Role Selection Machine */
};


/* Bridge ID is 8 octets long. See 802.1D, clause 9.2.5, for encoding */
struct bridge_id {
    uint8_t prio[BRIDGE_PRIO_LEN];  /* More significant */
    uint8_t addr[ETH_ALEN];         /* Less significant */
};


/* RSTP times for the bridge and port operation. See 802.1D, clause 17.14 */
struct rstp_times {
    uint16_t   message_age;
    uint16_t   max_age;
    uint16_t   hello_time;
    uint16_t   forward_delay;
};


/* Configuration BPDU. See 802.1D, clause 9.3.1. Note that the Port
   ID is 2 octets long. See 802.1D-2004, clause 9.2.7, for encoding
   (the less significant 12 bits are used to encode the Port Number, while
   the 4 more significant bits are used to encode the Port Priority) */
struct cfg_bpdu {
    uint16_t                protocol_identifier;
    uint8_t                 protocol_version_identifier;
    uint8_t                 bpdu_type;
    uint8_t                 flags;

    struct bridge_id        root_identifier;
    uint32_t                root_path_cost;
    struct bridge_id        bridge_identifier;
    uint16_t                port_identifier;

    struct rstp_times       times;
}; /* TODO: should we use: __attribute__((packed))?. Decide when BPDUs
      processing functions be written */;

/* Topology Change Notification BPDU. See 802.1D, clause 9.3.2 */
struct tcn_bpdu {
    uint16_t    protocol_identifier;
    uint8_t     protocol_version_identifier;
    uint8_t     bpdu_type;
};

/* Rapid Spanning Tree BPDU. See 802.1D, clause 9.3.3. This BPDU can act as
   a Configuration BPDU, as a TCN BPDU or as a RSTP BPDU */
struct rstp_bpdu {
    struct cfg_bpdu configuration_bpdu;
    uint8_t         version1_length;
};


/* Spanning Tree priority vector. See 802.1D, clause 17.5 */
struct st_priority_vector {
    struct bridge_id    RootBridgeId;
    uint32_t            RootPathCost;
    struct bridge_id    DesignatedBridgeId;
    uint16_t            DesignatedPortId;
    uint16_t            BridgePortId;
};

/* Keep track of state machines and their current states */
struct state_machine {
    struct bridge_data      *bridge;    /* Owner bridge */
    struct port_data        *port;      /* Owner port, if applicable */

    enum stmch_id           id;     /* For stmch identification */
    unsigned int            state;  /* States for each STMCH */

    void (* compute_transitions) (struct state_machine *);
};


/* RSTP performance parameters per port. These parameters are not modified by
the operation of RSTP, but are treated as constants. They may be modified by
management. See 802.1D, clause 17.13 */
struct rstp_port_mng_data {
    int AdminPortEnabled;   /* The Administrative status of the Port. If True(1)
                               the ST is enabled for this port */
    int AdminEdgePort;      /* 17.13.1 */ /* There's another related parameter
                                             called AutoEdgePort. It's optional
                                             and provided only for bridges that
                                             support the identification of edge
                                             ports. Not implemented for now. */
    int adminPointToPointMAC; /* 6.4.3 and 17.12 */

    uint8_t     PortPriority;   /* 17.13.10 */
    uint32_t    PortPathCost;   /* 17.13.11 and 17.19.20 */

};

/* Per-Port variables. See 802.1D, clause 17.17 and 17.19 */
struct port_data {
    uint8_t                     port_number;
    char                        port_name[16];
    unsigned int                link_status;    /* When non-zero: link is up */

    struct state_machine        stmch[NUM_STMCH_PER_PORT];

    struct rstp_port_mng_data   mng;
    struct rstp_bpdu            bpdu; /* This is to store the information of the
                                         last BPDU received in this port */

    /* RSTP data.
     * Per-Port RSTP variables. The state machines will use the information
     * stored in these variables to compute the state of the port. See 802.1D,
     * clause 17.19 and 17.17
    */
    /* TODO The ageingTime is mantained by the RTU and it's the same for all the
       ports. In a future we should probably have an independent instance for
       each port */
    /*unsigned long               ageingTime;*/     /* 17.19.1 */

    struct st_priority_vector   designatedPriority; /* 17.19.4 */
    struct rstp_times           designatedTimes;    /* 17.19.5 */
    struct st_priority_vector   msgPriority;        /* 17.19.14 */
    struct rstp_times           msgTimes;           /* 17.19.15 */
    struct st_priority_vector   portPriority;       /* 17.19.21 */
    struct rstp_times           portTimes;          /* 17.19.22 */

    enum port_info              infoIs;         /* 17.19.10 */
    enum received_info          rcvdInfo;       /* 17.19.26 */
    enum port_role              role;           /* 17.19.35 */
    enum port_role              selectedRole;   /* 17.19.37 */

    uint16_t                    portId;         /* 17.19.19 */
    uint8_t                     txCount;        /* 17.19.44 */

    uint8_t                     operPointToPointMAC;  /* 6.4.3 and 17.12 */

    /* RSTP port flags */
    uint32_t                    rstp_flags;     /* Flags defined above. Use the
                                                   macros to get/set/remove
                                                   each flag */
    /* Timers */
    uint16_t edgeDelayWhile;    /* 17.17.1 */
    uint16_t fdWhile;           /* 17.17.2 */
    uint16_t helloWhen;         /* 17.17.3 */
    uint16_t mdelayWhile;       /* 17.17.4 */
    uint16_t rbWhile;           /* 17.17.5 */
    uint16_t rcvdInfoWhile;     /* 17.17.6 */
    uint16_t rrWhile;           /* 17.17.7 */
    uint16_t tcWhile;           /* 17.17.8 */
};


/* RSTP performance parameters per bridge. These parameters are not modified by
the operation of RSTP, but are treated as constants. They may be modified by
management. See 802.1D, clause 17.13 */
struct rstp_bridge_mng_data {
    struct bridge_id        BridgeIdentifier;       /* 17.18.2 and 17.13.7 */

    uint16_t                BridgeForwardDelay;     /* 17.13.5 */
    uint16_t                BridgeHelloTime;        /* 17.13.6 */
    uint16_t                BridgeMaxAge;           /* 17.13.8 */
    uint16_t                MigrateTime;            /* 17.13.9 */
    enum protocol_version   forceVersion;           /* 17.13.4 */
    uint8_t                 TxHoldCount;            /* 17.13.12 */
};

/* Per-Bridge variables. See 802.1D, clause 17.18 */
struct bridge_data {
    struct port_data            ports[MAX_NUM_PORTS];
    struct state_machine        stmch;      /* STMCH per bridge */

    struct rstp_bridge_mng_data mng;

    int                         begin;              /* 17.18.1 */
    struct st_priority_vector   BridgePriority;     /* 17.18.3 */
    struct rstp_times           BridgeTimes;        /* 17.18.4 */
    uint16_t                    rootPortId;         /* 17.18.5 */
    struct st_priority_vector   rootPriority;       /* 17.18.6 */
    struct rstp_times           rootTimes;          /* 17.18.7 */
};


/* Operations on RSTP port flags */
/* Test whether the flag is set or not */
static inline uint32_t get_port_flag(uint32_t bitfield, enum port_flag flag)
{
    return ((bitfield >> flag) & 0x01);
}

/* Sets the flag to zero */
static inline void remove_port_flag(uint32_t bitfield, enum port_flag flag)
{
    (bitfield &= (~(0x01 << flag)));
}

/* Sets the flag to one */
static inline void set_port_flag(uint32_t bitfield, enum port_flag flag)
{
    (bitfield |= (0x01 << flag));
}

/* Rounds the timer to the nearest whole second */
static inline void round_timer(uint16_t timer)
{
    timer = ((timer % ONE_SECOND) >= (ONE_SECOND / 2)) ?
            (((timer >> 8) + 1) * ONE_SECOND) :
            (((timer >> 8) - 1) * ONE_SECOND);
}

/*** FUNCTIONS ***/
/* rstp_data.c */
int init_data(void);
void recompute_stmchs(void);
int cmp_priority_vectors(struct st_priority_vector *pv1,
                         struct st_priority_vector *pv2,
                         int cmp_RxPortId);
int cmp_times(struct rstp_times *t1, struct rstp_times *t2);

/* rstp_stmch.c */
void init_stmchs_data(struct bridge_data *br);
void stmch_compute_transitions(struct bridge_data *br);

/* rstp_stmch_*.c */
void initialize_prs(struct state_machine *stmch);
void initialize_pim(struct state_machine *stmch);
/* TODO
void initialize_prt(struct state_machine *stmch);
void initialize_prx(struct state_machine *stmch);
void initialize_pst(struct state_machine *stmch);
void initialize_tcm(struct state_machine *stmch);
void initialize_ppm(struct state_machine *stmch);
void initialize_ptx(struct state_machine *stmch);
void initialize_pti(struct state_machine *stmch);
void initialize_bdm(struct state_machine *stmch);
*/


#endif /* __WHITERABBIT_RSTP_DATA_H */
