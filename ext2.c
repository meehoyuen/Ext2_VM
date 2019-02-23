/******************************************************************************/
/*                                                                            */
/* Project : Ext2 File System                                                 */
/* File    : types.h                                                          */
/* Author  : Youngjoon Choi(venbo12@gmail.com), Jimin Back(inb7dni@gmail.com) */
/* Company : Soongsil Univ. Network Computing Lab.                            */
/* Notes   : Common macros header                                             */
/* Date    : 2019/2/16                                                        */
/*                                                                            */
/******************************************************************************/

#include "ext2.h"


/******************************************************************************/
/* bit operation	                                                          */
/******************************************************************************/

// nr�� ��Ʈ get 
static __inline__ int get_bit(int nr, const volatile void* addr)
{
	return ((1UL << (nr & 31)) & (((const volatile unsigned int *)addr)[nr >> 5])) != 0;
}

// nr�� ��Ʈ set 
static __inline__ void set_bit(int nr, volatile void* addr)
{
	if (get_bit(nr, addr) == 1) {
		(((volatile unsigned int *)addr)[nr >> 5]) &= (0xFFFFFFFF ^ (1UL << (nr & 31)));
	}
	else {
		(((volatile unsigned int *)addr)[nr >> 5]) |= (1UL << (nr & 31));
	}
}

// nr�� ��Ʈ get and set 
static __inline__ int get_set_bit(int nr, volatile void* addr)
{
	int oldbit;
	if ((oldbit = get_bit(nr, addr)) == 1) {
		(((volatile unsigned int *)addr)[nr >> 5]) &= (0xFFFFFFFF ^ (1UL << (nr & 31)));
	}
	else {
		(((volatile unsigned int *)addr)[nr >> 5]) |= (1UL << (nr & 31));
	}
	return oldbit;
}

// nr ��Ʈ �������� ó�� ������ zero bit ��ȣ ���� 
static int get_next_zero_bit(EXT2_FILESYSTEM* fs, int nr, volatile void* addr)
{
	UINT32 i;
	UINT32 bit;
	UINT32 blkBits = fs->sb_info.blockSize;

	for (i = 0; i < blkBits * 8; i++)
	{
		bit = (nr + i) % blkBits;
		if (get_bit(bit, addr) == 0)
			return bit;
	}
	return -1;
} 


/******************************************************************************/
/* read / write		                                                          */
/******************************************************************************/

/* ���� */
/* buffer�� offset���� length��ŭ file �о ��� */
int ext2_read(EXT2_NODE* file, unsigned long offset, unsigned long length, BYTE* buffer)
{

}

/* ���� */
/* offset���� length��ŭ buffer�� ������ file�� ���� */
int ext2_write(EXT2_NODE* file, unsigned long offset, unsigned long length, const BYTE* buffer)
{

}

/* ���� */
/* ��ϴ����� ��ũ �б� */
int read_block(EXT2_FILESYSTEM* fs, UINT32 block, BYTE* buffer)
{
	if (block <= 0) // 0�� ����� ��Ʈ �ڵ� ����
	{
		printf("error : invalid block number\n");
		return EXT2_ERROR;
	}

	UINT32 i;
	UINT32 sectorNumber = (EXT2_MIN_BLOCK_SIZE / MAX_SECTOR_SIZE) + (EXT2_BLOCK_SIZE / MAX_SECTOR_SIZE) * (block - 1);
	UINT32 sectorCount = EXT2_BLOCK_SIZE / MAX_SECTOR_SIZE;

	for (i = 0; i < sectorCount; i++)
	{
		fs->disk->read_sector(fs->disk, sectorNumber + i, &buffer[i * MAX_SECTOR_SIZE]);
	}

	return EXT2_SUCCESS;
}

/* ���� */
/* ��ϴ����� ��ũ ���� */
int write_block(EXT2_FILESYSTEM * fs, UINT32 block, const BYTE* buffer)
{
	if (block <= 0) // 0�� ����� ��Ʈ �ڵ� ����
	{
		printf("error : invalid block number\n");
		return EXT2_ERROR;
	}
	UINT32 i;
	UINT32 sectorNumber = (EXT2_MIN_BLOCK_SIZE / MAX_SECTOR_SIZE) + (EXT2_BLOCK_SIZE / MAX_SECTOR_SIZE) * (block - 1);
	UINT32 sectorCount = EXT2_BLOCK_SIZE / MAX_SECTOR_SIZE;

	for (i = 0; i < sectorCount; i++)
	{
		fs->disk->write_sector(fs->disk, sectorNumber + i, &buffer[i * MAX_SECTOR_SIZE]);
	}

	return EXT2_SUCCESS;
}


/******************************************************************************/
/* control count member 													  */
/******************************************************************************/

/* ���� */
/* dir count ���� */
int dec_dir_count(EXT2_FILESYSTEM* fs, UINT32 group)
{
	EXT2_GROUP_DESC desc;

	if (read_desc(fs, group, &desc) != EXT2_SUCCESS)
		return EXT2_ERROR;

	desc.bg_usedDirCount--;

	if (write_desc(fs, group, &desc) != EXT2_SUCCESS)
		return EXT2_ERROR;

	fs->sb_info.dirCount--;

	return EXT2_SUCCESS;
}

/* ���� */
/* dir count ���� */
int inc_dir_count(EXT2_FILESYSTEM* fs, UINT32 group)
{
	EXT2_GROUP_DESC desc;

	if (read_desc(fs, group, &desc) != EXT2_SUCCESS)
		return EXT2_ERROR;

	desc.bg_usedDirCount++;

	if (write_desc(fs, group, &desc) != EXT2_SUCCESS)
		return EXT2_ERROR;

	fs->sb_info.dirCount++;

	return EXT2_SUCCESS;
}

/* ���� */
/* free block count ���� */
int dec_freeb_count(EXT2_FILESYSTEM* fs, UINT32 group)
{
	EXT2_GROUP_DESC desc;

	fs->sb.freeBlockCount--;

	if (read_desc(fs, group, &desc) != EXT2_SUCCESS)
		return EXT2_ERROR;

	desc.bg_freeBlockCount--;

	if (write_desc(fs, group, &desc) != EXT2_SUCCESS)
		return EXT2_ERROR;

	fs->sb_info.freeBlockCount--;

	return EXT2_SUCCESS;
}

/* ���� */
/* free block count ���� */
int inc_freeb_count(EXT2_FILESYSTEM* fs, UINT32 group)
{
	EXT2_GROUP_DESC desc;

	fs->sb.freeBlockCount++;

	if (read_desc(fs, group, &desc) != EXT2_SUCCESS)
		return EXT2_ERROR;

	desc.bg_freeBlockCount++;

	if (write_desc(fs, group, &desc) != EXT2_SUCCESS)
		return EXT2_ERROR;

	fs->sb_info.freeBlockCount++;

	return EXT2_SUCCESS;
}

/* ���� */
/* free inode count ���� */
int dec_freei_count(EXT2_FILESYSTEM* fs, UINT32 group)
{
	EXT2_GROUP_DESC desc;

	fs->sb.freeInodeCount--;

	if (read_desc(fs, group, &desc) != EXT2_SUCCESS)
		return EXT2_ERROR;

	desc.bg_freeInodeCount--;

	if (write_desc(fs, group, &desc) != EXT2_SUCCESS)
		return EXT2_ERROR;

	fs->sb_info.freeInodeCount--;

	return EXT2_SUCCESS;
}

/* ���� */
/* free inode count ���� */
int inc_freei_count(EXT2_FILESYSTEM* fs, UINT32 group)
{
	EXT2_GROUP_DESC desc;

	fs->sb.freeInodeCount++;

	if (read_desc(fs, group, &desc) != EXT2_SUCCESS)
		return EXT2_ERROR;

	desc.bg_freeInodeCount++;

	if (write_desc(fs, group, &desc) != EXT2_SUCCESS)
		return EXT2_ERROR;

	fs->sb_info.freeInodeCount++;

	return EXT2_SUCCESS;
}


/******************************************************************************/
/* manage block and inode													  */
/******************************************************************************/

/* ���ڿ��� ��� �빮�ڷ� ���� */
void upper_string(char* str, int length)
{
	while (*str && length-- > 0)
	{
		*str = toupper(*str);
		str++;
	}
}

/* ���� */
/* inode ��ȣ�� �̿��� �ش� ������ ��Ƽ� ���� */
int get_inode(EXT2_FILESYSTEM* fs, UINT32 inodeNumber, BYTE* inode)
{
	UINT32 block, offset;
	UINT32 sectorNum;
	BYTE buffer[EXT2_BLOCK_SIZE];

	ZeroMemory(buffer, EXT2_BLOCK_SIZE);

	if(get_block_of_inode(fs, inodeNumber, &block) != EXT2_SUCCESS)
	{
		// inode�� ���� ��� ��ȣ
		printf("error : failed to get_block_of_inode() in get_inode()\n");
		return EXT2_ERROR;
	}

	if (read_block(fs, block, buffer) != EXT2_SUCCESS)
	{
		printf("error : failed to read_block() in get_inode()\n");
		return EXT2_ERROR;
	}

	offset = (inodeNumber % fs->sb_info.inodesPerGroup) % fs->sb_info.inodesPerBlock; // block ������ offset
	
	memcpy(inode, &((EXT2_INODE *)buffer)[offset], sizeof(EXT2_INODE));

	return EXT2_SUCCESS;
}

/* ���� */
/* inode ��ȣ�� �̿��� buffer�� ������ ���� */
int set_inode(EXT2_FILESYSTEM* fs, UINT32 inodeNumber, BYTE* inode)
{
	UINT32 block, offset;
	BYTE buffer[EXT2_BLOCK_SIZE];

	ZeroMemory(buffer, EXT2_BLOCK_SIZE);

	if (get_block_of_inode(fs, inodeNumber, &block) != EXT2_SUCCESS)
		return EXT2_ERROR;


	if (read_block(fs, block, buffer) != EXT2_SUCCESS)
	{
		printf("error : failed to read_block in get_inode()\n");
		return EXT2_ERROR;
	}

	offset = (inodeNumber % fs->sb_info.inodesPerGroup) % fs->sb_info.inodesPerBlock;

	memcpy(&((EXT2_INODE *)buffer)[offset], inode, sizeof(EXT2_INODE));

	if(write_block(fs, block, buffer) != EXT2_SUCCESS)
	{
		printf("error : failed to write_block() in set_inode()\n");
		return EXT2_ERROR;
	}

	return EXT2_SUCCESS;
}

