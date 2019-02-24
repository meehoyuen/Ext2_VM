/******************************************************************************/
/*                                                                            */
/* Project : Ext2 File System                                                 */
/* File    : ext2_shell.c                                                     */
/* Author  : Youngjoon Choi(venbo12@gmail.com), Jimin Back(inb7dni@gmail.com) */
/* Company : Soongsil Univ. Network Computing Lab.                            */
/* Notes   : Common macros header                                             */
/* Date    : 2019/2/16                                                        */
/*                                                                            */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "ext2_shell.h"

#define FSOPRS_TO_EXT2FS( a )      ( EXT2_FILESYSTEM* )a->pdata

int fs_read(DISK_OPERATIONS* disk, SHELL_FS_OPERATIONS* fsOprs, const SHELL_ENTRY* parent, SHELL_ENTRY* entry, unsigned long offset, unsigned long length, const char* buffer); 
int fs_write(DISK_OPERATIONS* disk, SHELL_FS_OPERATIONS* fsOprs, const SHELL_ENTRY* parent, SHELL_ENTRY* entry, unsigned long offset, unsigned long length, const char* buffer); 
int	fs_create(DISK_OPERATIONS* disk, SHELL_FS_OPERATIONS* fsOprs, const SHELL_ENTRY* parent, const char* name, SHELL_ENTRY* retEntry);
int fs_remove(DISK_OPERATIONS* disk, SHELL_FS_OPERATIONS* fsOprs, const SHELL_ENTRY* parent, const char* name); 
int fs_lookup(DISK_OPERATIONS* disk, SHELL_FS_OPERATIONS* fsOprs, const SHELL_ENTRY* parent, SHELL_ENTRY* entry, const char* name); 
int fs_read_dir(DISK_OPERATIONS* disk, SHELL_FS_OPERATIONS* fsOprs, const SHELL_ENTRY* parent, SHELL_ENTRY_LIST* list); 
int is_exist(DISK_OPERATIONS* disk, SHELL_FS_OPERATIONS* fsOprs, const SHELL_ENTRY* parent, const char* name); 
int fs_mkdir(DISK_OPERATIONS* disk, SHELL_FS_OPERATIONS* fsOprs, const SHELL_ENTRY* parent, const char* name, SHELL_ENTRY* retEntry); 
int fs_rmdir(DISK_OPERATIONS* disk, SHELL_FS_OPERATIONS* fsOprs, const SHELL_ENTRY* parent, const char* name, SHELL_ENTRY* retEntry);
int fs_format(DISK_OPERATIONS* disk, void* param); 
int fs_mount(DISK_OPERATIONS* disk, SHELL_FS_OPERATIONS* fsOprs, SHELL_ENTRY* root); 
void fs_umount(DISK_OPERATIONS* disk, SHELL_FS_OPERATIONS* fsOprs); 
int fs_stat(DISK_OPERATIONS* disk, SHELL_FS_OPERATIONS* fsOprs, unsigned int* totalSectors, unsigned int* usedSectors); 
int fs_dump(DISK_OPERATIONS*, int, int, int);
int fs_dumpdata(DISK_OPERATIONS*, struct SHELL_FS_OPERATIONS*, const SHELL_ENTRY*, const char*);

char* my_strncpy(char* dest, const char* src, int length)
{
	while (*src && *src != 0x20 && length-- > 0) 
		*dest++ = *src++;

	return dest; 
}

int my_strnicmp(const char* str1, const char* str2, int length) 
{
	char	c1, c2;

	while (((*str1 && *str1 != 0x20) || (*str2 && *str2 != 0x20)) && length-- > 0)
	{
		c1 = toupper(*str1);
		c2 = toupper(*str2);

		if (c1 > c2)
			return -1;
		else if (c1 < c2)
			return 1;

		str1++;
		str2++;
	}

	return 0;
}

int shell_entry_to_ext2_entry(const SHELL_ENTRY* shell_entry, EXT2_NODE* ext2_entry)
{
	EXT2_NODE* entry = (EXT2_NODE*)shell_entry->pdata;

	*ext2_entry = *entry; 

	return EXT2_SUCCESS;
}

