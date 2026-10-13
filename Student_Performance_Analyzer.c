
#include <stdio.h>

struct Student
{
    int rollNumber;
    char name[50];
    int marks1;
    int marks2;
    int marks3;
};

int calculateTotal(struct Student s)
{
    return s.marks1 + s.marks2 + s.marks3;
}

float calculateAverage(int total)
{
    return total / 3.0;
}

char calculateGrade(float average)
{
    if (average >= 85)
        return 'A';
    else if (average >= 70)
        return 'B';
    else if (average >= 50)
        return 'C';
    else if (average >= 35)
        return 'D';
    else
        return 'F';
}

void printPerformance(char grade)
{
    int stars;

    if (grade == 'A')
        stars = 5;
    else if (grade == 'B')
        stars = 4;
    else if (grade == 'C')
        stars = 3;
    else if (grade == 'D')
        stars = 2;
    else
        stars = 0;

    for (int i = 0; i < stars; i++)
    {
        printf("*");
    }
}

void printRollNumbers(struct Student students[], int current, int n)
{
    if (current >= n)
        return;

    printf("%d ", students[current].rollNumber);

    printRollNumbers(students, current + 1, n);
}

int main()
{
    int n;

    char localMessage[] = "Student Performance Analyzer";

    printf("%s\n\n", localMessage);

    printf("Enter number of students: ");
    scanf("%d", &n);

    if (n < 1 || n > 100)
    {
        printf("Error: Number of students must be between 1 and 100.\n");
        return 0;
    }

    struct Student students[100];

    for (int i = 0; i < n; i++)
    {
        printf("\nEnter roll number and name for student %d: ", i + 1);
        scanf("%d %49[^\n]", &students[i].rollNumber, students[i].name);

        printf("Enter marks in three subjects (1-100): ");
        scanf("%d %d %d",
              &students[i].marks1,
              &students[i].marks2,
              &students[i].marks3);

        if (students[i].marks1 < 1 || students[i].marks1 > 100 ||
            students[i].marks2 < 1 || students[i].marks2 > 100 ||
            students[i].marks3 < 1 || students[i].marks3 > 100)
        {
            printf("Error: Marks must be between 1 and 100.\n");
            return 0;
        }
    }

    printf("\n");

    for (int i = 0; i < n; i++)
    {
        int total = calculateTotal(students[i]);
        float average = calculateAverage(total);
        char grade = calculateGrade(average);

        printf("Roll: %d\n", students[i].rollNumber);
        printf("Name: %s\n", students[i].name);
        printf("Total: %d\n", total);
        printf("Average: %.2f\n", average);
        printf("Grade: %c\n", grade);

        if (average < 35)
        {
            continue;
        }

        printf("Performance: ");
        printPerformance(grade);
        printf("\n\n");
    }

    printf("List of Roll Numbers (via recursion): ");
    printRollNumbers(students, 0, n);
    printf("\n");

    return 0;
}
