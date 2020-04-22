#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

int hcfs_getattr(const char *path, struct stat *st){ /*Vedere man 2 stat*/
	st->st_dev= getdev(); 
	st->st_ino= getino();
	st->st_mode = getmode();
	st->st_nlink= getnlink();
	st->st_uid=getuid();
	st->st_gid=getgid();
    st->st_rdev=getrdev();
	st->st_size=getsize():
	st->st_blksize=getblocksize();
	st->st_blocks=getblocks();
	st->st_atime=time(NULL);
	st->st_mtime=time(NULL);
	st->st_ctime=time(NULL);
}


static struct fuse_operations operations = { /*Decommentare ogni volta che si scrive una delle funzioni*/
    .getattr		= hcfs_getattr,
	//.readlink 	= hcfs_readlink,
	//.mknod		= hcfs_mknod,
    //.mkdir		= hcfs_mkdir,
    //.unlink		= hcfs_unlink,
	//.rmdir 		= hcfs_rmdir,
	//.symlink 		= hcfs_symlink,
	//.rename 		= hcfs_rename,
	//.link 		= hcfs_link,
	//.chmode		= hcfs_chmode,
	//.chown		= hcfs_chown,
	//.truncate		= hcfs_truncate,
	//.open			= hcfs_open,
	//.read			= hcfs_read,
	//.write		= hcfs_write,
	//.statfs		= hcfs_statfs,
	//.flush		= hcfs_flush,
	//.release		= hcfs_release,
	//.fsync		= hcfs_fsync,
	//.setxattr		= hcfs_setxattr,
	//.getxattr		= hcfs_getxattr,
	//.listxattr	= hcfs_listxattr,
	//.removexattr	= hcfs_removexattr,
	//.opendir		= hcfs_opendir,
    //.readdir		= hcfs_readdir,
	//.releasedir 	= hcfs_releasedir,
	//.fsyncdir 	= hcfs_fsyncdir,
	//.init 		= hcfs_init,
	//.destroy 		= hcfs_destroy,
	//.access 		= hcfs_access,
	//.create 		= hcfs_create,
	//.ftruncate 	= hfcs_ftruncate,
	//.fgetattr 	= hfcs_fgetattr,
	//.lock 		= hcfs_lock,
	//.utimens		= hcfs_utimens,
	//.bmap 		= hcfs_bmap,
	//.ioctrl 		= hcfs_ioctrl,
	//.poll			= hcfs_poll,
	//.write_buf	= hcfs_write_buf,
	//.read_buf		= hcfs_read_buf,
	//.flock		= hcfs_flock,
	//.fallocate	= hcfs_fallocate
	};

int main( int argc, char *argv[] )
{
	return fuse_main( argc, argv, &operations, NULL );
}