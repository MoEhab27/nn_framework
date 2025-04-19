#define NN_IMPLEMENTATION
#include "nn.h"

#include <iso646.h>
#include<time.h>
#include <stdlib.h>
#include <stdio.h>




// size_t arch[] = {2, 2, 1};



//
// float cost(Xor m, Mat ti, Mat to){
//     assert(ti.rows == to.rows);
//     assert(to.cols == m.a2.cols);
//     size_t n = ti.rows;
//
//     float c = 0;
//
//     for(size_t i = 0; i < n; ++i){
//         Mat x = mat_row(ti, i);
//         Mat y = mat_row(to, i);
//
//         mat_copy(m.a0, x);
//         forward_xor(m);
//
//         size_t q = to.cols;
//
//         for(size_t j = 0; j < q; ++j){
//             float d = MAT_AT(m.a2, 0, j) - MAT_AT(y, 0, j);
//             c+= d * d;
//         }
//     }
//     return c / n;
// }
//
// void finite_diff(Xor m, Xor g, float eps, Mat ti, Mat to)
// {
//     float saved;
//     float c =  cost(m, ti, to);
//
//     for (size_t i = 0; i < m.w1.rows; ++i) {
//         for (size_t j = 0; j < m.w1.cols; ++j) {
//             saved = MAT_AT(m.w1, i, j);
//             MAT_AT(m.w1, i, j) += eps;
//             MAT_AT(g.w1, i, j) = (cost(m, ti, to) - c) / eps;
//             MAT_AT(m.w1, i, j) = saved;
//         }
//     }
//     for (size_t i = 0; i < m.b1.rows; ++i) {
//         for (size_t j = 0; j < m.b1.cols; ++j) {
//             saved = MAT_AT(m.b1, i, j);
//             MAT_AT(m.b1, i, j) += eps;
//             MAT_AT(g.b1, i, j) = (cost(m, ti, to) - c) / eps;
//             MAT_AT(m.b1, i, j) = saved;
//         }
//     }
//     for (size_t i = 0; i < m.w2.rows; ++i) {
//         for (size_t j = 0; j < m.w2.cols; ++j) {
//             saved = MAT_AT(m.w2, i, j);
//             MAT_AT(m.w2, i, j) += eps;
//             MAT_AT(g.w2, i, j) = (cost(m, ti, to) - c) / eps;
//             MAT_AT(m.w2, i, j) = saved;
//         }
//     }
//
//     for (size_t i = 0; i < m.b2.rows; ++i) {
//         for (size_t j = 0; j < m.b2.cols; ++j) {
//             saved = MAT_AT(m.b2, i, j);
//             MAT_AT(m.b2, i, j) += eps;
//             MAT_AT(g.b2, i, j) = (cost(m, ti, to) - c) / eps;
//             MAT_AT(m.b2, i, j) = saved;
//         }
//     }
// }
//
// void xor_learn(Xor m, Xor g, float rate) {
//     for (size_t i = 0; i < m.w1.rows; ++i) {
//         for (size_t j = 0; j < m.w1.cols; ++j) {
//             MAT_AT(m.w1, i, j) -= MAT_AT(g.w1, i, j) * rate;
//         }
//     }
//     for (size_t i = 0; i < m.b1.rows; ++i) {
//         for (size_t j = 0; j < m.b1.cols; ++j) {
//             MAT_AT(m.b1, i, j) -= MAT_AT(g.b1, i, j) * rate;
//         }
//     }
//     for (size_t i = 0; i < m.w2.rows; ++i) {
//         for (size_t j = 0; j < m.w2.cols; ++j) {
//             MAT_AT(m.w2, i, j) -= MAT_AT(g.w2, i, j) * rate;
//         }
//     }
//
//     for (size_t i = 0; i < m.b2.rows; ++i) {
//         for (size_t j = 0; j < m.b2.cols; ++j) {
//             MAT_AT(m.b2, i, j) -= MAT_AT(g.b2, i, j) * rate;
//         }
//     }
// }


float td[] = {
    0, 0, 0,
    0, 1, 1,
    1, 1, 0,
    1, 0, 1
};

int main(void)
{
    srand(time(NULL));
    size_t stride = 3;
    size_t n = sizeof(td)/sizeof(td[0])/stride; // the amount of samples (rows)

    Mat ti = {
        .rows = n,
        .cols = 2,
        .stride = stride,
        .es = td
    };

    Mat to = {
        .rows = n,
        .cols = 1,
        .stride = 3,
        .es = td + 2,
    };
    size_t arch[] = {2, 2, 1};
    NN nn = nn_alloc(arch, ARRAY_LEN(arch));
    nn_rand(nn ,0, 1);
    Mat row  = mat_row(ti, 1);
    MAT_PRINT(row);
    mat_copy(NN_INPUT(nn), row);
    nn_forward(nn);
    MAT_PRINT(NN_OUTPUT(nn));

    // NN_PRINT(nn);
    return 0;
    //
    //
    //
    //
    //
    // srand(time(NULL));
    //
    // Xor m = xor_alloc();
    // Xor g = xor_alloc();
    //
    //
    // mat_rand(m.w1, 0, 1);
    // mat_rand(m.w2, 0, 1);
    // mat_rand(m.b1, 0, 1);
    // mat_rand(m.b2, 0, 1);
    //
    // float eps = 1e-1;
    // float rate = 1e-1;
    //
    // printf("cost = %f\n", cost(m, ti, to));
    // for (size_t i = 0; i < 100000; i++) {
    //     finite_diff(m, g, eps, ti, to);
    //     xor_learn(m, g, rate);
    //    // printf("cost = %f\n", cost(m, ti, to));
    // }
    // printf("cost = %f\n", cost(m, ti, to));
    //
    // for(size_t i = 0; i < 2; ++i){
    //     for(size_t j = 0; j < 2; ++j){
    //         MAT_AT(m.a0, 0, 0) = i;
    //         MAT_AT(m.a0, 0, 1) = j;
    //         forward_xor(m);
    //         float y = *m.a2.es;
    //         printf("%zu ^ %zu = %f\n", i, j, y);
    //     }
    // }

    return 0;
}