/* ���� */
/* ���͸��� �Ҵ��� �׷��� ã�� �˰��� 1 */
int find_group_dir(EXT2_FILESYSTEM* fs, EXT2_NODE* parent, UINT32* retGroup)
{
	BYTE buffer[EXT2_BLOCK_SIZE];
	EXT2_SB_INFO* sb_info;
	UINT32 groupCount;
	EXT2_GROUP_DESC *desc, *bestDesc; // ���� ������ ��� �׷��� ��ũ����
	INT32 group, bestGroup;
	UINT32 aveFreeInode;

	sb_info = &fs->sb_info;
	groupCount = sb_info->groupCount; // ��ü group ����
	aveFreeInode = sb_info->freeInodeCount / groupCount; // free���� inode�� ��ü �׷� ��� ����
	bestGroup = -1; // ���� ������ ��� �׷� �ʱⰪ
	bestDesc = NULL;

	for (group = 0; group < groupCount; group++)
	{
		if (read_desc(fs, group, buffer) != EXT2_SUCCESS)
			return EXT2_ERROR;
		desc = (EXT2_GROUP_DESC *)buffer;

		if (desc == NULL || desc->bg_freeInodeCount == 0)
			continue;
		if (desc->bg_freeInodeCount < aveFreeInode);
		continue;
		if (bestDesc == NULL ||
			(desc->bg_freeBlockCount > bestDesc->bg_freeBlockCount))
		{
			bestGroup = group;
			bestDesc = desc;
		}
	}

	if (bestDesc == NULL)
		return EXT2_ERROR;

	*retGroup = bestGroup;

	return EXT2_SUCCESS;
}

/* ���� */
/* ���͸��� �Ҵ��� �׷��� ã�� �˰��� 2 */
int find_group_orlov(EXT2_FILESYSTEM* fs, EXT2_NODE* parent, UINT32* retGroup)
{
	BYTE buffer[EXT2_BLOCK_SIZE];
	EXT2_SB_INFO* sb_info;
	EXT2_GROUP_DESC *desc, *bestDesc; // ���� ������ ��� �׷��� ��ũ����
	UINT32 parentBlock, parentGroup; // �θ� ���丮�� ���̳�尡 ���� ��ϰ� �׷� ��ȣ
	UINT32 aveFreeInodes, aveFreeBlocks;
	UINT32 groupCount;
	UINT32 inodesPerGroup;
	UINT32 freeInodes, freeBlocks;
	UINT32 blocksPerDir;
	UINT32 dirCount;
	UINT32 maxDirs, minBlocks, minInodes;
	INT32 group = -1, i;

	srand((UINT32)time(NULL));

	if (get_block_of_inode(fs, parent->entry.inode, &parentBlock) != EXT2_SUCCESS)
		return EXT2_ERROR;

	if (get_group_of_block(fs, parentBlock, &parentGroup) != EXT2_SUCCESS)
		return EXT2_ERROR;

	sb_info = &fs->sb_info;
	groupCount = sb_info->groupCount;
	inodesPerGroup = sb_info->inodesPerGroup;

	freeInodes = sb_info->freeInodeCount;
	aveFreeInodes = freeInodes / groupCount; // free inode ���� ��ü �׷� ��� 
	freeBlocks = sb_info->freeBlockCount;
	aveFreeBlocks = freeBlocks / groupCount;
	dirCount = sb_info->dirCount; // ������� ���丮 ����


								  // �θ� ���丮�� ��Ʈ ���丮 �� ��
	if (is_root_dir(parent) == EXT2_SUCCESS)
	{
		EXT2_GROUP_DESC* bestDesc = NULL;
		UINT32 bestDirCount = sb_info->inodesPerGroup;
		UINT32 bestGroup = -1;

		group = rand();
		parentGroup = (UINT32)group % groupCount;
		for (i = 0; i < groupCount; i++)
		{
			group = (parentGroup + i) % groupCount;
			ZeroMemory(buffer, sizeof(buffer));
			read_desc(fs, group, buffer);
			desc = (EXT2_GROUP_DESC *)buffer;

			if (desc == NULL || desc->bg_freeInodeCount == NULL)
				continue;
			if (desc->bg_usedDirCount >= bestDirCount)
				continue;
			if (desc->bg_freeInodeCount < aveFreeInodes)
				continue;
			if (desc->bg_freeBlockCount < aveFreeBlocks)
				continue;

			bestGroup = group;
			bestDirCount = desc->bg_usedDirCount;
			bestDesc = desc;
		}
		if (bestGroup >= 0)
		{
			desc = bestDesc;
			group = bestGroup;
			goto found;
		}
		goto fallback;
	}

	// ���丮�� ���� ��
	if (dirCount == 0)
		dirCount = 1;

	maxDirs = dirCount / groupCount + inodesPerGroup / 16;
	minInodes = aveFreeInodes - inodesPerGroup / 4;
	minBlocks = aveFreeBlocks - sb_info->blocksPerGroup / 4;

	/* debt�� ���ٰ� ���� */

	for (i = 0; i < groupCount; i++)
	{
		group = (parentGroup + i) % groupCount;
		ZeroMemory(buffer, sizeof(buffer));
		read_desc(fs, group, buffer);
		desc = (EXT2_GROUP_DESC *)buffer;

		if (desc == NULL || desc->bg_freeInodeCount == 0)
			continue;
		if (desc->bg_usedDirCount >= maxDirs)
			continue;
		if (desc->bg_freeInodeCount < minInodes)
			continue;
		if (desc->bg_freeBlockCount < minBlocks)
			continue;
		goto found;
	}

fallback:

	for (i = 0; i < groupCount; i++)
	{
		group = (parentGroup + i) % groupCount;
		ZeroMemory(buffer, sizeof(buffer));
		read_desc(fs, group, buffer);
		desc = (EXT2_GROUP_DESC *)buffer;

		if (desc == NULL || desc->bg_freeInodeCount)
			continue;
		if (desc->bg_freeInodeCount >= aveFreeInodes)
			goto found;

		// free inode ������ ��հ��� ���� ū �׷��� ���� ��
		if (aveFreeInodes)
		{
			aveFreeInodes = 0;
			goto fallback;
		}
	}
	return EXT2_ERROR;

found:

	*retGroup = group;

	return EXT2_SUCCESS;
}

/* ���� */
/* ������ �Ҵ��� �׷��� ã�� �˰��� */
int find_group_other(EXT2_FILESYSTEM* fs, EXT2_NODE* parent, UINT32* retGroup)
{
	BYTE buffer[EXT2_BLOCK_SIZE];
	UINT32 parentGroup, parentBlock;
	UINT32 groupCount;
	EXT2_GROUP_DESC* desc;
	UINT32 group, i;

	if (get_block_of_inode(fs, parent->entry.inode, &parentBlock) != EXT2_SUCCESS)
		return EXT2_ERROR;

	if (get_group_of_block(fs, parentBlock, &parentGroup) != EXT2_SUCCESS)
		return EXT2_ERROR;

	// ��� 1.
	// �θ� inode
	group = parentGroup;
	ZeroMemory(buffer, sizeof(buffer));
	read_desc(fs, group, buffer);
	desc = (EXT2_GROUP_DESC *)buffer;
	if (desc != NULL && desc->bg_freeInodeCount != NULL &&
		desc->bg_freeBlockCount != NULL)
		goto found;


	// �θ� ���丮�� ���� �׷쿡 �ڸ��� ���� �� �ؽ� �˰���
	// �ٸ� �׷��� inode �Ҵ� ����.
	group = (group + parent->entry.inode) % groupCount;

	for (i = 1; i < groupCount; i <<= 1)
	{
		group += i;
		if (group >= groupCount)
			group -= groupCount;
		ZeroMemory(buffer, sizeof(buffer));
		read_desc(fs, group, buffer);
		desc = (EXT2_GROUP_DESC *)buffer;
		if (desc != NULL && desc->bg_freeInodeCount != 0 &&
			desc->bg_freeBlockCount != 0)
			goto found;
	}

	// ���� ��� ��� ���н� ���� Ž��
	group = parentGroup;
	for (i = 0; i < groupCount; i++)
	{
		if (++group >= groupCount)
			group = 0;
		ZeroMemory(buffer, sizeof(buffer));
		read_desc(fs, group, buffer);
		desc = (EXT2_GROUP_DESC *)buffer;
		if (desc != NULL && desc->bg_freeInodeCount != 0)
			goto found;
	}
	return EXT2_ERROR;

found:

	*retGroup = group;

	return EXT2_SUCCESS;
}

/* ���� */
/* ���� ����� ������ �ִ��� Ȯ�� */
int has_free_blocks(EXT2_FILESYSTEM* fs)
{
	if (fs->sb_info.freeBlockCount == 0)
		return EXT2_ERROR;
	else
		return EXT2_SUCCESS;
}

/* ���� */
/* indirect ��� �Ҵ�*/
int alloc_ind_block()
{
}

/* ��� ��Ʈ�� �޾ƿ� */
int read_block_bitmap(EXT2_FILESYSTEM* fs, UINT32 group, BYTE* buffer)
{
}

/* block bitmap�� buffer�� �������� ���� */
int write_block_bitmap(EXT2_FILESYSTEM* fs, UINT32 group, BYTE* buffer)
{
}

/* inode tabla ��Ʈ�� �޾ƿ� */
int read_inode_bitmap(EXT2_FILESYSTEM* fs, UINT32 group, BYTE* buffer)
{
}

/* ���� */
/* inode bitmap�� buffer�� �������� ���� */
int write_inode_bitmap(EXT2_FILESYSTEM* fs, UINT32 group, BYTE* buffer)
{
	EXT2_GROUP_DESC desc;

	if (read_desc(fs, group, &desc) != EXT2_SUCCESS)
		return EXT2_ERROR;

	if (write_block(fs, desc.bg_inodeBitmap, buffer) != EXT2_SUCCESS)
		return EXT2_ERROR;

	return EXT2_SUCCESS;
}

/*
* 1. ��ũ���� ������Ʈ
* 2. ��Ʈ�� ����
* 3. sb, sb_info freeCount ������Ʈ
*/

