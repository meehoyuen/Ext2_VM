#ifndef _EXT2_SHELL_H_
#define _EXT2_SHELL_H_

#include "ext2.h"
#include "shell.h"

void shell_register_filesystem(SHELL_FILESYSTEM*);

int fs_dump(DISK_OPERATIONS* disk, int group, int type, int target);
int fs_dumpdata(DISK_OPERATIONS* disk, struct SHELL_FS_OPERATIONS* fsOprs, const SHELL_ENTRY* parent, const char* name);

#endif