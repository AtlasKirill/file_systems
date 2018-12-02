//
// Created by kirill on 03.12.18.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <ext2fs/ext2_fs.h>
#include <fcntl.h>
#include <zconf.h>

#ifndef EXT2_PARSING_EXT2LS_H
#define EXT2_PARSING_EXT2LS_H


const uint16_t EXT2_DIR = 0x4000;


void seek_to(int image_file, long int offset, int whence) {
    if (lseek(image_file, offset, whence) < 0) {
        perror("Connot seek:");
        exit(1);
    }
}

void single_indirect(int image_file,
                     size_t offset_single_block, char **buf, size_t block_size,
                     int *end) {
    seek_to(image_file, offset_single_block, SEEK_SET);
    read(image_file, buf[0], block_size);
    for (size_t i = 0, read_size = 0; i < block_size && read_size < block_size
                                      && *end == 0; i += 4) {
        size_t offset = *((uint32_t *) (buf[0] + i)) * block_size;
        seek_to(image_file, offset, SEEK_SET);
        read(image_file, buf[1], block_size);
        struct ext2_dir_entry_2 *dir_entry =
                (struct ext2_dir_entry_2 *) buf[0];
        while (dir_entry->inode > 0) {
            if (dir_entry->rec_len > EXT2_NAME_LEN) {
                dir_entry->rec_len = sizeof(dir_entry);
                *end = 1;
            }
            dir_entry->name[dir_entry->name_len] = '\0';
            printf("%s\n", dir_entry->name);
            read_size += dir_entry->rec_len;
            dir_entry = (struct ext2_dir_entry_2 *)
                    ((char *) dir_entry + dir_entry->rec_len);
        }
    }
}

void double_indirect(int image_file,
                     size_t offset_double_block, char **buf, size_t block_size,
                     int *end) {
    seek_to(image_file, offset_double_block, SEEK_SET);
    read(image_file, buf[0], block_size);
    for (size_t i = 0; i < block_size && *end == 0; i += 4) {
        size_t single_offset = *((uint32_t *) (buf[0] + i)) * block_size;
        single_indirect(image_file, single_offset, buf + 1,
                        block_size, end);
    }
}

#endif //EXT2_PARSING_EXT2LS_H