/* ���� */
/* block �Ҵ� */
int alloc_block(EXT2_FILESYSTEM* fs, UINT32 inodeNumber, UINT32 *retBlk)
{
	BYTE buffer[EXT2_BLOCK_SIZE];
	EXT2_SB_INFO* sb_info;
	EXT2_GROUP_DESC* desc;
	EXT2_INODE* inode;
	UINT32 blockCount;
	UINT32 blk;
	BYTE bit;
	UINT32 group, block, offset;
	UINT32 i;
	UINT32 currGroup;
	sb_info = &fs->sb_info;

	ZeroMemory(buffer, sizeof(buffer));

	group = inodeNumber + (sb_info->inodesPerGroup - 1) / sb_info->inodesPerGroup; // inode�� ���� �׷�
	get_block_of_inode(fs, inodeNumber, &block); // inode��ȣ�� ���� ��� ��ȣ ���ϱ�
	offset = inodeNumber % sb_info->inodesPerBlock; // ��� �� inode offset
	read_block(fs, block, buffer); // inode�� ���� ��� �б�
	inode = (EXT2_INODE *)buffer; // ���� inode* �� ĳ����

	UINT32 blkSize = sb_info->blockSize;
	UINT32 ptrsPerBlk = blkSize / sizeof(UINT32); // �� ��ϴ� ������ ���� 
	UINT32 ind = ptrsPerBlk;
	UINT32 dind = ptrsPerBlk * ptrsPerBlk;
	UINT32 tind = ptrsPerBlk * ptrsPerBlk * ptrsPerBlk;

	UINT32 groupCount = sb_info->groupCount;

	if (blockCount < 0)
	{
		printf("error : no block to read\n");
		return EXT2_ERROR;
	}
		
	else if (blockCount < EXT2_NDIR_BLOCKS)
	{
		read_block_bitmap(fs, group, buffer);	// inode�� ���� �׷��� ��� ��Ʈ�� �б�
		if (get_bit(inode[offset].blockCount + 1, buffer == 0))
		{	// ������ �Ҵ��� ����� ���� ��� Ȯ��
			blk = inode[offset].blockCount + 1; // �Ҵ� �ȵ����� �Ҵ�
			goto found;
		}
		else
		{
			if ((blk = get_next_zero_bit(fs, inode[offset].blockCount + 1, buffer)) != EXT2_SUCCESS)
				goto other;	// �ٸ� �׷� Ž��
			else
				goto found;
		}
	}
	else
	{
		if (alloc_ind_block(fs, &inode[offset], &blk) != EXT2_ERROR)
			return EXT2_ERROR;
		else
			goto found;
	}

other:
	currGroup = group;
	for (i = 1; i < groupCount; i++) // ���� Ž��
	{
		currGroup = (currGroup + i) % groupCount;
		ZeroMemory(buffer, sizeof(buffer));
		read_block_bitmap(fs, currGroup, buffer);
		if (get_next_zero_bit(fs, 0, buffer) != EXT2_SUCCESS)
			return EXT2_ERROR;
	}

found:
	write_block(fs, block, buffer);
	return EXT2_SUCCESS;
}

/* ���� */
/* inode �Ҵ� */
int alloc_inode(EXT2_FILESYSTEM* fs, EXT2_NODE* parent, EXT2_NODE* entry)
{
	EXT2_SB_INFO* sb_info = &fs->sb_info;
	EXT2_INODE* inode;
	BYTE descBuf[EXT2_BLOCK_SIZE];
	BYTE inodeBuf[EXT2_BLOCK_SIZE];
	UINT32 group, i;
	UINT32 ino;
	EXT2_GROUP_DESC* desc;
	UINT32 inodeNumber;
	UINT32 result;

	ZeroMemory(descBuf, EXT2_BLOCK_SIZE);
	ZeroMemory(inodeBuf, EXT2_BLOCK_SIZE);

	if (is_dir(entry) == EXT2_SUCCESS)
	{
		if (fs->sb.defaultMountOpt & EXT2_MOUNT_OLDALLOC)
			result = find_group_dir(fs, parent, &group);
		else
			result = find_group_orlov(fs, parent, &group);
	}
	else
		result = find_group_other(fs, parent, &group);

	if (result == EXT2_ERROR)
		goto fail;

	for (i = 0; i < sb_info->groupCount; i++)
	{

		/*
		// ���� �׷��ȣ�� ���� ��ũ���� �б�
		if (read_desc(fs, group, descBuf) != EXT2_SUCCESS)
		goto fail;

		desc = (EXT2_GROUP_DESC *)descBuf;
		if (desc == NULL)
		{
		if (++group == sb_info->groupCount)
		group = 0;
		continue;
		}
		*/

		if (++group == sb_info->groupCount)
		{
			group = 0;
			continue;
		}

		// inode ��Ʈ�� �о����
		if (read_inode_bitmap(fs, group, inodeBuf) != EXT2_SUCCESS)
			goto fail;
		ino = 0;

	repeat_in_this_group:
		// ino ������ ���� ���� ������ ��Ʈ��ȣ return ����
		ino = get_next_zero_bit(fs, ino, inodeBuf);
		//ino ��ȣ�� �׷�� inode �������� �Ѿ�� ���� �׷� �˻�
		if (ino >= sb_info->inodesPerGroup)
		{
			//���� �׷��� �׷�ī���͸� �ʰ��ϸ� �׷� 0������
			if (++group == sb_info->groupCount)
				group = 0;
			continue;
		}
		if (ino == -1) // �ش� ��Ʈ�ʿ� ���� ���
		{
			if (++ino >= sb_info->inodesPerGroup)
			{
				if (++group == sb_info->groupCount)
					group = 0;
				continue;
			}
			goto repeat_in_this_group;
		}
		//��Ʈ ����
		set_bit(ino, inodeBuf);
		write_inode_bitmap(fs, group, inodeBuf);

		goto got;
	}

got:
	ino += group * sb_info->inodesPerGroup + 1; // ino ��Ʈ�� �ش��ϴ� inode ��ȣ

	if (ino < EXT2_BAD_INO || ino > fs->sb.inodeCount)
	{
		goto fail;
	}

	entry->entry.inode = ino; // ���ο� entry�� inode �ʱ�ȭ

	if (is_dir(entry) == EXT2_SUCCESS)
		inc_dir_count(fs, group); // dir ���� �ʵ� ������Ʈ

	dec_freei_count(fs, group); // free inode ���� �ʵ� ������Ʈ

	ZeroMemory(inodeBuf, sizeof(inodeBuf));
	if (get_inode(fs, ino, inodeBuf) != EXT2_SUCCESS)
		goto fail;

	inode = (EXT2_INODE *)inodeBuf;
	inode->blockCount = 0;
	if (is_dir(entry) == EXT2_SUCCESS)
		inode->fileMode |= FILE_TYPE_DIR;
	else
		inode->fileMode |= FILE_TYPE_FILE;
	inode->fileSize = 0;

	set_inode(fs, ino, inode);

	return EXT2_SUCCESS;

fail:
	return EXT2_ERROR;
}

/* ���Ͽ� �Ҵ�� ��ϵ��� �ٽ� free ���·� ��ȯ */
int free_block(EXT2_NODE* retEntry)
{

}

/* ���Ͽ� �Ҵ�� inode�� �ٽ� free ���·� ��ȯ */
int free_inode(EXT2_NODE* retEntry)
{

}


/******************************************************************************/
/* format																	  */
/******************************************************************************/

/* ���� */
/* ���ۺ�� ����ü ��� �� ���� */
int fill_super_block(EXT2_SUPER_BLOCK* sb, UINT32 numberOfSectors, UINT32 bytesPerSector)
{
	UINT32 groupCount;
	UINT32 descTableBlks;

	UINT32 totalSize, blkSize;
	UINT32 totalBlkCnt;
	UINT32 blkPerGroup;
	UINT32 inoBlksPerGroup, inodesPerGroup;
	UINT32 inodeCount;

	ZeroMemory(sb, sizeof(*sb));

	blkSize = EXT2_MIN_BLOCK_SIZE << EXT2_BLOCK_SIZE_BIT; // �ϳ��� ��� ������ 
	totalSize = numberOfSectors * bytesPerSector; // ��ũ ��ü ũ�� 
	totalBlkCnt = (totalSize + (blkSize - 1)) / blkSize; // ��ü ��� ���� 

	sb->inodeCount = totalBlkCnt / 2;
	sb->blockCount = totalBlkCnt;

	blkPerGroup = blkSize << 3; // ��Ʈ�� �� ����� ��Ʈ �� 
	groupCount = ((totalBlkCnt - 1) / blkPerGroup) + 1;


	descTableBlks = ((EXT2_DESC_SIZE * groupCount) + (blkSize - 1)) / blkSize; // �׷� ��ũ���� ���̺��� �����ϴ� ��� ����
	inodesPerGroup = sb->inodeCount / groupCount;
	inoBlksPerGroup = ((sb->inodeSize * inodesPerGroup) + (blkSize - 1)) / blkSize; // �׷�� inode table ��� ��
	descTableBlks = ((EXT2_DESC_SIZE * groupCount) + (blkSize - 1)) / blkSize; // �׷� ��ũ���Ͱ� �����ϴ� ��� ���� 

	sb->magicSignature = 0xEF53;

	sb->reservedBlockCount = totalBlkCnt / 20;
	sb->freeBlockCount = (blkPerGroup - 1 - descTableBlks - 2 - inoBlksPerGroup) * groupCount;
	sb->freeInodeCount = sb->inodeCount;
	sb->firstDataBlock = 1;
	sb->logBlockSize = EXT2_BLOCK_SIZE_BIT;
	sb->logFragSize = EXT2_BLOCK_SIZE_BIT;
	sb->inodesPerGroup = inodesPerGroup;

	sb->firstInode = EXT2_GOOD_OLD_FIRST_INO;
	sb->inodeSize = EXT2_INODE_SIZE;
	sb->blocksPerGroup = blkPerGroup;

	memcpy(sb->fsID, "EXT2", 4);
	memcpy(sb->volumeName, VOLUME_LABEL, VOLUME_LABEL_LENGTH);

	// ����� �ڵ�
	sb->firstMetaBlockGroup = 99;
	sb->reserved[189] = 98;
	/*
	printf("\ninodeCount		%#08x\t%X\n", &sb->inodeCount, sb->inodeCount);
	printf("blockCount		%#08x\t%X\n", &sb->blockCount, sb->blockCount);
	printf("reservedBlockCount	%#08x\t%X\n", &sb->reservedBlockCount, sb->reservedBlockCount);
	printf("freeBlockCount		%#08x\t%X\n", &sb->freeBlockCount, sb->freeBlockCount);
	printf("freeInodeCount		%#08x\t%X\n", &sb->freeInodeCount, sb->freeInodeCount);
	printf("firstDataBlock		%#08x\t%X\n", &sb->firstDataBlock, sb->firstDataBlock);
	printf("logBlockSize		%#08x\t%X\n", &sb->logBlockSize, sb->logBlockSize);
	printf("logFragSize		%#08x\t%X\n", &sb->logFragSize, sb->logFragSize);
	printf("blocksPerGroup		%#08x\t%X\n", &sb->blocksPerGroup, sb->blocksPerGroup);
	printf("inodesPerGroup		%#08x\t%X\n", &sb->inodesPerGroup, sb->inodesPerGroup);
	printf("magicSignature		%#08x\t%X\n", &sb->magicSignature, sb->magicSignature);
	printf("firstInode		%#08x\t%X\n", &sb->firstInode, sb->firstInode);
	printf("inodeSize		%#08x\t%X\n", &sb->inodeSize, sb->inodeSize);
	printf("fsID(EXT2)		%#08x\t%X %X %X %X\n", &sb->fsID, 'E', 'X', 'T', '2');
	printf("volumeName(EXT2 BY YJM)	%#08x\t%X %X %X %X %X %X %X %X %X %X %X\n", &sb->volumeName, 'E', 'X', 'T', '2', ' ', 'B', 'Y', ' ', 'Y', 'J', 'M');
	printf("firstMetaBlkGroup	%#08x\t%X\n", &sb->firstMetaBlockGroup, sb->firstMetaBlockGroup);
	printf("reserved[189]		%#08x\t%X\n\n", &sb->reserved[189], sb->reserved[189]);
	*/
	return EXT2_SUCCESS;
}

