/******************************************************************************/
/*                                                                            */
/* Project : Ext2 File System                                                 */
/* File    : ext2.h                                                           */
/* Author  : Youngjoon Choi(venbo12@gmail.com), Jimin Back(inb7dni@gmail.com) */
/* Company : Soongsil Univ. Network Computing Lab.                            */
/* Notes   : Common macros header                                             */
/* Date    : 2019/2/16                                                        */
/*                                                                            */
/******************************************************************************/

#ifndef _EXT2_H_
#define _EXT2_H_

#include "common.h"
#include "disk.h"

#define VOLUME_LABEL			"EXT2 BY YJM"
#define VOLUME_LABEL_LENGTH		11

#define MAX_SECTOR_SIZE			512
#define MAX_NAME_LENGTH			256
#define MAX_ENTRY_NAME_LENGTH	24
#define EXT2_MIN_BLOCK_SIZE		1024
#define	EXT2_MAX_BLOCK_SIZE		4096
#define EXT2_MIN_BLOCK_LOG_SIZE	10

#define EXT2_BLOCK_SIZE_BIT		0
#define EXT2_BLOCK_SIZE			1024		
#define EXT2_SUPER_BLOCK_SIZE	1024
#define EXT2_DESC_SIZE			32
#define EXT2_INODE_SIZE			128

#define COUNT_UP				0x01
#define COUNT_DOWN				0x00

/* reserved inode numbers */
#define	EXT2_BAD_INO			1	/* Bad blocks inode */
#define EXT2_ROOT_INO		 	2	/* Root inode */
#define EXT2_BOOT_LOADER_INO	5	/* Boot loader inode */
#define EXT2_UNDEL_DIR_INO	 	6	/* Undelete directory inode */

/* First non-reserved inode for old ext2 filesystems */
#define EXT2_GOOD_OLD_FIRST_INO	11

/* file system state */
#define EXT2_VALID_FS			0x0001 /* normality */
#define EXT2_ERROR_FS			0x0002 /* abnormality */

/* Behaviour when detecting errors */
#define EXT2_ERRORS_CONTINUE	0
#define EXT2_ERRORS_RO			1
#define EXT2_ERRORS_PANIC		2
#define EXT2_ERRORS_DEFAULT		EXT2_ERRORS_CONTINUE

/* os ID mounting file system */
#define EXT2_OS_LINUX 			0
#define EXT2_OS_HURD			1
#define EXT2_OS_MASIX			2
#define EXT2_OS_FREEBSD			3
#define EXT2_OS_LITES			4

/* major version of file system */
#define EXT2_ORIGINAL_VERSION	0 /* fixed inode size */
#define EXT2_DYNAMIC_VERSION	1 /* unfixed inode size */

/* compatible feature flags */
#define EXT2_FEATURE_COMPAT_DIR_PREALLOC	0x0001
#define EXT2_FEATURE_COMPAT_IMAGIC_INODES	0x0002
#define EXT3_FEATURE_COMPAT_HAS_JOURNAL		0x0004
#define EXT2_FEATURE_COMPAT_EXT_ATTR		0x0008
#define EXT2_FEATURE_COMPAT_RESIZE_INO		0x0010
#define EXT2_FEATURE_COMPAT_DIR_INDEX		0x0020
#define EXT2_FEATURE_COMPAT_ANY				0xFFFFFFFF

/* incompatible feature flags */
#define EXT2_FEATURE_INCOMPAT_COMPRESSION	0x0001
#define EXT2_FEATURE_INCOMPAT_FILETYPE		0x0002
#define EXT3_FEATURE_INCOMPAT_RECOVER		0x0004
#define EXT3_FEATURE_INCOMPAT_JOURNAL_DEV	0x0008
#define EXT2_FEATURE_INCOMPAT_META_BG		0x0010
#define EXT2_FEATURE_INCOMPAT_ANY			0x0020

/* read-only feature flags */
#define EXT2_FEATURE_RO_COMPAT_SPARSE_SUPER 0x0001
#define EXT2_FEATURE_RO_COMPAT_LARGE_FILE	0x0002
#define EXT2_FEATURE_RO_COMPT_BTREE_DIR		0x0004
#define EXT2_FEATURE_RO_COMPAT_ANY			0xFFFFFFFF

/* permition mask */
#define PERMITION_USER		0x01C0
#define PERMITION_GROUP		0x0038
#define PERMITION_OTHERS	0x0009

#define EXT2_MOUNT_OLDALLOC 0x0002

/* FAT structures are written based on MS Hardware White Paper */
#ifdef _WIN32
#pragma pack(push,fatstructures)
#endif
#pragma pack(1)

