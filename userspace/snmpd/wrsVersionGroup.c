/*
 *  White Rabbit Switch versions.
 *
 *  Based on work by Alessandro Rubini for CERN, 2014
 *  Adam Wujek
 */

#include "wrsSnmp.h"
#include "wrsVersionGroup.h"

/* Macros for fscanf function to read line with maximum of "x" characters
 * without new line. Macro expands to something like: "%10[^\n]" */
#define LINE_READ_LEN_HELPER(x) "%"#x"[^\n]"
#define LINE_READ_LEN(x) LINE_READ_LEN_HELPER(x)

#define VERSION_COMMAND "/wr/bin/wrs_version -t"
#define LAST_UPDATE_DATE_FILE "/update/last_update"

struct wrs_v_item {
	char *key;
};

static struct pickinfo wrsVersion_pickinfo[] = {
	FIELD(wrsVersion_s, ASN_OCTET_STR, wrsVersions[0]),
	FIELD(wrsVersion_s, ASN_OCTET_STR, wrsVersions[1]),
	FIELD(wrsVersion_s, ASN_OCTET_STR, wrsVersions[2]),
	FIELD(wrsVersion_s, ASN_OCTET_STR, wrsVersions[3]),
	FIELD(wrsVersion_s, ASN_OCTET_STR, wrsVersions[4]),
	FIELD(wrsVersion_s, ASN_OCTET_STR, wrsVersions[5]),
	FIELD(wrsVersion_s, ASN_OCTET_STR, wrsVersions[6]),
	FIELD(wrsVersion_s, ASN_OCTET_STR, wrsVersions[7]),
	FIELD(wrsVersion_s, ASN_OCTET_STR, wrsVersions[8]),
	FIELD(wrsVersion_s, ASN_OCTET_STR, wrsVersions[9]),
	FIELD(wrsVersion_s, ASN_OCTET_STR, wrsVersions[10]),
	FIELD(wrsVersion_s, ASN_OCTET_STR, wrsVersions[11]),
	FIELD(wrsVersion_s, ASN_OCTET_STR, wrsVersions[12]),
	FIELD(wrsVersion_s, ASN_OCTET_STR, wrsVersionLastUpdateDate),
};

struct wrsVersion_s wrsVersion_s;

static struct wrs_v_item wrs_version[] = {
	/* Warning: the order here must match the MIB file
	 * wrs_version has to have the same size as wrsVersion_s.wrsVersions */
	[0] = {.key = "software-version:"},
	[1] = {"bult-by:"},
	[2] = {"build-date:"},
	[3] = {"backplane-version:"},
	[4] = {"fpga-type:"},
	[5] = {"manufacturer:"},
	[6] = {"serial-number:"},
	[7] = {"scb-version:"},
	[8] = {"gateware-version:"},
	[9] = {"gateware-build:"},
	[10] = {"wr_switch_hdl-commit:"},
	[11] = {"general-cores-commit:"},
	[12] = {"wr-cores-commit:"},
};


static void get_last_update_date(void)
{
	FILE *f;
	f = fopen(LAST_UPDATE_DATE_FILE, "r");
	if (f) {
		/* readline without newline */
		fscanf(f, LINE_READ_LEN(32),
		       wrsVersion_s.wrsVersionLastUpdateDate);
		fclose(f);
	} else {
		snprintf(wrsVersion_s.wrsVersionLastUpdateDate, 32,
			 "0000.00.00-00:00:00");
	}
}

time_t wrsVersion_data_fill(void)
{
	char s[80], key[40], value[40];
	FILE *f;
	int i;
	int guess_index;
	static int run_once = 0;

	time_t time_cur;
	time_cur = get_monotonic_sec();

	/* assume that version does not change in runtime */
	if (run_once) {
		/* return time like there was an update */
		return time_cur;
	}

	run_once = 1;

	f = popen(VERSION_COMMAND, "r");
	if (!f) {
		snmp_log(LOG_ERR, "SNMP: wrsVersion filed to execute "
			 VERSION_COMMAND"\n");
		/* try again next time */
		run_once = 0;
		return time_cur;
	}

	guess_index = 0;
	while (fgets(s, sizeof(s), f)) {
		if (sscanf(s, "%s %[^\n]", key, value) != 2) {
			/* try again next time */
			run_once = 0;
			continue; /* error... */
		}

		/* try educated guess to find position in array */
		if (!strcmp(key, wrs_version[guess_index].key)) {
			strncpy(wrsVersion_s.wrsVersions[guess_index],
				value, 32);
			guess_index++;
			continue;
		}

		/* check all */
		for (i = 0; i < ARRAY_SIZE(wrs_version); i++) {
			if (strcmp(key, wrs_version[i].key))
				continue;
			strncpy(wrsVersion_s.wrsVersions[i], value, 32);
		}
		guess_index++;
	}
	pclose(f);

	get_last_update_date();
	return time_cur;
}

#define GT_OID WRSVERSION_OID
#define GT_PICKINFO wrsVersion_pickinfo
#define GT_DATA_FILL_FUNC wrsVersion_data_fill
#define GT_DATA_STRUCT wrsVersion_s
#define GT_GROUP_NAME "wrsVersionGroup"
#define GT_INIT_FUNC init_wrsVersionGroup

#include "wrsGroupTemplate.h"