/* ���� */
/* ���ۺ���� �ϳ��� ��ϱ׷쿡 �� */
int write_super_block(DISK_OPERATIONS* disk, EXT2_SUPER_BLOCK* sb, UINT32 blkGroupNumber)
{
	BYTE buffer[MAX_SECTOR_SIZE];
	UINT32 blkSize = EXT2_BLOCK_SIZE;
	UINT32 sectorsPerBlock = blkSize / MAX_SECTOR_SIZE;
	UINT32 sectorsPerGroup = sectorsPerBlock * sb->blocksPerGroup;

	UINT32 sectorNumber = EXT2_MIN_BLOCK_SIZE / MAX_SECTOR_SIZE + blkGroupNumber * sectorsPerGroup;
	UINT32 offset = 0;
	UINT32 sbSize = sizeof(EXT2_SUPER_BLOCK);

	if (disk == NULL || sb == NULL || blkGroupNumber < 0)
	{
		printf("error : wrong argument\n");
		return EXT2_ERROR;
	}

	while (offset < sbSize)
	{
		ZeroMemory(buffer, sizeof(buffer));
		memcpy(buffer, &((BYTE *)sb)[offset], sizeof(buffer));
		disk->write_sector(disk, sectorNumber, buffer);
		offset += MAX_SECTOR_SIZE;
		sectorNumber++;
	}

	return EXT2_SUCCESS;
}

/* ���� */
/* �ϳ��� ��ϱ׷쿡 ���� �׷� ��ũ���� ��� �� ���� */
int fill_desc(DISK_OPERATIONS* disk, EXT2_SUPER_BLOCK* sb, EXT2_GROUP_DESC* desc, UINT32 blkGroupNumber)
{
	UINT32 blkSize = EXT2_MIN_BLOCK_SIZE << sb->logBlockSize; // ��� ũ�� 
	UINT32 groupCount = ((sb->blockCount - sb->firstDataBlock - 1) / sb->blocksPerGroup) + 1; // ��� �׷� ���� 
	UINT32 descTableBlks = ((sizeof(EXT2_GROUP_DESC) * groupCount) + (blkSize - 1)) / blkSize; // �׷� ��ũ���� ���̺� ��� ���� 																				  
	UINT32 inoBlksPerGroup = ((sb->inodesPerGroup * sb->inodeSize) + (blkSize - 1)) / blkSize; // �׷�� inode table ��� �� 

	if (disk == NULL || sb == NULL || blkGroupNumber < 0)
	{
		printf("error : wrong argument\n");
		return EXT2_ERROR;
	}

	ZeroMemory(desc, sizeof(EXT2_GROUP_DESC));

	desc->bg_blockBitmap = sb->firstDataBlock + descTableBlks + 1 + blkGroupNumber * sb->blocksPerGroup; // block bitmap ��� ��ȣ 
	desc->bg_inodeBitmap = desc->bg_blockBitmap + 1;
	desc->bg_inodeTable = desc->bg_inodeBitmap + 1;

	// �ʱ� free ���� ��� ���� 
	// �׷�� ��ϼ� - inode table ��ϼ� - ��� ��ũ���� ���̺� ��ϼ� - 3(���ۺ��, bitmap) 
	desc->bg_freeBlockCount = sb->blocksPerGroup - inoBlksPerGroup - descTableBlks - 3;
	desc->bg_freeInodeCount = sb->inodeCount / groupCount;
	desc->bg_usedDirCount = 0;
	/*
	printf("\nbg_blockBitmap		%#08x\t%X\n", &desc->bg_blockBitmap, desc->bg_blockBitmap);
	printf("bg_inodeBitmap		%#08x\t%X\n", &desc->bg_inodeBitmap, desc->bg_inodeBitmap);
	printf("bg_inodeTable		%#08x\t%X\n", &desc->bg_inodeTable, desc->bg_inodeTable);
	printf("bg_freeBlockCount	%#08x\t%X\n", &desc->bg_freeBlockCount, desc->bg_freeBlockCount);
	printf("bg_freeInodeCount	%#08x\t%X\n", &desc->bg_freeInodeCount, desc->bg_freeInodeCount);
	printf("bg_usedDirCount		%#08x\t%X\n\n", &desc->bg_usedDirCount, desc->bg_usedDirCount);
	*/
	return EXT2_SUCCESS;
}

/* ���� */
/* �ϳ��� ��ϱ׷쿡 ���� �׷� ��ũ���� ���̺� �ʱ�ȭ */
int init_desc(DISK_OPERATIONS* disk, EXT2_SUPER_BLOCK* sb, UINT32 blkGroupNumber)
{
	BYTE buffer[MAX_SECTOR_SIZE];
	EXT2_GROUP_DESC desc;	// �׷� ��ũ���� 
	EXT2_GROUP_DESC* pdesc = &desc;	// �׷� ��ũ���� ������ 
	UINT32 groupCount = ((sb->blockCount - sb->firstDataBlock - 1) / sb->blocksPerGroup) + 1; // ��ü �׷� ���� 
	UINT32 blkSize = EXT2_MIN_BLOCK_SIZE << sb->logBlockSize; // ��� �ϳ� ũ�� 
	UINT32 sectorsPerBlock = blkSize / MAX_SECTOR_SIZE; // ��� �� ���� ��
	UINT32 sectorsPerGroup = sectorsPerBlock * sb->blocksPerGroup; // ��ϱ׷� �� ���� ��
	UINT32 sectorNumber = (EXT2_MIN_BLOCK_SIZE + blkSize) / MAX_SECTOR_SIZE + sectorsPerGroup * blkGroupNumber; // write ���� ���� ��ȣ 
	UINT32 offset = 0;
	UINT32 i;

	if (disk == NULL || sb == NULL || blkGroupNumber < 0)
	{
		printf("error : wrong argument\n");
		return EXT2_ERROR;
	}

	for (i = 0; i < groupCount; i++)
	{
		ZeroMemory(pdesc, sizeof(EXT2_GROUP_DESC));
		if (fill_desc(disk, sb, pdesc, i) != EXT2_SUCCESS) // i�� �׷� ��ũ���� �ʱ�ȭ
		{
			printf("error : faid to fill group descriptor of group %d\n", i);
			return EXT2_ERROR;
		}

		memcpy(&((EXT2_GROUP_DESC *)buffer)[offset++], pdesc, sizeof(desc)); // ���ۿ� ��ũ���� ���� 

		if (offset >= (MAX_SECTOR_SIZE / sizeof(desc))) // ���� ���� ���� ��ũ�� ��� 
		{
			disk->write_sector(disk, sectorNumber++, buffer);
			offset = 0;
			ZeroMemory(buffer, sizeof(buffer)); // ���� Ŭ���� 
		}
	}

	return EXT2_SUCCESS;
}

/* ���� */
/* block bitmap�� inode bitmap �ʱ�ȭ */
int clear_bitmap(DISK_OPERATIONS* disk, EXT2_SUPER_BLOCK* sb, UINT32 blkGroupNumber)
{
	BYTE buffer[MAX_SECTOR_SIZE];
	UINT32 offset = 0;
	UINT32 blkSize = EXT2_MIN_BLOCK_SIZE << sb->logBlockSize;
	UINT32 groupCount = ((sb->blockCount - sb->firstDataBlock - 1) / sb->blocksPerGroup) + 1; // �� �׷� ����
	UINT32 descTableBlks = ((EXT2_DESC_SIZE * groupCount) + (blkSize - 1)) / blkSize; // �׷� ��ũ���Ͱ� �����ϴ� ��� ����
	UINT32 sectorsPerBlock = blkSize / MAX_SECTOR_SIZE; // ��� �� ���� ��
	UINT32 sectorsPerGroup = sectorsPerBlock * sb->blocksPerGroup; // ��ϱ׷� �� ���� ��
	UINT32 sectorNumber = (EXT2_MIN_BLOCK_SIZE + blkSize + descTableBlks * blkSize) / MAX_SECTOR_SIZE + blkGroupNumber * sectorsPerGroup; // write ���� ���� ��ȣ 

	while (offset < blkSize * 2) // block bitmap, inode bitmap clear 
	{
		ZeroMemory(buffer, sizeof(buffer));
		disk->write_sector(disk, sectorNumber, buffer);
		offset += MAX_SECTOR_SIZE;
		sectorNumber++;
	}

	return EXT2_SUCCESS;
}

/* ���� */
/* inode table �ʱ�ȭ */
int clear_inode_table(DISK_OPERATIONS* disk, EXT2_SUPER_BLOCK* sb, UINT32 blkGroupNumber)
{
	BYTE buffer[MAX_SECTOR_SIZE];
	UINT32 blkSize = EXT2_MIN_BLOCK_SIZE << sb->logBlockSize; // ��� ũ�� 
	UINT32 groupCount = ((sb->blockCount - sb->firstDataBlock - 1) / sb->blocksPerGroup) + 1; // ��� �׷� ���� 
	UINT32 descTableBlks = ((EXT2_DESC_SIZE * groupCount) + (blkSize - 1)) / blkSize; // �׷� ��ũ���Ͱ� �����ϴ� ��� �� 

	UINT32 inoBlksPerGroup = ((sb->inodesPerGroup * sb->inodeSize) + (blkSize - 1)) / blkSize; // �׷� �� inode table ��� �� 
	UINT32 inoSecPerGroup = inoBlksPerGroup * EXT2_BLOCK_SIZE / MAX_SECTOR_SIZE; // �׷� �� inode table ���� ��

	UINT32 sectorsPerBlock = blkSize / MAX_SECTOR_SIZE; // ��� �� ���� ��
	UINT32 sectorsPerGroup = sectorsPerBlock * sb->blocksPerGroup; // ��ϱ׷� �� ���� ��
	UINT32 sectorNumber = (EXT2_MIN_BLOCK_SIZE + blkSize + descTableBlks + 2 * blkSize) / MAX_SECTOR_SIZE + blkGroupNumber * sectorsPerGroup; // write ���� ���� ��ȣ 

	UINT32 i;

	if (disk == NULL || sb == NULL || blkGroupNumber < 0)
	{
		printf("error : wrong argument\n");
		return EXT2_ERROR;
	}

	for (i = 0; i < inoSecPerGroup; i++)
	{
		ZeroMemory(buffer, sizeof(buffer));
		disk->write_sector(disk, sectorNumber + i, buffer);
	}

	return EXT2_SUCCESS;
}

