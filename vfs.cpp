#include<stdio.h>

#include<fcntl.h>
#include<io.h>
#include<string.h>
#include<stdlib.h>

#define MAXINODE 50
#define READ 1
#define WRITE 2
#define MAXFILESIZE 1024
#define REGULAR 1
#define SPECIAL 2
#define START 0
#define CURRENT 1
#define END 2

typedef struct superblock
{
 int TotalInodes;
 int FreeInodes;
}SUPERBLOCK,*PSUPERBLOCK;

typedef struct inode
{
 char Filename[50];
 int InodeNumber;
 int FileSize;
 int FileActualSize;
 int FileType;
 int LinkCount;
 int ReferenceCount;
 char *Buffer;
 int Permission;
 struct inode *next;
}INODE,*PINODE,**PPINODE;

typedef struct filetable
{
 int ReadOffset;
 int WriteOffset;
 int Count;
 int Mode;
 PINODE ptrinode;
}FILETABLE,*PFILETABLE;

typedef struct ufdt
{
 PFILETABLE ptrfiletable;
}UFDT;


UFDT UFDTArr[50];
SUPERBLOCK SUPERBLOCKobj;
PINODE head=NULL;

void man(char *name)
 {
	 if(name==NULL)return ;

	 if(_stricmp(name,"create")==0)
	 {
		printf("Description:Used to create new regularfile\n");
		printf("Usage:create File_name Permission\n");
	 }
	 else if(_stricmp(name,"read")==0)
	 {
		printf("Description:Used to read data from file\n");
		printf("Usage:read File_name No_of_byts_to_read\n");
	 }
	 else if(_stricmp(name,"write")==0)
	 {
		printf("Description:Used to write into regular file\n");
		printf("Usage:write file_name\nAfter this enter data to be written\n");
	 }
	 else if(_stricmp(name,"ls")==0)
	 {
		printf("Description:Used to list all info about files\n");
		printf("Usage:ls\n");
	 }
	 else if(_stricmp(name,"stat")==0)
	 {
		printf("Description:Used to display info about file\n");
		printf("Usage:stat File_name\n");
	 }
	 else if(_stricmp(name,"fstat")==0)
	 {
		printf("Description:Used dispaly info about  file\n");
		printf("Usage:stat File_Descriptor\n");
	 }
	 else if(_stricmp(name,"truncate")==0)
	 {
		printf("Description:Used to remove data from file\n");
		printf("Usage:truncate file name\n");

	 }
	 else if(_stricmp(name,"open")==0)
	 {
		printf("Description:Used to open existing file\n");
		printf("Usage:open File_name mode\n");
	 }

	 else if(_stricmp(name,"close")==0)
	 {
		printf("Description:Used to close opened file\n");
		printf("Usage:close File_name\n");

	 }
	 else if(_stricmp(name,"closeall")==0)
	 {
		printf("Description:Used to close all opened file\n");
		printf("Usage:closeall\n");
	 }

	 else if(_stricmp(name,"lseek")==0)
	 {
		printf("Description:Used to change file offset\n");
		printf("Usage:lseek File_name ChangeInOffset StartPoint\n");

	 }
	 else if(_stricmp(name,"rm")==0)
	 {
		printf("Description:Used to remove file\n");
		printf("Usage:rm File_name\n");
	 }

	 else
	 {
		printf("ERROR:No manual entry available\n");
	 }
}

void DisplayHelp()
{
	printf("ls:to list out all files\n");
	printf("clear:to clera the console\n");
	printf("open:to open the files\n");
	printf("close:to close the file\n");
	printf("closeall:to close all opened files\n");
	printf("write:to write in the file\n");
	printf("exit:to terminate the sytem\n");
	printf("stat:to display info of using name\n");
	printf("fstat:to display info of file using file descriptor\n");
	printf("truncate:to remove all data from file\n");
	printf("rm:To delete the file\n");
}
int GetFdFromName(char * name)
{
	int i=0;
	while(i<50)
	{
		if(UFDTArr[i].ptrfiletable!=NULL)
		{
			if(strcmp((UFDTArr[i].ptrfiletable->ptrinode->Filename),name)==0)
							break;
		}
	    i++;
	}
	if(i==50)  
		return -1;
	else       
		return i;
}
PINODE Get_Inode(char*name)
{
 PINODE temp=head;
 if(name==NULL)
	   return NULL;
 
 while(temp!=NULL)
 {
	 if(_stricmp(name,temp->Filename)==0)
		   break;

	 temp=temp->next;
 }
 return temp;
}
 
