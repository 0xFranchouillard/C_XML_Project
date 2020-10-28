#include <stdio.h>
#include <stdlib.h>

struct Prop{
    char* name;
};
typedef struct Prop Prop;

void property(const char* line, Prop* p){
    char c;
    int i = 0, j = 0, start = 0, zap = 0;

    while(line[i] != '>'){
        if(!start){
            while (line[i++] != '<' && line[i] != '\0'){}
            start++;
        }
        while (line[i++] != ' ' && line[i] != '\0') {}
        if(line[i-1] == ' '){
            while ((c = line[i]) != '=' && zap == 0){
                if(c == ' '){
                    zap++;
                }printf("%s", p[j].name);
                sprintf(p[j].name,"%s%c",p[j].name,c);
                i++;
            }
            j++;
        } else{
            i++;
        }
    }
}


void printProp(Prop* p){
    printf("%s",p->name);
}

int main() {
    char* line = malloc(sizeof(char) * 200);
    FILE* fo = fopen("../file.xml","r+");

    if(fo == NULL) return -1;

    while((fgets(line,200,fo)) != NULL){
        printf("%s",line);
        Prop* p = malloc(sizeof(property));
        property(line,p);
        printProp(p);
        free(p);
    }

    fclose(fo);

    return 0;
}