/* ���� */
/* ��Ʈ ���͸� ���� */
int create_root(DISK_OPERATIONS* disk, EXT2_SUPER_BLOCK* sb)
{
	BYTE buffer[MAX_SECTOR_SIZE];
	EXT2_INODE* inode;
	EXT2_GROUP_DESC* desc;
	EXT2_DIR_ENTRY* entry;
	UINT32 firstDataBlk, inoBlksPerGroup, descTableBlks;
	UINT32 blkSize, groupCount;
	UINT32 sectorNumber;
	UINT32 sectorsPerBlock;

	groupCount = ((sb->blockCount - sb->firstDataBlock - 1) / sb->blocksPerGroup) + 1; // ��� �׷� ���� 
	blkSize = EXT2_MIN_BLOCK_SIZE << sb->logBlockSize; // �ϳ��� ��� ũ�� 
	descTableBlks = ((EXT2_DESC_SIZE * groupCount) + (blkSize - 1)) / blkSize; // �׷� ��ũ���Ͱ� �����ϴ� ��� ���� 
	inoBlksPerGroup = ((sb->inodeSize * sb->inodesPerGroup) + (blkSize - 1)) / blkSize; // �׷�� inode table ��� �� 

	// 0�� ��� �׷� ������ ���� ���� ��� 
	firstDataBlk = (EXT2_MIN_BLOCK_SIZE + (3 + descTableBlks + inoBlksPerGroup) * EXT2_BLOCK_SIZE) / EXT2_BLOCK_SIZE;
	sectorsPerBlock = blkSize / MAX_SECTOR_SIZE;

	// INODE
	ZeroMemory(buffer, sizeof(buffer));
	inode = (EXT2_INODE *)buffer;
	inode[EXT2_ROOT_INO - 1].fileMode = ACCESSED_BY_ANYONE | FILE_TYPE_DIR;
	inode[EXT2_ROOT_INO - 1].linkCount = 2; // dot, dotdot 
	inode[EXT2_ROOT_INO - 1].blockCount = 1;
	inode[EXT2_ROOT_INO - 1].i_block[0] = firstDataBlk; // 0�� ��� �׷��� ������ ���� �� �պκ��� ��Ʈ ���丮 ��Ʈ�� 

	sectorNumber = firstDataBlk * (EXT2_BLOCK_SIZE / MAX_SECTOR_SIZE);
	disk->write_sector(disk, sectorNumber, buffer); // inode table 2��(��Ʈ ���丮)�� ��� 

													// EXT2_DIR_ENTRY
	ZeroMemory(buffer, sizeof(buffer));
	entry = (EXT2_DIR_ENTRY *)buffer;

	// dot entry 
	entry->inode = EXT2_ROOT_INO;
	entry->dir2.nameLength = 1;
	memcpy(entry->name, ".", 1);
	entry->dir2.fileType = EXT2_FT_DIR;
	entry++;

	// dotdot entry 
	entry->inode = EXT2_ROOT_INO;
	entry->dir2.nameLength = 2;
	memcpy(entry->name, "..", 2);
	entry->dir2.fileType = EXT2_FT_DIR;

	// 0�� ��� �׷��� ������ ���� �� �պκп� ��Ƽ ���丮 ��Ʈ�� ��� 
	sectorNumber = firstDataBlk * sectorsPerBlock;
	disk->write_sector(disk, sectorNumber, buffer);

	// �׷� ��ũ���� ������Ʈ 
	desc = (EXT2_GROUP_DESC *)buffer;
	desc->bg_freeBlockCount--;
	desc->bg_freeInodeCount--;
	desc->bg_usedDirCount++;

	// ���� ��� ������Ʈ 
	sb->freeBlockCount--;
	sb->freeInodeCount--;

	return EXT2_SUCCESS;
}

/* ���� */
/* �� ��� �׷��� ���� �ʱ�ȭ �� ��Ʈ ���͸� ���� */
int ext2_format(DISK_OPERATIONS* disk)
{
	EXT2_SUPER_BLOCK sb;
	UINT32 groupCount;
	UINT32 blkGroupNumber = 0;
	BYTE buffer[MAX_SECTOR_SIZE];
	EXT2_SUPER_BLOCK * p_sb = &sb;

	if (fill_super_block(p_sb, disk->numberOfSectors, disk->bytesPerSector) != EXT2_SUCCESS)
	{
		printf("error : failed to fill super block\n");
		return EXT2_ERROR;
	}

	groupCount = ((p_sb->blockCount - p_sb->firstDataBlock - 1) / p_sb->blocksPerGroup) + 1; // �� �׷� ���� 

	while (blkGroupNumber < groupCount)
	{
		write_super_block(disk, p_sb, blkGroupNumber);
		init_desc(disk, p_sb, blkGroupNumber);
		clear_bitmap(disk, p_sb, blkGroupNumber); // block bitmap, inode bitmap �ʱ�ȭ 
		clear_inode_table(disk, p_sb, blkGroupNumber);

		blkGroupNumber++;
	}

	printf("total sector count		: %u\n", disk->numberOfSectors);
	printf("total block count		: %u\n", sb.blockCount);
	printf("total inode count		: %u\n", sb.inodeCount);
	printf("sector byte size		: %u\n", MAX_SECTOR_SIZE);
	printf("block byte size			: %u\n", EXT2_BLOCK_SIZE);
	printf("inode byte size			: %u\n", sb.inodeSize);
	printf("\n");

	create_root(disk, p_sb);

	return EXT2_SUCCESS;
}


/******************************************************************************/
/* mount / umount	                                                          */
/******************************************************************************/

/* ���� */
/* ���ۺ�� ��ȿ�� �˻� */
int validate_superblock(EXT2_FILESYSTEM* fs)
{
	UINT32 blockSize;
	EXT2_SUPER_BLOCK* sb = &fs->sb;
	EXT2_SB_INFO* sb_info = &fs->sb_info;

	if (sb->logBlockSize < 0 || sb->logBlockSize > 2) // ��� ũ�� �˻� 
	{
		printf("error : invalid block size\n");
		return EXT2_ERROR;
	}

	if (sb->magicSignature != 0xEF53) // �ñ״�ó �˻� 
	{
		printf("error : invalid signature\n");
		return EXT2_ERROR;
	}

	return EXT2_SUCCESS;
}

/* ���� */
/* location�� entry ������ ���� */
int set_entry(EXT2_FILESYSTEM* fs, EXT2_DIR_ENTRY_LOCATION* location, EXT2_DIR_ENTRY* newEntry)
{
	EXT2_SUPER_BLOCK* sb = &fs->sb;
	UINT32 block;
	BYTE buffer[EXT2_BLOCK_SIZE];

	block = sb->firstDataBlock + location->group * sb->blocksPerGroup + location->block;
	ZeroMemory(buffer, sizeof(buffer));

	if(read_block(fs, block, buffer) != EXT2_SUCCESS)
	{
		printf("error : failed to read_block() in get_entry()\n");
		return EXT2_ERROR;
	}
	memcpy(&((EXT2_DIR_ENTRY *)buffer)[location->offset], newEntry, sizeof(EXT2_DIR_ENTRY));

	if(write_block(fs, block, buffer) != EXT2_SUCCESS)
	{
		printf("error : failed to write_block() in get_entry()\n");
		return EXT2_ERROR;
	}

	return EXT2_SUCCESS; 
}

/* ���� */
/* location�� entry ������ ���� */
int get_entry(EXT2_FILESYSTEM* fs, EXT2_DIR_ENTRY_LOCATION* location, EXT2_DIR_ENTRY* retEntry)
{
	EXT2_SUPER_BLOCK* sb = &fs->sb;
	UINT32 block;
	BYTE buffer[EXT2_BLOCK_SIZE];

	block = sb->firstDataBlock + location->group * sb->blocksPerGroup + location->block;
	ZeroMemory(buffer, sizeof(buffer));

	if(read_block(fs, block, buffer) != EXT2_SUCCESS)
	{
		printf("error : failed to read_block() in get_entry()\n");
		return EXT2_ERROR;
	}
	memcpy(retEntry, &((EXT2_DIR_ENTRY *)buffer)[location->offset], sizeof(EXT2_DIR_ENTRY));

	return EXT2_SUCCESS; 
}

/* ���� */
/* ��ũ���� ���ۺ���� ������ �о�� */
int ext2_read_superblock(EXT2_FILESYSTEM* fs, EXT2_NODE* root)
{
	BYTE buffer[EXT2_MIN_BLOCK_SIZE];
	int result;
	int nameLength;
	UINT32 descTableBlks, inoBlksPerGroup;
	UINT32 groupCount, inodesPerGroup;
	if (fs == NULL || fs->disk == NULL)
	{
		printf("error : wrong argumenet\n");
		printf("DISK OPERATIONS : %p \nFAT_FILESYSTEM : %p \n", fs, fs->disk);
		return EXT2_ERROR;
	}

	int readNum = sizeof(fs->sb);
	UINT32 sectorNumber = EXT2_MIN_BLOCK_SIZE / MAX_SECTOR_SIZE;
	int i = 0;
	int offset = 0;

	while (offset < readNum)
	{
		result = fs->disk->read_sector(fs->disk, sectorNumber + i, &buffer[offset]); // ���ۿ� ���ۺ���� ������ �о�� 
		if (result)
		{
			printf("error : failed to read sector %d\n", sectorNumber + i);
			return EXT2_ERROR;
		}	
		offset += MAX_SECTOR_SIZE;
		i++;
	}

	memcpy(&fs->sb, buffer, EXT2_MIN_BLOCK_SIZE);

	result = validate_superblock(fs);

	if (result) // ���� ��� ������ ��ȿ���� ������
	{
		printf("error : invalid super block\n");
		return EXT2_ERROR;
	}


	groupCount = ((fs->sb.blockCount - fs->sb.firstDataBlock - 1) / fs->sb.blocksPerGroup) + 1;
	descTableBlks = ((EXT2_DESC_SIZE * groupCount) + (EXT2_BLOCK_SIZE - 1)) / EXT2_BLOCK_SIZE;
	inoBlksPerGroup = ((fs->sb.inodeSize * fs->sb.inodesPerGroup) + (EXT2_BLOCK_SIZE - 1)) / EXT2_BLOCK_SIZE;

	ZeroMemory(root, sizeof(EXT2_NODE));

	root->fs = fs;
	root->location.group = 0;
	root->location.block = (3 + descTableBlks + inoBlksPerGroup) * EXT2_BLOCK_SIZE / EXT2_BLOCK_SIZE;
	root->location.offset = 0;

	get_entry(fs, &root->location, &root->entry);
	printf("root->entry.inode : %d\n", root->entry.inode);

	return EXT2_SUCCESS;
}

