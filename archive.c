#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

typedef struct
{
    char name[100];
    char mode[8];
    char owner[8];
    char group[8];
    char size[12];
    char modified[12];
    char checksum[8];
    char type[1];
    char link[100];
    char padding[255];
}header_T;

header_T header; //here we will store the header info for each file
struct stat my_stat;
const unsigned char *bytes=(const unsigned char *)&header;
void get_header(const char *name)
{
    //struct stat my_stat;
    stat(name, &my_stat);
    memset( &header, 0, sizeof(header_T) );
    snprintf( header.name, 100, "%s", name  );
    snprintf( header.mode, 8, "%07o", my_stat.st_mode );
    snprintf( header.owner, 8, "%07o", my_stat.st_uid);
    snprintf( header.group, 8, "%07o", my_stat.st_gid );
    snprintf( header.size, 12, "%011lo", my_stat.st_size);
    snprintf( header.modified, 12, "%011lo", my_stat.st_mtime );
    memset( header.checksum, ' ', 8);
    header.type[0] = '0';

    //computing the checksum
    size_t sum=0;
    //const unsigned char *bytes=(const unsigned char *)&header;
    for(int i=0; i<sizeof(header_T); ++i ){
        sum += bytes[i];
    }
    snprintf( header.checksum, 8, "%07lo", sum );

}

void arr(FILE *tar, FILE *to_add, const char *name)
{
    get_header(name); //at this point the header info is stored in header so we can write it
    fwrite(bytes, 1, sizeof(header_T), tar); //writing the header
    while (!feof(to_add))
    {
        char content[512];
        size_t content_bytes=fread(content, 1, 512, to_add);
        fwrite(content, 1, content_bytes, tar);
    }
    if(my_stat.st_size % 512 != 0) //padding with 0s to get blocks of 512 bytes
    {
        size_t length=512-my_stat.st_size;
        char zeros[length];
	    memset(zeros, 0, length);
	    fwrite(zeros, 1, length, tar);
    }
}

int main(int argc, char *argv[])
{
    if(argc==1)
    {
        printf("usage: ./archive.exe \"archive.tar\" \"file1\" \"file2\" ... \n");
        exit(1);
    }

    FILE *archive = fopen(argv[1], "wb");//argv[1] should be the .tar file

	for(int i = 2; i < argc; i++)
	{
		FILE *current_file = fopen(argv[i], "rb");

		if(!current_file)
		{
			printf("Unable to open file!\n");
			exit(1);
		}

		arr(archive, current_file, argv[i]);

		fclose(current_file);
	}


    char zeros[512];
	memset(zeros, 0, 512);
	fwrite(zeros, 1, 512, archive);//fill with two 512 bytes to signal the end of the archive

	fclose(archive);
    return 0;
}
