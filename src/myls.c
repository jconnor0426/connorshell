#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>


int list_dir( char* directory_name )
{
	DIR * dirp;
	struct dirent * dp;
	struct stat sb;

	dirp = opendir( directory_name );
	if (dirp == NULL)
		return (-1);

	if( chdir( directory_name ) == -1) 
	{
		perror( "chdir failed");
		return( -1);
	}

	printf( "Directory %s\n\n", directory_name );

	while ((dp = readdir(dirp)) != NULL) 
	{
		if( stat( dp->d_name, &sb ) == -1 )
		{
			perror( "stat failed");
			break;
		}

		printf( (S_ISDIR(sb.st_mode)) ? "d" : "-");
		printf( (sb.st_mode & S_IRUSR) ? "r" : "-");
		printf( (sb.st_mode & S_IWUSR) ? "w" : "-");
		printf( (sb.st_mode & S_IXUSR) ? "x" : "-");
		printf( (sb.st_mode & S_IRGRP) ? "r" : "-");
		printf( (sb.st_mode & S_IWGRP) ? "w" : "-");
		printf( (sb.st_mode & S_IXGRP) ? "x" : "-");
		printf( (sb.st_mode & S_IROTH) ? "r" : "-");
		printf( (sb.st_mode & S_IWOTH) ? "w" : "-");
		printf( (sb.st_mode & S_IXOTH) ? "x" : "-");
		printf( "\t%ld", (long) sb.st_nlink);
		printf( "\t%ld\t%ld ", (long) sb.st_uid, (long) sb.st_gid);
		printf( "\t%lld",(long long) sb.st_size);
		printf( "\t%s", dp->d_name );
		printf( "\t%s", ctime(&sb.st_mtime) );
	}

	printf( "\n\n");

	(void)closedir(dirp);
	
	return 0;
}

int main( int argc, char ** argv)
{
	int i; 

	for( i = 1; i < argc; i++ )
		list_dir( argv[i] );
}
