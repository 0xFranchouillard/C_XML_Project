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
    printf("Veuillez entrez le chemin du fichier XML : ");
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
        printf("%s\"%s\"\n",var->name, tmp);
        printf("Balise ouvrante != de balise fermante");
        exit(0);
    }
    var = var->parent;
    return var;
}

XML* balise_ouvrante(int* cpt_xml, char* xml, int* cpt_tmp, char* tmp, XML* var){
    *cpt_xml += 1;

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

void parse_XML(char* xml, XML* var){
    char tmp[300];
    int cpt_tmp = 0;
    int cpt_xml = 0;

    while (xml[cpt_xml] != '\0'){
        if(xml[cpt_xml] == '<'){
            tmp[cpt_tmp] = '\0';
            if(xml[cpt_xml+1] == '?'){
                while (xml[cpt_xml++] != '>'){}
                cpt_xml++;
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
            }else{
                var = balise_ouvrante(&cpt_xml,xml,&cpt_tmp,tmp,var);
            }
            cpt_tmp = 0;
            cpt_xml += 1;
        } else {
            tmp[cpt_tmp++] = xml[cpt_xml++];
        }
    }

}

void read_XML(XML* root){ // a standartisé
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

    parse_XML(xml,var);
}


int main() {
    XML* var = create_XML_tree(NULL);
    XML* root = var;

    read_XML(root);

    printf("%s : ", root->name);
    for (int i = 0; i < root->taille_enfant; ++i) {
        printf("%s",root->enfant[i]->name);
    }

    free_XML(root);
    return 0;
}
