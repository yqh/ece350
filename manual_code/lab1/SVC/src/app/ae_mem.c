/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE 350 RTOS LAB
 *
 *                     Copyright 2020-2021 Yiqing Huang
 *
 *          This software is subject to an open source license and
 *          may be freely redistributed under the terms of MIT License.
 ****************************************************************************
 */

/**************************************************************************//**
 * @file        ae_mem.c
 * @brief       memory lab auto-tester
 *
 * @version     V1.2021.01
 * @authors     Yiqing Huang
 * @date        2021 JAN
 *
 *****************************************************************************/

#include "rtx.h"
#include "Serial.h"
#include "printf.h"

int test_case_1 ();
int test_case_2 ();
int test_case_3 ();

// Runs 3 test cases
// returns 0 if all cases pass, negative value if at least one case fails
// Negative return value shows how many cases faied (e.g. -2 means two cases failed)
int test_mem(void) {
	 int res1 = test_case_1();
	 int res2 = test_case_2();
	 int res3 = test_case_3();

    //Running 100 tests for each testcase to calculate Throughput
    /*int test_1_start_time = time();
    for (int i = 0; i < 100; i++){
        res1 = test_case_1();
    }
    int test_2_start_time = time();
    for (int i = 0; i < 100; i++){
        res2 = test_case_2();
    }

    int test_3_start_time = time();
    for (int i = 0; i < 100; i++){
        res3 = test_case_3();
    }
    int test_3_end_time = time();
    printf("Test 1 duration: %d", (test_2_start_time - test_1_start_time));
    printf("Test 2 duration: %d", (test_3_start_time - test_2_start_time));
    printf("Test 3 duration: %d", (test_3_end_time - test_3_start_time));*/
    return res1 + res2 + res3;
}

// TEST CASE 1: Testing Overhead and External Fragmentation
// Allocates chunks of memory that are size 8, 16, 32, 64, and 128 bytes
// Deallocates size 16 and 64 bytes chunks
// Uses mem_count_extfrag() to verify the state of the heap
// returns 0 if successful and -1 otherwise
int test_case_1 () {
    void *p[6];                                     //pointers for allocated chunks
    int bytes_to_alloc = 8;                         //size of the first allocated chunk
    int byte_input[] = {9, 32, 33, 80, 81, 500};    //inputs to mem_count_extfrag()
    const int INPUT_SIZE = 6;                       //amount of inputs tested
    int output = 0;                                 //ouput of mem_count_extfrag
    int expected_output[] = {0, 0, 1, 1, 2, 2};     //expected output from inputs byte_input[]
    int total_bytes_allocated = 0;                  //for measuring Heap utilization ratio
    const int HEAP_SIZE = (0xBFFFFFFF - 0x80000000);//entire heap size
    //float heap_util_ratio = 0;                    //Heap utilization ratio

    printf("\r\n TEST CASE 1: Testing Overhead and External Fragmentation\r\n");

    // This for loop allocates chunks of memory that are size 8, 16, 32, 64, and 128 bytes in that order
    for (int i = 0; i < 6; i++) {
        p[i] = mem_alloc(bytes_to_alloc);
        printf("Starting Address for pointer %d: 0x%x\r\n", i, (U32) p[i]);
        total_bytes_allocated = total_bytes_allocated + bytes_to_alloc;
        bytes_to_alloc = bytes_to_alloc * 2;
    }

    printf("Total bytes allocated = %d\r\n", total_bytes_allocated);

    // NOTE: for verifying overhead
    // We know the the overhead for a mem frag is 16 bytes by seeing sizeof(struct node_t) in k_mem.c
    // We can verify this By analyzing the pointers returned from mem_alloc in succession
    // We know that the free usable space (without header) allocated to pointer p[2] is 32 bytes
    // So the address of p[3] subtracted by the address of p[2] will give us the size of the allocated space + overhead
    // subtracting this by 32 bytes will give us the overhead (which should be 16 bytes)
    // this can also be done for the other pointers created


    // Dealloc the 16 and 64 bytes (32 and 80 bytes with the header) chunks
    mem_dealloc(p[1]);
    mem_dealloc(p[3]);

    // Print the number of free memory fragments for the values in the byte_input array
    for (int i = 0; i < INPUT_SIZE; i++) {
        output = mem_count_extfrag(byte_input[i]);
        printf("Number of free memory regions strictly less than %d bytes: %d\r\n", byte_input[i], output);
        if (output != expected_output[i]) {
            printf("TEST CASE 1 FAILED\r\n");
            return -1;
        }
    }

    printf("Entire heap size = %d\r\n", HEAP_SIZE);
    //Calculate Heap utilization ratio and print
    //heap_util_ratio = total_bytes_allocated / HEAP_SIZE;
    //printf("Heap utilization ratio = %0.5f\r\n", HEAP_SIZE);

    //cleanup
    mem_dealloc(p[0]);
    mem_dealloc(p[2]);
    mem_dealloc(p[4]);
    mem_dealloc(p[5]);

    printf("Number of free memory regions strictly less than %d bytes: %d\r\n", 5000, mem_count_extfrag(5000));

    return 0;
}

