#include "wrsSnmp.h"
#include "wrsBootStatusGroup.h"

#define DOTCONFIGDIR "/tmp"
#define DOTCONFIG_PROTO "dot-config_proto"
#define DOTCONFIG_HOST "dot-config_host"
#define DOTCONFIG_FILENAME "dot-config_filename"

/* Macros for fscanf function to read line with maximum of "x" characters
 * without new line. Macro expands to something like: "%10[^\n]" */
#define LINE_READ_LEN_HELPER(x) "%"#x"[^\n]"
#define LINE_READ_LEN(x) LINE_READ_LEN_HELPER(x)

static struct pickinfo wrsBootStatus_pickinfo[] = {
	FIELD(wrsBootStatus_s, ASN_COUNTER, wrsBootCnt),
	FIELD(wrsBootStatus_s, ASN_COUNTER, wrsRebootCnt),
	FIELD(wrsBootStatus_s, ASN_INTEGER, wrsRestartReason),
	FIELD(wrsBootStatus_s, ASN_OCTET_STR, wrsFaultIP),
	FIELD(wrsBootStatus_s, ASN_OCTET_STR, wrsFaultLR),
	FIELD(wrsBootStatus_s, ASN_INTEGER, wrsConfigSource),
	FIELD(wrsBootStatus_s, ASN_OCTET_STR, wrsConfigSourceHost),
	FIELD(wrsBootStatus_s, ASN_OCTET_STR, wrsConfigSourceFilename),
};

struct wrsBootStatus_s wrsBootStatus_s;

static void get_dotconfig_source(void)
{
	char buff[10];
	FILE *f;
	/* Check dotconfig source.
	 * dotconfig source can change in runtime, i.e. from remote to local by
	 * web-interface */
	/* read protocol used to get dotconfig */
	f = fopen(DOTCONFIGDIR "/" DOTCONFIG_PROTO, "r");
	if (f) {
		/* readline without newline */
		fscanf(f, LINE_READ_LEN(10), buff);
		fclose(f);
		if (!strncmp(buff, "tftp", 10))
			wrsBootStatus_s.wrsConfigSource =
						WRS_CONFIG_SOURCE_PROTO_TFTP;
		else if (!strncmp(buff, "http", 10))
			wrsBootStatus_s.wrsConfigSource =
						WRS_CONFIG_SOURCE_PROTO_HTTP;
		else if (!strncmp(buff, "ftp", 10))
			wrsBootStatus_s.wrsConfigSource =
						WRS_CONFIG_SOURCE_PROTO_FTP;
		else if (!strncmp(buff, "local", 10))
			wrsBootStatus_s.wrsConfigSource =
						WRS_CONFIG_SOURCE_PROTO_LOCAL;
		else /* unknown proto */
			wrsBootStatus_s.wrsConfigSource =
						WRS_CONFIG_SOURCE_PROTO_ERROR;
	} else {
		/* proto file not found, probably something else caused
		 * a problem */
		wrsBootStatus_s.wrsConfigSource =
					WRS_CONFIG_SOURCE_PROTO_ERROR_MINOR;
	}
	/* read host used to get dotconfig */
	f = fopen(DOTCONFIGDIR "/" DOTCONFIG_HOST, "r");
	if (f) {
		/* readline without newline */
		fscanf(f, LINE_READ_LEN(WRS_CONFIG_SOURCE_HOST_LEN),
		       wrsBootStatus_s.wrsConfigSourceHost);
		fclose(f);
	} else {
		/* host file not found, put "error" into wrsConfigSourceHost */
		strcpy(wrsBootStatus_s.wrsConfigSourceHost, "error");
	}

	/* read filename used to get dotconfig */
	f = fopen(DOTCONFIGDIR "/" DOTCONFIG_FILENAME, "r");
	if (f) {
		/* readline without newline */
		fscanf(f, LINE_READ_LEN(WRS_CONFIG_SOURCE_FILENAME_LEN),
		       wrsBootStatus_s.wrsConfigSourceFilename);
		fclose(f);
	} else {
		/* host file not found, put "error" into
		 * wrsConfigSourceFilename */
		strcpy(wrsBootStatus_s.wrsConfigSourceFilename, "error");
	}
}

time_t wrsBootStatus_data_fill(void)
{
	static time_t time_update;
	time_t time_cur;
	time_cur = time(NULL);

	if (time_update
	    && time_cur - time_update < WRSBOOTSTATUS_CACHE_TIMEOUT) {
		/* cache not updated, return last update time */
		return time_update;
	}
	time_update = time_cur;

	/* get dotconfig source information */
	get_dotconfig_source();

	/* there was an update, return current time */
	return time_update;
}

#define GT_OID WRSBOOTSTATUS_OID
#define GT_PICKINFO wrsBootStatus_pickinfo
#define GT_DATA_FILL_FUNC wrsBootStatus_data_fill
#define GT_DATA_STRUCT wrsBootStatus_s
#define GT_GROUP_NAME "wrsBootStatusGroup"
#define GT_INIT_FUNC init_wrsBootStatusGroup

#include "wrsGroupTemplate.h"
