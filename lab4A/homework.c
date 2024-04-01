/*
 * file:        homework.c
 * description: skeleton file for CS 5600 system
 *
 * CS 5600, Computer Systems, Northeastern CCIS
 * Peter Desnoyers, November 2019
 * updated by CS3650 staff, March 2024
 */

#define FUSE_USE_VERSION 30
#define _FILE_OFFSET_BITS 64
#define FS_BLOCK_SIZE 4096
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <fuse3/fuse.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include "fs3650.h"

/* disk access. All access is in terms of 4KB blocks; read and
 * write functions return 0 (success) or -EIO.
 */
extern int block_read(void *buf, int lba, int nblks);
extern int block_write(void *buf, int lba, int nblks);

/* Path splitting - interface is similar to the parsing function in Lab 2.
 *    char buf[PATH_MAX], *pathv[20];
 *    int n = split(path, &pathv, 20, &buf, sizeof(buf));
 */
int split(const char *path, char **pathv, int pathv_len, char *buf, int buf_len)
{
    char *p = buf, **pv = pathv;
    *pv++ = p;
    path++;

    if (*path == 0) return 0; // path is "/"

    while (pv < (pathv+pathv_len) && (p < buf+buf_len+1) && *path != 0) {
        char c = *path++;
        if (c == '/') {
            *p++ = 0;
            *pv++ = p;
        }
        else
            *p++ = c;
    }
    *p++ = 0;
    return pv-pathv;
}

// will split path into an array without /'s -- helper 
char** split2(const char *path, char **pathv, int pathv_len, char *buf, int buf_len)
{
    char *p = buf, **pv = pathv;
    *pv++ = p;
    path++;

    if (*path == 0) return 0; // path is "/"

    if (strlen(path) > 0 && strchr(path, '/') == NULL) {
      strcpy(p, path);
      *pv++ = p;
      *pv++ = NULL;
      return pathv;
    }

    while (pv < (pathv+pathv_len) && (p < buf+buf_len+1) && *path != 0) {
        char c = *path++;
        if (c == '/') {
            *p++ = 0;
            *pv++ = p;
        }
        else
            *p++ = c;
    }
    *p++ = 0;
    *pv = NULL;
    return pathv;
}

/* init - this is called once by the FUSE framework at startup.
 * it reads the superblock into 'super'
 */
struct fs_super super;

void* fs_init(struct fuse_conn_info *conn, struct fuse_config *cfg)
{
    block_read(&super, 0, 1);   /* read superblock */
    assert(super.magic == FS_MAGIC); /* confirm magic number */
    cfg->direct_io = 1;         /* allow non-block-sized reads */

    return NULL;
}

/* Note on path translation errors:
 * In addition to the method-specific errors listed below, almost
 * every method can return one of the following errors if it fails to
 * locate a file or directory corresponding to a specified path.
 *
 * ENOENT - a component of the path doesn't exist.
 * ENOTDIR - an intermediate component of the path (e.g. 'a' or 'b' in
 *           /a/b/c) is not a directory
 */

/* Exercises: You will need to implement the following functions:
 *   fs_getattr
 *   fs_readdir
 *   fs_read
 */



/* Exercise 1:
 * getattr - get file or directory attributes. For a description of
 *  the fields in 'struct stat', see 'man lstat'.
 *
 * You should:
 *  1. parse the path given by "const char *path",
 *     find the inode of the specified file,
 *  2. copy inode's information to "struct stat",
 *     Note - for several fields in 'struct stat' there is
 *         no corresponding information in our file system:
 *          st_nlink - always set it to 1
 *          st_atime, st_ctime - set to same value as st_mtime
 *  3. and return:
 *     * success - return 0
 *     * errors - path translation, ENOENT
 *
 *  hints:
 *  - the helper function "split(...)" is useful
 *  - "block_read(...)" is useful; it reads of data from disk
 *  - it will be useful to create your own helper function to
 *    translate a path to an inode number
 */

// helper -- will traverse path to find inode 
int get_inode(char **path, int pathlen) {
    struct fs_inode rootDirInode;
    struct fs_dirent directories[128];

    if (block_read(&rootDirInode, 1, 1) != 0) { // this will read root dir inode
        return -ENOENT; // in case read fails
    }

    if (block_read(directories, rootDirInode.ptrs[0], 1) != 0) { // will place root dir into dirent 
        return -ENOENT; // in case read fails
    }

    int ret = -ENOENT;

    struct fs_inode tempNode; // temp node (with each read call will get filled w/ new info)
    bool fileFound = false;
    for (int k = 0; k < pathlen; k++) { // traverse through each val of path i.e fs then file.1
         fileFound = false;
         for (int j = 0; j < 128; j++) { // traverse through each directory in dirent
             if (strcmp(directories[j].name, path[k]) == 0) { // if path is found 
                fileFound = true;
                if (block_read(&tempNode, directories[j].inode, 1) != 0) { // place inode info into temp node
                    return -ENOENT; // in case read fails
                 }
                 if (S_ISDIR(tempNode.mode) != 0) { // if it's a directory update dirent
                     if(block_read(directories, tempNode.ptrs[0], 1) != 0) {
                       return -ENOENT;
                      } 
                      ret = directories[j].inode;
                 } else {
                     return directories[j].inode; // if it's just an inode return the blockid
                 }
                 break; // stops for loop once path is found
             }
         }
     }

     if (!fileFound) {
        return -ENOENT;
     }

   return ret; // will return the correct directory inode
}