int ext2_entry_to_shell_entry(EXT2_FILESYSTEM* fs, const EXT2_NODE* ext2_entry, SHELL_ENTRY* shell_entry) 
{
	EXT2_NODE* entry = (EXT2_NODE*)shell_entry->pdata; 
	EXT2_INODE inodeBuffer; 

	ZeroMemory(shell_entry, sizeof(SHELL_ENTRY));

	int inode = ext2_entry->entry.inode;

	if (get_inode(fs, inode, &inodeBuffer)) 
	{
		printf("error : failed to get inode\n");
		return EXT2_ERROR;
	}

	memcpy(shell_entry->name, ext2_entry->entry.name, ext2_entry->entry.dir2.nameLength);


	if (EXT2_FT_DIR == ext2_entry->entry.dir2.fileType) // 디렉터리인 경우
	{
//		printf("EXT2_FT_DIR : %d\next2_entry->entry.dir2.fileType : %d\n", EXT2_FT_DIR, ext2_entry->entry.dir2.fileType);
		shell_entry->isDirectory = 1;
	}

	shell_entry->size = inodeBuffer.fileSize; 


	*entry = *ext2_entry; 

	return EXT2_SUCCESS;
}

static SHELL_FILE_OPERATIONS g_file =
{
	fs_create,
	fs_remove,
	fs_read,
	fs_write
};

static SHELL_FS_OPERATIONS   g_fsOprs =
{
	fs_read_dir,
	fs_stat,
	fs_mkdir,
	fs_rmdir,
	fs_lookup,
	fs_dump,
	fs_dumpdata,
	&g_file,
	NULL
};

static SHELL_FILESYSTEM g_ext2 =
{
	"EXT2",
	fs_mount,
	fs_umount,
	fs_format
};

void shell_register_filesystem(SHELL_FILESYSTEM* fs) 
{
	*fs = g_ext2;
}

int adder(EXT2_FILESYSTEM* fs, void* list, EXT2_NODE* entry) /* entry를 lsit에 추가 */
{
	SHELL_ENTRY_LIST*   entryList = (SHELL_ENTRY_LIST*)list;
	SHELL_ENTRY         newEntry;

	ext2_entry_to_shell_entry(fs, entry, &newEntry); /* SHELL_ENTRY로 변환 후 */
printf("newEntry.size : %d\n", newEntry.size);	
	add_entry_list(entryList, &newEntry); /* ENTRY_LIST에 추가 */

	return EXT2_SUCCESS;
}

int fs_read(DISK_OPERATIONS* disk, SHELL_FS_OPERATIONS* fsOprs, const SHELL_ENTRY* parent, SHELL_ENTRY* entry, unsigned long offset, unsigned long length, const char* buffer) /* 파일 읽기 */
{
	EXT2_NODE EXT2Entry;

	shell_entry_to_ext2_entry(entry, &EXT2Entry); /* EXT2_ENTRY로 변환 후 */

	return ext2_read(&EXT2Entry, offset, length, buffer); /* offset 위치부터 length만큼 buffer로 읽어옴 */
}

int fs_write(DISK_OPERATIONS* disk, SHELL_FS_OPERATIONS* fsOprs, const SHELL_ENTRY* parent, SHELL_ENTRY* entry, unsigned long offset, unsigned long length, const char* buffer) /* 파일 쓰기 */
{
	EXT2_NODE EXT2Entry;

	shell_entry_to_ext2_entry(entry, &EXT2Entry); /* EXT2_ENTRY로 변환 후 */
	
	return ext2_write(&EXT2Entry, offset, length, buffer); /* offset 위치부터 length만큼 buffer로 읽어옴 */
}

int	fs_create(DISK_OPERATIONS* disk, SHELL_FS_OPERATIONS* fsOprs, const SHELL_ENTRY* parent, const char* name, SHELL_ENTRY* retEntry) /* 파일 생성 */
{
	EXT2_NODE	EXT2Parent;
	EXT2_NODE	EXT2Entry;
	int			result;

	shell_entry_to_ext2_entry(parent, &EXT2Parent); /* EXT2_ENTRY로 변환 후 */
	result = ext2_create(&EXT2Parent, name, &EXT2Entry); /* 파일명을 FAT_ENTRY 형식에 맞게 수정한 뒤 해당 이름을 가지는 엔트리가 존재하지 않으면 부모 디렉터리에 추가해줌 */
	ext2_entry_to_shell_entry(EXT2Parent.fs, &EXT2Entry, retEntry); /* SHELL_ENTRY로 변환 */

	return result;
}