// TEST CASE 2: Testing Coalescing
// Allocates chunks of memory that are size 8, 16, 32, and 64 bytes
// Checks that there are 0 free memory regions less than a large size (5000 bytes)
//   (at this point should be 4 chunks of used memory and very large chunk unused)
// Deallocates the 16 bytes chunk
//   Check that it is deallocated (result of 1 from mem_count_extfrag(33))
// Deallocates the 32 bytes chunk
//   Checks that free 16 bytes chunk no longer exists (result of 0 frommem_count_extfrag(33))
//   Checks that free 32 bytes chunk no longer exists (result of 0 frommem_count_extfrag(49))
//   Checks that the two adjacent free chunks are coalesced (result of 1 from mem_count_extfrag(81))
// Deallocates the 8 bytes chunk
//   Checks that free 80 bytes chunk no longer exists (result of 0 frommem_count_extfrag(81))
//   Checks that free 8 bytes chunk no longer exists (result of 0 frommem_count_extfrag(9))
//   Checks that the two adjacent free chunks are coalesced (result of 1 from mem_count_extfrag(105))
int test_case_2() {
    void *p[4];                                                 //pointers for allocated chunks
    int bytes_to_alloc = 8;                                     //size of the first allocated chunk
    int count_inputs[] = {5000, 33, 33, 49, 81, 81, 9, 105};    //inputs for mem_count_extfrag()
    int expected_outputs[] = {0, 1, 0, 0, 1, 0, 0, 1};          //expected outputs from mem_count_extfrag()
    int res = 0;                                                //temp variable
    int total_bytes_allocated = 0;                  //for measuring Heap utilization ratio
    const int HEAP_SIZE = (0xBFFFFFFF - 0x80000000);//entire heap size
    //float heap_util_ratio = 0;                      //Heap utilization ratio

    printf("\r\n TEST CASE 2: Testing Coalescing\r\n");

    // This for loop allocates chunks of memory that are size 8, 16, 32, and 64 bytes in that order
    for (int i = 0; i < 4; i++) {
        p[i] = mem_alloc(bytes_to_alloc);
        total_bytes_allocated = total_bytes_allocated + bytes_to_alloc;
        bytes_to_alloc = bytes_to_alloc * 2;
    }

    printf("Total bytes allocated = %d\r\n", total_bytes_allocated);

    // basically what this for loop is doing:
    //   mem_count_extfrag(5000);
    //   mem_dealloc(p[1]);
    //   mem_count_extfrag(33);
    //   mem_dealloc(p[2]);
    //   mem_count_extfrag(33);
    //   mem_count_extfrag(49);
    //   mem_count_extfrag(81);
    //   mem_dealloc(p[0]);
    //   mem_count_extfrag(81);
    //   mem_count_extfrag(9);
    //   mem_count_extfrag(105);
    for (int i = 0; i < 8; i++) {
        res = mem_count_extfrag(count_inputs[i]);
        printf("Number of free memory regions strictly less than %d bytes: %d\r\n", count_inputs[i], res);
        if (res != expected_outputs[i]) {
            printf("TEST CASE 2 FAILED\r\n");
            return -1;
        }
        if (i == 0) {
            mem_dealloc(p[1]);
        } else if (i == 1) {
            mem_dealloc(p[2]);
        } else if (i == 4) {
            mem_dealloc(p[0]);
        }
    }

    printf("Entire heap size = %d\r\n", HEAP_SIZE);
    //Calculate Heap utilization ratio and print
    //heap_util_ratio = total_bytes_allocated / HEAP_SIZE;
    //printf("Heap utilization ratio = %0.5f\r\n", HEAP_SIZE);

    //cleanup
    mem_dealloc(p[3]);
    res = mem_count_extfrag(5000);
    printf("Number of free memory regions strictly less than %d bytes: %d\r\n", 5000, res);
    if (res != 0) {
        printf("TEST CASE 2 FAILED\r\n");
        return -1;
    }
    return 0;
}

