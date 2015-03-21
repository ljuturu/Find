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
		//DIR *dirPtr;
		if (argc != 2)
		{
			printf ("Usage: UID filename \n");
			exit(0);
		}
		//dirPtr =opendir(argv[1]);
		//if (dirPtr!=NULL)
		//{
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
	unsigned long ftype;
	//time_t tm;
	dirPtr =opendir(inp);
	//node = malloc (strlen(inp)+strlen("/"));
	while((d=readdir(dirPtr))!=NULL)
	{
		if ((strcasecmp(d->d_name,".") == 0) || (strcasecmp(d->d_name,"..") == 0))
			continue;

		node = malloc (strlen(inp)+strlen(d->d_name)+2);
		strcpy(node,inp);
		strcat(node,"/");
		strcat(node,d->d_name);
		//printf("test.. %s \n",node);
		//printf("Path %s \n",node);
		if((lstat(node, &ds)) != -1)
		{
			ftype = ds.st_mode & 0xF000;
			if (ftype == S_IFDIR)
			{
				printf ("DIR\t%s \n",d->d_name);
				visitDir(node);
			}
			else if (ftype == S_IFREG)
				printf ("REG\t%s \n",d->d_name);

		//		if((stat(node,&ds))!= -1)
			else
				printf ("OTHER\t%s \n",d->d_name);


		}
		else
		{
			perror("Error: ");
			continue;
		}
		free(node);
	}
	closedir(dirPtr);
}