int fs_remove(DISK_OPERATIONS* disk, SHELL_FS_OPERATIONS* fsOprs, const SHELL_ENTRY* parent, const char* name) /* 파일 삭제 */
{
	EXT2_NODE	EXT2Parent;
	EXT2_NODE	EXT2Entry;

	shell_entry_to_ext2_entry(parent, &EXT2Parent); /* EXT2_ENTRY로 변환 후 */
	ext2_lookup(&EXT2Parent, name, &EXT2Entry); /* 현재 디렉터리에서 해당 파일을 찾음 */

	return ext2_remove(&EXT2Entry); /* 찾은 파일을 삭제 */
}

int fs_lookup(DISK_OPERATIONS* disk, SHELL_FS_OPERATIONS* fsOprs, const SHELL_ENTRY* parent, SHELL_ENTRY* entry, const char* name) /* 현재 디렉터리에서 해당 파일을 찾음 */
{
	EXT2_NODE	EXT2Parent;
	EXT2_NODE	EXT2Entry;
	int			result;

	shell_entry_to_ext2_entry(parent, &EXT2Parent); /* EXT2_ENTRY로 변환 후 */

	if (result = ext2_lookup(&EXT2Parent, name, &EXT2Entry)) /* 현재 디렉터리에서 해당 파일을 찾음 */
		return result;

	ext2_entry_to_shell_entry(EXT2Parent.fs, &EXT2Entry, entry); /* SHELL_ENTRY로 변환 */

	return result;
}

int fs_read_dir(DISK_OPERATIONS* disk, SHELL_FS_OPERATIONS* fsOprs, const SHELL_ENTRY* parent, SHELL_ENTRY_LIST* list) /* 디렉터리의 엔트리들을 list에 담음 */
{
	EXT2_NODE   entry;

	if (list->count) /* list를 비우고 */
		release_entry_list(list);

	shell_entry_to_ext2_entry(parent, &entry); /* EXT2_ENTRY로 변환 */
	ext2_read_dir(&entry, adder, list); /* 디렉터리의 엔트리들을 list에 담음 */

	return EXT2_SUCCESS;
}

int is_exist(DISK_OPERATIONS* disk, SHELL_FS_OPERATIONS* fsOprs, const SHELL_ENTRY* parent, const char* name) /* 현재 디렉터리에 해당 이름의 엔트리가 존재하는지 검사 */
{
	SHELL_ENTRY_LIST		list;
	SHELL_ENTRY_LIST_ITEM*  current;

	init_entry_list(&list); /* ENTRY_LIST 초기화 */
	fs_read_dir(disk, fsOprs, parent, &list); /* 디렉터리의 엔트리들을 list에 담음 */
	current = list.first; /* list의 첫 엔트리부터 */

	while (current) /* 마지막 엔트리까지 name의 엔트리가 있는지 검사*/
	{
		if (my_strnicmp((char*)current->entry.name, name, 12) == 0) /* 있으면 */
		{
			release_entry_list(&list); /* ENTRY_LIST 삭제 후 에러 리턴 */
			return EXT2_ERROR;
		}

		current = current->next; 
	}
	release_entry_list(&list); /* ENTRY_LIST 삭제 후 성공 리턴 */

	return EXT2_SUCCESS;
}

int fs_mkdir(DISK_OPERATIONS* disk, SHELL_FS_OPERATIONS* fsOprs, const SHELL_ENTRY* parent, const char* name, SHELL_ENTRY* retEntry) /* 디렉터리 생성 */
{
	EXT2_FILESYSTEM* ext2;
	EXT2_NODE EXT2_Parent;
	EXT2_NODE EXT2_Entry;
	int result;

	ext2 = (EXT2_FILESYSTEM*)fsOprs->pdata;

	if (is_exist(disk, fsOprs, parent, name)) /* 해당 이름을 가진 엔트리가 이미 존재하면 에러 */
		return EXT2_ERROR;

	shell_entry_to_ext2_entry(parent, &EXT2_Parent); /* EXT2_ENTRT로 변환 */
	result = ext2_mkdir(&EXT2_Parent, name, &EXT2_Entry); /* name을 가지는 엔트리 생성 */
	ext2_entry_to_shell_entry(ext2, &EXT2_Entry, retEntry); /* SHELL_ENTRY로 변환 */

	return result;
}

