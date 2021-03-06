#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/param.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>

void visitDir(char inp[]);
int procArg(int c,char *a[]);
char *selTable[4]={"-name","-mtime","-user",NULL};
char *sel=NULL;
char *arg=NULL;
int stIdx;

int main(int argc, char *argv[])
{
		struct stat buf;
		unsigned long ftype;
		//DIR *dirPtr;
		if (argc < 2 || argc >4)
		{
			printf ("Incorrect number of arguments !!! \n");
			exit(0);
		}
		//dirPtr =opendir(argv[1]);

		sel = malloc(strlen(argv[1]) + strlen("/") + 30 );
		arg = malloc(strlen(argv[1]) + strlen("/") + 30 );
//		if (dirPtr!=NULL)
//		{
			stat(argv[1],&buf);
			ftype=buf.st_mode & 0xF000;
			//printf("ftype=%x\n", ftype);
			if (ftype == 0x4000)
			{
				procArg(argc,argv);
				visitDir(argv[1]);
			}
			else
			{
				perror(argv[1]);
				exit(1);
			}

//		}
//		else
//		{
//			perror("Error: ");
//		}
		free(sel);
		free(arg);
		return 0;
}

void visitDir(char inp[])
{
	char *node=NULL;
	struct dirent *d;
	struct stat ds;
	struct passwd *pd;
	DIR *dirPtr;
	long inpTime,sTime,x,currTime;
	int y;
	unsigned long ftype;
	if((dirPtr =opendir(inp))!=NULL)
	{
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
			switch(stIdx)
			{
				case 0:
						if (ftype == S_IFDIR)
						{
							if((strcasecmp(d->d_name,arg)) == 0)
							{
								//printf ("DIR\t %s\n",d->d_name);
								printf ("DIR\t%s\n",node);
							}
							visitDir(node);
							free(node);
						}
						else if (ftype == S_IFREG)
						{
							if((strcasecmp(d->d_name,arg)) == 0)
							{
								//printf ("REG\t %s\n",d->d_name);
								printf ("REG\t%s\n",node);
								continue;
							}
						}
					//		if((stat(node,&ds))!= -1)
						else
						{
							if((strcasecmp(d->d_name,arg)) == 0)
							{
								//printf ("OTHER\t%s \n",d->d_name);
								printf ("OTHER\t%s\n",node);
								continue;
							}
						}
						break;
				case 1:
						y=atoi(arg);
						inpTime=((y)*86400);
						//printf("arg pointer value..%d\n",y);
						//printf("input time in secs...%ld\n",inpTime);
						time(&sTime);
						//printf("system time in secs...%ld\n",sTime);
						//x=sTime % 86400;
						//y=(sTime-inpTime)-x;
						if(ftype==0x4000)
						{
							visitDir(node);
							free(node);
						}
						else if(ftype==0x8000)
						{
							//time(&currTime);
							//if(ds.st_mtime > y && currTime)
							//printf("modi time in secs...%ld\n",ds.st_mtime);
							x=sTime-ds.st_mtime;
							//printf("subt time in secs...%ld\n",x);
							if(x <= inpTime)
							{
								printf("REG\t%s\t%s\n",node,ctime(&ds.st_mtime));
								continue;
							}
							//else printf("No file modified\n");
						}
						else
						{
						  printf("");
						}
						break;
				case 2:
						pd=getpwnam(arg);
						if(!pd)
						{
							printf("Invalid userId\n");
							exit(0);
						}
						if(ftype==0x4000)
						{
							if(pd->pw_uid==ds.st_uid)
							{
								//printf("%s\t%s\t\n",pd->pw_name,d->d_name);
								printf("DIR\t%s\n",node);
							}
							visitDir(node);
							free(node);
						}
						else if(ftype==0x8000)
						{
							if(pd->pw_uid==ds.st_uid)
							{
								printf("REG\t%s\n",node);
								continue;
							}
						}
						else
						{
							if(pd->pw_uid==ds.st_uid)
							{
								printf("OTHER\t%s\n",node);
								continue;
							}
						}
				        break;
				case -1:
						if(ftype==0x4000)
					    {
							printf("DIR\t%s\n",d->d_name);
					        visitDir(node);
					        free(node);
					    }
					    else if(ftype==0x8000)
					    	printf("REG\t%s\n",d->d_name);
					    else
					    	printf("OTHER\t%s\n",d->d_name);
						break;
			}

		}
		else
		{
			perror("Error ");
			continue;
		}

	}
		closedir(dirPtr);
	}
	else
	{
		perror(inp);
	}
	free(node);
}

int procArg(int z,char *argv[])
{
	int i;
	if(z>2)
	{
		for(i=0;i<3;i++)
		{
			if(strcasecmp(argv[2],selTable[i])==0)
			{
				printf("source=%s selection=%s arg=%s\n",argv[1],argv[2],argv[3]);
				strcpy(sel,argv[2]);
				strcpy(arg,argv[3]);
				stIdx=i;

			}
		}
	}
	else if(z==2)
	{
		stIdx=-1;
	}
	//printf("arg value..%s\n",argv[3]);
	return 0;
}