void InitializeSuperBlock()
{
 int i=0;
 while(i<50)
 {
  UFDTArr[i].ptrfiletable = NULL;
  i++;
 
 }
 SUPERBLOCKobj.TotalInodes= MAXINODE;
 SUPERBLOCKobj.FreeInodes = MAXINODE;

}


void CreateDILB()
{
 PINODE newn=NULL;
 PINODE temp=head;
 int i=1;

 while(i<=MAXINODE)
 {
  newn = (PINODE)malloc(sizeof(INODE));
  newn->next=NULL;
  newn->LinkCount = newn->ReferenceCount = 0;
  newn->FileType = newn->FileSize = newn->FileActualSize = 0;
  newn->Buffer = NULL;
  newn->InodeNumber = i;

  if(temp == NULL)
  {
   head = newn;
   temp = head;
  }
  else
  {
   temp->next = newn;
   temp=temp->next;
  }
  i++;
 }
}

int CreateFile(char*name,int permission)
{
  int i=0;
  PINODE temp=head;

  if((name==NULL)||(permission==0)||(permission>3))
	  return -1;

  if(SUPERBLOCKobj.FreeInodes==0)
   return -2;
  
  if(Get_Inode(name)!=NULL)
  return -3;

  (SUPERBLOCKobj.FreeInodes)--;

  while(temp!=NULL)
  {
   if(temp->FileType==0)
      break;
   temp=temp->next;
  }

  while(i<50)
  {
   if(UFDTArr[i].ptrfiletable==NULL)
      break;
   i++;

  }

  UFDTArr[i].ptrfiletable=(PFILETABLE)malloc(sizeof(FILETABLE));
  if(UFDTArr[i].ptrfiletable==NULL)
	  return -4;

  UFDTArr[i].ptrfiletable->Count=1;
  UFDTArr[i].ptrfiletable->Mode =permission;
  UFDTArr[i].ptrfiletable->ReadOffset=0;
  UFDTArr[i].ptrfiletable->WriteOffset=0;
  UFDTArr[i].ptrfiletable->ptrinode=temp;

 strcpy_s(UFDTArr[i].ptrfiletable->ptrinode->Filename,name);

  UFDTArr[i].ptrfiletable->ptrinode->FileType=REGULAR;
  UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount=1;
  UFDTArr[i].ptrfiletable->ptrinode->LinkCount=0;
  UFDTArr[i].ptrfiletable->ptrinode->FileSize=MAXFILESIZE;
  UFDTArr[i].ptrfiletable->ptrinode->FileActualSize=0;
  UFDTArr[i].ptrfiletable->ptrinode->Permission=permission;
  UFDTArr[i].ptrfiletable->ptrinode->Buffer=(char*)malloc(sizeof(MAXFILESIZE));
  memset(UFDTArr[i].ptrfiletable->ptrinode->Buffer,0,1024);

  return i;

}

int rm_File(char * name)
{
    int fd=0;
	
	fd=GetFdFromName(name);
	if(fd==-1)  return -1;

	(UFDTArr[fd].ptrfiletable->ptrinode->LinkCount)--;
	
	if(UFDTArr[fd].ptrfiletable->ptrinode->LinkCount==0)
	{
		UFDTArr[fd].ptrfiletable->ptrinode->FileType = 0;
		free(UFDTArr[fd].ptrfiletable->ptrinode->Buffer);
		free(UFDTArr[fd].ptrfiletable);
	}
	
	UFDTArr[fd].ptrfiletable=NULL;
	(SUPERBLOCKobj.FreeInodes)++;
}

