#include <stdio.h>
#include <stdint.h>

typedef struct Superblock {
	uint16_t magic;
	uint8_t version;
	uint32_t inode_start;
	uint32_t data_start;
} Superblock;

int main()
{
	Superblock sb;
	sb.magic = 0x1F0B4FA;
	sb.version = 1;
	sb.inode_start = 1024;
	sb.data_start = 4096;

	return 0;
}

