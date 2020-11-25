#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct XML{
    char* name;
    int value;
    struct XML* parent;
    struct XML** enfant;
    int taille_alloue;
    int taille_enfant;
};
typedef struct XML XML;

struct DTD{
    char* name;
    char** value;
    int taille_alloue;
    int taille_value;
};

typedef struct DTD DTD;

struct list_DTD{
    int taille_alloue;
    int taille_tab_DTD;
    DTD* tab_DTD;
};

typedef struct list_DTD list_DTD;

void free_XML(XML* xml){
    free(xml->name);
    for (int i = 0; i < xml->taille_enfant; ++i) {
        free_XML(xml->enfant[i]);
    }
    free(xml);
}


XML* create_XML_tree(XML* parent){
    XML* xml = malloc(sizeof(XML));
    xml->parent = parent;
    xml->name = NULL;
    xml->value = 0;
    xml->taille_alloue = 1;
    xml->taille_enfant = 0;
    xml->enfant = malloc(sizeof(XML*)*xml->taille_alloue);
    if(parent != NULL){
        while(parent->taille_enfant >= parent->taille_alloue){
            parent->taille_alloue *= 2;
        }
        XML** enfant_tmp = malloc(sizeof(XML*) * parent->taille_alloue);
        for (int i = 0; i < parent->taille_alloue; ++i) {
            enfant_tmp[i] = parent->enfant[i];
        }
        free(xml->enfant);
        parent->enfant = enfant_tmp;
        parent->enfant[parent->taille_enfant] = xml;
        parent->taille_enfant += 1;
    }
    return xml;
}

FILE* open_XML(){
    printf("Veuillez entrez le chemin du fichier XML :  ");
    char* path[255];
    gets(path);
    FILE* file = fopen(path,"r+");
    return file;
}

XML* balise_fermante(int* cpt_xml, char* xml, int* cpt_tmp, char* tmp, XML* var){

    if(var == NULL){
        printf("Racine atteinte");
        exit(0);
    }

    *cpt_xml += 2;

    while(xml[*cpt_xml] != '>'){
        tmp[*cpt_tmp] = xml[*cpt_xml];
        *cpt_tmp += 1;
        *cpt_xml += 1;

        if(*cpt_xml == strlen(xml)){
            printf("Fichier terminé sans balise fermante");
            exit(0);
        }
    }
    tmp[*cpt_tmp] = '\0';
    if(strcmp(tmp,var->name) != 0){
        //printf("%s\"%s\"\n",var->name, tmp);
        printf("Balise ouvrante != de balise fermante");
        exit(0);
    }
    var = var->parent;
    return var;
}

XML* balise_ouvrante(int* cpt_xml, char* xml, int* cpt_tmp, char* tmp, XML* var){
    *cpt_xml += 1;
    *cpt_tmp = 0;

    if(var->name != NULL) {
        var = create_XML_tree(var);
    }
    while (xml[*cpt_xml] != '>'){
        tmp[*cpt_tmp] = xml[*cpt_xml];
        *cpt_tmp += 1;
        *cpt_xml += 1;
    }
    tmp[*cpt_tmp] = '\0';
    var->name = strdup(tmp);
    *cpt_tmp = 0;
    *cpt_xml += 1;

    return var;
}

void addValue_ELEMENT(DTD* dtd, char* tmp){
    if(dtd->taille_value > 0 && (strcmp(tmp,"#PCDATA") == 0 || strcmp(dtd->value[0],"#PCDATA") == 0)){
        printf("Erreur PCDATA non seul");
        exit(0);
    }
    while (dtd->taille_value >= dtd->taille_alloue){
        dtd->taille_alloue *= 2;
    }
    char** value_tmp = malloc(sizeof(char*)*dtd->taille_alloue);

    for (int i = 0; i < dtd->taille_alloue; ++i) {
        value_tmp[i] = dtd->value[i];
    }
    free(dtd->value);
    dtd->value = value_tmp;
    dtd->value[dtd->taille_value] = strdup(tmp);
    dtd->taille_value += 1;
}

void getValue_ELEMENT(char* file, int* cpt_file, DTD* dtd, char* tmp, int* cpt_tmp){
    if(file[*cpt_file-1] == '('){
        *cpt_tmp = 0;
        while (file[*cpt_file] == ')'){
            if(*cpt_file == strlen(file)){
                printf("Erreur DTD à la fermeture");
                exit(0);
            }
            if(file[*cpt_file] == '('){
                while(file[(*cpt_file)++] == ')'){}
            }
            tmp[(*cpt_tmp)++] = file[(*cpt_file)++];
            if(file[*cpt_file] == ','){
                tmp[*cpt_tmp] = '\0';
                addValue_ELEMENT(dtd,tmp);
                *cpt_tmp = 0;
                *cpt_file += 1;
            }
        }
        tmp[*cpt_tmp] = '\0';
        addValue_ELEMENT(dtd,tmp);
        *cpt_tmp = 0;
    }
}

void addElement_listDtd(list_DTD* listDtd,DTD* dtd){
    while(listDtd->taille_tab_DTD >= listDtd->taille_alloue){
        listDtd->taille_alloue *= 2;
    }
    list_DTD* value_tmp = malloc(sizeof(list_DTD*)*listDtd->taille_alloue);

    for (int i = 0; i < listDtd->taille_tab_DTD; ++i) {
        value_tmp[i].tab_DTD = listDtd->tab_DTD;
    }

    free(listDtd->tab_DTD);
    listDtd->tab_DTD = value_tmp;
    listDtd->tab_DTD[listDtd->taille_tab_DTD] = *dtd;
}

