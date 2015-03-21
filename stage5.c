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
void procEntry(char *node);
int procArg(int c, char *a[]);
char selTable[4][12] = { "-name", "-mtime", "-user", NULL };
char **cmd_lst;
//cmd_lst=(char**)malloc(sizeof(char*)*N);
char sel[12];
char arg[800];
int stIdx = 0, N, p;
FILE *shFile;
int main(int argc, char *argv[]) {
	struct stat buf;
	unsigned long ftype;
	DIR *dirPtr;
	if (argc < 2) {
		printf("No arguments !!! \n");
		exit(1);
	}
	dirPtr = opendir(argv[1]);

//		sel = malloc(strlen(argv[1]) + strlen("/") + 30 );
//		arg = malloc(strlen(argv[1]) + strlen("/") + 30 );
//		if (dirPtr!=NULL)
//		{
	procArg(argc, argv);
	stat(argv[1], &buf);
	ftype = buf.st_mode & 0xF000;
	//printf("ftype=%x\n", ftype);
	if (ftype == 0x4000) {
		shFile = fopen("shFile.sh", "w+");
		visitDir(argv[1]);
		fclose(shFile);
		system("sh -f shFile.sh");
	}
	//visitDir(argv[1]);
	else
		printf("Directory does not exist\n");

//		}
//		else
//		{
//			perror("Error: ");
//		}
	free(sel);
	free(arg);
	return 0;
}

void visitDir(char inp[800]) {
	char *node = NULL;
	struct dirent *d;
	struct stat ds;
	struct passwd *pd;
	DIR *dirPtr;
	long inpTime, sTime, x, currTime;
	int i, y;
	unsigned long ftype;
	if ((dirPtr = opendir(inp)) != NULL) {
		//node = malloc (strlen(inp)+strlen("/"));
		while ((d = readdir(dirPtr)) != NULL) {
			if ((strcasecmp(d->d_name, ".") == 0)
					|| (strcasecmp(d->d_name, "..") == 0))
				continue;

			node = malloc(strlen(inp) + strlen(d->d_name) + 2);
			node[0] = 0;
			strcpy(node, inp);
			strcat(node, "/");
			strcat(node, d->d_name);
			//printf("test.. %s \n",node);
			//printf("Path %s \n",node);
			if ((lstat(node, &ds)) != -1) {
				ftype = ds.st_mode & 0xF000;
				switch (stIdx) {
				case 0:
					if (ftype == S_IFDIR) {
						if ((strcasecmp(d->d_name, arg)) == 0) {
							//printf ("DIR\t %s\n",d->d_name);
							printf("DIR\t%s\n", node);
							procEntry(node);
						}
						visitDir(node);
						free(node);
					} else if (ftype == S_IFREG) {
						if ((strcasecmp(d->d_name, arg)) == 0) {
							//printf ("REG\t %s\n",d->d_name);
							printf("REG\t%s\n", node);
							procEntry(node);
							continue;
						}
					}
					//		if((stat(node,&ds))!= -1)
					else {
						if ((strcasecmp(d->d_name, arg)) == 0) {
							//printf ("OTHER\t%s \n",d->d_name);
							printf("OTHER\t%s\n", node);
							procEntry(node);
							continue;
						}
					}
					break;
				case 1:
					y = atoi(arg);
					inpTime = ((y) * 86400);
					//printf("arg pointer value..%d\n",y);
					//printf("input time in secs...%ld\n",inpTime);
					time(&sTime);
					//printf("system time in secs...%ld\n",sTime);

					x = sTime - ds.st_mtime;
					if (ftype == 0x4000) {
						if (x <= inpTime) {
							printf("REG\t%s\t%s\n", node, ctime(&ds.st_mtime));
							procEntry(node);
						}
						visitDir(node);
						free(node);
					} else if (ftype == 0x8000) {
						//printf("modi time in secs...%ld\n",ds.st_mtime);
						//printf("subt time in secs...%ld\n",x);
						if (x <= inpTime) {
							printf("REG\t%s\t%s\n", node, ctime(&ds.st_mtime));
							procEntry(node);
							continue;
						}
						//else printf("No file modified\n");
					} else {
						if (x <= inpTime) {
							printf("OTHER\t%s\n", node);
							procEntry(node);
							continue;
						}
					}
					break;
				case 2:
					pd = getpwnam(arg);
					if (ftype == 0x4000) {
						if (pd->pw_uid == ds.st_uid) {
							//printf("%s\t%s\t\n",pd->pw_name,d->d_name);
							printf("DIR\t%s\n", node);
							procEntry(node);
						}
						visitDir(node);
						free(node);
					} else if (ftype == 0x8000) {
						if (pd->pw_uid == ds.st_uid) {
							printf("REG\t%s\n", node);
							procEntry(node);
							continue;
						}
					} else {
						if (pd->pw_uid == ds.st_uid) {
							printf("OTHER\t%s\n", node);
							procEntry(node);
							continue;
						}
					}
					break;
				case -1:
					if (ftype == 0x4000) {
						printf("DIR\t%s\n", d->d_name);
						visitDir(node);
						free(node);
					} else if (ftype == 0x8000)
						printf("REG\t%s\n", d->d_name);
					else
						printf("OTHER\t%s\n", d->d_name);
					break;
				}

			} else {
				perror("Error ");
				continue;
			}

		}
		closedir(dirPtr);
	} else {
		perror("Error ");
	}
	free(node);
}

int procArg(int c, char *argv[]) {
	int i, j, k;
	if (c > 4) {
		for (i = 0; i < 4; i++) {
			if (strcasecmp(argv[2], selTable[i]) == 0) {
				printf("source=%s selection=%s arg=%s\n", argv[1], argv[2],
						argv[3]);
				strcpy(sel, argv[2]);
				strcpy(arg, argv[3]);
				stIdx = i;

			}
		}
		//printf("arg value..%s\n",argv[3]);
		N = c - 3;
		cmd_lst = (char **) malloc(sizeof(char *) * N);
		for (i = 0; i < N; i++)
			cmd_lst[i] = (char *) malloc(sizeof(char) * 80);
		strcpy(cmd_lst[0], argv[4] + 1);
		i = 1;
		j = 5;
		while (c > j && strncmp(argv[j], "-", 1) == 0) {
			strcpy(cmd_lst[i], argv[j]);
			j++;
			i++;
		}
		p = i;
		for (k = i + 1; j < c; k++) {
			strcpy(cmd_lst[k], argv[j]);
			j++;
		}
	} else if (c == 2) {
		stIdx = -1;
	} else if (c == 4) {
		for (i = 0; i < 4; i++) {
			if (strcasecmp(argv[2], selTable[i]) == 0) {
				printf("source=%s selection=%s arg=%s\n", argv[1], argv[2],
						argv[3]);
				strcpy(sel, argv[2]);
				strcpy(arg, argv[3]);
				stIdx = i;

			}
		}
		cmd_lst = NULL;

	}
	return 0;
}

void procEntry(char *node) {
	int i;
	if(cmd_lst!=NULL)
	{
	strcpy(cmd_lst[p], node);
	for (i = 0; i < N; i++)
		fprintf(shFile, "%s ", cmd_lst[i]);
	fprintf(shFile, "\n");
	}

}
