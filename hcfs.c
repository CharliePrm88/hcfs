#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

//In this array I will memorize directory list. Note: I CAN'T memorize a directory in the directory with this manner. Have to research how to replace this part of code.
char dir_list[ 256 ][ 256 ];
int curr_dir_idx = -1;
//file list. file limits is 256 files and 256 charachters for the file name. have to replace
char files_list[ 256 ][ 256 ];
int curr_file_idx = -1;
//content of the file. limits is 256 charachter for file
char files_content[ 256 ][ 256 ];
int curr_file_content_idx = -1;
//end of "have to replace"

void add_dir( const char *dir_name )
{
	curr_dir_idx++;
	strcpy( dir_list[ curr_dir_idx ], dir_name );
}

int is_dir( const char *path )
{
	path++; // Eliminating "/" in the path
	
	for ( int curr_idx = 0; curr_idx <= curr_dir_idx; curr_idx++ )
		if ( strcmp( path, dir_list[ curr_idx ] ) == 0 )
			return 1;
	
	return 0;
}

void add_file( const char *filename )
{
	curr_file_idx++;
	strcpy( files_list[ curr_file_idx ], filename );
	
	curr_file_content_idx++;
	strcpy( files_content[ curr_file_content_idx ], "" );
}

int is_file( const char *path )
{
	path++; // Eliminating "/" in the path
	
	for ( int curr_idx = 0; curr_idx <= curr_file_idx; curr_idx++ )
		if ( strcmp( path, files_list[ curr_idx ] ) == 0 )
			return 1;
	
	return 0;
}

int get_file_index( const char *path )
{
	path++; // Eliminating "/" in the path
	
	for ( int curr_idx = 0; curr_idx <= curr_file_idx; curr_idx++ )
		if ( strcmp( path, files_list[ curr_idx ] ) == 0 )
			return curr_idx;
	
	return -1;
}

void write_to_file( const char *path, const char *new_content )
{
	int file_idx = get_file_index( path );
	
	if ( file_idx == -1 ) // No such file
		return;
		
	strcpy( files_content[ file_idx ], new_content ); 
}

//Part before this have to be trimmed. some fuse operation have to be redefined


	/** Get file attributes.
	 *
	 * Similar to stat().  The 'st_dev' and 'st_blksize' fields are
	 * ignored.	 The 'st_ino' field is ignored except if the 'use_ino'
	 * mount option is given.
	 */
static int hcfs_getattr(const char *path, struct stat *st){ /*Vedere man 2 stat*/
	/*st->st_dev= getdev(); 
	st->st_ino= getino();
	st->st_uid=getuid();
	st->st_gid=getgid();
    st->st_rdev=getrdev();
	st->st_blksize=getblocksize();
	st->st_blocks=getblocks();
	st->st_ctime=time(NULL);*/
	st->st_uid=getuid();
	st->st_gid=getgid();
	st->st_atime=time(NULL);
	st->st_mtime=time(NULL);
	if(strcmp( path, "/")==0 || is_dir(path)==1)
	{
		st->st_mode=S_IFDIR |0755;
		st->st_nlink=2;
	}
	else if (is_file(path) == 1){
		st->st_mode = S_IFREG | 0644;
		st->st_nlink = 1;
		st->st_size = 1024;
	}
	else
	{
		return -ENOENT;
	}
	
	return 0;
}

	/** Read directory
	 *
	 * The filesystem may choose between two modes of operation:
	 *
	 * 1) The readdir implementation ignores the offset parameter, and
	 * passes zero to the filler function's offset.  The filler
	 * function will not return '1' (unless an error happens), so the
	 * whole directory is read in a single readdir operation.
	 *
	 * 2) The readdir implementation keeps track of the offsets of the
	 * directory entries.  It uses the offset parameter and always
	 * passes non-zero offset to the filler function.  When the buffer
	 * is full (or an error happens) the filler function will return
	 * '1'.
	 *
	 * Introduced in version 2.3
	 * The "flags" argument added in version 3.0
	 */

