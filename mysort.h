#ifndef MYSORT_H
#define MYSORT_H
#include <iostream>
class mySort
{
public:
    void list(int *array_X, int *array_Y, int MaxSize);
    void swap(int *a, int *b);
    void selection_sort(int *array, int MaxSize);
private:
    int a, b;
    int MaxSize;
    int *array;
    int *array_X, *array_Y;
};

void mySort::list(int *array_X, int *array_Y, int MaxSize)
{
    using std::cout;
    using std::cin;
    for (int i = 0; i < MaxSize; i++)
    {
        printf("%7d,%d", array_X[i], array_Y[i]);
        if (((i+1) % 5) == 0)
            std::cout << "\n";
    }
    std::cout << "\n";
}

void mySort::swap(int *a, int *b)
{
    int temp;

    temp = *a;
    *a = *b;
    *b = temp;
}

void mySort::selection_sort(int *array, int MaxSize)
{
    int i, j, min;
    for (i=0; i<MaxSize-1; i++)
    {
        min = i;
        for (j = i + 1; j < MaxSize; j++) {
            if (array[j] < array[min])
                min = j;
        }
        swap(&array[min], &array[i]);
    }
}
#endif // MYSORT_H