int ReadFile(int fd,char *arr,int isize)
{
	int read_size=0;

	if(UFDTArr[fd].ptrfiletable==NULL)
				return -1;

	if(UFDTArr[fd].ptrfiletable->Mode!=READ && UFDTArr[fd].ptrfiletable->Mode !=READ+WRITE )
		        return -2;

	if((UFDTArr[fd].ptrfiletable->ptrinode->Permission!=READ) &&
		(UFDTArr[fd].ptrfiletable->ptrinode->Permission!=READ+WRITE))
			    return -2;

	if(UFDTArr[fd].ptrfiletable->ReadOffset==UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)
		       return -3;

	if(UFDTArr[fd].ptrfiletable->ptrinode->FileType!=REGULAR)
			   return -4;

	read_size=(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)-(UFDTArr[fd].ptrfiletable->ReadOffset);
	if(read_size < isize)
	{
			strncpy(arr,
				(UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->ReadOffset),
				 read_size);

			UFDTArr[fd].ptrfiletable->ReadOffset=UFDTArr[fd].ptrfiletable->ReadOffset+read_size;

	}
	else
	{
			strncpy(arr,
				   (UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->ReadOffset),
				    isize);
	
			UFDTArr[fd].ptrfiletable->ReadOffset=UFDTArr[fd].ptrfiletable->ReadOffset+isize;
	}

	return isize;
}

