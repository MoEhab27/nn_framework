#ifndef NN_H_
#define NN_H_

#include <stddef.h>
#include <stdio.h>
#include <math.h>

#ifndef NN_MALLOC
#include<stdlib.h>
#define NN_MALLOC malloc
#endif // NN_MALLOC

#ifndef NN_ASSERT
#include<assert.h>
#define NN_ASSERT assert
#endif // NN_ASSERT

// Macro to return array size
#define ARRAY_LEN(xs) sizeof((xs))/sizeof((xs)[0])

typedef struct{
    size_t rows;
    size_t cols;
    size_t stride;
    float *es;
} Mat;

#define MAT_AT(m, i, j) (m).es[(i) * (m).stride + (j)]

float rand_float(void);
float sigmoidf(float x);




Mat mat_alloc(size_t rows, size_t cols);
void mat_fill(Mat a, float x);
void mat_rand(Mat m, float low, float high);
Mat mat_row(Mat m, size_t row);
void mat_copy(Mat dest, Mat src);
void mat_dot(Mat dest, Mat a, Mat b);
void mat_sum(Mat dest, Mat a);
void mat_sig(Mat m);
void mat_print(Mat m, const char* name, size_t padding);
#define MAT_PRINT(m) mat_print((m), (#m), 0)




typedef struct{
    size_t count;
    Mat *ws; // weights;
    Mat *bs; // biases
    Mat *as; // inputs (the amount of activations is count + 1)
}NN;

#define NN_INPUT(nn) (nn).as[0]
#define NN_OUTPUT(nn) (nn).as[(nn).count]
NN nn_alloc(size_t *arch, size_t arch_count);
void nn_print(NN nn, const char* name);
#define NN_PRINT(nn) nn_print((nn), (#nn))
void nn_rand(NN nn, float low, float high);
void nn_forward(NN nn);
float nn_cost(NN nn, Mat ti, Mat to);
void nn_finite_diff(NN n,NN g, float eps, Mat ti, Mat to);
void nn_learn(NN n, NN g, float rate);


#endif // NN_H_


#ifdef NN_IMPLEMENTATION


float rand_float(void){
#ifndef RAND_MAX
#define RAND_MAX 0x7FFF
#endif // RAND_MAX
    return (float) rand() / (float) RAND_MAX;
}


float sigmoidf(float x){
    return (1.f / (1.f + expf(-x)));
}


Mat mat_alloc(size_t rows, size_t cols)
{
    Mat m;
    m.rows = rows;
    m.cols = cols;
    m.stride = cols;
    m.es = NN_MALLOC(sizeof(*m.es) * rows * cols);
    NN_ASSERT(m.es != NULL);
    return m;
}

void mat_fill(Mat a, float x){
    for(size_t i = 0; i < a.rows; ++i){
        for(size_t j = 0; j < a.cols; ++j){
            MAT_AT(a, i, j) = x;
        }
    }
}

void mat_dot(Mat dest, Mat a, Mat b)
{
    NN_ASSERT(a.cols == b.rows);
    size_t n = a.cols;
    NN_ASSERT(dest.rows == a.rows);
    NN_ASSERT(dest.cols == b.cols);

    for(size_t i = 0; i < dest.rows; ++i){
        for(size_t j = 0; j < dest.cols; ++j){
            MAT_AT(dest, i, j) = 0;
            for(size_t k = 0; k < n; ++k){
                MAT_AT(dest, i, j) += MAT_AT(a, i, k) * MAT_AT(b, k, j);
            }
        }
    }
}
void mat_sum(Mat dest, Mat a)
{
    NN_ASSERT(dest.rows == a.rows);
    NN_ASSERT(dest.cols == a.cols);
    for(size_t i = 0; i < dest.rows; ++i){
        for(size_t j = 0; j < dest.cols; ++j){
            MAT_AT(dest, i, j) += MAT_AT(a, i, j);
        }
    }
    (void) dest;
    (void) a;
}

void mat_sig(Mat m){
    for(size_t i = 0; i < m.rows; ++i){
        for(size_t j = 0; j < m.cols; ++j){
            MAT_AT(m, i, j) = sigmoidf(MAT_AT(m, i, j));
        }
    }
}

void mat_print(Mat m, const char* name, size_t padding){
    printf("%*s%s = [\n",(int) padding, "" , name);
    for (size_t i = 0; i < m.rows; ++i) {
        printf("%*s", (int) padding, "" );
        for (size_t j = 0; j < m.cols; ++j) {
            printf("%f ",MAT_AT(m, i, j));
        }
        printf("\n");
    }
    printf("%*s]\n", (int) padding, "");
    return;
}

void mat_rand(Mat m, float low, float high){
    for (size_t i = 0; i < m.rows; ++i) {
        for (size_t j = 0; j < m.cols; ++j) {
            MAT_AT(m, i, j) = rand_float() * (high - low) + low;
        }
    }
    return;
}

// returns a  matrix that references to the row in the parameters
Mat mat_row(Mat m, size_t row){
    return (Mat){
        .rows = 1,
        .cols = m.cols,
        .stride = m.stride,
        .es = &MAT_AT(m, row, 0)
    };
}

// deep copy a matrix into another, must be the same sizes
void mat_copy(Mat dest, Mat src){
    NN_ASSERT(dest.rows == src.rows);
    NN_ASSERT(dest.cols == src.cols);
    for(size_t i = 0; i < dest.rows; ++i){
        for(size_t j = 0; j < dest.cols; ++j){
            MAT_AT(dest, i, j) =  MAT_AT(src, i, j);
        }
    }
}