typedef struct ext2_super_block{
	UINT32 		inodeCount; /* total inode count */
	UINT32 		blockCount; /* total block count */
	UINT32 		reservedBlockCount;
	UINT32 		freeBlockCount;
	UINT32 		freeInodeCount;
	UINT32 		firstDataBlock; /* first block Number */
	UINT32 		logBlockSize; /* shift value for block size(0, 1, 2), 1024 << logBlockSize */
	UINT32 		logFragSize; /* log fragmentation size */
	UINT32 		blocksPerGroup; /* blocks per group */
	UINT32 		fragPerGroup;
	UINT32 		inodesPerGroup;

	UINT32 		mTime; /* last mount time */
	UINT32 		wTime; /* last superblock update time */
	UINT16 		mountCount; /* number of mount */
	UINT16 		maxMountCount; /* max number of mount */
	UINT16 		magicSignature; /* magic signature of superblock */
	UINT16 		vstate;
	UINT16 		errors;
	UINT16 		minorVersion;
	UINT32 		lastCheckTime; /* last time that checked filesystem */
	UINT32 		checkInterval;
	UINT32 		creatorOS;
	UINT32 		majorVersion;

	UINT16 		rsvUid; /* UID that can use reserved blocks */
	UINT16 		rsvGid; /* GID that can use reserved blocks */

	UINT32 		firstInode; /* first non_reserved inode number */
	UINT16 		inodeSize; /* bytes per inode */
	UINT16		blockGroup; /* block group number of current super block */

	UINT32 		featureCompat;  /* compatible feature flags */
	UINT32 		featureIncompat; /* incompatible feature flags */
	UINT32 		featureROCompat; /* read-only feature flags */

	BYTE 		fsID[16]; /* file system ID */
	BYTE 		volumeName[16];
	BYTE 		lastMountedPath[64];
	UINT32 		algorithmUsageBitmap; /* algorithm relative to bitmap save */
	BYTE 		preallocBlocks; /* preallocated blocks count */
	BYTE 		preallocDirBlocks; /* preallocated directory blocks count */
	UINT16 		padding1;
	BYTE 		journalID[16];
	UINT32 		journalInode; /* journal inode number */
	UINT32 		journalDev; /* device able to jounal */
	UINT32 		orphanList; /* head pointer of orphan inode list */
	UINT32 		hashSeed[4]; /* hash code for hashing algorithm */
	BYTE 		hashVersion; /* defined hash version */
	BYTE 		padding2;
	UINT16 		padding3;
	UINT32 		defaultMountOpt; 		/* default mount option */
	UINT32 		firstMetaBlockGroup; /* first data block number */
	UINT32 		reserved[190];		/* padding to the end of the block */
} EXT2_SUPER_BLOCK;

typedef struct ext2_group_desc{
	UINT32		bg_blockBitmap;		/* Blocks bitmap block */
	UINT32		bg_inodeBitmap;		/* Inodes bitmap block */
	UINT32		bg_inodeTable;		/* Inodes table block */
	UINT16		bg_freeBlockCount;	/* Free blocks count */
	UINT16		bg_freeInodeCount;	/* Free inodes count */
	UINT16		bg_usedDirCount;	/* Directories count */
	UINT16		bg_padding;
	UINT32		bg_reserved[3];
} EXT2_GROUP_DESC;

typedef struct ext2_sb_info {
	UINT32 		inodesPerBlock;		/* number of inodes per block*/
	UINT32		sectorsPerGroup;	/* number of sectors per block group */
	UINT32 		blocksPerGroup;		/* number of blocks per block group */
	UINT32 		inodesPerGroup;		/* number of inodes per block group */
	UINT32 		itbPerGroup;		/* number of blocks for inode table per block group */
	UINT32 		blocksPerDesc;		/* number of blocks for group descriptor table */
	UINT32		sectorsPerBlock;	/* number of sectors per block */
	UINT32 		descPerBlock;		/* number of group descriptor per block */
	UINT32 		groupCount;			/* number of block group */

	UINT32 		firstDescBlock;		/* start block of group descriptor table */
	UINT32 		descPerBlock_bits;	/* power of 2 (5, 6, 7) */
	UINT32 		blockSize_bits;		/* power of 2 (0, 1, 2) */
	UINT32 		inodeSize;			/* bytes of inode struct */
	UINT32 		firstInode;			/* first useable inode number*/
	UINT32 		blockSize;			/* block size */
	UINT32 		dirCount;			/* number of directory of current block group */
	UINT32 		freeBlockCount;		/* number of non-used block */
	UINT32 		freeInodeCount;		/* number of non-used inode */
} EXT2_SB_INFO;


/* Constants relative to the data blocks */
#define	EXT2_NDIR_BLOCKS		12
#define	EXT2_IND_BLOCK			EXT2_NDIR_BLOCKS
#define	EXT2_DIND_BLOCK			(EXT2_IND_BLOCK + 1)	/* double indirect */
#define	EXT2_TIND_BLOCK			(EXT2_DIND_BLOCK + 1)	/* triple indirect */
#define	EXT2_N_BLOCKS			(EXT2_TIND_BLOCK + 1)	/* i_block Count */

/* Inode 접근 권한 */
#define USER_READ					 0x100
#define USER_WRITE					 0x080
#define USER_EXEC					 0x040
#define GROUP_READ					 0x020
#define GROUP_WRITE					 0x010
#define GROUP_EXEC				 	 0x008
#define OTHERS_READ					 0x004
#define OTHERS_WRITE	 			 0x002
#define OTHERS_EXEC 				 0x001
#define ACCESSED_BY_ANYONE			 0x1FF