int fs_rmdir(DISK_OPERATIONS* disk, SHELL_FS_OPERATIONS* fsOprs, const SHELL_ENTRY* parent, const char* name, SHELL_ENTRY* retEntry)
{
	EXT2_NODE EXT2_Parent;
	EXT2_NODE EXT2_Entry;

	shell_entry_to_ext2_entry(parent, &EXT2_Parent); /* EXT2_ENTRT로 변환 */
	ext2_lookup(&EXT2_Parent, name, &EXT2_Entry); /* 해당 이름을 가진 엔트리의 위치를 찾음 */

	return ext2_rmdir(&EXT2_Entry); 
}

int fs_format(DISK_OPERATIONS* disk, void* param) /* 수퍼 블록 설정, 블록 그룹 영역 할당, 루트 디렉터리 생성 */
{
	printf("formatting as a %s\n\n", (char *)param);
	ext2_format(disk);

	return  1;
}

int fs_mount(DISK_OPERATIONS* disk, SHELL_FS_OPERATIONS* fsOprs, SHELL_ENTRY* root) /* sb_info 구조체 변수 설정 */
{

	EXT2_FILESYSTEM* fs;
	EXT2_NODE ext2_entry;
	EXT2_SB_INFO* sb_info;
	EXT2_SUPER_BLOCK* sb;

	*fsOprs = g_fsOprs;

	fsOprs->pdata = malloc(sizeof(EXT2_FILESYSTEM));
	fs = FSOPRS_TO_EXT2FS(fsOprs);
	ZeroMemory(fs, sizeof(EXT2_FILESYSTEM));
	fs->disk = disk;
	
	sb = &fs->sb;

	if (ext2_read_superblock(fs, &ext2_entry)) /* superblock을 fs로 읽어와서 유효성 검사 후 */
		return EXT2_ERROR; 

	sb_info = &fs->sb_info;

	if (fill_sb_info(fs)) /* sb_info 영역의 값 설정 */
		return EXT2_ERROR;
	
	printf("\nnumber of groups         	: %u\n", sb_info->groupCount);
	printf("blocks per group         	: %u\n", sb_info->blocksPerGroup);
	printf("block byte size          	: %u\n", sb_info->blockSize);
	printf("inode byte size			: %u\n", sb_info->inodeSize);
	printf("free block count		: %u\n", sb_info->freeBlockCount);
	printf("free inode count		: %u\n", sb_info->freeInodeCount);
	printf("first non reserved inode 	: %u\n", sb_info->firstInode);
	printf("first data block number  	: %u\n", sb->firstDataBlock);

	ext2_entry_to_shell_entry(fs, &ext2_entry, root);

	return EXT2_SUCCESS;
}

void fs_umount(DISK_OPERATIONS* disk, SHELL_FS_OPERATIONS* fsOprs) /* mount 해제 */
{
	if (fsOprs && fsOprs->pdata)
	{
		ext2_umount(FSOPRS_TO_EXT2FS(fsOprs));
		free(fsOprs->pdata);
		fsOprs->pdata = 0; 
	}
	return;
}

int fs_stat(DISK_OPERATIONS* disk, SHELL_FS_OPERATIONS* fsOprs, unsigned int* totalSectors, unsigned int* usedSectors)
{
	EXT2_NODE entry;

	return ext2_df(FSOPRS_TO_EXT2FS(fsOprs), totalSectors, usedSectors);
}


/******************************************************************************/
/* dump										                                  */
/******************************************************************************/

int fs_dump(DISK_OPERATIONS* disk, int group, int type, int target)
{
	return ext2_dump(disk, group, type, target);
}

int fs_dumpdata(DISK_OPERATIONS* disk, struct SHELL_FS_OPERATIONS* fsOprs, const SHELL_ENTRY* parent, const char* name)
{
	EXT2_NODE ext2_parent;
	EXT2_NODE ext2_entry;
	EXT2_INODE inode;
	int i;

	if (is_exist(disk, fsOprs, parent, name) != EXT2_SUCCESS)
		return EXT2_ERROR;

	shell_entry_to_ext2_entry(parent, &ext2_parent);
	ext2_lookup(&ext2_parent, name, &ext2_entry);
	//get_inode(&inode, ext2_entry.entry.inode);

	printf("inode number\t: %u\n", ext2_entry.entry.inode);
	for (i = 0; i < inode.blockCount; i++)
		printf("i_block[i]\t: %u\n", inode.i_block[i]);

	return EXT2_SUCCESS;
}