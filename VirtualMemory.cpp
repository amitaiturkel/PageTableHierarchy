#include <cmath>
#include "VirtualMemory.h"
#include "PhysicalMemory.h"

    uint64_t getPhysicalAdressegg(uint64_t virtualAddress);

    uint64_t calculateCyclicDistance(uint64_t pageInMemory, uint64_t pageToInsert)
    {
        int64_t absoluteDistance = std::abs (static_cast<int64_t>(pageToInsert
                                                                  - pageInMemory));
        uint64_t distanceFromStart = NUM_PAGES - absoluteDistance;
        uint64_t distanceFromEnd = absoluteDistance;
        return std::fmin (distanceFromStart, distanceFromEnd);
    }


    int isPageEmpty(uint64_t PageIndex){
      for(int entry = 0; entry<PAGE_SIZE; entry++){
        word_t value;
        PMread (PageIndex * PAGE_SIZE + entry,&value);
        if(value != 0){
          return false;
        }
      }
      return true;
    }

    void clearPage(uint64_t PageIndex){
      for (uint64_t i = 0 ; i< PAGE_SIZE;i++){
        //PAGE_SIZE * PageIndex is the real address of the beginning of the table
        PMwrite (PAGE_SIZE * PageIndex + i,0);
      }
    }
    void VMinitialize(){
      clearPage (0);
    }

    word_t findFreeFrameHelper(word_t index, int depth, int current_depth,
                               int &max_frame_index, int64_t page_swapped_in,
                               word_t &max_dis_frame,
                               int64_t &max_dis_frame_vaddr,
                               int64_t current_vaddr,
                               word_t page_fault_frame,
                               int64_t &max_distance,
                               int64_t parent_index,
                               int64_t &max_parent_index){
        max_frame_index++;
        if (current_depth >= depth) {
            int64_t curr_distance = calculateCyclicDistance(current_vaddr,
                                                      page_swapped_in);
            if (index != page_fault_frame && curr_distance > max_distance) {
              max_dis_frame = index;
              max_dis_frame_vaddr = current_vaddr;
              max_distance = curr_distance;
              max_parent_index = parent_index;
            }
            return -1;
        }
        int total_table_bits = VIRTUAL_ADDRESS_WIDTH - OFFSET_WIDTH;
        int bits_per_table_root = total_table_bits / depth;
        int bits_per_table = CEIL((float) total_table_bits / (float)depth);

        uint64_t entry_in_table = PAGE_SIZE;
        if(current_depth == 0){

            entry_in_table = std::pow(2,bits_per_table_root);
        }
        for (uint64_t i = 0; i < entry_in_table; ++i) {
            word_t value;
            PMread(index * PAGE_SIZE + i, &value);
            if (value == 0)
            {
              continue;
            }
            // Option 1: Frame containing an empty table
            if (isPageEmpty(value) && value != page_fault_frame &&
            current_depth < depth-1) // because
              // root is init with 0 in it
            {
                // Detach the son from the parent
                PMwrite(index * PAGE_SIZE + i, 0);
                return value;
            }
            // Recursively traverse child tables
            word_t new_value = findFreeFrameHelper(value, depth, current_depth + 1, max_frame_index,
                                                   page_swapped_in,
                                                   max_dis_frame,
                                                   max_dis_frame_vaddr,
                                                   current_vaddr + (i << ((depth -current_depth - 1) * bits_per_table)),
                                                   page_fault_frame,
                                                   max_distance,
                                                   index * PAGE_SIZE + i,
                                                   max_parent_index);
            if (new_value != -1) {
              return new_value;
            }
        }

        // Option 2: Unused frame
        if (current_depth == 0) {
            if ((max_frame_index + 1) < NUM_FRAMES) {
                return max_frame_index+1 ;
            }
            // Option 3: Evict a page if no unused frame is found
            if (max_dis_frame != 0) {
                // Detach the son from the parent
                PMwrite (max_parent_index,0);
                // Evict the page with the maximum cyclic distance (-the son)
                PMevict(max_dis_frame, max_dis_frame_vaddr);
                return max_dis_frame;
            }
        }
        return -1;
    }

    uint64_t getPhysicalAdresses(uint64_t virtualAddress) {
        if (virtualAddress >= VIRTUAL_MEMORY_SIZE) {
            return -1;
        }
        int depth = CEIL(((VIRTUAL_ADDRESS_WIDTH - OFFSET_WIDTH) / (double)OFFSET_WIDTH));
        int total_table_bits = VIRTUAL_ADDRESS_WIDTH - OFFSET_WIDTH;
        int bits_per_table_root = total_table_bits / depth;
        int bits_per_table = CEIL((float)total_table_bits / (float)depth);
        uint64_t offset_mask = (1 << OFFSET_WIDTH) - 1;
        uint64_t page_offset = virtualAddress & offset_mask;
        uint64_t index_mask = (1 << bits_per_table) - 1;
        uint64_t index_mask_root = (1 << bits_per_table_root) - 1;
        uint64_t current_frame = 0;
        uint64_t addr = 0;
        uint64_t page_swapped_in = (virtualAddress >> OFFSET_WIDTH) & (
            (1<<total_table_bits)-1);


        for (int current_depth = 0; current_depth < depth; ++current_depth) {
            uint64_t index = -1;
            if(current_depth == 0){
                int shift_amount = (depth - 1 - current_depth) *
                    bits_per_table_root + OFFSET_WIDTH;
                index = (virtualAddress >> shift_amount) & index_mask_root;
            }
            else {
                int shift_amount = (depth - 1 - current_depth) *
                    bits_per_table + OFFSET_WIDTH;
                index = (virtualAddress >> shift_amount) & index_mask;
            }
            PMread(current_frame * PAGE_SIZE + index, reinterpret_cast<word_t *>(&addr));
            if (addr == 0)
            {
                int max_frame_index = -1;
                word_t max_dis_frame = 0;
                int64_t max_dis_frame_vaddr = 0;
                int64_t max_distance = 0;
                int64_t max_parent_index = 0;
                // The page where the fault occurs
                word_t free_frame = findFreeFrameHelper(0, depth, 0,
                                                        max_frame_index,
                                                        page_swapped_in,
                                                        max_dis_frame,
                                                        max_dis_frame_vaddr,
                                                        0, current_frame,
                                                        max_distance,0,
                                                        max_parent_index);
                // Find a free frame
                if (free_frame == 0) {
                  return -1; // Handle the case where no free frame is found and eviction fails
                }
                if (current_depth < depth -1)
                {
                  clearPage(free_frame);
                }
                else // last level
                {
                  // Restore the page into the free frame
                  PMrestore(free_frame, page_swapped_in);
                }
                  PMwrite(current_frame * PAGE_SIZE + index, free_frame);
                  addr = free_frame;
            }
            current_frame = addr;
        }
        uint64_t physicalAddress = current_frame * PAGE_SIZE + page_offset;
        return physicalAddress;
    }


    int VMread(uint64_t virtualAddress, word_t* value) {
      if (value == nullptr) {
        return 0;
      }
      uint64_t physicalAddress = getPhysicalAdresses(virtualAddress);
        if (physicalAddress == (uint64_t)-1) {
        return 0;
      }
      PMread(physicalAddress, value);
      return 1;
    }



    /* writes a word to the given virtual address
     *
     * returns 1 on success.
     * returns 0 on failure (if the address cannot be mapped to a physical
     * address for any reason)
     */

    int VMwrite(uint64_t virtualAddress, word_t value){
      uint64_t physicalAddress = getPhysicalAdresses(virtualAddress);
      if (physicalAddress == (uint64_t)-1) {
        return 0;
      }
      PMwrite(physicalAddress, value);
      return 1;
    }