/* 실행 파일 및 디렉토리 속성 */
#define STICKY_BIT					 0x200
#define SGID						 0x400
#define	SUID						 0x800

/* file type */
#define FILE_TYPE_FIFO               0x1000
#define FILE_TYPE_CHARACTERDEVICE    0x2000
#define FILE_TYPE_DIR				 0x4000
#define FILE_TYPE_BLOCKDEVICE        0x6000
#define FILE_TYPE_FILE				 0x8000
#define FILE_TYPE_SYMBOLIC			 0xA000
#define FILE_TYPE_SOCKET			 0xC000

/* inode 파일 플레그 항목 */
#define EXT2_SECRM_FL				0x00000001
#define EXT2_UNRM_FL				0x00000002
#define EXT2_COMPR_FL				0x00000004
#define EXT2_SYNC_FL				0x00000008
#define EXT2_IMMUTABLE_FL			0x00000010
#define EXT2_APPEND_FL				0x00000020
#define EXT2_NODUMP_FL				0x00000040
#define EXT2_NOATIME_FL				0x00000080

/* Structure of an inode on the disk */
typedef struct ext2_inode {
	UINT16	fileMode;				/* File mode */
	UINT16	uid;					/* Low 16 bits of Owner Uid */
	UINT32	fileSize;				/* Size in bytes */
	UINT32	aTime;					/* Access time */
	UINT32	cTime;					/* Creation time */
	UINT32	mTime;					/* Modification time */
	UINT32	dTime;					/* Deletion Time */
	UINT16	gid;					/* Low 16 bits of Group Id */
	UINT16	linkCount;				/* Links count */
	UINT32	blockCount;				/* Blocks count */
	UINT32	flags;					/* File flags */
	UINT32 	osd1;					/* OS description1 */
	UINT32	i_block[EXT2_N_BLOCKS];	/* Pointers to blocks */
	UINT32	generation;				/* File version (for NFS) */
	UINT32	fileAcl;				/* File ACL */
	UINT32	dirAcl;					/* Directory ACL */
	UINT32	fragAddr;				/* Fragment address */
	UINT32  osd2[3];				/* OS description2 */
} EXT2_INODE;

enum {
	EXT2_FT_UNKNOWN		= 0,
	EXT2_FT_REG_FILE	= 1,
	EXT2_FT_DIR			= 2,
	EXT2_FT_CHRDEV		= 3,
	EXT2_FT_BLKDEV		= 4,
	EXT2_FT_FIFO		= 5,
	EXT2_FT_SOCK		= 6,
	EXT2_FT_SYMLINK		= 7,
	EXT2_FT_MAX
};

typedef struct ext2_dir_entry {
	UINT32 		inode;				/* inode number */
	UINT16 		recordLength;		/* directory entry length */
	union {
		struct {
			UINT16 nameLength;
		} dir1;
		struct {
			BYTE nameLength;
			BYTE fileType;
		} dir2;
	};			/* file name, up to EXT2_NAME_LEN */
	BYTE name[24];
} EXT2_DIR_ENTRY;

typedef struct ext2_dir_entry_location {
	UINT32 group;
	UINT32 block;
	UINT32 offset;
} EXT2_DIR_ENTRY_LOCATION;

typedef struct ext2_filesystem {
	EXT2_SUPER_BLOCK sb;
	EXT2_SB_INFO sb_info;
	DISK_OPERATIONS* disk;
	EXT2_DIR_ENTRY_LOCATION location;
} EXT2_FILESYSTEM;

typedef struct ext2_node {
	EXT2_FILESYSTEM* fs;
	EXT2_DIR_ENTRY entry;
	EXT2_DIR_ENTRY_LOCATION location;
} EXT2_NODE;

typedef int(*EXT2_NODE_ADD)(void*, EXT2_NODE*);

int ext2_read(EXT2_NODE* file, unsigned long offset, unsigned long length, BYTE* buffer);
int ext2_write(EXT2_NODE* file, unsigned long offset, unsigned long length, const BYTE* buffer);

int ext2_format(DISK_OPERATIONS* disk); 
int ext2_read_superblock(EXT2_FILESYSTEM* fs, EXT2_NODE* root); 
void ext2_umount(EXT2_SB_INFO* sb_info); 

int ext2_lookup(EXT2_NODE* parent, const char* entryName, EXT2_NODE* retEntry);
int ext2_read_dir(EXT2_NODE* dir, EXT2_NODE_ADD adder, void* list); 
int ext2_mkdir(const EXT2_NODE* parent, const char* entryName, EXT2_NODE* retEntry); 
int ext2_rmdir(EXT2_NODE* node); 

int ext2_create(EXT2_NODE* parent, const char* entryName, EXT2_NODE* retEntry);
int ext2_remove(EXT2_NODE* file); 

int ext2_df(EXT2_FILESYSTEM* fs, UINT32* totalSectors, UINT32* usedSectors);
int ext2_dump(DISK_OPERATIONS* disk, int blockGroupNum, int type, int target);

#endif