// TEST CASE 3: Testing Reusing Newly Freed Regions and Leakage
// Allocates 2 chunks of memory that are size 16 bytes and prints the starting addresses of the fragments
// Deallocates the first chunk
// runs mem_count_extfrag(33)); to make sure it is freed
// Allocates a new chunk of 8 bytes
// Checks if the pointer is the same as the original first 16 chunk starting pointer
//   If they are the same, the chunk that was just freed was reused
// Deallocates the just created 8 byte chunk
// Allocates a 32 bytes chunk
//   the starting address should be greater than the starting address of pointer 2 as it is too large to fit in the freed chunk
// Allocates a 16 byte chunk
//   the starting address should again be the starting address of the firstly created chunk as it fits in the freed space
//   This shows that the memory is utilized without leakage
int test_case_3() {
    void *p[5];                                                 //pointers for allocated chunks
    int bytes_to_alloc = 16;                                        //size of the first allocated chunk
    int total_bytes_allocated = 0;                  //for measuring Heap utilization ratio
    const int HEAP_SIZE = (0xBFFFFFFF - 0x80000000);//entire heap size
    //float heap_util_ratio = 0;                      //Heap utilization ratio

    printf("\r\n TEST CASE 3: Testing Reusing Newly Freed Regions\r\n");

    // This for loop allocates 2 chunks of memory that are size 16 byte
    for (int i = 0; i < 2; i++) {
        p[i] = mem_alloc(bytes_to_alloc);
        total_bytes_allocated = total_bytes_allocated + bytes_to_alloc;
        printf("Starting Address for pointer %d: 0x%x\r\n", i, (U32) p[i]);
    }

    // deallocate one of the 16 byte memory sizes 
    mem_dealloc(p[0]);

    printf("Number of free memory regions strictly less than %d bytes: %d\r\n", 33, mem_count_extfrag(33));

    // allocate an 8 byte memory 
    p[2] = mem_alloc(8);
    total_bytes_allocated = total_bytes_allocated + 8;
    // should go into the 16 byte memory region that was just deallocated 
    printf("Starting Address for pointer %d: 0x%x\r\n", 3, (U32) p[2]);
    if (p[2] != p[0]) {
        printf("TEST CASE 3 FAILED\r\n");
        return -1;
    }

    // deallocate the 8 byte memory 
    mem_dealloc(p[2]);

    // allocate a 32 byte memory region
    p[3] = mem_alloc(32);
    total_bytes_allocated = total_bytes_allocated + 32;
    if (p[3] == p[0]) {
        // 32 byte memory should not go into the first chunk since we just allocated 8 bytes of memory 
        printf("TEST CASE 3 FAILED\r\n");
        return -1;
    }
    printf("Starting Address for pointer %d: 0x%x\r\n", 4, (U32) p[3]);

    // allocate a 16 byte memory region 
    p[4] = mem_alloc(16);
    total_bytes_allocated = total_bytes_allocated + 16;
    printf("Starting Address for pointer %d: 0x%x\r\n", 5, (U32) p[4]);
    // should go into just recently deallocated 8 byte memory region (8 + 8 = 16)
     if (p[4] != p[0]) {
        printf("TEST CASE 3 FAILED\r\n");
        return -1;
    }

     printf("Total bytes allocated = %d\r\n", total_bytes_allocated);
     printf("Entire heap size = %d\r\n", HEAP_SIZE);
     //Calculate Heap utilization ratio and print
     //heap_util_ratio = total_bytes_allocated / HEAP_SIZE;
     //printf("Heap utilization ratio = %0.5f\r\n", HEAP_SIZE);


    //cleanup

    mem_dealloc(p[1]);
    mem_dealloc(p[3]);
    mem_dealloc(p[4]);
    printf("Number of free memory regions strictly less than %d bytes: %d\r\n", 5000, mem_count_extfrag(5000));

    return 0;
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
