#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <err.h>

#include <zlib.h>

#include "ius.h"

const char *strs[] = {
	"U0",
	"UBOOT",
	"RD",
	"RD_",
	"LK",
	"NK",
	"ADR_SYS",
	"ADR_USER",
	"NDISK",
	"ADR_AS",
};

static struct ius_header hdr;
static struct ius_entry entries[30];

static char *progname;
static char *outfile;

static void
write_entry(int i, const char *fname, FILE *ofp)
{
	char buf[512];
	FILE *ifp;
	size_t nr;

	if ((ifp=fopen(fname, "rb")) == NULL)
		errx(EXIT_FAILURE, "Unable to open file '%s' for entry %d!", fname, i);

	memset(buf, 0, sizeof(buf));

	while((nr=fread(buf, 1, sizeof(buf), ifp)) > 0) {
		hdr.datacrc = crc32(hdr.datacrc, (void*)buf, sizeof(buf));
		fwrite(buf, 1, sizeof(buf), ofp);
		memset(buf, 0, sizeof(buf));
	}

	fclose(ifp);
}

static int
find_type(const char *s)
{
	int i;

	for (i = 0; i < IUS_NUM_TYPES; i++)
		if (strcmp(strs[i], s) == 0)
			return i;

	return -1;
}

static void
parse_fwversion(char *s)
{
	int v[6] = { 0, 0, 0, 0, 0, 0 };
	int i = 0;
	char *p;

	while((p=strsep(&s,".")) != NULL)
		v[i++] = atoi(p) & 0xff;

	hdr.swversion = (v[0] << 16) | (v[4] << 8) | v[5];
	hdr.hwversion = (v[1] << 16) | (v[2] << 8) | v[3];
}

static void
usage(void)
{
	fprintf(stderr, "usage: %s [-o imagefile] [-m bmagic] [-v firmware-version] [part:file...]\n", progname);
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	int i, ch;
	off_t pos;
	FILE *fp;

	progname = argv[0];

	while ((ch = getopt(argc, argv, "v:o:m:")) != -1) {
		switch (ch) {
		case 'o':
			outfile = strdup(optarg);
			break;

		case 'm':
			hdr.boardmagic = strtoul(optarg, NULL, 0);
			break;

		case 'v':
			parse_fwversion(optarg);
			break;
				
		default:
			usage();
		}
	}

	argc -= optind;
	argv += optind;

	if (argc < 1) 
		usage();

	strcpy(hdr.magic, WUI_MAGIC);
	hdr.num_entries = argc;
	pos = sizeof(hdr) + sizeof(entries);

	for (i = 0; i < argc; i++) {
		char typestr[256];
		struct stat st;
		char *fname;
		int type;

		fname = strchr(argv[i], ':');
		if (!fname)
			errx(EXIT_FAILURE, "Missing colon in '%s'!", argv[i]);

		memset(typestr, 0, sizeof(typestr));
		memcpy(typestr, argv[i], fname - argv[i]);
		fname++;

		if (stat(fname,&st) != 0)
			err(EXIT_FAILURE, "stat");

		strcpy(entries[i].magic, WUI_MAGIC);
		type = find_type(typestr);
		if (type < 0)
			errx(EXIT_FAILURE, "Unknown type '%s' for file '%s'!", typestr, fname);
		entries[i].type = type;
		entries[i].start_sector = pos / 512UL;
		entries[i].num_sectors = (st.st_size + 511UL) / 512UL;
		pos += entries[i].num_sectors * 512UL;
	}

	fp=fopen(outfile, "w+");
	if (fp == NULL)
		errx(EXIT_FAILURE, "Unable to create image file '%s'!", outfile);

	fwrite(&hdr, sizeof(hdr), 1, fp);
	fwrite(entries, sizeof(entries), 1, fp);

	hdr.datacrc = crc32(0, (void*)entries, sizeof(entries));
	for (i=0; i < argc; i++) {
		char *key = argv[i], *val;
		val = strchr(key, ':');
		if (!val)
			errx(EXIT_FAILURE, "Unrecognized file '%s'!", argv[i]);
		*val++ = '\0';

		write_entry(i, val, fp);
	}

	hdr.datalen = ftell(fp) - 512UL;
	hdr.headercrc = crc32(0, (void*)&hdr, sizeof(hdr));

	fseek(fp, 0, SEEK_SET);
	fwrite(&hdr, sizeof(hdr), 1, fp);
	fwrite(entries, sizeof(entries), 1, fp);
	fclose(fp);

	exit(EXIT_SUCCESS);
}
