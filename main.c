#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
        p->num_production = num_count;
        Grammar[pro_count++] = p;

        strcpy(to_production_string, "");
        fclose(buffer_fp);

    };

    return Grammar;

}

void print_grammar(Grammar g, int n) {
    for(int i = 0; i < n; i++) {
        int num = g[i]->num_production;
        for(int j = 0; j < n; j++) {
            printf("%c->%s\n", g[i]->from_production, g[i]->to_production[j]);
        }
    }
}

int find_non_terminal(Grammar g, int n, char c) {
    for(int i = 0; i < n; i++) {
        if (g[i]->from_production == c) {
            return i;
        }
    }
    return -1;
}

char* get_random_production(Grammar g, int from_production) {

    int num_production = g[from_production]->num_production;
    char* to_production = g[from_production]->to_production[rand() % num_production];

    char* production = (char*) malloc(sizeof(char) * (strlen(to_production) + 1));
    production = to_production;
    return production;

}

char* collect_string(char* word, int start, int end) {

    char* dest = (char*) malloc(sizeof(char) * ((end-start) + 1));

    for(int i = 0; i < (end - start); i++) {
        dest[i] = word[start+i];
    }

    dest[end=start] = '\0';

    return dest;

}

char* helper(char* left, char* production, char* right) {

    size_t total_size = strlen(left) + strlen(production) + strlen(right) + 1;

    char* dest = (char*) malloc(sizeof(char) * total_size);

    strcpy(dest, left);
    strcat(dest, production);
    strcat(dest, right);

    return dest;

}

void string_iter(Grammar g, int n, char* word, int curr_index) {

    if (curr_index == strlen(word)) {
        return;
    }

    for(int i = 0; i < strlen(word); i++) {

        char* symbol_production;
        char symbol = word[i];
        int is_non_terminal = find_non_terminal(g, n, symbol);

        if(is_non_terminal == -1) {
            return;
        } else {

            symbol_production = get_random_production(g, is_non_terminal);

            char* left = collect_string(word, 0, i);
            char* right = collect_string(word, i+1, strlen(word));

            word = helper(left, symbol_production, right);
            string_iter(g, n, word, curr_index++);

        }

    }

}


int main(int argc, char** argv) {

    srand(clock());

    int n = atoi(argv[1]); // total number of productions to include from the productions_file
    int d = atoi(argv[2]); // depth till which the derivation should be carried

    FILE* fp = fopen("./productions", "r");
    Grammar g = read_productions(fp, n);
    int i = find_non_terminal(g, n, 'S');

    char* entry_point = get_random_production(g, i);
    string_iter(g, n, entry_point, 0);
    

    fclose(fp);

    return 0;

}