void get_ELEMENT(list_DTD* listDtd, char* file, int* cpt_file){
    char tmp[300];
    int cpt_tmp = 0;

    DTD* dtd = malloc(sizeof(DTD));
    dtd->taille_alloue = 1;
    dtd->taille_value = 0;
    dtd->name = NULL;
    dtd->value = malloc(sizeof(char*));

    while (file[*cpt_file] != '>'){
        tmp[cpt_tmp++] = file[*cpt_file++];

        if(tmp[cpt_tmp-1] == ' '){
            cpt_tmp--;
            continue;
        }

        if(file[*cpt_file] == ' ' && file[*cpt_file-1] != ' ' && dtd->name == NULL){
            tmp[cpt_tmp] = '\0';
            dtd->name = strdup(tmp);
            cpt_tmp = 0;
        }
        getValue_ELEMENT(file,cpt_file,dtd,tmp,&cpt_tmp);
    }
    addElement_listDtd(listDtd,dtd);
    cpt_tmp++;
}

int parse_DTD(list_DTD* listDtd, char* file){
    char tmp[300];
    int cpt_file = 0;
    int cpt_tmp = 0;

    while (file[cpt_file] != '\0' && file[cpt_file] != ']'){
        tmp[cpt_tmp++] = file[cpt_file++];
        tmp[cpt_tmp] = '\0';

        if(tmp[cpt_tmp-1] == ' '){
            cpt_tmp--;
            continue;
        }
        if(strcmp(tmp,"<!ELEMENT") == 0){
            get_ELEMENT(listDtd,file,cpt_file);
            cpt_tmp = 0;
            cpt_file++;
            continue;
        }
    }
    if(file[cpt_file] == ']'){
        while (file[cpt_file++] != '>'){}
        return cpt_file;
    } else {
        return 0;
    }
}

void parse_XML(char* xml, XML* var,list_DTD* listDtd){
    char tmp[300];
    int cpt_tmp = 0;
    int cpt_xml = 0;
    char* PATH_DTD = malloc(sizeof(char)*250);
    strcpy(PATH_DTD,"INT");
    int cpt_dtd = 0;

    while (xml[cpt_xml] != '\0'){
        if(xml[cpt_xml] == '<'){
            tmp[cpt_tmp] = '\0';
            if(xml[cpt_xml+1] == '?'){
                while (xml[cpt_xml++] != '>'){}
                continue;
            }
            if(xml[cpt_xml+1] == '!' && strstr(xml,"SYSTEM") != NULL){
                while (xml[cpt_xml++] != '"'){}
                while (xml[cpt_xml] != '"'){
                    PATH_DTD[cpt_dtd] = xml[cpt_xml];
                    printf("%c",xml[cpt_xml]);
                    cpt_xml++;
                    cpt_dtd++;
                }
                PATH_DTD[cpt_dtd] = '\0';
                cpt_xml++;
                printf("\n");
                FILE* dtd = fopen(PATH_DTD,"r+");
                if(dtd != NULL){
                    fseek(dtd,0,SEEK_END);
                    long size = ftell(dtd);
                    fseek(dtd,0,SEEK_SET);

                    char* filedtd = malloc(sizeof(char)*size+1);
                    fread(filedtd,1,size,dtd);
                    filedtd[size] = '\0';
                    cpt_xml += parse_DTD(listDtd,filedtd);
                    //printf("%s\n",filedtd);
                    continue;
                    } else {
                        printf("Erreur DTD allocation");
                        exit(0);
                    }
            } else if (xml[cpt_xml+1] == '[') {
                cpt_xml += parse_DTD(listDtd, xml);
                continue;
            }

            if(cpt_tmp > 0){
                if(var == NULL){
                    printf("Erreur, texte en dehors de balises");
                    exit(0);
                }
                if(var->name != NULL){
                    var->value = 1;
                }
                cpt_tmp = 0;
            }
            if(xml[cpt_xml+1] == '/'){
                var = balise_fermante(&cpt_xml,xml,&cpt_tmp,tmp,var);
            }else if(xml[cpt_xml+1] != '!'){
                var = balise_ouvrante(&cpt_xml,xml,&cpt_tmp,tmp,var);
            }
            cpt_tmp = 0;
            cpt_xml += 1;
        } else {
            tmp[cpt_tmp++] = xml[cpt_xml++];
            if(tmp[cpt_tmp-1] == ' '){
                cpt_tmp--;
                continue;
            }
        }
    }
    strstr(PATH_DTD,"INT")==NULL?printf("<---  %s  --->\n",PATH_DTD):printf("DTD INTERNE\n");
}

void read_XML(XML* root,list_DTD* listDtd){
    FILE* file = open_XML();
    if(file == NULL){
        printf("Erreur d'allocation");
        exit(0);
    }
    XML* var = root;
    fseek(file,0,SEEK_END);
    long size = ftell(file);
    fseek(file,0,SEEK_SET);

    char* xml = malloc(sizeof(char)*size+1);
    fread(xml,1,size,file);
    xml[size] = '\0';
    fclose(file);

    parse_XML(xml,var,listDtd);
}

int main() {
    XML* var = create_XML_tree(NULL);
    XML* root = var;
    list_DTD* listDtd = malloc(sizeof(list_DTD));

    listDtd->taille_tab_DTD = 0;
    listDtd->taille_alloue = 0;
    listDtd->tab_DTD = malloc(sizeof(DTD));

    read_XML(root,listDtd);

    for (int i = 0; i < listDtd->taille_tab_DTD ; i++){
        printf("\n%s ->",listDtd->tab_DTD[i].name);
    }

    printf("%s : ", root->name);
    for (int i = 0; i < root->taille_enfant; ++i) {
        printf("%s",root->enfant[i]->name);
    }

    free_XML(root);
    printf("\nOK");
    return 0;
}
