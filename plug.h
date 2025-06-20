#ifndef _PLUG_H_
#define _PLUG_H_

#include <math.h>
#include <string.h>
#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>

#define WIDTH 500
#define HEIGHT 500

/* #define WIDTH 1760 */
/* #define HEIGHT 990 */

#define BUFFER_SIZE 1024 * 1024

typedef enum OP {
    OP_ADD,                     // INTERNAL NODE
    OP_SUB,                     // INTERNAL NODE
    OP_MUL,                     // INTERNAL NODE
    OP_DIV,                     // INTERNAL NODE
    OP_SIN,                     // INTERNAL NODE
    OP_COS,                     // INTERNAL NODE
    OP_TAN,                    // INTERNAL NODE
    OP_VAL,                     // LEAF     NODE
    OP_X,                       // VAR LEAF NODE X 
    OP_Y,                       // VAR LEAF NODE Y
} OP;

typedef struct Node {
    struct Node* left;
    struct Node* right;
    char* value;                // LEAF NODE
    OP operation;               // INTERNAL NODE
} Node;

typedef struct Token_Info {
    char** token_list;
    int total_tokens;
} Token_Info;

typedef struct Production {     // S -> aS | aa
    int num_production;         // 2 Productions
    char from_production;       // S
    char** to_production;       // aS, aa
} Production;

typedef Production** Grammar;   // Set of Productions = Grammar

typedef struct WaveInfo {
    float amplitude;
    float k;
    float w;
    float offset_r, offset_g, offset_b;
    float wave_time;
} WaveInfo;

typedef struct ImgGrid {
    Color c;
} ImgGrid;

const char* get_op_name(OP op);
float randf(float min, float max);
float evaluate_tree(Node* node, float X, float Y);
ImgGrid** map_to_img_grid(Node* parse_tree_root, char* motion_flag);
void update_wave(ImgGrid** img_grid, char* motion_flag, int i, int j, WaveInfo* wave_info);

#endif // _PLUG_H_
