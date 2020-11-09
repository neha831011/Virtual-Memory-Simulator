# Virtual-Memory-Simulator
In this project, a virtual memory simulator is implemented using a page table. The execution of processes that are not completely in the memory is done via virtual memory using page table and page replacement techniques. Mapping information between the virtual address and physical addresses are stored in the page table. The simulating system is 16-bit (8 bits for index and 8 bits for offset).
The main() function is contained in vm.c, where pt_init() and init_replacement() functions are initially called. pt_init() initialize the page hit and miss count to zero in page table. From the input we can get the pid, virtual address and type (R or W) of the process. The memory management function, MMU () is called to get the physical address from the virtual address. The Memory Management Unit has been enhanced in pagetable.c to maintain page table entries as well as hit and miss count, perform disk writes as well as perform the replacement of pages if necessary.

If the page is found in Page Table then it is a page hit and thus the corresponding frame number is returned. or else it is a page  miss and it needs to be handled.
If it is a page miss, first check if there is any free frame available. If available then read that page in to this available frameNo. Then update the details of this page in the Page Table that is the frame number, valid bit and dirty bit.

If no free frame is available, then call the function pagefault_handler(pid, pageNo, type). To get the frame number we need to use the specified page replacement algorithm. So call the function page_replacement() defined in replacement.c file. In page_replacement() we can call any one of these defined algorithms (Random,Fifo/Lru) for getting the victim frame number for replacement of the page.

Through the fifo() we will find the victim frame in the first in first out manner, by using fifo_currFrame counter.In the LRU algorithm we will use one dimensional array lru_List[] which will hold the least recently used frame details using the variable lru_counter. The lru_counter will increase each time a reference string is used. 

The page_update(frameNo, page_hit) function is called every time inside MMU(int pid, int addr, char type) function, for each of the input values entered. Depending upon the type of replacement algorithm, variables will be updated.
Before replacing the page of the victim frame, check if the existing page number in the victim frame is present in page table or not, and the status of valid bit and dirty bit. If it is a valid page with true dirty bit then first write this page into the disk. And make the valid bit and dirty bit of that page to false in the page table. Also erase the details of this victim page (old page). 
Now read the pageNo from the disk and replace the pageNo in that frame, which is obtained by using replacement algorithm.
Now in Page Table, update the details of this page and newly allocated frame. LRU is implemented using an array lru_List which uses a variable lru_counter to keep track of the least recently used index(frame).

Debug statements have been used throughout and their use has been hidden by uncommenting the "#define NDEBUG" statement in vm.h file.
