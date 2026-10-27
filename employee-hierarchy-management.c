#include <stdio.h>
#include <stdlib.h>

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

/* Find an employee using their ID */
Employee *findEmployee(int id)
{
    Employee *current = employees;

    while (current != NULL)
    {
        if (current->id == id)
            return current;

        current = current->nextEmployee;
    }

    return NULL;
}

/* Add a new employee */
void addEmployee(void)
{
    Employee *newEmployee;
    Employee *manager;
    Employee *last;

    int managerId;

    newEmployee = malloc(sizeof(Employee));

    if (newEmployee == NULL)
    {
        printf("Memory allocation failed.\n");
        return;
    }

    printf("Enter Employee ID: ");
    if (scanf("%d", &newEmployee->id) != 1)
    {
        printf("Invalid employee ID.\n");
        free(newEmployee);
        while (getchar() != '\n');
        return;
    }

    if (findEmployee(newEmployee->id) != NULL)
    {
        printf("Employee ID already exists.\n");
        free(newEmployee);
        return;
    }

    printf("Enter Employee Name: ");
    scanf(" %99[^\n]", newEmployee->name);

    printf("Enter Designation: ");
    scanf(" %99[^\n]", newEmployee->designation);

    newEmployee->manager = NULL;
    newEmployee->firstReport = NULL;
    newEmployee->nextReport = NULL;
    newEmployee->nextEmployee = NULL;

    while (1)
    {
        printf("Enter Reporting Manager ID (0 for no manager): ");

        if (scanf("%d", &managerId) != 1)
        {
            printf("Invalid manager ID. Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }

        if (managerId == 0)
        {
            manager = NULL;
            break;
        }

        manager = findEmployee(managerId);

        if (manager == NULL)
        {
            printf("Manager not found. Please enter a valid ID.\n");
            continue;
        }

        if (managerId == newEmployee->id)
        {
            printf("An employee cannot be their own manager.\n");
            continue;
        }

        break;
    }

    newEmployee->manager = manager;

    /* Add employee to the main employee list */
    if (employees == NULL)
    {
        employees = newEmployee;
    }
    else
    {
        last = employees;

        while (last->nextEmployee != NULL)
            last = last->nextEmployee;

        last->nextEmployee = newEmployee;
    }

    /* Add employee to their manager's reporting list */
    if (manager != NULL)
    {
        if (manager->firstReport == NULL)
        {
            manager->firstReport = newEmployee;
        }
        else
        {
            last = manager->firstReport;

            while (last->nextReport != NULL)
                last = last->nextReport;

            last->nextReport = newEmployee;
        }
    }

    printf("Employee added successfully!\n");
}

/* Count all employees reporting under an employee */
int countEmployees(Employee *employee)
{
    Employee *current;
    int count = 0;

    if (employee == NULL)
        return 0;

    current = employee->firstReport;

    while (current != NULL)
    {
        count++;
        count += countEmployees(current);
        current = current->nextReport;
    }

    return count;
}

/* Display an employee and their reporting hierarchy */
void displayHierarchy(Employee *employee, int level)
{
    Employee *current;
    int i;

    if (employee == NULL)
        return;

    for (i = 0; i < level; i++)
        printf("    ");

    printf("ID: %d | Name: %s | Designation: %s\n",
           employee->id,
           employee->name,
           employee->designation);

    current = employee->firstReport;

    while (current != NULL)
    {
        displayHierarchy(current, level + 1);
        current = current->nextReport;
    }
}

/* Display the hierarchy starting from one employee */
void displayEmployeeHierarchy(void)
{
    int id;
    Employee *employee;

    printf("Enter Employee ID to display: ");

    if (scanf("%d", &id) != 1)
    {
        printf("Invalid employee ID.\n");
        while (getchar() != '\n');
        return;
    }

    employee = findEmployee(id);

    if (employee == NULL)
    {
        printf("Employee not found.\n");
        return;
    }

    displayHierarchy(employee, 0);
}

/* Display the complete organization hierarchy */
void displayCompleteHierarchy(void)
{
    Employee *current = employees;
    int foundRoot = 0;

    if (employees == NULL)
    {
        printf("No employees available.\n");
        return;
    }

    while (current != NULL)
    {
        if (current->manager == NULL)
        {
            displayHierarchy(current, 0);
            foundRoot = 1;
        }

        current = current->nextEmployee;
    }

    if (!foundRoot)
        printf("No top-level employee found.\n");
}

/* Remove an employee and transfer their reports */
void removeEmployee(void)
{
    int id;
    Employee *emp;
    Employee *manager;
    Employee *sibling;
    Employee *bestSibling = NULL;
    Employee *current;
    Employee *previous;
    Employee *child;
    Employee *last;
    Employee *target;

    int directReports;
    int minimumReports = 0;

    printf("Enter Employee ID to remove: ");

    if (scanf("%d", &id) != 1)
    {
        printf("Invalid employee ID.\n");
        while (getchar() != '\n');
        return;
    }

    emp = findEmployee(id);

    if (emp == NULL)
    {
        printf("Employee not found.\n");
        return;
    }

    manager = emp->manager;

    /* Find the sibling with the fewest direct reports */
    if (manager != NULL)
    {
        sibling = manager->firstReport;

        while (sibling != NULL)
        {
            if (sibling != emp)
            {
                directReports = 0;
                child = sibling->firstReport;

                while (child != NULL)
                {
                    directReports++;
                    child = child->nextReport;
                }

                if (bestSibling == NULL || directReports < minimumReports)
                {
                    bestSibling = sibling;
                    minimumReports = directReports;
                }
            }

            sibling = sibling->nextReport;
        }

        /* Remove employee from manager's reporting list */
        if (manager->firstReport == emp)
        {
            manager->firstReport = emp->nextReport;
        }
        else
        {
            previous = manager->firstReport;

            while (previous != NULL &&
                   previous->nextReport != emp)
            {
                previous = previous->nextReport;
            }

            if (previous != NULL)
                previous->nextReport = emp->nextReport;
        }
    }

    /* Use the sibling with the fewest reports.
       If no sibling exists, use the original manager. */
    target = (bestSibling != NULL) ? bestSibling : manager;

    /* Transfer the removed employee's direct reports */
    child = emp->firstReport;

    while (child != NULL)
    {
        Employee *nextChild = child->nextReport;

        child->manager = target;
        child->nextReport = NULL;

        if (target != NULL)
        {
            if (target->firstReport == NULL)
            {
                target->firstReport = child;
            }
            else
            {
                last = target->firstReport;

                while (last->nextReport != NULL)
                    last = last->nextReport;

                last->nextReport = child;
            }
        }
        else
        {
            /* No manager or sibling: make the child top-level */
            child->manager = NULL;
        }

        child = nextChild;
    }

    /* Remove employee from the main employee list */
    if (employees == emp)
    {
        employees = emp->nextEmployee;
    }
    else
    {
        previous = employees;

        while (previous != NULL &&
               previous->nextEmployee != emp)
        {
            previous = previous->nextEmployee;
        }

        if (previous != NULL)
            previous->nextEmployee = emp->nextEmployee;
    }

    free(emp);

    printf("Employee removed successfully!\n");
}

/* Count reports under a selected employee */
void displayEmployeeCount(void)
{
    int id;
    Employee *employee;

    printf("Enter Employee ID to count reports: ");

    if (scanf("%d", &id) != 1)
    {
        printf("Invalid employee ID.\n");
        while (getchar() != '\n');
        return;
    }

    employee = findEmployee(id);

    if (employee == NULL)
    {
        printf("Employee not found.\n");
        return;
    }

    printf("Total Employees Under %d: %d\n",
           id, countEmployees(employee));
}

/* Main menu */
int main(void)
{
    int choice;
    Employee *current;
    Employee *next;

    do
    {
        printf("\n===== Employee Hierarchy Management =====\n");
        printf("1. Add Employee\n");
        printf("2. Remove Employee\n");
        printf("3. Display Employee Hierarchy\n");
        printf("4. Display Complete Hierarchy\n");
        printf("5. Count Employees Under an Employee\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid choice. Please enter a number.\n");
            while (getchar() != '\n');
            continue;
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
                displayEmployeeCount();
                break;

            case 6:
                printf("Exiting program...\n");
                break;

            default:
                printf("Invalid choice. Please try again.\n");
        }

    } while (choice != 6);

    /* Free all remaining employees */
    current = employees;

    while (current != NULL)
    {
        next = current->nextEmployee;
        free(current);
        current = next;
    }

    return 0;
}