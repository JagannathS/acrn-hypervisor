/*
 * Copyright (C) 2018 Intel Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <types.h>
#include <errno.h>
#include <multiboot.h>
#include <pgtable.h>
#include <dbg_cmd.h>
#include <logmsg.h>
#include <vboot.h>

#define DBG_LEVEL_PARSE		6

int32_t parse_hv_cmdline(void)
{
	const char *start;
	const char *end = NULL;
	struct multiboot_info *mbi = NULL;

	if (boot_regs[0] != MULTIBOOT_INFO_MAGIC) {
		return -EINVAL;
	}

	mbi = (struct multiboot_info *)(hpa2hva_early((uint64_t)boot_regs[1]));
	dev_dbg(DBG_LEVEL_PARSE, "Multiboot detected, flag=0x%x", mbi->mi_flags);

	if ((mbi->mi_flags & MULTIBOOT_INFO_HAS_CMDLINE) == 0U) {
		dev_dbg(DBG_LEVEL_PARSE, "no hv cmdline!");
		return -EINVAL;
	}

	start = (char *)hpa2hva_early((uint64_t)mbi->mi_cmdline);
	dev_dbg(DBG_LEVEL_PARSE, "hv cmdline: %s", start);

	while ((start != NULL) && ((*start) != '\0')) {
		while ((*start) == ' ')
			start++;
		if ((*start) != '\0') {
			end = start + 1;
			while ((*end != ' ') && ((*end) != '\0'))
				end++;

			if (!handle_dbg_cmd(start, (int32_t)(end - start))) {
				/* if not handled by handle_dbg_cmd, it can be handled further */
			}
			start = end;
		}
	}

	return 0;
}