/* ���� */
/* SB_INFO ����ü ��� �� ���� */
int fill_sb_info(EXT2_FILESYSTEM* fs)
{
	EXT2_SUPER_BLOCK* sb = &fs->sb;
	EXT2_SB_INFO* sb_info = &fs->sb_info;
	UINT32 sectorPerBlock;
	int i = 0;

	ZeroMemory(sb_info, sizeof(EXT2_SB_INFO));

	sb_info->blockSize = EXT2_MIN_BLOCK_SIZE << sb->logBlockSize;
	sb_info->groupCount = ((sb->blockCount - sb->firstDataBlock - 1) / sb->blocksPerGroup) + 1;
	sb_info->inodesPerBlock = sb_info->blockSize / EXT2_INODE_SIZE;
	sb_info->blocksPerGroup = sb_info->blockSize * 8;
	sb_info->inodesPerGroup = sb->inodesPerGroup;
	sb_info->itbPerGroup = (sb->inodesPerGroup * EXT2_INODE_SIZE + (sb_info->blockSize - 1)) / sb_info->blockSize;
	sb_info->blocksPerDesc = (sizeof(EXT2_GROUP_DESC) * sb_info->groupCount + (sb_info->blockSize - 1)) / sb_info->blockSize;
	sb_info->sectorsPerBlock = sb_info->blockSize / MAX_SECTOR_SIZE;
	sb_info->sectorsPerGroup = sb_info->blocksPerGroup * sectorPerBlock;
	sb_info->descPerBlock = sb_info->blockSize / EXT2_DESC_SIZE;
	sb_info->firstDescBlock = sb->firstDataBlock + 1;
	sb_info->descPerBlock_bits = sb->logBlockSize + 5;
	sb_info->blockSize_bits = sb->logBlockSize;
	sb_info->inodeSize = EXT2_INODE_SIZE;
	sb_info->firstInode = sb->firstInode;
	sb_info->dirCount = 1;
	sb_info->freeBlockCount = sb->freeBlockCount;
	sb_info->freeInodeCount = sb->freeInodeCount;

	return EXT2_SUCCESS;
}

/* ���� */
/* mount ���� */
void ext2_umount(EXT2_SB_INFO* sb_info)
{
	return; // do nothing
}


/******************************************************************************/
/* ls								                                          */
/******************************************************************************/

/* ���� */
/* �� ���� ���͸� ��Ʈ������ list�� �߰� */
int read_dir_from_block(EXT2_FILESYSTEM* fs, BYTE* buffer, EXT2_NODE_ADD adder, void* list)
{
	EXT2_DIR_ENTRY* entry;
	EXT2_NODE node;
	UINT32 offset = 0;
	UINT32 maxEntry = EXT2_BLOCK_SIZE / sizeof(EXT2_DIR_ENTRY);
	int i;

	ZeroMemory(&entry, sizeof(entry));
	entry = (EXT2_DIR_ENTRY*)buffer;

	for (i = 0; i < maxEntry; i++)
	{
		if (entry->dir2.fileType == EXT2_FT_FREE)
			;
		else if (entry->dir2.fileType == EXT2_FT_NO_MORE) // ������ ��Ʈ���� ��
			return -1;
		else
		{
			node.fs = fs;
			node.entry = *entry;
			adder(list, &node);
		}
		entry++;
	}

	return EXT2_SUCCESS;
}

/* ���� */
/* ���͸� ��Ʈ���� �������� ���� */
int ext2_read_dir(EXT2_NODE* dir, EXT2_NODE_ADD adder, void* list) // ���͸��� ��Ʈ���� �о� list�� �߰� 
{
	EXT2_INODE inode;
	BYTE buffer[EXT2_BLOCK_SIZE];
	UINT32 offset = 0;
	int i;

	ZeroMemory(&inode, sizeof(EXT2_INODE));
	ZeroMemory(buffer, sizeof(buffer));

	if (get_inode(dir->fs, dir->entry.inode, &inode) != EXT2_SUCCESS)
	{
		printf("error : failed to get inode\n");
		return EXT2_ERROR;
	}

	printf("fileMode	: %#X\n", inode.fileMode);
	printf("fileSize	: %u\n", inode.fileSize);
	printf("blockCount	: %u\n", inode.blockCount);

	for (i = 0; i < inode.blockCount; i++)
	{
		read_block(dir->fs, inode.i_block[i], buffer); // ��ϴ����� ��ũ���� �о��
		read_dir_from_block(dir->fs, buffer, adder, list); // ���� ��Ʈ���� list�� �߰�
	}

	return EXT2_SUCCESS;
}


/******************************************************************************/
/* cd									                                      */
/******************************************************************************/

/* ���� */
/* �̸��� ���Ͻý��� ���Ŀ� �°� ���� */ /* ���� �ؾ��� */
int format_name(EXT2_FILESYSTEM* fs, char* name)
{
	UINT32	i, length;
	UINT32	extender = 0, nameLength = 0;
	UINT32	extenderCurrent = 8;
	BYTE	regularName[MAX_NAME_LENGTH];
	BYTE	nameBuf[MAX_ENTRY_NAME_LENGTH];

	memset(regularName, 0x20, sizeof(regularName));
	memset(nameBuf, 0x20, sizeof(nameBuf));
	length = strlen(name);

	if (strncmp(name, "..", 2) == 0)
	{
		memcpy(nameBuf, "..", 2);
		memcpy(name, nameBuf, MAX_ENTRY_NAME_LENGTH);
		return EXT2_SUCCESS;
	}
	else if (strncmp(name, ".", 1) == 0)
	{
		memcpy(nameBuf, ".", 1);
		memcpy(name, nameBuf, MAX_ENTRY_NAME_LENGTH);
		return EXT2_SUCCESS;
	}
	else
	{
		upper_string(name, MAX_NAME_LENGTH);

		for (i = 0; i < length; i++)
		{
			if (name[i] != '.' && !isdigit(name[i]) && !isalpha(name[i]))
				return EXT2_ERROR;
				
			if (name[i] == '.')
			{
				if (extender)
					return EXT2_ERROR;
				extender = 1;
			}
			else if (isdigit(name[i]) || isalpha(name[i]))
			{
				if (extender)
					regularName[extenderCurrent++] = name[i];
				else
					regularName[nameLength++] = name[i];
			}
			else
				return EXT2_ERROR;
		}

		if (nameLength > MAX_ENTRY_NAME_LENGTH - 3 || nameLength == 0 || extenderCurrent > MAX_ENTRY_NAME_LENGTH)
			return EXT2_ERROR;
	}

	memcpy(name, regularName, sizeof(regularName));
	return EXT2_SUCCESS;
}

/* ���� */
/* ��� ���� number��° ��Ʈ���� ã�� */
int find_entry_at_block(EXT2_FILESYSTEM* fs, const BYTE block, const char* entryName, UINT32* number)
{
}

/* ���� */
/* ���� ���� ���� ��� �˻� */
int indirect_traversal(EXT2_FILESYSTEM* fs, UINT32 block, const EXT2_INODE* inode, UINT32 *probe, BYTE* prevBuf, UINT32* retBlk)
{
	EXT2_SB_INFO* sb_info = &fs->sb_info;
	UINT32 ptrsPerBlk = sb_info->blockSize / sizeof(UINT32);
	BYTE nextBuf[EXT2_BLOCK_SIZE];
	UINT32 offset;

	ZeroMemory(nextBuf, sizeof(nextBuf));

	(*probe)--;

	if (*probe == 0)
	{
		// ���� ������ ��Ͽ��� �ε��� ���ϱ�
		offset = (block - EXT2_NDIR_BLOCKS) % ptrsPerBlk;
		*retBlk = ((UINT32 *)prevBuf)[offset];
		return EXT2_SUCCESS;
	}
	else
	{
		// ���� ����� �ε��� ���ϱ�, 8�� 2�� 8��(256)�� �ǹ���
		offset = (block + (1 << (*probe * 8) - 1)) / (1 << (*probe * 8));
		read_block(fs, &((UINT32 *)prevBuf)[offset], nextBuf);
	}

	return indirect_traversal(fs, block, inode, probe, nextBuf, retBlk);
}

/* ���� */
/* ���� ���� ��� ���� */
int get_indirect_block(EXT2_FILESYSTEM* fs, UINT32 block, const EXT2_INODE* inode, UINT32* retBlk)
{
	EXT2_SB_INFO* sb_info = &fs->sb_info;
	UINT32 ptrsPerBlk = sb_info->blockSize / sizeof(UINT32);
	BYTE buffer[EXT2_BLOCK_SIZE];
	UINT32 probe = -1;

	if (probe == -1)
	{
		if (block >= EXT2_IND_BLOCK &&
			block < EXT2_IND_BLOCK + ptrsPerBlk)
			probe = 1; // ���� ���� ���� ���
		else if (block < EXT2_IND_BLOCK + ptrsPerBlk)
			probe = 2; // ���� ���� ���� ���
		else
			probe = 3; // ���� ���� ���� ���
	}

	// ù ���� ��� �о ����
	ZeroMemory(buffer, sizeof(buffer));
	read_block(fs, inode->i_block[EXT2_IND_BLOCK], buffer);
	indirect_traversal(fs, block, inode, &probe, buffer, retBlk);

	return EXT2_SUCCESS;
}

/* ���� */
/* ��� ��ȣ�� ���� �׷� ��ȣ */
int get_group_of_block(EXT2_FILESYSTEM* fs, UINT32 block, UINT32* retGroup)
{
	*retGroup = (block - fs->sb.firstDataBlock) / fs->sb_info.blocksPerGroup;

	return EXT2_SUCCESS;
}

/* ���� */
/* inode ��ȣ�� ���� ��� ��ȣ */
int get_block_of_inode(EXT2_FILESYSTEM* fs, UINT32 inode, UINT32* retBlk)
{
	EXT2_SB_INFO* sb_info = &fs->sb_info;
	UINT32 group;
	UINT32 block;


	group = (inode - 1) / sb_info->inodesPerGroup; // inode�� �ִ� �׷�

	// inode table ���� ���
	block = EXT2_MIN_BLOCK_SIZE / MAX_SECTOR_SIZE + group * sb_info->blocksPerGroup + 1 + sb_info->blocksPerDesc + 1 + 1; 
	*retBlk = block + ((inode - 1) % sb_info->inodesPerGroup) / sb_info->inodesPerBlock;

	if(*retBlk < 0) 
	{
		printf("error : invalid block number in get_block_of_inode()\n");
		return EXT2_ERROR;
	}

	return EXT2_SUCCESS;
}

/* ���� */
/* inode�� block ��° �Ҵ���� ��� ��ȣ ���� */
int get_allocated_block(EXT2_FILESYSTEM* fs, UINT32 block, const EXT2_INODE* inode, UINT32* retBlk)
{
	if (block < 0)
		return EXT2_ERROR;
	else if (block < EXT2_NDIR_BLOCKS)
		block = inode->i_block[block];
	else
	{
		if (get_indirect_block(fs, block, inode, retBlk) != EXT2_SUCCESS) // i�� ���� ��� ��ȣ ����
		{
			printf("error : failed to get indirect block\n");
			return EXT2_ERROR;
		}
	}

	return EXT2_SUCCESS;
}

