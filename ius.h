#ifndef IUS_H
#define IUS_H

#include <inttypes.h>

#define WUI_MAGIC	"WuI"

struct ius_header {
	char magic[4];		/* WUI_MAGIC */
	uint32_t boardmagic;	/* board specific magic */
	uint32_t hwversion;
	uint32_t num_entries;
	uint32_t datalen;
	uint32_t swversion;
	uint32_t headercrc;
	uint32_t datacrc;
};

struct ius_entry {
	char magic[4];		/* WUI_MAGIC */
	uint32_t type;		/* see table below */
	uint32_t start_sector;
	uint32_t num_sectors;	/* sectors are 512 bytes */
};

/* values for ius_entry::type */
enum {
	IUS_U0 = 0,
	IUS_UBOOT,
	IUS_RD,
	IUS_RD_,
	IUS_LK,
	IUS_NK,
	IUS_ADR_SYS,
	IUS_ADR_USER,
	IUS_NDISK,
	IUS_ADR_AS,
	IUS_NUM_TYPES
};

#endif /* IUS_H */
