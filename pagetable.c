#include <stdio.h>
#include <stdlib.h>
#include "vm.h"
#include "disk.h"
#include "pagetable.h"

PT *pageTable;
Invert *frame_table;
STATS stats;
int freeFrameNumber=0;

int is_page_hit(int pid, int pageNo, char type)
{
	if(pageTable[pid].entry[pageNo].valid == true)
	{
		return pageTable[pid].entry[pageNo].frameNo;
	}
	else 
	{
		return -1;
	}
}

int MMU(int pid, int addr, char type, bool *hit)
{
		int frameNo;
		int pageNo = (addr >> 8);
		int offset = addr - (pageNo << 8);
		int physicalAddr = 0;
		
		if(pageNo >= NUM_PAGE) {
				printf("invalid page number (NUM_PAGE = 0x%x): pid %d, addr %x\n", NUM_PAGE, pid, addr);
				return -1;
		}
		if(pid > (NUM_PROC-1)) {
				printf("invalid pid (valid pid = 0 - %d): pid %d, addr %x\n", NUM_PROC-1, pid, addr);
				return -1;
		}
				
		// hit
		frameNo = is_page_hit(pid, pageNo, type);
		if(frameNo >= 0) {
				stats.hitCount++;
				physicalAddr = (frameNo << 8) + offset;
				
				*hit = true;
				return physicalAddr;
		}
		if(type=='W')
			pageTable[pid].entry[pageNo].dirty = true;

		page_update(frameNo);

		stats.missCount++;
		*hit = false;
		
		// miss. handle 
		if(freeFrameNumber<NUM_FRAME)
		{
			frameNo = freeFrameNumber;
			++freeFrameNumber;
			pageTable[pid].entry[pageNo].frameNo = frameNo;
			pageTable[pid].entry[pageNo].valid = true;
			pageTable[pid].entry[pageNo].dirty = false;
			
			swap_read(frameNo,pid,pageNo);
			
			frame_table[frameNo].pid = pid;
			frame_table[frameNo].page = pageNo;
		}
		// pagefault
		else{
			frameNo = page_replacement();

			int oldpid = frame_table[frameNo].pid;
			int oldpage = frame_table[frameNo].page;
		
			if(pageTable[oldpid].entry[oldpage].valid == true)
			{
				if(pageTable[oldpid].entry[oldpage].dirty == true)
				swap_write(frameNo, oldpid, oldpage);
				pageTable[oldpid].entry[oldpage].valid = false;
				pageTable[oldpid].entry[oldpage].dirty = false;
			}

			frame_table[frameNo].pid = pid;
			frame_table[frameNo].page = pageNo;

			pageTable[pid].entry[pageNo].frameNo = frameNo;
			pageTable[pid].entry[pageNo].valid = true;
			pageTable[pid].entry[pageNo].dirty = false;
		
			swap_read(frameNo, pid, pageNo);
		}
		physicalAddr = (frameNo << 8) + offset;
		return physicalAddr;
}

void pt_print_stats()
{
		int req = stats.hitCount + stats.missCount;
		int hit = stats.hitCount;
		int miss = stats.missCount;

		printf("Request: %d\n", req);
		printf("Page Hit: %d (%.2f%%)\n", hit, (float) hit*100 / (float)req);
		printf("Page Miss: %d (%.2f%%)\n", miss, (float)miss*100 / (float)req);

}

void pt_init()
{
		int i,j;
		pageTable = (PT*) malloc(sizeof(PT) * NUM_PROC);
		frame_table = (Invert*) malloc(sizeof(Invert*) * NUM_FRAME);
		stats.hitCount = 0;
		stats.missCount = 0;

		for(i = 0; i < NUM_PROC; i++) {
				pageTable[i].entry = (PTE*) malloc(sizeof(PTE) * NUM_PAGE);
				for(j = 0; j < NUM_PAGE; j++) {
						pageTable[i].entry[j].valid = false;
				}
		}
}