/* ���� */
/* inode �� ���� ��ȸ�ϸ� entryName Ž�� */
/* */
int lookup_entry(EXT2_FILESYSTEM* fs, const EXT2_INODE* inode, const BYTE* entryName, EXT2_NODE* ret)
{
	BYTE* buffer[EXT2_BLOCK_SIZE];
	EXT2_SB_INFO* sb_info = &fs->sb_info;
	UINT32 block, retBlk, number;
	UINT32 usedBlk;
	UINT32 i, result;

	ZeroMemory(buffer, sizeof(buffer));

	usedBlk = inode->blockCount;

	for (i = 0; i < usedBlk; i++)
	{
		if (get_allocated_block(fs, i, inode, &retBlk) != EXT2_SUCCESS) // inode ����ü�� i_block[i]�� ��� ��ȣ ����
		{
			printf("error : failed to get allocated block number\n");
			return EXT2_ERROR;
		}

		if (read_block(fs, retBlk, buffer) != EXT2_SUCCESS)
		{
			printf("error : failed to read block in lookup_entry()\n");
			return EXT2_ERROR;
		}
		result = find_entry_at_block(fs, buffer, entryName, &number);
		if (result == EXT2_ERROR)
		{
			printf("error : failed to find entry\n");
		}
	}

	return result;
}

/* ���� */
/* ���� ���͸��� entryName��� ��Ʈ���� �ִ��� �˻� */
int ext2_lookup(EXT2_NODE* parent, const char* entryName, EXT2_NODE* retEntry)
{
	unsigned char* name;
	EXT2_INODE inode;

	my_strncpy(name, entryName, MAX_ENTRY_NAME_LENGTH);

	if (format_name(parent->fs, name) == EXT2_ERROR)
		return EXT2_ERROR;

	if (get_inode(parent->fs, parent->entry.inode, &inode)) 
	{
		printf("error : failed to get inode\n");
		return EXT2_ERROR;
	}

	return lookup_entry(parent->fs, &inode, name, retEntry);
}


/******************************************************************************/
/* touch, fill							                                      */
/******************************************************************************/

/* ���� */
/* �ش� ��� �׷��� ��ũ���� ����ü�� �о�� */
int read_desc(EXT2_FILESYSTEM* fs, UINT32 blkGroupNumber, BYTE* retDesc)
{
	BYTE buffer[EXT2_BLOCK_SIZE];
	EXT2_SB_INFO* sb_info = &fs->sb_info;
	UINT32 block;
	UINT32 offset;

	ZeroMemory(buffer, sizeof(buffer));

	offset = blkGroupNumber % sb_info->descPerBlock;
	block = sb_info->firstDescBlock + blkGroupNumber / sb_info->descPerBlock;
	read_block(fs, block, buffer);
	memcpy(retDesc, &((EXT2_GROUP_DESC *)buffer)[offset], sizeof(EXT2_GROUP_DESC));

	return EXT2_SUCCESS;
}

/* ���� */
/* �ش� ��� �׷��� ��ũ���� ����ü�� �� */
int write_desc(EXT2_FILESYSTEM* fs, UINT32 blkGroupNumber, BYTE* retDesc)
{
	BYTE buffer[EXT2_BLOCK_SIZE];
	EXT2_SB_INFO* sb_info = &fs->sb_info;
	UINT32 block;
	UINT32 offset;

	ZeroMemory(buffer, sizeof(buffer));

	offset = blkGroupNumber % sb_info->descPerBlock;
	block = sb_info->firstDescBlock + blkGroupNumber / sb_info->descPerBlock;
	read_block(fs, block, buffer);
	memcpy(&((EXT2_GROUP_DESC *)buffer)[offset], retDesc, sizeof(EXT2_GROUP_DESC));

	write_block(fs, block, buffer);

	return EXT2_SUCCESS;
}

/* ���� */
/* ���丮 ���� Ȯ�� */
int is_dir(EXT2_NODE* entry)
{
	if (entry->entry.dir2.fileType == EXT2_FT_DIR)
		return EXT2_SUCCESS;
	else
		return EXT2_ERROR;
}

/* ���� */
/* ��Ʈ ���͸����� Ȯ�� */
int is_root_dir(EXT2_NODE* entry)
{
	if (entry->entry.inode == EXT2_ROOT_INO && entry->entry.dir2.fileType == EXT2_FT_DIR)
		return EXT2_SUCCESS;
	else
		return EXT2_ERROR;
}
/* ���� */
/* entry�� ���� inode �Ҵ����ְ�, entry ��� �ʱ�ȭ */
int make_entry(EXT2_FILESYSTEM* fs, EXT2_NODE* parent, const char* name, UINT32 fileType, EXT2_NODE* newEntry)
{
	EXT2_SB_INFO* sb_info = &fs->sb_info;
	
	if(alloc_inode(fs, parent, newEntry) != EXT2_SUCCESS)
	{
		printf("error : failed to alloc_inode() in make_entry\n");
		return EXT2_ERROR;
	}
	newEntry->entry.recordLength = sizeof(EXT2_DIR_ENTRY);
	newEntry->entry.dir2.nameLength = MAX_ENTRY_NAME_LENGTH;
	newEntry->entry.dir2.fileType = fileType;
	memcpy(newEntry->entry.name, name, MAX_ENTRY_NAME_LENGTH);

	return EXT2_SUCCESS;
}


