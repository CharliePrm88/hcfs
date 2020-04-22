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
	
    
}

int main( int argc, char *argv[] )
{
	return fuse_main( argc, argv, &operations, NULL );
}