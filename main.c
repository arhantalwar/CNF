#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>

typedef enum OP {
    OP_ADD,
    OP_MUL,
    OP_VAL,     // LEAF NODES
    OP_X,       // X
    OP_Y,       // Y
} OP;

typedef struct Node {
    struct Node* left;
    struct Node* right;
    char* value; // LEAF NODE
    OP operation;
} Node;

typedef struct Token_Info {
    char** token_list;
    int total_tokens;
} Token_Info;

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

        memset(p, 0, sizeof(Production));

        int num_count = 0;

        FILE* buffer_fp = fmemopen(buffer, strlen(buffer), "r");

        char symbol = getc(buffer_fp);
        p->from_production = symbol;

        while(symbol != EOF) {

            symbol = getc(buffer_fp);

            if (symbol == '=' || symbol == '>') {
                continue;
            } else if (symbol == '|') {
                num_count++;
                strcat(to_production_string, token);
                strcat(to_production_string, "|");
                token[0] = '\0';
            } else {
                size_t token_len = strlen(token);
                if (token_len < sizeof(token) - 1) {
                    token[token_len] = symbol;
                    token[token_len + 1] = '\0';
                }            
            }

        }

        size_t token_len = strlen(token);
        token[token_len-2] = '\0';

        strcat(token, &symbol);
        strcat(to_production_string, token);
        token[0] = '\0';

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

        to_production_string[0] = '\0';
        fclose(buffer_fp);

    };

    return Grammar;

}

