#include "ext2ls.h"

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("2 arguments: inode number, image file>\n");
        return 1;
    }

    char *endptr = NULL;
    uint64_t inode_number = strtoull(argv[1], &endptr, 10);

    int image_file = open(argv[2], O_RDONLY); //
    if (image_file < 0) {
        printf("Cannot open imagery file\n");
        return 1;
    }
    //Jump through first empty 1kb
    seek_to(image_file, 1024, SEEK_SET);

    struct ext2_super_block super_block;
    if (read(image_file, &super_block, sizeof(struct ext2_super_block)) < 0) {
        printf("Cannot read super block:");
        return(1);
    };

    size_t block_size = 0;
    if (super_block.s_log_block_size == 0)
        block_size = 1024;
    else block_size = super_block.s_log_block_size;

    //num of block group
    size_t block_group_num = inode_number / super_block.s_inodes_per_group;
    inode_number %= super_block.s_inodes_per_group;

    struct ext2_group_desc block_group;
    seek_to(image_file, sizeof(struct ext2_group_desc) * block_group_num, SEEK_CUR);
    //reading block group with our inode
    if (read(image_file, &block_group, sizeof(struct ext2_group_desc)) < 0){
        printf("Cannot read block group:");
        return(1);
    };
    size_t inode_offset = block_group.bg_inode_table * block_size +
                          (inode_number - 1) * sizeof(struct ext2_inode);

    struct ext2_inode inode;
    seek_to(image_file, inode_offset, SEEK_SET);
    //reading one inode
    if (read(image_file, &inode, sizeof(struct ext2_inode)) < 0){
        printf("Cannot read inode:");
        return(1);
    };

    if ((inode.i_mode & EXT2_DIR) == 0) {
        printf("Given inode is not associated with directory\n");
        return 1;
    }

    int end = 0;
    char **buf = calloc(sizeof(*buf), 4);
    for (size_t i = 0; i < 4; i++)
        buf[i] = calloc(block_size, 4);
    for (size_t i = 0, read_size = 0; i < 12 && read_size < block_size
                                      && end == 0; i++, read_size = 0) {
        size_t offset = inode.i_block[i] * block_size;
        seek_to(image_file, offset, SEEK_SET);
        if (read(image_file, buf[0], block_size) < 0){
            printf("Cannot read buffer:");
            return(1);
        };
        struct ext2_dir_entry_2 *dir_entry =
                (struct ext2_dir_entry_2 *) buf[0];
        while (dir_entry->inode > 0 && end == 0) {
            if (dir_entry->rec_len > EXT2_NAME_LEN) {
                dir_entry->rec_len = sizeof(dir_entry);
                end = 1;
            }
            dir_entry->name[dir_entry->name_len] = '\0';
            printf("%s\n", dir_entry->name);
            read_size += dir_entry->rec_len;
            dir_entry = (struct ext2_dir_entry_2 *)
                    ((char *) dir_entry + dir_entry->rec_len);
        }
    }
    if (end == 0) {
        size_t offset_single_block = inode.i_block[12] * block_size;
        single_indirect(image_file, offset_single_block, buf,
                        block_size, &end);
    }
    if (end == 0) {
        size_t offset_double_block = inode.i_block[13] * block_size;
        double_indirect(image_file, offset_double_block, buf,
                        block_size, &end);
    }
    if (end == 0) {
        size_t offset_triple_block = inode.i_block[14] * block_size;
        single_indirect(image_file, offset_triple_block, buf,
                        block_size, &end);
    }

    for (size_t i = 0; i < 4; i++)
        free(buf[i]);
    free(buf);

    close(image_file);
    return 0;
}
