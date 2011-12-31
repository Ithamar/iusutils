/*-
 * Copyright (c) 2011 Team Embedded. All rights reserved.
 * Written by Ithamar R. Adema.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <err.h>

#include "ius.h"

struct ius_entry_type {
	const char *filename;
	const char *desc;
} ius_types[] = {
	{ "u-boot-nand.bin", "U0" },
	{ "u-boot.img",      "UBOOT" },
	{ "boot.img",        "RD" },
	{ "recovery.img",    "RD_" },
	{ "bootkernel.img",  "LK" },
	{ "recvkernel.img",  "NK" },
	{ "zSYS.img",        "ADR_SYS" },
	{ "userdata.img",    "ADR_USER" },
	{ "ndisk.bin",       "NDISK" },
	{ "system.img",      "ADR_AS" },
};

const char *progname;
int strip_uboothdr = 0;

static void
write_image(const char *path, uint8_t *buf, uint32_t size)
{
	int fd;

	if (strip_uboothdr && buf[0] == 0x27 && buf[1] == 0x05 &&
		buf[2] == 0x19 && buf[3] == 0x56) {
		buf += 64;
		size -= 64;
	}

	if ((fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1 ||
	    write(fd, buf, size) == -1 || close(fd) == -1)
		err(EXIT_FAILURE, "%s", path);
}

static void
usage(void)
{
	fprintf(stderr, "usage: [-s] %s image\n", progname);
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	off_t size;
	int fd, ch;
	void *buf;

	progname = argv[0];

	while ((ch = getopt(argc, argv, "s")) != -1) {
		switch (ch) {
		case 's':
			strip_uboothdr = 1;
			break;
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;

	if (argc != 1) 
		usage();

	if ((fd = open(argv[0], O_RDONLY)) == -1)
		err(EXIT_FAILURE, "%s", argv[0]);

	if ((size = lseek(fd, 0, SEEK_END)) == -1)
		err(EXIT_FAILURE, "%s", argv[0]);

	if ((buf = mmap(NULL, size, PROT_READ, MAP_SHARED | MAP_FILE, fd, 0))
	    == MAP_FAILED)
		err(EXIT_FAILURE, "%s", argv[0]);

	if (memcmp(buf, WUI_MAGIC, 4) != 0)
		errx(EXIT_FAILURE, "invalid signature");

	{
		struct ius_header* hdr = buf;
		struct ius_entry* ent = (buf + sizeof(struct ius_header));
		long idx;

		printf("Firmware version: %d.%d.%d.%d.%d.%d\n", hdr->swversion >> 16,
			hdr->hwversion >> 16, (hdr->hwversion >> 8) & 0xff, hdr->hwversion & 0xff,
			(hdr->swversion >> 8) & 0xff, hdr->swversion & 0xff);

		printf("Boardmagic: 0x%08x\n\n", hdr->boardmagic);

		for (idx = 0; idx < hdr->num_entries; idx++, ent++) {
			printf("%s:%s\n",
				ius_types[ent->type].desc,
				ius_types[ent->type].filename);

			write_image(ius_types[ent->type].filename, buf + (ent->start_sector * 512L), ent->num_sectors * 512L);
		}
	}

	printf("\nunpacked\n");

	munmap(buf, size);
	close(fd);

	return EXIT_SUCCESS;
}
