#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
typedef struct
{
    char name[100];
    char size[12];
}tar_T;

void extract(FILE *archive)
{
    tar_T head;
    size_t offset=0;


    while (1) {

    fseek(archive, offset, SEEK_SET);//place pointer at offset 0 to read the name
    fread(head.name, 100, 1, archive);
    printf("name is %s\n", head.name);

    fseek(archive, offset+124, SEEK_SET); //place pointer at offset 124 using fseek to read the size
    fread(head.size, 12, 1, archive);
    printf("size is %s\n", head.size);


    FILE *ptr=fopen(head.name, "wb"); //create a new file with the given name in which to write the read data
    size_t size=strtoul(head.size, NULL, 8);

    size_t blocks_nr=(floor(size/512)+1)+1;
    char block[512];


    fseek(archive, offset+512, SEEK_SET);//set the pointer to start of content(end of header)
    offset+=512*blocks_nr;

    while (blocks_nr--)
    {
        fread(block, 512, 1, archive);

        if(blocks_nr==0)//we reached the last block
        {
            char flag[512];
            memset(flag, 0, 512);
            if(strcmp(flag, block)==0)//we reached the end of the archive
            {
                fclose(ptr);
                return;
            }

        }
        else
        {
            fwrite(block, size, 1, ptr);
            memset(block, 0, 512); //set the written bytes to 0
        }
    }
    fclose(ptr);
}


}

int main(int argc, char *argv[])
{
    if(argc==1)
    {
        printf("usage: ./extract.exe \"archive.tar\" \n" );
        exit(1);
    }

    FILE *tar=fopen(argv[1], "rb"); //the first argument and only argument must be the tar archive
    extract(tar);
    fclose(tar);
    return 0;
}
