#include "plug.h"
#include <stdio.h>

const char* get_op_name(OP op) {
    switch (op) {
        case OP_ADD: return "ADD";
        case OP_SUB: return "SUB";
        case OP_MUL: return "MUL";
        case OP_DIV: return "DIV";
        case OP_VAL: return "VAL";
        case OP_X: return "X";
        case OP_Y: return "Y";
        case OP_SIN: return "SIN";
        case OP_COS: return "COS";
        default: return "INVALID";
    }
}

float randf(float min, float max) {
    return (min + (float) rand() / (float)(RAND_MAX / (max - min)));
}

float evaluate_tree(Node* node, float X, float Y) {

    if (node->operation == OP_VAL) {
        return atof(node->value);
    } else if (node->operation == OP_X) {
        return X;
    } else if (node->operation == OP_Y) {
        return Y;
    }

    float left, right;

    if (node->left != NULL) {
        left = evaluate_tree(node->left, X, Y);
    }

    if (node->right != NULL) {
        right = evaluate_tree(node->right, X, Y);
    }

    switch (node->operation) {

        case OP_ADD:
            return left + right;

        case OP_SUB:
            return left - right;

        case OP_MUL:
            return left * right;

        case OP_DIV:
            return left / right;

        case OP_SIN:
            return sinf(left + right);

        case OP_COS:
            return cosf(left + right);

        default:
            printf("UNREACHABLE\n");
            return 0.0;

    }

}

ImgGrid** map_to_img_grid(Node* parse_tree_root, char* motion_flag) {

    ImgGrid** img_grid = (ImgGrid**) malloc(sizeof(ImgGrid*) * (WIDTH));
    int rand_num = rand() % 11;

    for(int i = 0; i < WIDTH; i++) {

        img_grid[i] = (ImgGrid*) malloc(sizeof(ImgGrid) * HEIGHT);
        float ni = (float)i/WIDTH*2.0f - 1;

        for(int j = 0; j < HEIGHT; j++) {

            float nj = (float)j/HEIGHT*2.0f - 1;
            float eval = evaluate_tree(parse_tree_root, ni, nj);
            float scaled = (eval - 1)/2 * 255.0f;

            if (strcmp(motion_flag, "-p") == 0) {

                img_grid[i][j].c = (Color){
                        .r = scaled * 2,
                        .g = scaled * 3,
                        .b = scaled * randf(2, 3),
                        .a = 255,
                        /* tweak's */
                        /* You can multiply r, g, b with random values to get different colors */
                        /* multiply with randf(start, end) to get distored pixel effect */
                };

            } else if (strcmp(motion_flag, "-f2") == 0)  {
                
                img_grid[i][j].c = (Color){
                        .r = scaled * 3 * (randf(1, 2)),
                        .g = scaled * rand_num, // How many curves you want
                        .b = scaled,
                        .a = 255,
                        /* tweak's */
                        /* You can multiply r, g, b with random values to get different colors */
                        /* multiply with randf(start, end) to get distored pixel effect */
                };

            } else {

                img_grid[i][j].c = (Color){
                        .r = scaled * 3,
                        .g = scaled * 4, // How many curves you want
                        .b = scaled,
                        .a = 255,
                        /* tweak's */
                        /* You can multiply r, g, b with random values to get different colors */
                        /* multiply with randf(start, end) to get distored pixel effect */
                };

            }


        }
    }

    return img_grid;

}

void update_wave(ImgGrid** img_grid, char* motion_flag, int i, int j, WaveInfo* wave_info) {

    if (strcmp(motion_flag, "-f") == 0 || strcmp(motion_flag, "-f2") == 0) {

        img_grid[i][j].c.r = (img_grid[i][j].c.r + 1) % 256;
        img_grid[i][j].c.g = (img_grid[i][j].c.g + 1) % 256;
        img_grid[i][j].c.b = (img_grid[i][j].c.b + 1) % 256;

    } else if (strcmp(motion_flag, "-p") == 0) {

        wave_info->offset_r = wave_info->amplitude * cos((wave_info->k * i) - (wave_info->w * j) + wave_info->wave_time + 3.0f);
        wave_info->offset_g = wave_info->amplitude * cos((wave_info->k * i) - (wave_info->w * j) + wave_info->wave_time + 6.0f);
        wave_info->offset_b = wave_info->amplitude * cos((wave_info->k * i) - (wave_info->w * j) + wave_info->wave_time + 9.0f);

        int r = img_grid[i][j].c.r + wave_info->offset_r * 9;
        int g = img_grid[i][j].c.g + wave_info->offset_g * 3;
        int b = img_grid[i][j].c.b + wave_info->offset_b * 6;

        img_grid[i][j].c.r = ((r % 256)) % 256;
        img_grid[i][j].c.g = ((g % 256)) % 256;
        img_grid[i][j].c.b = ((b % 256)) % 256;

        if (wave_info->wave_time == 1000000.0f) {
            motion_flag = "-f";
        } else if (wave_info->amplitude > 2.0f) {
            wave_info->amplitude = 0.0f;
        } else {
            wave_info->wave_time += 1.0f/2.0f;
            wave_info->amplitude += 0.01f;
        }

    }
}
