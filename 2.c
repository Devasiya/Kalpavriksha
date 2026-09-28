#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "users.txt"

struct User {
    int id;
    char name[50];
    int age;
};

void createFile() {
    FILE *file = fopen(FILE_NAME, "a");

    if (file == NULL) {
        printf("Error: Could not create/open file.\n");
        return;
    }

    fclose(file);
}

void addUser() {
    struct User user;

    FILE *file = fopen(FILE_NAME, "a");

    if (file == NULL) {
        printf("Error: Could not open file.\n");
        return;
    }

    printf("Enter ID: ");
    scanf("%d", &user.id);

    printf("Enter Name: ");
    scanf(" %[^\n]", user.name);

    printf("Enter Age: ");
    scanf("%d", &user.age);

    fprintf(file, "%d|%s|%d\n", user.id, user.name, user.age);

    fclose(file);

    printf("User added successfully.\n");
}

void readUsers() {
    struct User user;

    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL) {
        printf("No users found.\n");
        return;
    }

    printf("\n--- Users ---\n");
    while (fscanf(file, "%d|%49[^|]|%d\n",
                  &user.id, user.name, &user.age) == 3) {
        printf("ID: %d | Name: %s | Age: %d\n",
               user.id, user.name, user.age);
    }
    fclose(file);
}

void updateUser() {
    struct User user;
    int searchId;
    int found = 0;

    FILE *file = fopen(FILE_NAME, "r");
    FILE *temp = fopen("temp.txt", "w");

    if (file == NULL) {
        printf("No users found.\n");
        return;
    }

    if (temp == NULL) {
        printf("Error: Could not create temporary file.\n");
        fclose(file);
        return;
    }

    printf("Enter ID to update: ");
    scanf("%d", &searchId);

    while (fscanf(file, "%d|%49[^|]|%d\n",
                  &user.id, user.name, &user.age) == 3) {

        if (user.id == searchId) {
            found = 1;

            printf("Enter new Name: ");
            scanf(" %[^\n]", user.name);

            printf("Enter new Age: ");
            scanf("%d", &user.age);
        }

        fprintf(temp, "%d|%s|%d\n",
                user.id, user.name, user.age);
    }

    fclose(file);
    fclose(temp);

    remove(FILE_NAME);
    rename("temp.txt", FILE_NAME);

    if (found) {
        printf("User updated successfully.\n");
    } else {
        printf("User with ID %d not found.\n", searchId);
    }
}

void deleteUser() {
    struct User user;
    int deleteId;
    int found = 0;

    FILE *file = fopen(FILE_NAME, "r");
    FILE *temp = fopen("temp.txt", "w");

    if (file == NULL) {
        printf("No users found.\n");
        return;
    }

    if (temp == NULL) {
        printf("Error: Could not create temporary file.\n");
        fclose(file);
        return;
    }

    printf("Enter ID to delete: ");
    scanf("%d", &deleteId);

    while (fscanf(file, "%d|%49[^|]|%d\n",
                  &user.id, user.name, &user.age) == 3) {

        if (user.id == deleteId) {
            found = 1;
            continue;
        }

        fprintf(temp, "%d|%s|%d\n",
                user.id, user.name, user.age);
    }

    fclose(file);
    fclose(temp);

    remove(FILE_NAME);
    rename("temp.txt", FILE_NAME);

    if (found) {
        printf("User deleted successfully.\n");
    } else {
        printf("User with ID %d not found.\n", deleteId);
    }
}

int main() {
    int choice;

    createFile();

    while (1) {
        printf("\n===== User Management System =====\n");
        printf("1. Create/Add User\n");
        printf("2. Read Users\n");
        printf("3. Update User\n");
        printf("4. Delete User\n");
        printf("5. Exit\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addUser();
                break;

            case 2:
                readUsers();
                break;

            case 3:
                updateUser();
                break;

            case 4:
                deleteUser();
                break;

            case 5:
                printf("Exiting program.\n");
                return 0;

            default:
                printf("Invalid choice.\n");
        }
    }

    return 0;
}