static int hcfs_readdir( const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi ){
	filler( buffer, ".", NULL, 0 ); // Current Directory
	filler( buffer, "..", NULL, 0 ); // Parent Directory
	
	if ( strcmp( path, "/" ) == 0 )
	{
		for ( int curr_idx = 0; curr_idx <= curr_dir_idx; curr_idx++ )
			filler( buffer, dir_list[ curr_idx ], NULL, 0 );
	
		for ( int curr_idx = 0; curr_idx <= curr_file_idx; curr_idx++ )
			filler( buffer, files_list[ curr_idx ], NULL, 0 );
	}
	return 0;
}

	/** Read data from an open file
	 *
	 * Read should return exactly the number of bytes requested except
	 * on EOF or error, otherwise the rest of the data will be
	 * substituted with zeroes.	 An exception to this is when the
	 * 'direct_io' mount option is specified, in which case the return
	 * value of the read system call will reflect the return value of
	 * this operation.
	 *
	 * Changed in version 2.2
	 */

static int hcfs_read( const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi )
{
	int file_idx = get_file_index( path );
	
	if ( file_idx == -1 )
		return -1;
	
	char *content = files_content[ file_idx ];
	
	memcpy( buffer, content + offset, size );
		
	return strlen( content ) - offset;
}


	/** Create a directory
	 *
	 * Note that the mode argument may not have the type specification
	 * bits set, i.e. S_ISDIR(mode) can be false.  To obtain the
	 * correct directory type bits use  mode|S_IFDIR
	 * */

static int hcfs_mkdir( const char *path, mode_t mode )
{
	path++;
	add_dir( path );
	
	return 0;
}
	/** Create a file node
	 *
	 * This is called for creation of all non-directory, non-symlink
	 * nodes.  If the filesystem defines a create() method, then for
	 * regular files that will be called instead.
	 */


static int hcfs_mknod( const char *path, mode_t mode, dev_t rdev )
{
	path++;
	add_file( path );
	
	return 0;
}


	/** Write data to an open file
	 *
	 * Write should return exactly the number of bytes requested
	 * except on error.	 An exception to this is when the 'direct_io'
	 * mount option is specified (see read operation).
	 *
	 * Changed in version 2.2
	 */
static int hcfs_write( const char *path, const char *buffer, size_t size, off_t offset, struct fuse_file_info *info )
{
	write_to_file( path, buffer );
	
	return size;
}

//https://github.com/libfuse/libfuse/blob/579c3b03f57856e369fd6db2226b77aba63b59ff/include/fuse.h#L102-L577

static struct fuse_operations operations = { /*uncomment when you write the corrispondent function*/
    .getattr		= hcfs_getattr,
	//.readlink 	= hcfs_readlink,
	.mknod			= hcfs_mknod,
    .mkdir		= hcfs_mkdir,
    //.unlink		= hcfs_unlink,
	//.rmdir 		= hcfs_rmdir,
	//.symlink 		= hcfs_symlink,
	//.rename 		= hcfs_rename,
	//.link 		= hcfs_link,
	//.chmode		= hcfs_chmode,
	//.chown		= hcfs_chown,
	//.truncate		= hcfs_truncate,
	//.open			= hcfs_open,
	.read			= hcfs_read,
	.write			= hcfs_write,
	//.statfs		= hcfs_statfs,
	//.flush		= hcfs_flush,
	//.release		= hcfs_release,
	//.fsync		= hcfs_fsync,
	//.setxattr		= hcfs_setxattr,
	//.getxattr		= hcfs_getxattr,
	//.listxattr	= hcfs_listxattr,
	//.removexattr	= hcfs_removexattr,
	//.opendir		= hcfs_opendir,
    .readdir		= hcfs_readdir,
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