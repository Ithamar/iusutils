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
