// consumer.c - Shared Memory Project - CSC 280 Preston Dickerson
// Import Statements
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

// Definitions
#define SHARED_MEMORY_NAME "DIPROD"
#define FALSE 0
#define TRUE 1

// Struct
struct inner{
        char name[32];
};

struct outer{
        int num;
        struct inner str[10];
};

// Function Prototypes
void sig_handle(int signum);

// Global Variables
struct outer *items;
int flags;
mode_t permissions;
size_t size;
int fd;
int c;

// Main
int main(){
        flags = O_RDWR | O_CREAT | O_EXCL;
        permissions = S_IRUSR | S_IWUSR;
        size = sizeof(struct outer);

        // Try Creating the shared memory
        int SHM_creator_status = TRUE;
        fd = shm_open(SHARED_MEMORY_NAME, flags, permissions);
        if(fd == -1){
                if(errno == EEXIST){
                        SHM_creator_status = FALSE;
                        flags = O_RDWR;
                        fd = shm_open(SHARED_MEMORY_NAME, flags, permissions);
                        if(fd == -1){
                                printf("failed to open shared memory\n");
                                exit(1);
                        }
                }
                else{
                        printf("shm_open failed, something is wrong, exiting\n");
                        exit(1);
                }
        }

    	// Resize the shared memory using ftruncate
        if(ftruncate(fd, size) != 0){
                printf("ftrucate failed, exiting\n");
                exit(1);
        }

        // map it
        items = (struct outer*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if(items == MAP_FAILED){
                printf("mmap failed, exiting\n");
                exit(1);
        }        

	// Display / Exit Program do while loop
	while(1){
		if(items -> num == 0){
			printf("All Items Gone! Going to sleep\n");
			signal(SIGINT, sig_handle);
			printf("Sleeping...\n");
			sleep(30);
			printf("Checking for new entries\n");
		}
		else{
        		while(items -> num > 0){
				signal(SIGINT, sig_handle);
				printf("Current Box:\n");
        			printf("Item: %s\n", items -> str[0].name);
				printf("Sending item\n"); 
				strcpy(items -> str[0].name, "");
				items -> num = items -> num - 1;
                  		for(int i = 0; i < 10; i++){
                                        strcpy(items -> str[i].name, items-> str[i + 1].name);
                                        strcpy(items -> str[i + 1].name, "");
                                }
				sleep(30);
        		}
		}
	}
}

// Function Definitions
// signal handler for SIGINT
void sig_handle(int signum){
	int status;
	printf("Starting the cleanup process\n");
	
	// Unmap all the items
	status = munmap(items, size);
	if(status != 0){
		printf("Couldn't unmap the shared memory\n");
	}
	else{
		printf("Successfully unmapped\n");
	}		

	// Close the shared memory
	status = close(fd);
	if(status != 0){
		printf("Couldn't close the shared memory\n");
	}
	else{
		printf("Successfully closed the shared memory\n");
	}


	status = shm_unlink(SHARED_MEMORY_NAME);
	if(status != 0){
		printf("Couldn't delete the shared memory\n");
	}
	else{
		printf("Successfully deleted shared memory, exiting\n");
	}
	exit(signum);
}
