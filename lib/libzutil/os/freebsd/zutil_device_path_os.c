/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */

#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>

#include <libgeom.h>

#include <libzutil.h>

/*
 * We don't strip/append partitions on FreeBSD.
 */

char *
zfs_strip_partition(char *dev)
{
	return (dev);
}

char *
zfs_strip_partition_path(char *path)
{
	return (path);
}

int
zfs_append_partition(char *path, size_t max_len)
{
	return (max_len);
}

char *
zfs_get_underlying_path(const char *dev_name)
{
	/* implement me */
	abort();
	return (NULL);
}

boolean_t
zfs_dev_is_whole_disk(const char *dev_name)
{
	int fd;

	fd = g_open(dev_name, 0);
	if (fd >= 0) {
		g_close(fd);
		return (B_TRUE);
	}
	return (B_FALSE);
}

/*
 * Wait up to timeout_ms for udev to set up the device node.  The device is
 * considered ready when libudev determines it has been initialized, all of
 * the device links have been verified to exist, and it has been allowed to
 * settle.  At this point the device the device can be accessed reliably.
 * Depending on the complexity of the udev rules this process could take
 * several seconds.
 */
int
zpool_label_disk_wait(const char *path, int timeout_ms)
{
	int settle_ms = 50;
	long sleep_ms = 10;
	hrtime_t start, settle;
	struct stat64 statbuf;

	start = gethrtime();
	settle = 0;

	do {
		errno = 0;
		if ((stat64(path, &statbuf) == 0) && (errno == 0)) {
			if (settle == 0)
				settle = gethrtime();
			else if (NSEC2MSEC(gethrtime() - settle) >= settle_ms)
				return (0);
		} else if (errno != ENOENT) {
			return (errno);
		}

		usleep(sleep_ms * MILLISEC);
	} while (NSEC2MSEC(gethrtime() - start) < timeout_ms);

	return (ENODEV);
}

/* ARGSUSED */
boolean_t
is_mpath_whole_disk(const char *path)
{
	return (B_FALSE);
}