int fs_getattr(const char *path, struct stat *sb, struct fuse_file_info *fi)
{
    /* TODO: your code here */
    char buf[PATH_MAX];
    char *pathv[20];
    int res;
    struct fs_inode inode;
//    struct fs_inode dirNode;
    int inodeNum;
    char **resPath;
    res = split(path, pathv, 20, buf, sizeof(buf));
    if (res < 0) {
        return -ENOENT;
    }
    if (res == 0) {
       block_read(&inode, 1, 1);
       sb->st_uid = inode.uid;
       sb->st_gid = inode.gid;
       sb->st_mode = inode.mode;
       sb->st_atime = inode.mtime;
       sb->st_ctime = inode.mtime;
       sb->st_mtime = inode.mtime;
       sb->st_size = inode.size;
       sb->st_nlink = 1;
       return 0;
    }

    resPath = split2(path, pathv, 20, buf, sizeof(buf));
    if (resPath[0] == NULL) {
        return -ENOENT;
    }

    inodeNum = get_inode(resPath, res);

    if (block_read(&inode, inodeNum, 1) != 0) {
        return -ENOENT;
    }

    sb->st_uid = inode.uid;
    sb->st_gid = inode.gid;
    sb->st_mode = inode.mode;
    sb->st_atime =inode.mtime;
    sb->st_ctime = inode.mtime;
    sb->st_mtime = inode.mtime;
    sb->st_size = inode.size;
    sb->st_nlink = 1;

    return 0;
}

/* Exercise 2:
 * readdir - get directory contents.
 *
 * call the 'filler' function once for each valid entry in the
 * directory, as follows:
 *     filler(buf, <name>, <statbuf>, 0, 0)
 * where
 *   - "ptr" is the second argument
 *   - <name> is the name of the file/dir (the name in the direntry)
 *   - <statbuf> is a pointer to struct stat, just like in getattr.
 *
 * success - return 0
 * errors - path resolution, ENOTDIR, ENOENT
 *
 * hints:
 *   - this process is similar to the fs_getattr:
 *     -- you will walk file system to find the dir pointed by "path",
 *     -- then you need to call "filler" for each of
 *        the *valid* entry in this dir
 *   - you can ignore "struct fuse_file_info *fi" (also apply to later Exercises)

 */
int fs_readdir(const char *path, void *ptr, fuse_fill_dir_t filler, off_t offset,
               struct fuse_file_info *fi, enum fuse_readdir_flags flags)
{
    /* TODO: your code here */

    char buf[PATH_MAX]; 
    char *pathv[20];
    int res;
    struct fs_inode inode;
    int inodeNum;
    char **resPath;
    struct fs_dirent directories[128];

    if (block_read(&inode, 1, 1) != 0) {
        return -ENOENT;
    }

    res = split(path, pathv, 20, buf, sizeof(buf));
    if (res < 0) {
        return -ENOENT;
    }

    if (res == 0) {
       if (block_read(directories, inode.ptrs[0], 1) != 0) {
           return -ENOENT;
       } 
       for (int k = 0; k < 128; k++) {
            if(directories[k].valid == 1) {
               filler(ptr, directories[k].name, NULL, 0, 0);
            }
       }
       return 0;
    }

    resPath = split2(path, pathv, 20, buf, sizeof(buf));
    if (resPath[0] == NULL) {
        return -ENOENT;
    }

    inodeNum = get_inode(resPath, res);

    if (block_read(&inode, inodeNum, 1) != 0) {
        return -ENOENT;
    }

    if (S_ISDIR(inode.mode) != 0) {
        if (block_read(directories, inode.ptrs[0], 1) != 0) {
            return -ENOENT; // in case read fails 
        }
    }

    for (int i = 0; i < 128; i++) {
        if (directories[i].valid == 1) {
            filler(ptr, directories[i].name, NULL, 0, 0); 
        }
    }

    return 0;
}

/* Exercise 3:
 * read - read data from an open file.
 * should return the number of bytes requested, except:
 *   - if offset >= file len, return 0
 *   - if offset+len > file len, return bytes from offset to EOF
 *   - on error, return <0
 * Errors - path resolution, ENOENT, EISDIR
 */
int fs_read(const char *path, char *buf, size_t len, off_t offset,
            struct fuse_file_info *fi)
{
    /* TODO: your code here */ 
    char *pathv[20];
    int res;
    struct fs_inode inode;
    int inodeNum;
    char **resPath;
    size_t bytesLeft;
    size_t totalBytesRead;
    size_t bytesLeftToRead;

    res = split(path, pathv, 20, buf, sizeof(buf));

    if (res < 0) {
       return -ENOENT;
    }

    resPath = split2(path, pathv, 20, buf, sizeof(buf));

    if (resPath[0] == NULL) {
       return -ENOENT;
    }

    inodeNum = get_inode(resPath, res);

    if (block_read(&inode, inodeNum, 1)) {
       return -ENOENT;
    }

    if (offset >= inode.size) {
       return 0;
    }

    bytesLeft = inode.size - offset;

    if (offset + len > inode.size) {
       if (block_read(buf, inodeNum + offset / FS_BLOCK_SIZE, (FS_BLOCK_SIZE - bytesLeft) / FS_BLOCK_SIZE) != 0) {
          return -ENOENT;
       }
       return bytesLeft;
    }

    if (len < bytesLeft) {
       bytesLeftToRead = len;
    } else {
        bytesLeftToRead = bytesLeft;
    }

    if (block_read(buf, inodeNum + offset / FS_BLOCK_SIZE, (FS_BLOCK_SIZE - bytesLeftToRead) / FS_BLOCK_SIZE) != 0) {
       return -ENOENT;
    }

    totalBytesRead = bytesLeftToRead; 


    return totalBytesRead;
}

/* operations vector. Please don't rename it, or else you'll break things
 */
struct fuse_operations fs_ops = {
    .init = fs_init,
    .getattr = fs_getattr,
    .readdir = fs_readdir,
    .read = fs_read,
};