int WriteFile(int fd,char*arr,int isize)
{
	if(((UFDTArr[fd].ptrfiletable->Mode)!=WRITE && 
		((UFDTArr[fd].ptrfiletable->Mode)!=READ+WRITE)))    return -1;

	if(((UFDTArr[fd].ptrfiletable->ptrinode->Permission)!=WRITE)&&
		((UFDTArr[fd].ptrfiletable->ptrinode->Permission)!=READ+WRITE))   return -1;

	if((UFDTArr[fd].ptrfiletable->WriteOffset)==MAXFILESIZE)
		return -2;

	if((UFDTArr[fd].ptrfiletable->ptrinode->FileType)!=REGULAR)
		return -3;

	strncpy((UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->WriteOffset),arr,isize);

	(UFDTArr[fd].ptrfiletable->WriteOffset)=(UFDTArr[fd].ptrfiletable->WriteOffset)+isize;

	(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)=(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+isize;

	return isize;

}

int OpenFile(char * name,int mode)
{
	int i=0;
	PINODE temp=NULL;

	if((name == NULL)||(mode<=0))
		 return -1;

	temp=Get_Inode(name);
	if(temp==NULL)
		 return -2;

	if(temp->Permission < mode)
		 return -3;

	while(i<50)
	{
		if(UFDTArr[i].ptrfiletable == NULL)
		    break;
		i++;
	}

	UFDTArr[i].ptrfiletable=(PFILETABLE)malloc(sizeof(FILETABLE));
	if(UFDTArr[i].ptrfiletable==NULL)
		 return -1;

	UFDTArr[i].ptrfiletable->Count=1;
	UFDTArr[i].ptrfiletable->Mode=mode;

	if(mode == READ + WRITE)
	{
		UFDTArr[i].ptrfiletable->ReadOffset=0;
		UFDTArr[i].ptrfiletable->WriteOffset=0;
	}
	else if(mode == READ)
	{
	 UFDTArr[i].ptrfiletable->ReadOffset=0;
	}
	else if(mode == WRITE)
	{
	 UFDTArr[i].ptrfiletable->WriteOffset=0;
	}
	UFDTArr[i].ptrfiletable->ptrinode=temp;
	(UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)++;

	return i;
	}

void CloseFileByName(int fd)
{
	UFDTArr[fd].ptrfiletable->ReadOffset=0;
	UFDTArr[fd].ptrfiletable->WriteOffset=0;
	(UFDTArr[fd].ptrfiletable->ptrinode->ReferenceCount)--;

}
int CloseFileByName(char *name)
{
	int i=0;

	i=GetFdFromName(name);
	if(i == -1)
	  return -1;

	UFDTArr[i].ptrfiletable->ReadOffset=0;
	UFDTArr[i].ptrfiletable->WriteOffset=0;
	(UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;

	return 0;
}

void CloseAllFile()
{
	int i=0;
	while(i<50)
	{
		if(UFDTArr[i].ptrfiletable!=NULL)
		{
			UFDTArr[i].ptrfiletable->ReadOffset=0;
			UFDTArr[i].ptrfiletable->WriteOffset=0;
			(UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;

		}
		i++;
	}
}
int LseekFile(int fd,int size,int from)
{

	if((fd<0)||(from>2))
		 return -1;

	if((UFDTArr[fd].ptrfiletable==NULL))
		 return -1;

	if((UFDTArr[fd].ptrfiletable->Mode==READ)||(UFDTArr[fd].ptrfiletable->Mode==READ+WRITE))
	{
		if(from==CURRENT)
		{
			if(((UFDTArr[fd].ptrfiletable->ReadOffset)+size)>
				(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
				return -1;
			if(((UFDTArr[fd].ptrfiletable->ReadOffset)+size)<0)
				return -1;

			(UFDTArr[fd].ptrfiletable->ReadOffset)=(UFDTArr[fd].ptrfiletable->ReadOffset)
				+size;
		}
		else if(from==START)
		{
			if(size>(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
				return -1;
			if(size<0)
				return -1;
			(UFDTArr[fd].ptrfiletable->ReadOffset)=size;
		}
		else if(from==END)
		{
			if((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size > MAXFILESIZE)
				return -1;

			if(((UFDTArr[fd].ptrfiletable->ReadOffset)+size)<0)
				return -1;
			(UFDTArr[fd].ptrfiletable->ReadOffset)=(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)
				+size;

		}

		}
	else if(UFDTArr[fd].ptrfiletable->Mode==WRITE)
	{
			if(from==CURRENT)
			{
				if(((UFDTArr[fd].ptrfiletable->WriteOffset)+size)>MAXFILESIZE)
					return -1;
				if(((UFDTArr[fd].ptrfiletable->WriteOffset)+size)<0)
					return -1;
				if(((UFDTArr[fd].ptrfiletable->WriteOffset)+size)>
					(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
					return -1;

				(UFDTArr[fd].ptrfiletable->WriteOffset)=(UFDTArr[fd].ptrfiletable->
					WriteOffset)+size;
			}
			else if(from==START)
			{
				if(size>MAXFILESIZE)
					return -1;
				if(size<0)
				    return -1;
				if(size>(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
					(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)=size;

				(UFDTArr[fd].ptrfiletable->WriteOffset)=size;
			}
			else if(from==END)
			{
				if((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size>
					MAXFILESIZE)
					return -1;
				if(((UFDTArr[fd].ptrfiletable->WriteOffset)+size)<0)
					return -1;
				(UFDTArr[fd].ptrfiletable->WriteOffset)=(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size;
			}
	      }
		}
			
void ls_file()
{
		PINODE temp=head;

		if(SUPERBLOCKobj.FreeInodes==MAXINODE)
		{
			printf("ERROR:There are no files\n");
			return ;
		}
		printf("\nFile Name\tInode NUmber\tFile Size\tLink Count\n");
		printf("-----------------------------------------------------\n");

		while(temp!=NULL)
		{
			if(temp->FileType!=0)
			{
				printf("%s\t\t%d\t\t%d\t\t%d\n");
				temp->Filename,
					temp->InodeNumber,
					    temp->FileSize,
						   temp->LinkCount;
			}
			temp=temp->next;
		}
		printf("-----------------------------------------------------\n");
		
}

int fstat_file(int fd)
{
	PINODE temp=head;

	if(fd<0)
		return -1;
	if(UFDTArr[fd].ptrfiletable==NULL)
		return -2;

	temp=UFDTArr[fd].ptrfiletable->ptrinode;

	printf("-----------Statistical infomation about file--------------\n");
	printf("File Name:%s\n",temp->Filename);
	printf("Inode Number:%d\n",temp->InodeNumber);
	printf("File Size:%d\n",temp->FileSize);
	printf("File Actual size:%d\n",temp->FileActualSize);
	printf("Link Count:%d\n",temp->LinkCount);
	printf("Reference count:%d\n",temp->ReferenceCount);

	if(temp->Permission==1)
			printf("File Permission:Read Only\n");
	else if(temp->Permission==2)
		    printf("File Permission:Write Only\n");
	else if(temp->Permission==3)
			printf("File PErmission:Read & Write\n");

	printf("-----------------------------------------------------\n\n");
	return 0;
}

int stat_file(char * name)
{
	PINODE temp=head;

	if(name==NULL)
	  return -1;

	while(temp!=NULL)
	{
		if(strcmp(name,temp->Filename)==0)
			break;
		temp=temp->next;

	}

	if(temp==NULL)
		return -2;

	printf("\n-----------Statistical info about file------------\n");
	printf("File Name:%s\n",temp->Filename);
	printf("File Actual size:%s\n",temp->FileActualSize);
	printf("Inode Number:%d\n",temp->InodeNumber);
	printf("Link Count:%d\n",temp->LinkCount);
	printf("Reference count:%d\n",temp->ReferenceCount);

	if(temp->Permission==1)
		printf("file Permission:Read only\n");
    else if(temp->Permission==2)
		    printf("File Permission:Write Only\n");
	else if(temp->Permission==3)
			printf("File PErmission:Read & Write\n");

	printf("-----------------------------------------------------\n\n");
	return 0;

}

int truncate_File(char *name)
{
		int fd=GetFdFromName(name);
		if(fd==-1)
			return -1;

		memset(UFDTArr[fd].ptrfiletable->ptrinode->Buffer,0,1024);
		UFDTArr[fd].ptrfiletable->ReadOffset=0;
		UFDTArr[fd].ptrfiletable->WriteOffset=0;
		UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize=0;
}

int main()
{
	char *ptr =NULL;
	int ret=0,fd=0,count=0;
	char command[4][80],str[80],arr[1024];

	InitializeSuperBlock();
	CreateDILB();

	while(1)
	{
      	 flushall();         //clearng buffer;
		 strcpy(str,"");

		 printf("\n Vipul VFS : >");
		 fgets(str,80,stdin);

		 count=sscanf(str,"%s%s%s%s",command[0],command[1],command[2],command[3]);

		 if(count==1)
		 {
			 if(stricmp(command[0],"ls")==0)
			 {
					ls_file(); 
			 }
			 else if(stricmp(command[0],"closeall")==0)
			 {
					CloseAllFile();
					printf("all files are closed successfully\n");
						continue;
			 }
		 
			 else if(stricmp(command[0],"clear")==0)
			 {
					system("cls");
					continue;
			 }

			 else if(stricmp(command[0],"help")==0)
			 {
					DisplayHelp();
					continue;
			 }

			 else if(stricmp(command[0],"exit")==0)
			 {
			 printf("Terminating the project\n");
			 break;
			 }
			 else
			 {
			  printf("\n ERROR:Command not found!!!\n");
			  continue;
			 }
		 }

		 else if(count==2)
		 {
		  
			 if(stricmp(command[0],"stat")==0)
			 {
					ret=stat_file(command[1]);
					if(ret==-1)
						printf("ERROR:Incorrect parameters\n");

					if(ret==-2)
					    printf("ERROR:there is no such file\n");
					continue;
			 }
			 else if(stricmp(command[0],"fstat")==0)
			 {
					ret=fstat_file(atoi(command[1]));
					if(ret==-1)
						 printf("ERROR:Incorrect parameters\n");
					if(ret==-2)
					    printf("ERROR:there is no such file\n");
			         continue;
			 }

			 else if(stricmp(command[0],"close")==0)
			 {
					ret=CloseFileByName(command[1]);
					if(ret==-1)
						printf("ERROR:there is no such file\n");
					continue;
			 }

			 else if(stricmp(command[0],"rm")==0)
			 {
					ret=rm_File(command[1]);
					if(ret==-1)
						printf("ERROR:there is no such file\n");
					continue;
			 
			 }

			 else if(stricmp(command[0],"man")==0)
			 {
					man(command[1]);
			 }

			 else if(stricmp(command[0],"write")==0)
			 {
					fd=GetFdFromName(command[1]);
					if(fd==-1)
					{
							 printf("ERROR:Incorrect parameters\n");
					         continue;
					}
					printf("enter the data:\n");
					scanf("%[^\n]",arr);

					ret=strlen(arr);
					if(ret==0)
					{
						printf("ERROR:Incorrect parameters\n");
						continue;
					}
					ret=WriteFile(fd,arr,ret);
						if(ret==-1)
						   printf("ERROR:Permission denied\n");
						if(ret==-2)
						   printf("ERROR:no sufficient memory\n");
					    if(ret==-3)
						   printf("ERROR:not regular file\n");
			 }
			 else if(_stricmp(command[0],"truncate")==0)
			 {
			 
					ret=truncate_File(command[1]);
					if(ret==-1)
						  printf("ERROR:Incorrect parameters\n");
						
			 }
			 else
			 {
			 printf("ERROR:command not found\n");
			 continue;
			 }

			
		 }

		 else if(count==3)
		 {
		   if(stricmp(command[0],"create")==0)
		   {
			   ret=CreateFile(command[1],atoi(command[2]));
			   if(ret>0)
			   printf("File is created successfully with file descriptor :%d\n",ret);

			   if(ret==-1)
			   printf("ERROR:Incorrect Parameters\n");

			   
			   if(ret==-2)
			   printf("ERROR:There is no free inode\n");

			   
			   if(ret==-3)
			   printf("ERROR:File alresdy exists\n");

			   
			   if(ret==-4)
			   printf("ERROR:memory allocation failure\n");

			   continue;
		   }

		   else if(stricmp(command[0],"open")==0)
		   {
			   ret=OpenFile(command[1],atoi(command[2]));

			   if(ret>=0)
				   printf("file is created successfully with file descriptor:%d\n",ret);
			   if(ret==-1)
				   printf("ERROR:Incorrect parameters\n");
			   if(ret==-2)
				   printf("ERROR:File is not present\n");
			   if(ret==-3)
				   printf("ERROR:permssion denied\n");
			   continue;
		   }

		   else if(stricmp(command[0],"read")==0)
		   {
					fd=GetFdFromName(command[1]);
					if(fd==-1)
					{
						printf("ERROR:Incorrect parameters\n");
						continue;
					}
					ptr=(char*)malloc(sizeof(atoi(command[2]))+1);
					if(ptr==NULL)
					{
					 printf("Memory allocation failure\n");
					}
					ret=ReadFile(fd,ptr,atoi(command[2]));

					if(ret==-1)
						printf("ERROR:File not existing\n");

					if(ret==-2)
						printf("permission denied\n");
					if(ret==-3)
						printf("ERROR:Reached at end of file\n");
					if(ret==-4)
						printf("ERROR:It is not regular file\n");
			        if(ret==0)
						printf("ERROR:File empty\n");
					if(ret>0)
					{
						_write(2,ptr,ret);
					}
					continue;
		   }
		   
		   else
		   {
		     printf("\n ERROR:Command not found!!!\n");
			  continue;
		   }
		 }

		 else if(count==4)
		 {
		   if(stricmp(command[0],"lseek")==0)
		   {
				fd =GetFdFromName(command[1]);
				if(fd==-1)
				{
					printf("ERROR:Incorrect parameters\n");
					continue;
				}
				ret=LseekFile(fd,atoi(command[2]),atoi(command[3]));

				if(ret==-1)
				{
					printf("ERROR:Unable to perform seek\n");
				}
		   }

		   else
		   {
		     printf("\n ERROR:Command not found!!!\n");
			  continue;
		   }
		 }
		 else 
		 {
		      printf("\n ERROR:Command not found!!!\n");
			  continue;
		 }
	}
	return 0;
}