/*
 function description
 parameters:
  - arch: the architecture of the neural network (the amount of neurons in each layer)
 for example: {2, 2, 1} means 2 inputs, 2 neurons in the first layer and 1 neuron in the second layer
  - arch_count: the amount of layers in the neural network including the input layer.
 for example: {2, 2, 1} means 3 layers (input, hidden and output)
    return value:
    - a NN struct with the weights, biases and activations

Note: if any of the allocations fail, the program will exit with an error message after freeing the previously allocated memory.
*/
NN nn_alloc(size_t *arch, size_t arch_count) {
    NN_ASSERT(arch_count > 0);
    NN nn;
    nn.count = arch_count - 1;
    nn.ws = NN_MALLOC(sizeof(*nn.ws) * nn.count);
    if (nn.ws == NULL) {
        fprintf(stderr, "Failed to allocate memory for weights\n");
        exit(EXIT_FAILURE);
    }
    nn.bs = NN_MALLOC(sizeof(*nn.bs) * nn.count);
    if (nn.bs == NULL) {
        free(nn.ws); // Free previously allocated memory
        fprintf(stderr, "Failed to allocate memory for biases\n");
        exit(EXIT_FAILURE);
    }
    nn.as = NN_MALLOC(sizeof(*nn.as) * (nn.count + 1));
    if (nn.as == NULL) {
        free(nn.ws);
        free(nn.bs);
        fprintf(stderr, "Failed to allocate memory for activations\n");
        exit(EXIT_FAILURE);
    }
    nn.as[0] = mat_alloc(1, arch[0]); // input layer
    for (size_t i = 1; i < arch_count; ++i) {
        nn.ws[i - 1] = mat_alloc(nn.as[i - 1].cols, arch[i]);
        nn.bs[i - 1] = mat_alloc(1, arch[i]);
        nn.as[i] = mat_alloc(1, arch[i]);
    }
    return nn;
}
void nn_print(NN nn, const char* name) {
    char buf[256];
    printf("%s = [\n", name);
    for (size_t i = 0; i < nn.count; ++i) {
        snprintf(buf, sizeof(buf), "ws[%zu]", i);
        mat_print(nn.ws[i], buf, 4);
        snprintf(buf, sizeof(buf), "bs[%zu]", i);
        mat_print(nn.bs[i], buf, 4);

    }
    printf("]\n");
}

void nn_rand(NN nn, float low, float high) {
    for (size_t i = 0; i < nn.count; ++i) {
        mat_rand(nn.ws[i], low, high);
        mat_rand(nn.bs[i], low, high);
    }
}

void nn_forward(NN nn){
    for (size_t i = 0; i < nn.count; ++i){
        mat_dot(nn.as[i + 1], nn.as[i], nn.ws[i]);
        mat_sum(nn.as[i + 1], nn.bs[i]);
        mat_sig(nn.as[i + 1]);
    }
}

float nn_cost(NN nn, Mat ti, Mat to){
    assert(ti.rows == to.rows); // ensures that the number of rows (samples) in the input matrix ti matches the number of rows in the output matrix to.
    assert(to.cols == NN_OUTPUT(nn).cols); // ensures that the number of columns (features) in the output matrix to matches the number of neurons in the output layer of the neural network (NN_OUTPUT(nn)).
    size_t n = ti.rows;
    float c = 0;
    for(size_t i = 0; i < n; ++i){
        Mat x = mat_row(ti, i);
        Mat y = mat_row(to, i);
        mat_copy(NN_INPUT(nn), x);
        nn_forward(nn);

        size_t q = to.cols;
        // calculate the cost using the mean squared error formula between the predicted output (NN_OUTPUT(nn)) and the actual output (y).
        for(size_t j = 0; j < q; ++j){
            float d = MAT_AT(NN_OUTPUT(nn), 0, j) - MAT_AT(y, 0, j);
            c+= d * d;
        }
    }
    return c / n;
}

void nn_finite_diff(NN n,NN g, float eps, Mat ti, Mat to) {
    float saved;
    float c = nn_cost(n, ti, to);
    for (size_t i = 0; i < n.count; ++i) {
        // apply finite difference method to calculate the gradient of the cost function with respect to the weights
        for (size_t j = 0; j < n.ws[i].rows; ++j) {
            for (size_t k = 0; k < n.ws[i].cols; ++k) {
                saved = MAT_AT(n.ws[i], j, k);
                MAT_AT(n.ws[i], j, k) += eps;
                MAT_AT(g.ws[i], j, k) = (nn_cost(n, ti, to) - c) / eps;
                MAT_AT(n.ws[i], j, k) = saved;
            }
        }
        // apply finite difference method to calculate the gradient of the cost function with respect to the biases
        for (size_t j = 0; j < n.bs[i].rows; ++j) {
            for (size_t k = 0; k < n.bs[i].cols; ++k) {
                saved = MAT_AT(n.bs[i], j, k);
                MAT_AT(n.bs[i], j, k) += eps;
                MAT_AT(g.bs[i], j, k) = (nn_cost(n, ti, to) - c) / eps;
                MAT_AT(n.bs[i], j, k) = saved;
            }
        }
    }
}

void nn_learn(NN n, NN g, float rate) {
    for (size_t i = 0; i < n.count; ++i) {
        for (size_t j = 0; j < n.ws[i].rows; ++j) {
            for (size_t k = 0; k < n.ws[i].cols; ++k) {
                MAT_AT(n.ws[i], j, k) -= MAT_AT(g.ws[i], j, k) * rate;
            }
        }
        for (size_t j = 0; j < n.bs[i].rows; ++j) {
            for (size_t k = 0; k < n.bs[i].cols; ++k) {
                MAT_AT(n.bs[i], j, k) -= MAT_AT(g.bs[i], j, k) * rate;
            }
        }
    }

}

#endif // NN_IMPLEMENTATION