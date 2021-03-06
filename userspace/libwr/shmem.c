/* Alessandro Rubini for CERN 2014, LGPL-2.1 or later */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <libwr/shmem.h>
#include <libwr/util.h>

#define SHM_LOCK_TIMEOUT_MS 50 /* in ms */

static char wrs_shm_path[50] = WRS_SHM_DEFAULT_PATH;
static int wrs_shm_locked = WRS_SHM_LOCKED;

/* Set custom path for shmem */
void wrs_shm_set_path(char *new_path)
{
	strncpy(wrs_shm_path, new_path, 50);
}

/* Allow to ignore the flag WRS_SHM_LOCKED
 * If this flag is not ignored then function wrs_shm_get_and_check is not able
 * to open shmem successfully due to lack of process running with the given pid
 */
void wrs_shm_ignore_flag_locked(int ignore_flag)
{
	if (ignore_flag)
		wrs_shm_locked = 0;
	else
		wrs_shm_locked = WRS_SHM_LOCKED;
}

/* Get wrs shared memory */
/* return NULL and set errno on error */
void *wrs_shm_get(enum wrs_shm_name name_id, char *name, unsigned long flags)
{
	struct wrs_shm_head *head;
	struct stat stbuf;
	uint64_t tv1, tv2;
	void *map;
	char fname[64];
	int write_access = flags & WRS_SHM_WRITE;
	int fd;

	if (name_id >= WRS_SHM_N_NAMES) {
		errno = EINVAL;
		return NULL;
	}

	sprintf(fname, "%.50s/"WRS_SHM_FILE, wrs_shm_path, name_id);
	fd = open(fname, O_RDWR | O_CREAT | O_SYNC, 0644);
	if (fd < 0)
		return NULL; /* keep errno */
	/* The file may be too short: enlarge it to the minimum size */

	if (fstat(fd, &stbuf) < 0)
		return NULL; /* keep errno */
	if (stbuf.st_size < WRS_SHM_MIN_SIZE) {
		lseek(fd, WRS_SHM_MIN_SIZE - 1, SEEK_SET);
		write(fd, "", 1);
	}

	map = mmap(0, WRS_SHM_MAX_SIZE,
		   PROT_READ | (write_access ? PROT_WRITE : 0),
		   MAP_SHARED, fd, 0);
	if (map == MAP_FAILED)
		return NULL; /* keep errno */
	head = map;

	if (!write_access) {
		/* This is a reader: if locked, wait for a writer */
		if (!(flags & wrs_shm_locked))
			return map;

		tv1 = get_monotonic_tics();
		while (1) {
			/* Releasing does not mean initial data is in place! */
			/* Read data with wrs_shm_seqbegin and
			   wrs_shm_seqend! */
			if (head->pid && kill(head->pid, 0) == 0)
				return map;

			usleep(10 * 1000);
			tv2 = get_monotonic_tics();
			if (((tv2 - tv1) / 1000) < SHM_LOCK_TIMEOUT_MS)
				continue;

			errno = ETIMEDOUT;
			return NULL;
		}
	}

	/* Writer: init the fields */
	if (head->pid && kill(head->pid, 0) == 0) {
		munmap(map, WRS_SHM_MAX_SIZE);
		errno = EBUSY;
		return NULL;
	}
	head->fd = fd;
	head->sequence = 1; /* a sort of lock */
	head->mapbase = head;
	strncpy(head->name, name, sizeof(head->name));
	head->name[sizeof(head->name) - 1] = '\0';
	head->stamp = 0;
	head->data_off = sizeof(*head);
	head->data_size = 0;
	if (flags & wrs_shm_locked)
		head->sequence = 1; /* a sort of lock */
	else
		head->sequence = 0;

	head->pid = getpid(); /* getpid() is a memory barrier, too */
	head->pidsequence++;
	/* version and size are up to the user (or to allocation) */

	return map;
}

/* Put wrs shared memory */
/* return 0 on success, !0 on error */
int wrs_shm_put(void *headptr)
{
	struct wrs_shm_head *head = headptr;
	int err;
	if (head->pid == getpid()) {
		head->pid = 0; /* mark that we are not writers any more */
		close(head->fd);
	}
	if ((err = munmap(headptr, WRS_SHM_MAX_SIZE)) < 0)
		return err;
	return 0;
}

/* Open shmem and check if data is available
 * return 0 when ok, otherwise error
 * 1 when openning shmem failed
 * 2 when version is 0 */
int wrs_shm_get_and_check(enum wrs_shm_name shm_name,
				 struct wrs_shm_head **head)
{
	int ii;
	int version;

	/* try to open shmem */
	if (!(*head) && !(*head = wrs_shm_get(shm_name, "",
					WRS_SHM_READ | WRS_SHM_LOCKED))) {
		return 1;
	}

	ii = wrs_shm_seqbegin(*head);
	/* read head version */
	version = (*head)->version;
	if (wrs_shm_seqretry(*head, ii) || !version) {
		/* data in shmem available and version not zero */
		return 2;
	}

	/* all ok */
	return 0;
}

/* The writer can allocate structures that live in the area itself */
void *wrs_shm_alloc(void *headptr, size_t size)
{
	struct wrs_shm_head *head = headptr;
	void *nextptr;

	if (head->pid != getpid())
		return NULL; /* we are not writers */
	if (head->data_off + head->data_size + size > WRS_SHM_MAX_SIZE)
		return NULL; /* no space left */
	nextptr = headptr + head->data_off + head->data_size;
	head->data_size += (size + 7) & ~7; /* force 8-alignment */

	/* Before we write to shmem, ensure the backing store exists */
	lseek(head->fd, head->data_off + head->data_size - 1, SEEK_SET);
	write(head->fd, "", 1);

	memset(nextptr, 0, size);
	return nextptr;
}

/* The reader can track writer's pointers, if they are in the area */
void *wrs_shm_follow(void *headptr, void *ptr)
{
	struct wrs_shm_head *head = headptr;

	if (ptr < head->mapbase || ptr > head->mapbase + WRS_SHM_MAX_SIZE)
		return NULL; /* not in the area */
	return headptr + (ptr - head->mapbase);
}

/* Before and after writing a chunk of data, act on sequence and stamp */
void wrs_shm_write(void *headptr, int flags)
{
	struct wrs_shm_head *head = headptr;

	if (flags == WRS_SHM_WRITE_END) {
		/* At end-of-writing update the timestamp too */
		head->stamp = get_monotonic_sec();
	}
	head->sequence++;
	return;
}

/* A reader can rely on the sequence number (in the <linux/seqlock.h> way) */
unsigned wrs_shm_seqbegin(void *headptr)
{
	struct wrs_shm_head *head = headptr;

	return head->sequence;
}

int wrs_shm_seqretry(void *headptr, unsigned start)
{
	struct wrs_shm_head *head = headptr;

	if (start & 1)
		return 1; /* it was odd: retry */

	return head->sequence != start;
}

/* A reader can check wether information is current enough */
int wrs_shm_age(void *headptr)
{
	struct wrs_shm_head *head = headptr;

	return get_monotonic_sec() - head->stamp;
}

/* A reader can get the information pointer, for a specific version, or NULL */
void *wrs_shm_data(void *headptr, unsigned version)
{
	struct wrs_shm_head *head = headptr;

	if (head->version != version)
		return NULL;

	return headptr + head->data_off;
}
