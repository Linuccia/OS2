#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "kernelmodule.h"

static void print_program_usage() {
    printf("Command: ./client [-vma|-vfs] <PID>\n");
}

int main( int argc, char *argv[] ) {
    FILE *vma_file = fopen(DEBUGFS_VMA_FILE_PATH, "r");
    FILE *vma_arg_file = fopen(DEBUGFS_VMA_ARG_FILE_PATH, "w");
    FILE *vfs_file = fopen(DEBUGFS_VFS_FILE_PATH, "r");
    if ( !vma_file || !vfs_file || !vma_arg_file ) {
        printf("Cannot open files for debugfs driver\n");
        return -1;
    }
    if ( argc < 2 || argc > 3 ) {
        print_program_usage();
        return -1;
    }
    char *line = NULL;
    size_t length = 0;
    if ( strcmp(argv[1], "-vma") == 0 ) {
        printf("Sending your PID to kernel module...\n");
        char buf[256];
        sprintf(buf, "%s", argv[2]);
        fwrite(&buf, 1, sizeof(buf), vma_arg_file);
        printf("Getting vm_area_struct information from kernel module...\n ");
        while (getline(&line, &length, vma_file) != -1) {
            printf("%s", line);
        }
    } else if ( strcmp(argv[1], "-vfs") == 0 ) {
        printf("Getting vfsmount struct information from kernel module...\n ");
        while (getline(&line, &length, vfs_file) != -1) {
            printf("%s", line);
        }
    } else {
        print_program_usage();
        return -1;
    }
    fclose(vma_file);
    fclose(vfs_file);
    fclose(vma_arg_file);

    return 0;
}
