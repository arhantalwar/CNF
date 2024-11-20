#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// CFG Specifically CNF
// S -> aS | aa
typedef struct Production {
    int num_production; // 2 Productions
    char from_production; // S
    char** to_production; // aS and aa
} Production;

typedef Production** Grammar;// Set of Productions = Grammar

Grammar read_productions(FILE* fp, int n) {

    int pro_count = 0;
    char token[128];
    char buffer[128];
    char to_production_string[512];
    Production** Grammar = (Production**) malloc(sizeof(Production*) * n);

    if(!Grammar) {
        fprintf(stderr, "[-] ERR ALLOCATING GRAMMAR");
    }

    memset(token, 0, 128);
    memset(buffer, 0, 128);
    memset(to_production_string, 0, 512);

    while(fgets(buffer, 128, fp)) {

        Production* p = (Production*) malloc(sizeof(struct Production));

        if(!p) {
            fprintf(stderr, "[-] ERR ALLOCATING PRODUCTION");
        }

        int num_count = 0;

        FILE* buffer_fp = fmemopen(buffer, strlen(buffer), "r");

        char symbol = getc(buffer_fp);
        p->from_production = symbol;

        while(symbol != EOF) {

            symbol = getc(buffer_fp);

            if (symbol == '-' || symbol == '>') {
                continue;
            } else if (symbol == '|') {
                num_count++;
                strcat(to_production_string, token);
                strcat(to_production_string, "|");
                strcpy(token, "");
            } else {
                strcat(token, &symbol);
            }

        }

        strcat(token, &symbol);
        strcat(to_production_string, token);
        strcpy(token, "");

        num_count++;

        char** build_to_production = malloc(sizeof(char*)*num_count);
        if(!build_to_production) {
            fprintf(stderr, "[-] ERR ALLOCATING BUILD_TO_PRODUCTION");
        }
        int num_count_2 = 0;
        char* rule = strtok(to_production_string, "|");

        while (rule != NULL) {
            build_to_production[num_count_2++] = strdup(rule);
            rule = strtok(NULL, "|");
        }

        p->to_production = build_to_production;
        Grammar[pro_count++] = p;

        strcpy(to_production_string, "");
        fclose(buffer_fp);

    };

    return Grammar;

}

int main(int argc, char** argv) {

    FILE* fp = fopen("./productions", "r");
    Grammar g = read_productions(fp, atoi(argv[1]));
    fclose(fp);


    return 0;

}
