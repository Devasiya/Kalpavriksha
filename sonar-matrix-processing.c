#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void displayMatrix(int *matrix, int n)
{
    int i, j;

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
            printf("%4d", *(matrix + i * n + j));

        printf("\n");
    }
}

void rotateClockwise(int *matrix, int n)
{
    int i, j, temp;

    for (i = 0; i < n / 2; i++)
    {
        for (j = i; j < n - 1 - i; j++)
        {
            temp = *(matrix + i * n + j);

            *(matrix + i * n + j) =
                *(matrix + (n - 1 - j) * n + i);

            *(matrix + (n - 1 - j) * n + i) =
                *(matrix + (n - 1 - i) * n + (n - 1 - j));

            *(matrix + (n - 1 - i) * n + (n - 1 - j)) =
                *(matrix + j * n + (n - 1 - i));

            *(matrix + j * n + (n - 1 - i)) = temp;
        }
    }
}

void smoothMatrix(int *matrix, int n)
{
    int *previous = malloc(n * sizeof(int));
    int *current  = malloc(n * sizeof(int));
    int *next     = malloc(n * sizeof(int));
    int *result   = malloc(n * sizeof(int));

    int i, j, r, c;

    if (previous == NULL || current == NULL ||
        next == NULL || result == NULL)
    {
        printf("Memory allocation failed.\n");
        free(previous);
        free(current);
        free(next);
        free(result);
        return;
    }

    /* Save the first two original rows. */
    for (j = 0; j < n; j++)
    {
        *(current + j) = *(matrix + j);

        if (n > 1)
            *(next + j) = *(matrix + n + j);
    }

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            int sum = 0;
            int count = 0;

            for (r = i - 1; r <= i + 1; r++)
            {
                if (r < 0 || r >= n)
                    continue;

                for (c = j - 1; c <= j + 1; c++)
                {
                    if (c < 0 || c >= n)
                        continue;

                    if (r == i - 1)
                        sum += *(previous + c);
                    else if (r == i)
                        sum += *(current + c);
                    else
                        sum += *(next + c);

                    count++;
                }
            }

            *(result + j) = sum / count;
        }

        /* Replace the current row only after calculating it. */
        for (j = 0; j < n; j++)
            *(matrix + i * n + j) = *(result + j);

        /* Advance the saved original rows. */
        if (i + 1 < n)
        {
            int *temp = previous;
            previous = current;
            current = next;
            next = temp;
        }

        /* Load the next row before it is modified. */
        if (i + 2 < n)
        {
            for (j = 0; j < n; j++)
                *(next + j) = *(matrix + (i + 2) * n + j);
        }
    }

    free(previous);
    free(current);
    free(next);
    free(result);
}

int main(void)
{
    int n, i, j;
    int *matrix;

    printf("Enter matrix size (2-10): ");

    if (scanf("%d", &n) != 1 || n < 2 || n > 10)
    {
        printf("Error: Invalid matrix size.\n");
        return 1;
    }

    matrix = malloc(n * n * sizeof(int));

    if (matrix == NULL)
    {
        printf("Memory allocation failed.\n");
        return 1;
    }

    srand((unsigned int)time(NULL));

    /* Generate random intensity values. */
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
            *(matrix + i * n + j) = rand() % 256;
    }

    printf("\nOriginal Randomly Generated Matrix:\n");
    displayMatrix(matrix, n);

    rotateClockwise(matrix, n);

    printf("\nMatrix after 90-degree Clockwise Rotation:\n");
    displayMatrix(matrix, n);

    smoothMatrix(matrix, n);

    printf("\nMatrix after Applying 3x3 Smoothing Filter:\n");
    displayMatrix(matrix, n);

    free(matrix);

    return 0;
}

