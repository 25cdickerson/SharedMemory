// inventory.c - Shared Memory Project - CSC 280 Preston Dickerson
// Import Statements
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>


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
	int run;
	do{
		printf("Enter 1 to display shared memory\nEnter 2 to exit\n");
		scanf("%d", &run);
		printf("\n");

		if(run == 1){
        		for(int i = 0; i < items -> num; i++){
        			printf("\nitem [%d]: %s\n", i, items -> str[i].name);
        		}
		}

	}while(run == 1);

	// Close out of all of shared memory
	int status = close(fd);
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


	printf("Exiting\n");
	return 0;
}

