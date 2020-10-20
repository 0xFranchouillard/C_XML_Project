#include <stdio.h>
#include <stdlib.h>



int main() {
    char* line = malloc(sizeof(char) * 200);
    FILE* fo = fopen("../file.xml","r+");

    if(fo == NULL) return -1;

    while((fgets(line,50,fo)) != NULL){
        printf("%s",line);
    }

    fclose(fo);





    return 0;
}