void print_grammar(Grammar g, int n) {
    for(int i = 0; i < n; i++) {
        int num = g[i]->num_production;
        for(int j = 0; j < num; j++) {
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

    char* production = (char*) malloc(sizeof(char) * (strlen(to_production)));
    production = to_production;

    return production;

}

char* collect_string(char* word, int start, int end) {

    char* dest = (char*) malloc(sizeof(char) * ((end-start) + 1));

    for(int i = 0; i < (end - start); i++) {
        dest[i] = word[start+i];
    }

    dest[end-start] = '\0';

    return dest;

}

char* join(char* left, char* production, char* right) {

    size_t total_size = strlen(left) + strlen(production) + strlen(right) + 1;

    char* dest = (char*) malloc(sizeof(char) * total_size);

    strcpy(dest, left);
    strcat(dest, production);
    strcat(dest, right);

    return dest;

}

void sanitize_string(char* str) {
    int j = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (isprint(str[i]) || str[i] == '\n') {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
}

static int word_parsed_count = 0;

void generate_word(Grammar g, int n, char* word) {

    for(int i = 0; i < strlen(word); i++) {

        char* symbol_production;
        char symbol = word[i];
        int is_non_terminal = find_non_terminal(g, n, symbol);

        if(is_non_terminal == -1) {
            continue;
        } else {

            char* left = collect_string(word, 0, i);
            symbol_production = get_random_production(g, is_non_terminal);
            char* right = collect_string(word, i+1, strlen(word));

            word = join(left, symbol_production, right);

            /* printf("\nNEW WORD: %s\n", word); */

            /* printf("    Left: %s", left); */
            /* printf("    Production: %s", symbol_production); */
            /* printf("    Right: %s", right); */

            generate_word(g, n, word);
            word_parsed_count++;

            if(word_parsed_count == 1) {

                FILE* fp = fopen("./production_output", "w");

                if(!fp) {
                    fprintf(stderr, "[-] ERROR OPENING THE FILE 'production_output'\n");
                    exit(-1);
                }

                char* production_output_string = collect_string(word, 0, strlen(word));

                sanitize_string(production_output_string);

                if(fwrite(production_output_string , 1, strlen(production_output_string), fp) == 0) {
                    fprintf(stderr, "[-] ERROR WRITING TO THE FILE 'production_output'\n");
                    exit(-1);
                };

                fclose(fp);

            } else {

                return;

            }

        }

    }

    return;

}

Node* get_node() {

    Node* new_node = malloc(sizeof(struct Node));

    new_node->left = NULL;
    new_node->right = NULL;
    new_node->value = NULL;

    return new_node;

}

Token_Info* tokenize_expression(FILE* fp) {

    char token[128];
    char buffer[256];

    memset(token, 0, 128);
    memset(buffer, 0, 256);

    if(fgets(buffer, sizeof(buffer), fp) == NULL) {
        fprintf(stderr, "[-] ERR READING FROM OUTPUT FILE\n");
        exit(-1);
    };

    FILE* buffer_fp = fmemopen(buffer, strlen(buffer), "r");

    if(buffer_fp == NULL) {
        fprintf(stderr, "[-] ERR READING FROM OUTPUT FILE\n");
        exit(-1);
    };

    //--------------------------------------------------- TOKENIZATION

    int curr_index = 0;
    int total_tokens = 0;
    char symbol = buffer[curr_index];

    char** token_list = (char**) malloc(sizeof(char*) * 1);

    while(buffer[curr_index] != '\0') {

        symbol = buffer[curr_index];

        if (strlen(token) + 1 < sizeof(token)) {

            char temp[2] = {symbol, '\0'};
            strcat(token, temp);

        } else {

            fprintf(stderr, "[-] ERR TOKEN OVERFLOW");
            exit(-1);

        }

        /* printf("Current token : %s\n", token); */

        if (strcmp(token, "mul") == 0) {
            /* printf("FOUND %s\n", token); */
            
            token_list = realloc(token_list, (total_tokens + 1) * sizeof(char*));
            token_list[total_tokens++] = strdup(token);
            token[0] = '\0';

        } else if (strcmp(token, "add") == 0) {
            /* printf("FOUND %s\n", token); */

            token_list = realloc(token_list, (total_tokens + 1) * sizeof(char*));
            token_list[total_tokens++] = strdup(token);
            token[0] = '\0';

        } else if (strcmp(token, "(") == 0) {
            /* printf("FOUND %s\n", token); */

            token_list = realloc(token_list, (total_tokens + 1) * sizeof(char*));
            token_list[total_tokens++] = strdup(token);
            token[0] = '\0';

        } else if (strcmp(token, ")") == 0) {
            /* printf("FOUND %s\n", token); */

            token_list = realloc(token_list, (total_tokens + 1) * sizeof(char*));
            token_list[total_tokens++] = strdup(token);
            token[0] = '\0';

        } else if (strcmp(token, ",") == 0) {
            /* printf("FOUND %s\n", token); */

            token_list = realloc(token_list, (total_tokens + 1) * sizeof(char*));
            token_list[total_tokens++] = strdup(token);
            token[0] = '\0';

        } else if (strcmp(token, "y") == 0) {
            /* printf("FOUND %s\n", token); */

            token_list = realloc(token_list, (total_tokens + 1) * sizeof(char*));
            token_list[total_tokens++] = strdup(token);
            token[0] = '\0';

        } else if (strcmp(token, "x") == 0) {
            /* printf("FOUND %s\n", token); */

            token_list = realloc(token_list, (total_tokens + 1) * sizeof(char*));
            token_list[total_tokens++] = strdup(token);
            token[0] = '\0';

        } else if (strcmp(token, "-") == 0 || isdigit(symbol)) {

            int temp = curr_index;
            char sym = buffer[temp];

            if (sym == '-') {
                temp++;
                sym = buffer[temp];
            }

            while(isdigit(sym) || sym == '.') {
                temp++;
                sym = buffer[temp];
            }

            char* num = collect_string(buffer, curr_index, temp);
            /* printf("FOUND %s\n", num); */

            token_list = realloc(token_list, (total_tokens + 1) * sizeof(char*));
            token_list[total_tokens++] = strdup(num);
            token[0] = '\0';

            curr_index = temp-1;
            
        }

        /* printf("%c\n", symbol); */
        /* printf("%s\n", token); */

        curr_index++;

    }

    Token_Info* token_info = (Token_Info*) malloc(sizeof(Token_Info));
    token_info->token_list = token_list;
    token_info->total_tokens = total_tokens;

    return token_info;


}

Node* parse_tree(Token_Info token_info, Node* node, int* token_to_parse) {

    if (*token_to_parse == token_info.total_tokens) {
        return node;
    }

    char* token = token_info.token_list[*token_to_parse];

    if (*token_to_parse == 0) {

        if (strcmp(token, "mul") == 0) {
            node->operation = OP_MUL;
        } else if (strcmp(token, "add") == 0) {
            node->operation = OP_ADD;
        }

        (*token_to_parse)++;
        parse_tree(token_info, node, token_to_parse);

    } else if (strcmp(token, "mul") == 0) {

        node->operation = OP_MUL;

        (*token_to_parse)++;
        node->left = get_node();
        parse_tree(token_info, node->left, token_to_parse);

        (*token_to_parse)++;
        node->right = get_node();
        parse_tree(token_info, node->right, token_to_parse);

    } else if (strcmp(token, "add") == 0) {

        node->operation = OP_ADD;

        (*token_to_parse)++;
        node->left = get_node();
        parse_tree(token_info, node->left, token_to_parse);

        (*token_to_parse)++;
        node->right = get_node();
        parse_tree(token_info, node->right, token_to_parse);

    } else if (strcmp(token, "(") == 0) {

        (*token_to_parse)++;
        node->left = get_node();
        parse_tree(token_info, node->left, token_to_parse);

        (*token_to_parse)++;
        node->right = get_node();
        parse_tree(token_info, node->right, token_to_parse);

    } else if (strcmp(token, ")") == 0) {

        return node;

    } else if (strcmp(token, ",") == 0) {

        return node;

    } else if (strcmp(token, "x") == 0) {

        (*token_to_parse)++;

        node->left = NULL;
        node->right = NULL;

        node->value = strdup(token);
        node->operation = OP_X;

    } else if (strcmp(token, "y") == 0) {

        (*token_to_parse)++;

        node->left = NULL;
        node->right = NULL;

        node->value = strdup(token);
        node->operation = OP_Y;

    } else {

        (*token_to_parse)++;

        node->left = NULL;
        node->right = NULL;

        node->value = strdup(token);
        node->operation = OP_VAL;

    }

    return node;

}

float evaluate_tree(Node* node, float X, float Y) {

    if (node == NULL) {
        return 0.293423;
    }

    if (node->operation == OP_VAL) {
        return atof(node->value);
    }

    if (node->operation == OP_X) {
        return X;
    }

    if (node->operation == OP_Y) {
        return Y;
    }

    float left, right;


    left = evaluate_tree(node->left, X, Y);

    right = evaluate_tree(node->right, X, Y);

    if (node->operation == OP_ADD) {
        return left+right;
    }
    else if (node->operation == OP_MUL) {
        return left*right;
    }

}

void print_tree_inorder(Node* root) {
    if (root) {

        /* if (root->operation == OP_ADD) { */
        /*     printf("ADD\n"); */
        /* } else if (root->operation == OP_MUL) { */
        /*     printf("MUL\n"); */
        /* } else if (root->operation == OP_VAL) { */
        /*     printf("%s\n", root->value); */
        /* } */

        print_tree_inorder(root->left);
        print_tree_inorder(root->right);

    }
}

// TODO
// 1) Implement Parser
// 2) Pixel Engine
// 3) Normalizing Values when mapping

int main(int argc, char** argv) {

    srand(6);
    /* srand(47); */

    if(argv[1] == NULL) {
        fprintf(stderr, "[!] Pass Number Of Productions To Be Included\n");
        exit(-1);
    }

    int n = atoi(argv[1]); // total number of productions to include from the productions_file

    FILE* fp = fopen("./productions", "r");
    FILE* fp_output;

    if(!fp) {
        fprintf(stderr, "[!] ERROR READING FROM THE FILE 'Productions'\n");
        exit(-1);
    }

    Grammar g = read_productions(fp, n);

    int i = find_non_terminal(g, n, 'S');
    char* entry_point = get_random_production(g, i);

    /* printf("Entry Point S -> %s\n", entry_point); */
    generate_word(g, n, entry_point);
    /* print_grammar(g, n); */

    fp_output = fopen("./production_output", "r");
    Token_Info* token_info = tokenize_expression(fp_output);

    Node* root;
    int token_to_parse = 0;

    root = parse_tree(*token_info, root, &token_to_parse);
    /* print_tree_inorder(root); */

    float ans = evaluate_tree(root, 1.1, 2.2);
    printf("%f\n", ans);

    free(g);
    free(entry_point);
    fclose(fp);
    fclose(fp_output);

    return 0;

}
