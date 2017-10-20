/**
Name: Luis Torres
Disk Simulation
Date: 05/02/2017
**/

#include "disk.h"

/**
Disk simulator.
Format:
-200 cylinders within a disk.
-10 heads within each cylinder.
-128 sectors within each head. 
-Each sector of memory consists of 1280 chars. //logical block same size as a sector
4 functions needed:
-log-to-phys //ltp & ptl first to implement. test wiith random address
-phys-to-log
-read
-write
**/

cylinder_t disks[NUM_OF_CYLS];

/**
This function translates a logical to a physical address.
**/
int log_to_phys(int logaddr, physaddr_t *phaddr)
{
	if (logaddr >= 0 && logaddr < MAX_LOGICAL_SECTOR)
	{
		phaddr->cyl = logaddr/(NUM_OF_SECTS * NUM_OF_HEADS);
		phaddr->head = (logaddr/NUM_OF_SECTS) % NUM_OF_HEADS;
		phaddr->sect = logaddr % NUM_OF_SECTS;

		printf("\n");
		printf("The logical address is: %d\n", logaddr);
		printf("The physical address is:\n");
		printf("Cylinder: %d\n", phaddr->cyl);
		printf("Head: %d\n", phaddr->head);
		printf("Sector: %d\n", phaddr->sect);
		printf("\n");
	}
	else
	{
		printf("Error: Your logical address value is not within the bounds.\n");
	}

	return 0;
}

/**
This function translates a physical to a logical address.
**/
int phys_to_log(physaddr_t *phaddr)
{
	int logaddr;
	if (phaddr->head > 0)
	{
		/* code */
		logaddr = (NUM_OF_SECTS * phaddr->head) + phaddr->sect + (phaddr->cyl * SECT_SIZE);
	}
	else
	{
		logaddr = phaddr->sect;
	}
	
	printf("The physical address is:\n");
	printf("Cylinder: %d\n", phaddr->cyl);
	printf("Head: %d\n", phaddr->head);
	printf("Sector: %d\n", phaddr->sect);
	printf("The logical address is: %d\n", logaddr);
	return logaddr;
}

/**
This function copies num_of_sectors sectors from the disk to the buffer 
starting with sector pointed to by logical_block_num. 
You should validate the parameters before serving the request.
**/
int read(int logical_block_num, int num_of_sectors, void **buffer)
{
	if (logical_block_num >= 0 && num_of_sectors > 0)
	{
		/* code */
		physaddr_t phaddr;
		log_to_phys(logical_block_num, &phaddr);
		buffer = malloc(sizeof(NUM_OF_HEADS * NUM_OF_CYLS * NUM_OF_SECTS * SECT_SIZE));
		if (phaddr.sect >= 0)
		{
			/* code */
			memcpy(buffer, disks[phaddr.head][phaddr.cyl][phaddr.sect], num_of_sectors);
			printf("Read: Successfully executed.\n");
			printf("The new buffer contains: %s\n", (char*) buffer);
		}
	}
	return 0;
}

/**
This function copies num_of_sectors sectors from the buffer to the disk 
starting at sector pointed to by logical_block_num. 
Here, you also should validate the parameters.
**/
int write(int logical_block_num, int num_of_sectors, void *buffer)
{
	//void* disk;
	if (logical_block_num >= 0 && num_of_sectors > 0)
	{
		/* code */
		physaddr_t phaddr;
		log_to_phys(logical_block_num, &phaddr);
		if (phaddr.sect >= 0)
		{
			/* code */
			memcpy(disks[phaddr.head][phaddr.cyl][phaddr.sect], buffer, num_of_sectors);
			printf("Write: Successfully executed.\n");
			//printf("Disk: %p\n", (disks[phaddr.head][phaddr.cyl][phaddr.sect]));
		}
	}
	return 0;
}