/* ���� */
/* ��Ʈ�� ���� */
int insert_entry(EXT2_NODE* parent, EXT2_NODE* newEntry, UINT32 overwrite)
{
	BYTE buffer[EXT2_BLOCK_SIZE];
	EXT2_INODE* inode;
	EXT2_DIR_ENTRY_LOCATION location;
	EXT2_SUPER_BLOCK* sb = &parent->fs->sb;
	EXT2_SB_INFO* sb_info = &parent->fs->sb_info;
	inode = (EXT2_INODE *)buffer;
	UINT32 blockNumber;

	EXT2_NODE entryNoMore;

	// �θ� ���丮�� inode �о��
	ZeroMemory(buffer, sizeof(buffer));
	inode = (EXT2_INODE *)buffer;
	if (get_inode(parent->fs, parent->entry.inode, inode) != EXT2_SUCCESS)
	{
		printf("error : failed to get inode\n");
		return EXT2_ERROR;
	}

	if(get_allocated_block(parent->fs, 0, inode, &blockNumber) != EXT2_SUCCESS)
	{
		printf("error : failed to get_allocated_block() in insert_entry()\n");
		return EXT2_ERROR;
	}

	location.group = (blockNumber - sb->firstDataBlock) / sb_info->blocksPerGroup; // ��� �׷� ��ȣ
	location.block = (blockNumber - sb->firstDataBlock) % sb_info->blocksPerGroup;
	location.offset = 0;

	// ��Ʈ ���丮�� �ƴϰ� overwrite�� set�̸�
	if (!is_root_dir(&parent->entry) && overwrite == 1)
	{
		if(set_entry(parent->fs, &location, &newEntry->entry) != EXT2_SUCCESS)
		{
			printf("error : failed to set_entry() in insert_entry()\n");
			return EXT2_ERROR;
		}
		newEntry->location = location;
		// �ϴ� ������
		location.offset = 1;
		ZeroMemory(&entryNoMore, sizeof(entryNoMore));
		entryNoMore.entry.dir2.fileType = EXT2_FT_NO_MORE;
		entryNoMore.entry.recordLength = EXT2_BLOCK_SIZE - sizeof(EXT2_DIR_ENTRY);
		if(set_entry(parent->fs, &location, &entryNoMore.entry) != EXT2_SUCCESS)
		{
			printf("error : failed to set_entry() in insert_entry()\n");
			return EXT2_ERROR;
		}

		return EXT2_SUCCESS;
	}

	entryNoMore.entry.dir2.fileType = EXT2_FT_FREE; // ������ ��Ʈ�� ã��
	if(lookup_entry(parent->fs, inode, NULL, &entryNoMore) == EXT2_SUCCESS)
	{
		set_entry(parent->fs, &entryNoMore.location, &newEntry->entry);
		newEntry->location = entryNoMore.location;
	}
	else
	{
		entryNoMore.entry.dir2.fileType = EXT2_FT_NO_MORE;
		if(lookup_entry(parent->fs, inode, NULL, &entryNoMore) != EXT2_SUCCESS)
			return EXT2_ERROR;
		
		set_entry(parent->fs, &entryNoMore.locatioinm &newEntry->entry);
		newEntry->location = entryNoMore.location;
		entryNoMore.location.number++;

		if(entryNoMore.location.number == (EXT2_BLOCK_SIZE / sizeof(EXT2_DIR_ENTRY)))
		{
			if(alloc_block(parent->fs, parent) != EXT2_SUCCESS)
			{
				printf("error : failed to alloc_block() in insert_entry()\n");
				return EXT2_ERROR;
			}
			get_allocated_block(parent->fs, inode->blockCount, inode, &entryNoMore.location)

	}
	

	return EXT2_SUCCESS;
}

/* ���� */
/* ���� ���� */
int ext2_create(EXT2_NODE* parent, const char* entryName, EXT2_NODE* retEntry)
{
	/*
	format_name() // �̸� ���� ���� *
	lookup_entry() // �̸��� ��Ʈ�� ���� ���� �˻�*
	insert_entry() // ���� ���͸��� ��Ʈ�� ����Ʈ�� ���ο� ��Ʈ�� �߰�*
	alloc_inode() // ������ inode �Ҵ�
	EXT2_DIR_ENTRY �ʱ�ȭ
	adjust_free_count() // �ɼǿ� ���� freeCount ���� (COUNT_UP / COUNT_DOWN)
	set_bitmap() // bitmap ����
	*/
	BYTE buffer[EXT2_BLOCK_SIZE];
	EXT2_DIR_ENTRY_LOCATION first;
	EXT2_INODE* inode;
	EXT2_FILESYSTEM* fs = &parent->fs;
	BYTE name[MAX_NAME_LENGTH] = { 0, };
	int result;

	strncpy(name, entryName, MAX_ENTRY_NAME_LENGTH);

	if (format_name(parent->fs, name))
		return EXT2_ERROR;

	ZeroMemory(retEntry, sizeof(EXT2_NODE));

	// ���ۿ� inode ��ü �о��
	if (get_inode(fs, parent->entry.inode, buffer) != EXT2_SUCCESS)
	{
		printf("error : failed to get inode\n");
		return EXT2_ERROR;
	}

	inode = (EXT2_INODE *)buffer;

	// inode�� ���� �̸� ��Ʈ�� ã��
	if (lookup_entry(parent->fs, inode, name, retEntry) == EXT2_SUCCESS)
	{
		printf("error : no entry named %s\n", name);
		return EXT2_ERROR;
	}

	if(make_entry(parent->fs, parent, name, EXT2_FT_REG_FILE, retEntry) != EXT2_SUCCESS)
	{
		printf("error : failed to make_entry() in ext2_create()\n");
		return EXT2_ERROR;
	}

	retEntry->fs = parent->fs;

	if (insert_entry(parent, retEntry, 0) != EXT2_SUCCESS)
	{
		printf("error : failed to insert entry\n");
		return EXT2_ERROR;
	}

	return EXT2_SUCCESS;
}


/******************************************************************************/
/* remove							                                          */
/******************************************************************************/

/* ���� ���� */
int ext2_remove(EXT2_NODE* file)
{

}


/******************************************************************************/
/* mkdir, mkdirst					                                          */
/******************************************************************************/

/* ���� */
/* ���͸� ���� */
int ext2_mkdir(const EXT2_NODE* parent, const char* entryName, EXT2_NODE* retEntry)
{
	EXT2_NODE dotNode, dotdotNode;
	EXT2_INODE inode;
	BYTE name[MAX_ENTRY_NAME_LENGTH];

	my_strncpy((char*)name, entryName);

	if (format_name(parent->fs, (char*)name) == EXT2_ERROR)
		return EXT2_ERROR;

	ZeroMemory(retEntry, sizeof(EXT2_NODE));
	memcpy(retEntry->entry.name, name, MAX_ENTRY_NAME_LENGTH);
	retEntry->entry.dir2.nameLength = strlen((char*)retEntry->entry.name);
	retEntry->fs = parent->fs;
	retEntry->entry.dir2.fileType = EXT2_FT_DIR;

	if (alloc_inode(parent->fs, parent, retEntry) == EXT2_ERROR)
	{
		printf("error : failed to alloc inode\n");
		return EXT2_ERROR;
	}

	if (insert_entry(parent, retEntry, 0) == EXT2_ERROR)
	{
		printf("error : failed to insert entry\n");
		return EXT2_ERROR;
	}

	if(alloc_block(parent->fs, retEntry->entry.inode, retEntry))
	{
		printf("error : failed to alloc block\n");
		return EXT2_ERROR;
	}

	ZeroMemory(&dotNode, sizeof(EXT2_NODE));
	memset(dotNode.entry.name, 0x20, 24);
	dotNode.entry.name[0] = '.';
	dotNode.fs = retEntry->fs;
	dotNode.entry.inode = retEntry->entry.inode;
	dotNode.entry.dir2.fileType = EXT2_FT_DIR;
	insert_entry(retEntry, &dotNode, 0);

	ZeroMemory(&dotNode, sizeof(EXT2_NODE));
	memset(dotNode.entry.name, 0x20, 24);
	dotNode.entry.name[0] = '.';
	dotNode.entry.name[1] = '.';
	dotNode.fs = retEntry->fs;
	dotNode.entry.inode = retEntry->entry.inode;
	dotNode.entry.dir2.fileType = EXT2_FT_DIR;
	insert_entry(retEntry, &dotNode, 0);

	return EXT2_SUCCESS;
}


/******************************************************************************/
/* rmdir							                                          */
/******************************************************************************/

/* ���� */
/* ���͸��� ���� ��Ʈ���� ������ �ִ��� �˻� */
int has_sub_entry(EXT2_FILESYSTEM* fs, EXT2_NODE* node)
{
	EXT2_DIR_ENTRY_LOCATION location = node->location;
	EXT2_NODE entry;
	UINT32 inode = node->entry.inode;

	location.offset += 2;

	if (lookup_entry(fs, inode, NULL, &entry) == EXT2_SUCCESS)
		return EXT2_ERROR;
	return EXT2_ERROR;
}

/* ���� */
/* ���͸� ���� */
int ext2_rmdir(EXT2_NODE* node)
{
	BYTE buffer[EXT2_BLOCK_SIZE];

	if (has_sub_entry(node->fs, &node->entry) == EXT2_SUCCESS) // ���� ��Ʈ�� ������ ���� ����
	{
		printf("error : this directory has entry yet\n");
		return EXT2_ERROR;
	}

	if (node->entry.dir2.fileType != EXT2_FT_DIR) // ���͸��� �ƴϸ� ����
	{
		printf("error : no directory\n");
		return EXT2_ERROR;
	}

	ZeroMemory(buffer, sizeof(buffer));
	node->entry.dir2.fileType = EXT2_FT_FREE; // ������ ��Ʈ�� ����
	free_block(node);
	free_inode(node);
	set_entry(node->fs, &node->location, &node->entry); // ����� ���� ����

	return EXT2_SUCCESS;
}


/******************************************************************************/
/* df								                                          */
/******************************************************************************/

int ext2_df(EXT2_FILESYSTEM* fs, UINT32* totalSectors, UINT32* usedSectors)
{

}


/******************************************************************************/
/* dump								                                          */
/******************************************************************************/

/* ���� */
/* ���� �޸𸮿� ����� �� ��� */
int ext2_dump(DISK_OPERATIONS* disk, int blockGroupNum, int type, int target)
{
	BYTE sector[MAX_SECTOR_SIZE];
	UINT32 totalBlock = (disk->bytesPerSector*disk->numberOfSectors + (EXT2_BLOCK_SIZE - 1)) / EXT2_BLOCK_SIZE;
	UINT32 groupCount = ((disk->numberOfSectors * disk->bytesPerSector + (EXT2_BLOCK_SIZE - 1)) / EXT2_BLOCK_SIZE - 1) / (EXT2_BLOCK_SIZE * 8) + 1; // ��ϱ׷��
	UINT32 blocksPerDesc = (sizeof(EXT2_GROUP_DESC) * groupCount + (EXT2_BLOCK_SIZE - 1)) / EXT2_BLOCK_SIZE; // ��ũ���� ��� ��
	UINT32 itbPerGroup = (((totalBlock / 2) / groupCount) * EXT2_INODE_SIZE + (EXT2_BLOCK_SIZE - 1)) / EXT2_BLOCK_SIZE;// inode table ��� ��
	UINT32 blockNum = 1 + blockGroupNum * EXT2_BLOCK_SIZE * 8; // ��� �׷� ���� ��� ��ȣ
	UINT32 sectorNum = 2 * blockNum; // ���� ���� ��ȣ
	UINT32 sectorCount; // ���� ���� ��
	UINT32 i, j;

	switch (type)
	{
	case 1: // super block
		sectorCount = EXT2_MIN_BLOCK_SIZE / MAX_SECTOR_SIZE;
		break;
	case 2: // group descriptor table
		blockNum += 1;
		sectorCount = blocksPerDesc * (EXT2_BLOCK_SIZE / MAX_SECTOR_SIZE);
		printf("blocksPerDesc : %u\n", blocksPerDesc);
		break;
	case 3: // inode bitmap
		blockNum += 1 + blocksPerDesc;
		sectorCount = EXT2_BLOCK_SIZE / MAX_SECTOR_SIZE;
		break;
	case 4: // block bitmap
		blockNum += 2 + blocksPerDesc;
		sectorCount = EXT2_BLOCK_SIZE / MAX_SECTOR_SIZE;
		break;
	case 5: // inode table
		blockNum += 3 + blocksPerDesc;
		printf("itbPerGroup : %u\n", itbPerGroup);
		sectorCount = itbPerGroup * (EXT2_BLOCK_SIZE / MAX_SECTOR_SIZE);
		break;
	case 6: // data by block number
		blockNum += 3 + blocksPerDesc + itbPerGroup;
		sectorCount = EXT2_BLOCK_SIZE / MAX_SECTOR_SIZE;
		break;
	}

	sectorNum += (blockNum - 1) * (EXT2_MIN_BLOCK_SIZE / MAX_SECTOR_SIZE);
	printf("block number : %u\tstart sector : %u\tsector count : %u\n", blockNum, sectorNum, sectorCount);
	printf("start adress : %p , end address : %p\n", sector, sector + MAX_SECTOR_SIZE * sectorCount - 1);

	for (i = 0; i < sectorCount; i++)
	{
		ZeroMemory(sector, sizeof(sector));
		disk->read_sector(disk, sectorNum + i, sector);
		for (j = 0; j < MAX_SECTOR_SIZE; j++)
		{
			if (j % 16 == 0)
				printf("\n%p\t", &sector[j] + 0x200 * i);
			else if (j % 2 == 0)
				printf(" ");
			printf("%02X ", sector[j]);

		}
		printf("\n");
	}
	printf("\n\n");

	return EXT2_SUCCESS;
}

void hexDump(DISK_OPERATIONS* disk, BYTE *addr, UINT32 len)
{
	BYTE* s = addr;
	BYTE* endPtr = (BYTE *)((UINT32)addr + len);
	UINT32 i;
	UINT32 remainder = len % 16;

	printf("\n Offset		Hex Value 		Ascii value\n");

	//print out 16byte blocks
	while ((UINT32)(s + 16) <= (UINT32)endPtr)
	{
		//offset ���
		printf("0x%08lx  ", (long)(s - addr));

		//16Byte
		for (i = 0; i < 16; i++)
		{
			printf("%02x ", s[i]);
		}
		printf(" ");

		for (i = 0; i < 16; i++)
		{
			if (s[i] >= 32 && s[i] <= 125)
				printf("%c", s[i]);
			else
				printf(".");
		}
		s += 16;
		printf("\n");
	}

	// Print out remainder
	if (remainder)
	{
		//offset ���
		printf("0x%08lx  ", (long)(s - addr));

		//16byte ������ ����ϰ� ���� �� ���
		for (i = 0; i < remainder; i++)
		{
			printf("%02x ", s[i]);
		}
		for (i = 0; i < (16 - remainder); i++)
		{
			printf("    ");
		}

		printf(" ");
		for (i = 0; i < remainder; i++)
		{
			if (s[i] >= 32 && s[i] <= 125)
				printf("%c", s[i]);
			else
				printf(".");
		}

		for (i = 0; i < (16 - remainder); i++)
		{
			printf(" ");
		}
		printf("\n");
	}
	return;
}

void print_hexDump(DISK_OPERATIONS* disk, UINT32 block)
{
	UINT32 offset = 0;
	UINT32 sectorNumber = (EXT2_MIN_BLOCK_SIZE / MAX_SECTOR_SIZE) + (EXT2_BLOCK_SIZE / MAX_SECTOR_SIZE) * (block - 1);
	BYTE addr[EXT2_BLOCK_SIZE];

	ZeroMemory(addr, EXT2_BLOCK_SIZE);

	while (offset < EXT2_BLOCK_SIZE)
	{
		disk->read_sector(disk, sectorNumber, &addr[offset]);
		offset += MAX_SECTOR_SIZE;
		sectorNumber++;
	}

	printf("sectorNumber : %d\n", sectorNumber);
	printf("addr[1023] : %d\n", ((UINT32 *)addr)[255]);

	hexDump(disk, addr, EXT2_BLOCK_SIZE);

	return EXT2_SUCCESS;
}