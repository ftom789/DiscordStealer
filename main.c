#include <stdio.h>
#include <Windows.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>



int main()
{
    char **files=malloc(0);
    char str[]="asdasdadads";
    int len=strlen(str);
    
	char *ptr=malloc(len);
    strcpy(ptr,str);
    
    int countOfFiles=1;
    files=realloc(files,1*sizeof(ptr));
    
    files[0]=ptr;

    files=realloc(files,2*sizeof(ptr));
    char str2[]="123123";
    int len2=strlen(str2);
    char *ptr2=malloc(len2);
    
    strcpy(ptr2,str2);
    files[1]=ptr2;
    printf("%s\n",(files[0]));
    printf("%s\n",(files[1]));
	return (0);
}