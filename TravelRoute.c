#include<stdio.h>
#include<string.h>

int main(){
    int n;
    printf("Enter a number of tickets: ");
    scanf("%d", &n);

    char source[n][30];
    char destination[n][30];

    printf("Enter the source and destination for each ticket:\n");
    for(int i = 0; i < n; i++){
        scanf("%29s %29s", source[i], destination[i]);
    }

    int start=-1;
    for(int i = 0; i < n; i++){
        int found = 0;
        for(int j = 0; j < n; j++){
            if(strcmp(source[i], destination[j]) == 0){
                found = 1;
                break;
            }
        }
        if(found==0){
            start = i;
            break;
        }
    }

    printf("The list is :\n");
    
    while(start!=-1){
        printf("%s -> %s\n", source[start], destination[start]);
        char next[30];
        strcpy(next, destination[start]);
        start=-1;
        for(int i=0;i<n;i++){
            if(strcmp(next,source[i])==0){
                start=i;
                break;
            }
        }
    }

    return 0;
}