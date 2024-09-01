
# PageTableHierarchy

## Introduction

Welcome to the **HierarchicalMemoryManager** project! This project offers a comprehensive look into memory management systems by implementing a hierarchical page table approach. If you're interested in learning how virtual memory maps to physical memory and the role of page tables, you’re in the right place!

### Overview

Efficient memory management is essential in modern computing. This project simulates the management of virtual and physical memory using a hierarchical page table system. It breaks down memory into frames and pages, demonstrating effective memory allocation and access. The hierarchical page table approach mirrors real-world memory management systems, providing a practical learning experience.

### Algorithm Example

Included in this repository is a detailed explanation of the hierarchical memory management algorithm. The [Algorithm Example.pdf](https://github.com/user-attachments/files/16829274/Algorithm.Example.pdf)
provides a visual representation and explanation of how virtual memory is mapped to physical memory using a hierarchical page table. This PDF will help you understand the structure and process of hierarchical page tables, offering insights into how memory management is handled in a layered approach.

### Getting Started

To build and run the project, follow these steps:

1. **Clone the Repository**:
   ```sh
   git clone <repository-url>
   cd <repository-directory>
   ```

2. **Build the Project**:
   Use the Makefile to compile the project and run tests:
   ```sh
   make
   ```

3. **Run Tests**:
   After building the project, you can run the tests to verify functionality:
   ```sh
   make run_tests
   ```

4. **Run the Sample Client**:
   To execute the sample client and observe the memory management system in action, use:
   ```sh
   make sample_client
   ```

5. **Clean Up**:
   To remove build artifacts and start fresh, run:
   ```sh
   make clean
   ```

## File Structure

- `PhysicalMemory.cpp` / `PhysicalMemory.h`: Implementation and definitions for physical memory.
- `VirtualMemory.cpp` / `VirtualMemory.h`: Implementation and definitions for virtual memory.
- `MemoryConstants.h`: Header file with memory constants.
- `SimpleTest.cpp`: A simple test case for the memory manager.
- `tests/`: Contains test cases and headers for validating the memory manager.
- `Algorithm Example.pdf`: Provides a detailed explanation and visual representation of the hierarchical memory management algorithm used in this project.

Feel free to explore the code and the provided test cases to gain a deeper understanding of hierarchical memory management. If you have any questions or need further assistance, don't hesitate to reach out!

---

This README now includes a detailed description of the PDF, explaining its role and content in relation to the project.
