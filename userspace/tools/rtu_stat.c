/*
 * rtu_stat:
 *
 * Give detail on the Routing-Table-Unit (RTU) and permit to modify it
 *
 *  Modified on: Oct 30, 2012
 *  Authors:
 *  	- Tomasz Wlostowski (tomasz.wlostowski@cern.ch)
 * 		- Benoit RAT (benoit<AT>sevensols.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License...
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libwr/util.h>

#include <libwr/shmem.h>
#include <libwr/hal_shmem.h>
#include <libwr/rtu_shmem.h>

#include <minipc.h>
#include <rtud_exports.h>
#include <mac.h>
#include <errno.h>

#define MINIPC_TIMEOUT 200

static struct minipc_ch *rtud_ch;

struct wrs_shm_head *rtu_port_shmem;
static struct rtu_vlan_table_entry vlan_tab_local[NUM_VLANS];
static struct rtu_filtering_entry rtu_htab_local[RTU_BUCKETS * HTAB_ENTRIES];

int rtudexp_clear_entries(int netif, int force)
{
	int val, ret;
	ret = minipc_call(rtud_ch, MINIPC_TIMEOUT, &rtud_export_clear_entries,&val,netif,force);
	return (ret<0)?ret:val;
}

int rtudexp_add_entry(const char *eha, int port, int mode)
{
	int val, ret;
	ret = minipc_call(rtud_ch, MINIPC_TIMEOUT, &rtud_export_add_entry,&val,eha,port,mode);
	return (ret<0)?ret:val;
}

int rtudexp_vlan_entry(int vid, int fid, const char *ch_mask, int drop, int prio, int has_prio,
		        int prio_override)
{
	int val, ret;
	int mask;
	sscanf(ch_mask,"%x", &mask);
	ret = minipc_call(rtud_ch, MINIPC_TIMEOUT, &rtud_export_vlan_entry,&val,vid,fid,mask,
			    drop,prio,has_prio,prio_override);
	return (ret<0)?ret:val;
}

/**
 * \brief Write mac address into a buffer to avoid concurrent access on static variable.
 */
