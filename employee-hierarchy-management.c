
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Employee
{
    int id;
    char name[100];
    char designation[100];

    struct Employee *manager;
    struct Employee *firstReport;
    struct Employee *nextReport;
    struct Employee *nextEmployee;
} Employee;

Employee *employees = NULL;

/* Find an employee using ID */
Employee *findEmployee(int id)
{
    Employee *temp = employees;

    while (temp != NULL)
    {
        if (temp->id == id)
            return temp;

        temp = temp->nextEmployee;
    }

    return NULL;
}

/* Add an employee */
void addEmployee(void)
{
    Employee *newEmployee, *manager = NULL, *temp;
    int id, managerID;

    printf("Enter Employee ID: ");
    scanf("%d", &id);

    if (findEmployee(id) != NULL)
    {
        printf("Employee ID already exists.\n");
        return;
    }

    printf("Enter Name: ");
    scanf(" %99[^\n]", (newEmployee = malloc(sizeof(Employee)))->name);

    printf("Enter Designation: ");
    scanf(" %99[^\n]", newEmployee->designation);

    printf("Enter Reporting Manager ID (0 for top-level): ");
    scanf("%d", &managerID);

    //if the managerID is not 0, find the manager till a valid manager is found
    while (1)
    {
        printf("Enter Reporting Manager ID (0 for top-level): ");
        scanf("%d", &managerID);

        if (managerID == 0)
        {
            manager = NULL;
            break;
        }

        manager = findEmployee(managerID);

        if (manager != NULL)
            break;

        printf("Reporting manager not found. Try again.\n");
    }

    newEmployee->id = id;
    newEmployee->manager = manager;
    newEmployee->firstReport = NULL;
    newEmployee->nextReport = NULL;
    newEmployee->nextEmployee = NULL;

    /* Add to the employee list */
    if (employees == NULL)
    {
        employees = newEmployee;
    }
    else
    {
        temp = employees;

        while (temp->nextEmployee != NULL)
            temp = temp->nextEmployee;

        temp->nextEmployee = newEmployee;
    }

    /* Add to the manager's reporting list */
    if (manager != NULL)
    {
        if (manager->firstReport == NULL)
        {
            manager->firstReport = newEmployee;
        }
        else
        {
            temp = manager->firstReport;

            while (temp->nextReport != NULL)
                temp = temp->nextReport;

            temp->nextReport = newEmployee;
        }
    }

    printf("Employee added successfully.\n");
}

/* Remove an employee and promote their direct reports
   to the removed employee's manager */
void removeEmployee(void)
{
    int id;
    Employee *emp, *manager, *temp, *prev, *child;

    printf("Enter Employee ID to remove: ");
    scanf("%d", &id);

    emp = findEmployee(id);

    if (emp == NULL)
    {
        printf("Employee not found.\n");
        return;
    }

    manager = emp->manager;

    /* Remove employee from manager's report list */
    if (manager != NULL)
    {
        temp = manager->firstReport;
        prev = NULL;

        while (temp != NULL && temp != emp)
        {
            prev = temp;
            temp = temp->nextReport;
        }

        if (temp != NULL)
        {
            if (prev == NULL)
                manager->firstReport = emp->nextReport;
            else
                prev->nextReport = emp->nextReport;
        }
    }

    /* Transfer direct reports to the removed employee's manager */
    child = emp->firstReport;

    while (child != NULL)
    {
        Employee *nextChild = child->nextReport;

        child->manager = manager;
        child->nextReport = NULL;

        if (manager != NULL)
        {
            if (manager->firstReport == NULL)
                manager->firstReport = child;
            else
            {
                temp = manager->firstReport;

                while (temp->nextReport != NULL)
                    temp = temp->nextReport;

                temp->nextReport = child;
            }
        }

        child = nextChild;
    }

    /* Remove employee from the main employee list */
    temp = employees;
    prev = NULL;

    while (temp != NULL && temp != emp)
    {
        prev = temp;
        temp = temp->nextEmployee;
    }

    if (prev == NULL)
        employees = emp->nextEmployee;
    else
        prev->nextEmployee = emp->nextEmployee;

    free(emp);

    printf("Employee removed successfully.\n");
}

/* Recursively count employees reporting under an employee */
int countEmployees(Employee *employee)
{
    int count = 0;
    Employee *child;

    if (employee == NULL)
        return 0;

    child = employee->firstReport;

    while (child != NULL)
    {
        count++;
        count += countEmployees(child);
        child = child->nextReport;
    }

    return count;
}

/* Recursively display an employee and their hierarchy */
void displayHierarchy(Employee *employee, int level)
{
    Employee *child;
    int i;

    if (employee == NULL)
        return;

    for (i = 0; i < level; i++)
        printf("    ");

    printf("%d - %s - %s\n",
           employee->id,
           employee->name,
           employee->designation);

    child = employee->firstReport;

    while (child != NULL)
    {
        displayHierarchy(child, level + 1);
        child = child->nextReport;
    }
}

/* Display hierarchy of a selected employee */
void displayEmployeeHierarchy(void)
{
    int id;
    Employee *employee;

    printf("Enter Employee ID: ");
    scanf("%d", &id);

    employee = findEmployee(id);

    if (employee == NULL)
    {
        printf("Employee not found.\n");
        return;
    }

    printf("\n===== EMPLOYEE HIERARCHY =====\n");

    displayHierarchy(employee, 0);

    printf("Total Employees Under %d: %d\n",
           id, countEmployees(employee));
}

/* Display the complete organization hierarchy */
void displayCompleteHierarchy(void)
{
    Employee *temp;
    int total = 0;

    if (employees == NULL)
    {
        printf("No employees in the organization.\n");
        return;
    }

    printf("\n===== ORGANIZATION HIERARCHY =====\n");

    temp = employees;

    while (temp != NULL)
    {
        total++;
        temp = temp->nextEmployee;
    }

    temp = employees;

    while (temp != NULL)
    {
        if (temp->manager == NULL)
            displayHierarchy(temp, 0);

        temp = temp->nextEmployee;
    }

    printf("Total Employees: %d\n", total);
}

int main(void)
{
    int choice;

    do
    {
        printf("\n===== EMPLOYEE HIERARCHY MANAGEMENT =====\n");
        printf("1. Add Employee\n");
        printf("2. Remove Employee\n");
        printf("3. Display Employee Hierarchy\n");
        printf("4. Display Complete Hierarchy\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input.\n");
            return 1;
        }

        switch (choice)
        {
            case 1:
                addEmployee();
                break;

            case 2:
                removeEmployee();
                break;

            case 3:
                displayEmployeeHierarchy();
                break;

            case 4:
                displayCompleteHierarchy();
                break;

            case 5:
                printf("Exiting program.\n");
                break;

            default:
                printf("Invalid choice.\n");
        }

    } while (choice != 5);

    return 0;
}
