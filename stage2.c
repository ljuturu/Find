#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/param.h>
#include <dirent.h>
#include <time.h>

void visitDir(char inp[]);

int main(int argc, char *argv[])
{
		struct stat buf;
		unsigned long ftype;
//		DIR *dirPtr;
		if (argc != 2)
		{
			printf ("usage: UID filename \n");
			exit(0);
		}
//		dirPtr =opendir(argv[1]);
//		if (dirPtr!=NULL)
//		{
			stat(argv[1],&buf);
			ftype=buf.st_mode & 0xF000;
			//printf("ftype=%x\n", ftype);
			if (ftype == 0x4000)
				visitDir(argv[1]);

		//}
		return 0;
}

void visitDir(char inp[])
{
	char *node;
	struct dirent *d;
	struct stat ds;
	DIR *dirPtr;
	//time_t tm;
	dirPtr =opendir(inp);
	strcat(inp,"/");
	node = malloc (strlen(inp)+strlen("/"));
	while((d=readdir(dirPtr))!=NULL)
	{
		strcpy(node,inp);
		strcat(node,d->d_name);
		//printf("Directory Entry %s \n",d->d_name);
		//printf("Path %s \n",node);
		if((stat(node,&ds))!= -1)
			printf("Directory Entry %s\tLast modified %ld \n",d->d_name,ds.st_mtime);

		else
		{
			perror("Error: ");
			continue;
		}
	}
	free(node);
	closedir(dirPtr);
}