//TODO: already defined in wrsw_rtud lib but we do not link to it. 3 opts: make it inline, move to libwr or link to the rtud lib?
char *mac_to_buffer(uint8_t mac[ETH_ALEN],char buffer[ETH_ALEN_STR])
{
 	if(mac && buffer)
 		snprintf(buffer, ETH_ALEN_STR, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    return buffer;
}

static int cmp_rtu_entries(const void *p1, const void *p2)
{
	const struct rtu_filtering_entry *e1 = p1;
	const struct rtu_filtering_entry *e2 = p2;

	return memcmp(e1->mac, e2->mac, 6);
}

char *decode_ports(int dpm, int nports)
{
	static char str[80],str2[20];
	int i;

	if ((dpm & ((1<<nports)-1)) == ((1<<nports)-1))
	{
		strcpy(str,"ALL");
		return str;
	}
	strcpy(str,"");

	for (i = 0; i < nports; i++)
	{
		sprintf(str2,"%d ", i);
		if(dpm&(1<<i)) strcat(str,str2);
	}

	if (dpm & (1<<nports))
		strcat(str, "CPU");

	return str;
}

void show_help(char *prgname)
{
	fprintf(stderr, "usage: %s <command> <values>\n", prgname);
	fprintf(stderr,""
			"   help:             Show this message\n"
			"   list:             List the routing table (same as empty command)\n"
			"   remove <ifnum>:   Remove all dynamic entries for one interface\n"
			"   add    <mac (XX:XX:XX:XX:XX)> <ifnum> [<mode>]: Add entry for a specific \n"
			"                     MAC address (mode={1=dynamic,0=static}\n"
			"   vlan   <vid> <fid> <hex mask> [<drop>, <prio>, <has_prio>, <prio_override>]: \n"
			"                    Add VLAN entry with vid, fid, mask and drop flag (Write mask=0x0 \n"
			"                    and drop=1 to remove the VLAN)\n");

	exit(1);
}

int get_nports_from_hal(void)
{
	struct hal_shmem_header *h;
	struct wrs_shm_head *hal_head = NULL;
	int hal_nports_local; /* local copy of number of ports */
	int ii;
	int n_wait = 0;
	int ret;

	/* wait for HAL */
	while ((ret = wrs_shm_get_and_check(wrs_shm_hal, &hal_head)) != 0) {
		n_wait++;
		if (n_wait > 10) {
			if (ret == 1) {
				fprintf(stderr, "rtu_stat: Unable to open "
					"HAL's shm !\n");
			}
			if (ret == 2) {
				fprintf(stderr, "rtu_stat: Unable to read "
					"HAL's version!\n");
			}
			exit(1);
		}
		sleep(1);
	}

	h = (void *)hal_head + hal_head->data_off;

	n_wait = 0;
	while (1) { /* wait for 10 sec for HAL to produce consistent nports */
		n_wait++;
		ii = wrs_shm_seqbegin(hal_head);
		/* Assume number of ports does not change in runtime */
		hal_nports_local = h->nports;
		if (!wrs_shm_seqretry(hal_head, ii))
			break;
		fprintf(stderr, "rtu_stat: Wait for HAL.\n");
		if (n_wait > 10) {
			exit(1);
		}
		sleep(1);
	}

	/* check hal's shm version */
	if (hal_head->version != HAL_SHMEM_VERSION) {
		fprintf(stderr, "rtu_stat: unknown HAL's shm version %i "
			"(known is %i)\n",
			hal_head->version, HAL_SHMEM_VERSION);
		exit(-1);
	}

	if (hal_nports_local > HAL_MAX_PORTS) {
		fprintf(stderr, "rtu_stat: Too many ports reported by HAL. "
			"%d vs %d supported\n",
			hal_nports_local, HAL_MAX_PORTS);
		exit(-1);
	}
	return hal_nports_local;
}

int read_vlans(void)
{
	unsigned ii;
	unsigned retries = 0;
	struct rtu_vlan_table_entry *vlan_tab_shm;
	struct rtu_shmem_header *rtu_hdr;

	rtu_hdr = (void *)rtu_port_shmem + rtu_port_shmem->data_off;
	vlan_tab_shm = wrs_shm_follow(rtu_port_shmem, rtu_hdr->vlans);
	if (!vlan_tab_shm)
		return -2;
	/* read data, with the sequential lock to have all data consistent */
	while (1) {
		ii = wrs_shm_seqbegin(rtu_port_shmem);
		memcpy(&vlan_tab_local, vlan_tab_shm,
		       NUM_VLANS * sizeof(*vlan_tab_shm));
		retries++;
		if (retries > 100)
			return -1;
		if (!wrs_shm_seqretry(rtu_port_shmem, ii))
			break; /* consistent read */
		usleep(1000);
	}

	return 0;
}

/* Read filtes from rtud's shm, convert hashtable to regular table */
int read_htab(int *read_entries)
{
	unsigned ii;
	unsigned retries = 0;
	struct rtu_filtering_entry *htab_shm;
	struct rtu_shmem_header *rtu_hdr;
	struct rtu_filtering_entry *empty;

	rtu_hdr = (void *)rtu_port_shmem + rtu_port_shmem->data_off;
	htab_shm = wrs_shm_follow(rtu_port_shmem, rtu_hdr->filters);
	if (!htab_shm)
		return -2;

	/* Read data, with the sequential lock to have all data consistent */
	while (1) {
		ii = wrs_shm_seqbegin(rtu_port_shmem);
		memcpy(&rtu_htab_local, htab_shm,
		       RTU_BUCKETS * HTAB_ENTRIES * sizeof(*htab_shm));
		retries++;
		if (retries > 100)
			return -1;
		if (!wrs_shm_seqretry(rtu_port_shmem, ii))
			break; /* consistent read */
		usleep(1000);
	}

	/* Convert hash table to ordered table. Table will be qsorted later,
	 * no need to qsort entire table */
	*read_entries = 0;
	empty = rtu_htab_local;
	for (ii = 0; ii < RTU_BUCKETS * HTAB_ENTRIES; ii++) {
		if (rtu_htab_local[ii].valid) {
			memcpy(empty, &rtu_htab_local[ii], sizeof(*htab_shm));
			empty++;
			(*read_entries)++;
		}
	}

	return 0;
}

int open_rtu_shm(void)
{
	int n_wait = 0;
	int ret;
	/* open rtu shm */
	while ((ret = wrs_shm_get_and_check(wrs_shm_rtu, &rtu_port_shmem)) != 0) {
		n_wait++;
		if (n_wait > 10) {
			if (ret == 1) {
				fprintf(stderr, "rtu_stat: Unable to open "
					"RTUd's shm !\n");
			}
			if (ret == 2) {
				fprintf(stderr, "rtu_stat: Unable to read "
					"RTUd's version!\n");
			}
			exit(1);
		}
		sleep(1);
	}

	/* check rtu shm version */
	if (rtu_port_shmem->version != RTU_SHMEM_VERSION) {
		fprintf(stderr, "rtu_stat: unknown rtud's version %i "
			"(known is %i)\n", rtu_port_shmem->version,
			RTU_SHMEM_VERSION);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	int i, isok;
	int nports;
	int htab_read_entries;
	int vid_active = 0;
	char mac_buf[ETH_ALEN_STR];
	int n_wait = 0;

	nports = get_nports_from_hal();

	while (!(rtud_ch = minipc_client_create("rtud", 0)))
	{
		if (n_wait > 5) {
			/* Don't print first 5 times, it may take few seconds
			 * for rtud to start */
			fprintf(stderr, "rtu_stat: Can't connect to RTUd "
				"mini-rpc server\n");
		}
		n_wait++;
		sleep(1);
	}
	minipc_set_logfile(rtud_ch,stderr);

	/* Open rtud's shmem, it should be available after connecting rtud's
	 * minipc */
	if (open_rtu_shm()) {
		fprintf(stderr, "rtu_stat: Can't open RTUd shmem\n");
		return -1;
	}

	isok=0;
	if(argc>1)
	{
		if(strcmp(argv[1], "remove")==0)
		{
			i=atoidef(argv[2],-1);
			if((0 <= i && i < 18) && (rtudexp_clear_entries(i,atoidef(argv[3],0))==0))	isok=1;
			else printf("Could not %s entry for wr%d\n",argv[1],i);
		}
		else if(strcmp(argv[1], "add")==0)
		{
			if((argc > 3) && (rtudexp_add_entry(argv[2],atoi(argv[3]),atoidef(argv[4],0))==0)) isok=1;
			else printf("Could not %s entry for %s\n",argv[2],argv[3]);
		}
		else if(strcmp(argv[1], "vlan")==0)
		{
			if((argc > 3 ) && (rtudexp_vlan_entry(atoi(argv[2]),atoi(argv[3]),argv[4],
					    atoidef(argv[5],0),atoidef(argv[6],0),atoidef(argv[7],0),
					    atoidef(argv[8],0))==0))  isok=1;
			else printf("Could not %s entry for %s\n",argv[2],argv[3]);
			exit(1);
		}
		else if(strcmp(argv[1], "list")==0) isok=1;

		//Does not continue
		if(!isok) show_help(argv[0]);


	}

	/* read filter entires from shm to local memory for data consistency */
	if (read_htab(&htab_read_entries)) {
		printf("Too many retries while reading htab entries from RTUd "
		       "shmem\n");
		return -1;
	}

	qsort(rtu_htab_local, htab_read_entries,
	      sizeof(struct rtu_filtering_entry), cmp_rtu_entries);

	printf("RTU Filtering Database Dump: %d rules\n", htab_read_entries);
	printf("\n");
	printf("MAC                     Dst.ports      FID          Type               Age [s]\n");
	printf("----------------------------------------------------------------------------------\n");

	for (i = 0; i < htab_read_entries; i++)
	{
		if (!rtu_htab_local[i].valid)
			continue;
		printf("%-25s %-12s %2d          %s (hash %03x:%x)   ",
			mac_to_buffer(rtu_htab_local[i].mac, mac_buf),
			decode_ports(rtu_htab_local[i].port_mask_dst, nports),
			rtu_htab_local[i].fid,
			rtu_htab_local[i].dynamic ? "DYNAMIC" : "STATIC ",
			rtu_htab_local[i].addr.hash,
			rtu_htab_local[i].addr.bucket);
		if (rtu_htab_local[i].dynamic)
			printf("%d\n", rtu_htab_local[i].age);
		else
			printf("-\n");
	}
	printf("\n");

	/* read vlans from shm to local memory for data consistency */
	if (read_vlans()) {
		printf("Too many retries while reading vlans from RTUd "
		       "shmem\n");
		return -1;
	}

	printf("RTU VLAN Table Dump:\n");
	printf("\n");
	printf("  VID    FID       MASK       DROP    PRIO    PRIO_OVERRIDE\n");
	printf("-----------------------------------------------------------\n");

	for (i = 0; i < NUM_VLANS; i++) {
		if ((vlan_tab_local[i].drop != 0)
		     && (vlan_tab_local[i].port_mask == 0x0))
			continue;

		printf("%4d   %4d      0x%8x    ", i, vlan_tab_local[i].fid,
		       vlan_tab_local[i].port_mask);
		if (vlan_tab_local[i].drop == 0)
			printf("NO ");
		else
			printf("YES");
		if (vlan_tab_local[i].has_prio == 0)
			printf("     --    ");
		else
			printf("     %1d    ", vlan_tab_local[i].prio);

		if (vlan_tab_local[i].prio_override == 0)
			printf("     NO ");
		else
			printf("     YES ");
		printf("\n");
		vid_active++;
	}
	printf("\n");
	printf("%d active VIDs defined\n", vid_active);
	printf("\n");

	return 0;
}
