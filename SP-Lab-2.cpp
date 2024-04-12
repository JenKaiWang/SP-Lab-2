#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_NAME_LEN 64
#define MAX_LINE_LEN 256
#include "readfile.c"
#include "readfile.h"

typedef struct {
    int id;
    char fname[MAX_NAME_LEN];
    char lname[MAX_NAME_LEN];
    int salary;
} Employee;

int compareByID(const void *a, const void *b) {
    const Employee *empA = static_cast<const Employee *>(a);
    const Employee *empB = static_cast<const Employee *>(b);
    return empA->id - empB->id;
}

void printDatabase(const char *filename) {
    if (open_file(filename) != 0) {
        perror("Error opening file");
        return;
    }

    int capacity = 10;
    int employeeCount = 0;
    Employee *employees = static_cast<Employee*>(malloc(capacity * sizeof(Employee)));
    if (employees == NULL) {
        perror("Memory allocation failed");
        close_file();
        return;
    }

    char buffer[MAX_LINE_LEN];
    while (read_string(buffer, MAX_LINE_LEN) == 0) {
        if (employeeCount >= capacity) {
            capacity *= 2;
            Employee *newArray = static_cast<Employee*>(realloc(employees, capacity * sizeof(Employee)));
            if (newArray == NULL) {
                perror("Memory reallocation failed");
                free(employees);
                close_file();
                return;
            }
            employees = newArray;
        }
        char *token = strtok(buffer, ",");
        if (token) {
            employees[employeeCount].id = atoi(token);
            token = strtok(NULL, ",");
            strncpy(employees[employeeCount].fname, token, MAX_NAME_LEN - 1);
            token = strtok(NULL, ",");
            strncpy(employees[employeeCount].lname, token, MAX_NAME_LEN - 1);
            token = strtok(NULL, ",");
            employees[employeeCount].salary = atoi(token);
            employees[employeeCount].fname[MAX_NAME_LEN - 1] = '\0';
            employees[employeeCount].lname[MAX_NAME_LEN - 1] = '\0';
            employeeCount++;
        }
    }
    close_file();

    qsort(employees, employeeCount, sizeof(Employee), compareByID);

    printf("\n%-10s %-20s %-20s %-10s\n", "ID", "FIRST NAME", "LAST NAME", "SALARY");
    printf("-------------------------------------------------------------\n");
    for (int i = 0; i < employeeCount; i++) {
        printf("%-10d %-20s %-20s %-10d\n", employees[i].id, employees[i].fname, employees[i].lname, employees[i].salary);
    }

    printf("Number of Employees: %d\n", employeeCount);
    free(employees);
}

void lookupByID(const char *filename) {
    int searchID, id, salary;
    char fname[MAX_NAME_LEN], lname[MAX_NAME_LEN];
    char buffer[MAX_LINE_LEN];
    
    // let user to enter the employee ID to search for
    printf("Enter a 6 digit employee id: ");
    scanf("%d", &searchID);
    
    // Open the specified file
    if (open_file(filename) != 0) {
        // Print an error message if the file cannot be opened
        perror("Error opening file");
        return;
    }

    int found = 0;
    while (read_string(buffer, MAX_LINE_LEN) == 0) {
        char* token = strtok(buffer, ",");
        if (token) {
            id = atoi(token);
            // Check if the current employee ID matches the search ID
            if (id == searchID) {
                token = strtok(NULL, ",");
                strcpy(fname, token);
                token = strtok(NULL, ",");
                strcpy(lname, token);
                token = strtok(NULL, ",");
                salary = atoi(token);

                // Create a full name with space between first name and last name
                char fullname[MAX_NAME_LEN * 2];
                snprintf(fullname, sizeof(fullname), "%s %s", fname, lname);

                printf("\n%-20s %-20d %-10d\n", fullname, salary, id);
                found = 1;
                break; // Break the loop once the record is found
            }
        }
    }
    // If the employee with the given ID is not found, print a message
    if (!found) {
        printf("Employee with ID %d not found.\n", searchID);
    }

    close_file();
}


void lookupByLastName(const char *filename) {
    char searchLastName[MAX_NAME_LEN];
    printf("Enter Employee's last name (no extra spaces): ");
    scanf("%63s", searchLastName);  

    // Open the file for reading
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char line[MAX_LINE_LEN];
    int found = 0;
    printf("\n%-20s %-20s %-10s\n", "NAME", "SALARY", "ID");
   // Read each line from the file
    while (fgets(line, sizeof(line), file)) {
        char fname[MAX_NAME_LEN], lname[MAX_NAME_LEN];
        int id, salary;

        if (sscanf(line, "%d,%63[^,],%63[^,],%d", &id, fname, lname, &salary) == 4) {
            if (strcmp(lname, searchLastName) == 0) {
                // Print employee information if the last name matches
                printf("%-20s %-20d %-10d\n", strcat(strcat(fname, " "), lname), salary, id);
                found = 1;
            }
        }
    }

    if (!found) {
        printf("Employee with Last Name '%s' not found.\n", searchLastName);
    }

    fclose(file);
}

int isAlphaString(const char *str) {
    while (*str) {
        // Check if the current character is not alphabetic
        if (!isalpha((unsigned char)*str)) {
            // Return 0 if the character is not alphabetic
            return 0;
        }
        str++;
    }
    // Return 1 if all characters are alphabetic
    return 1;
}

void getStringInput(const char *prompt, char *str, int maxLen) {
    int isValid;
    do {
        printf("%s", prompt);
        scanf("%63s", str);
        // Check if the input string contains only alphabetic characters
        isValid = isAlphaString(str);
        if (!isValid) {
            printf("Input must only contain letters. Please try again.\n");
        }
    } while (!isValid);
}

void addEmployee(const char *filename) {
    char fname[MAX_NAME_LEN], lname[MAX_NAME_LEN];
    int salary, confirm;
    int maxId = 0, id;

    // Open the file in append mode to add new records
    FILE *file = fopen(filename, "a+");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // Find the maximum ID in the existing database
    fseek(file, 0, SEEK_SET);
    char line[MAX_LINE_LEN];
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d", &id) == 1) {
            if (id > maxId) {
                maxId = id;
            }
        }
    }


    getStringInput("Enter the first name of the employee: ", fname, MAX_NAME_LEN);
    getStringInput("Enter the last name of the employee: ", lname, MAX_NAME_LEN);

	int inputResult;
    do {
        printf("Enter employee's salary (30000 to 150000): ");
        inputResult = scanf("%d", &salary);
        
        if (inputResult != 1 || salary < 30000 || salary > 150000) {
            printf("Salary must be an integer between 30000 and 150000. Please enter again.\n");
            // Clear the input buffer
            while (getchar() != '\n');
            inputResult = 0; 
        }
    } while (inputResult == 0);

    printf("Do you want to add the following employee to the DB?\n");
    printf("\t%s %s, salary: %d\n", fname, lname, salary);
    printf("Enter 1 for yes, 0 for no: ");
    scanf("%d", &confirm);

    if (confirm == 1) {
        fprintf(file, "\n%d,%s,%s,%d", maxId + 1, fname, lname, salary);
        printf("Employee added successfully.\n");
    }

    fclose(file); 
}

void removeEmployee(const char *filename) {
    int id, salary;
    char fname[MAX_NAME_LEN], lname[MAX_NAME_LEN];
    int searchID;
    printf("Enter the employee ID to remove: ");
    scanf("%d", &searchID);

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    // Dynamic array to hold employee records. Start small and grow as needed.
    int capacity = 10;
    int count = 0;
    Employee *employees = (Employee*)malloc(capacity * sizeof(Employee)); // Explicit cast needed for C++

    if (employees == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return;
    }

    // Read the file and fill the array
    while (fscanf(file, "%d,%63[^,],%63[^,],%d\n", &id, fname, lname, &salary) == 4) {
        if (count >= capacity) {
            capacity *= 2; // Double the capacity
            employees = (Employee*)realloc(employees, capacity * sizeof(Employee)); // Explicit cast needed for C++
            if (employees == NULL) {
                perror("Memory reallocation failed");
                fclose(file);
                return;
            }
        }
        employees[count].id = id; // Assign fields individually
        strcpy(employees[count].fname, fname);
        strcpy(employees[count].lname, lname);
        employees[count].salary = salary;
        count++;
    }
    fclose(file);

    int found = 0;
    for (int i = 0; i < count; i++) {
        if (employees[i].id == searchID) {
            found = 1;
            // Shift all subsequent employees back by one to remove the employee
            for (int j = i; j < count - 1; j++) {
                employees[j] = employees[j + 1];
            }
            count--; // Decrement count to reflect the removed employee
            break;
        }
    }

    if (!found) {
        printf("Employee with ID %d not found.\n", searchID);
        free(employees);
        return;
    }

    // Rewrite the file without the removed employee
    file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file");
        free(employees);
        return;
    }
    for (int i = 0; i < count; i++) {
        fprintf(file, "%d,%s,%s,%d\n", employees[i].id, employees[i].fname, employees[i].lname, employees[i].salary);
    }
    fclose(file);
    free(employees);
    printf("Employee removed successfully.\n");
}

void updateEmployee(const char *filename) {
    int searchID;
    printf("Enter the employee ID to update: ");
    scanf("%d", &searchID);

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    int found = 0, id, salary;
    char fname[MAX_NAME_LEN], lname[MAX_NAME_LEN];
    Employee *employees = NULL;
    int count = 0, capacity = 10;
    employees = (Employee*)malloc(capacity * sizeof(Employee));

    while (fscanf(file, "%d,%[^,],%[^,],%d\n", &id, fname, lname, &salary) == 4) {
        if (count >= capacity) {
            capacity *= 2;
            employees = (Employee*)realloc(employees, capacity * sizeof(Employee));
        }
	if (count < capacity) {
    employees[count].id = id;
    strcpy(employees[count].fname, fname);
    strcpy(employees[count].lname, lname);
    employees[count].salary = salary;
    count++;
	}
        if (id == searchID) found = 1;
    }
    fclose(file);

    if (!found) {
        printf("Employee with ID %d not found.\n", searchID);
        free(employees);
        return;
    }

    printf("Employee found. Choose field to update:\n");
    printf("1. First Name\n2. Last Name\n3. Salary\n4. All\nEnter choice: ");
    int choice;
    scanf("%d", &choice);
    getchar(); // consume newline

    for (int i = 0; i < count; i++) {
        if (employees[i].id == searchID) {
            switch(choice) {
                case 1:
                    printf("Enter new first name: ");
                    scanf("%63s", employees[i].fname);
                    break;
                case 2:
                    printf("Enter new last name: ");
                    scanf("%63s", employees[i].lname);
                    break;
                case 3:
                    printf("Enter new salary: ");
                    scanf("%d", &employees[i].salary);
                    break;
                case 4:
                    printf("Enter new first name: ");
                    scanf("%63s", employees[i].fname);
                    printf("Enter new last name: ");
                    scanf("%63s", employees[i].lname);
                    printf("Enter new salary: ");
                    scanf("%d", &employees[i].salary);
                    break;
                default:
                    printf("Invalid choice.\n");
            }
            printf("Employee updated successfully.\n");
            break;
        }
    }

    file = fopen(filename, "w");
    for (int i = 0; i < count; i++) {
        fprintf(file, "%d,%s,%s,%d\n", employees[i].id, employees[i].fname, employees[i].lname, employees[i].salary);
    }
    fclose(file);
    free(employees);
}

int compareSalary(const void *a, const void *b) {
    Employee *employeeA = (Employee *)a;
    Employee *employeeB = (Employee *)b;
    return employeeB->salary - employeeA->salary;
}

void printTopMSalaries(const char *filename) {
    int M;
    printf("Enter the number of employees (M) with the highest salaries you want to print: ");
    scanf("%d", &M);

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    int count = 0, capacity = 10;
    // 在C++中，必須顯式轉換malloc的返回類型
    Employee *employees = static_cast<Employee*>(malloc(capacity * sizeof(Employee)));
    if (employees == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return;
    }

    int id, salary;
    char fname[MAX_NAME_LEN], lname[MAX_NAME_LEN];
    while (fscanf(file, "%d,%63[^,],%63[^,],%d\n", &id, fname, lname, &salary) == 4) {
        if (count >= capacity) {
            capacity *= 2;
            // 在C++中，必須顯式轉換realloc的返回類型
            Employee *newEmployees = static_cast<Employee*>(realloc(employees, capacity * sizeof(Employee)));
            if (newEmployees == NULL) {
                perror("Memory reallocation failed");
                free(employees);
                fclose(file);
                return;
            }
            employees = newEmployees;
        }
        employees[count].id = id;
        strcpy(employees[count].fname, fname);
        strcpy(employees[count].lname, lname);
        employees[count].salary = salary;
        count++;
    }
    fclose(file);

    qsort(employees, count, sizeof(Employee), compareSalary);

    printf("\nTop %d Employees with the Highest Salaries:\n", M);
    for (int i = 0; i < M && i < count; i++) {
        printf("ID: %d, Name: %s %s, Salary: %d\n", employees[i].id, employees[i].fname, employees[i].lname, employees[i].salary);
    }

    free(employees);
}


void findEmployeesByFirstName(const char *filename) {
    char searchName[MAX_NAME_LEN];
    printf("Enter the first name to search for: ");
    scanf("%63s", searchName);

    // Convert the search name to lower case for case-insensitive comparison
    for (int i = 0; searchName[i]; i++) {
        searchName[i] = tolower((unsigned char)searchName[i]);
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    char line[MAX_LINE_LEN];
    int id, salary;
    char fname[MAX_NAME_LEN], lname[MAX_NAME_LEN];
    printf("\nEmployees with first name '%s':\n", searchName);
    while (fscanf(file, "%d,%63[^,],%63[^,],%d", &id, fname, lname, &salary) == 4) {
        // Convert each read first name to lower case to perform a case-insensitive comparison
        for (int i = 0; fname[i]; i++) {
            fname[i] = tolower((unsigned char)fname[i]);
        }

        if (strcmp(fname, searchName) == 0) {
            // If a match is found, print the employee's details
            printf("ID: %d, Name: %s %s, Salary: %d\n", id, fname, lname, salary);
        }
    }

    fclose(file);
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];
    int choice;

    do {
        printf("\nEmployee DB Menu:\n");
        printf("----------------------------\n");
        printf("(1) Print the Database\n");
        printf("(2) Lookup by ID\n");
        printf("(3) Lookup by Last Name\n");
        printf("(4) Add an Employee\n");
        printf("(5) Quit\n");
        printf("(6) remove\n");
        printf("(7) update\n");
        printf("(8) Print the M employees with the highest salaries\n");
        printf("(9) Find all employees with matching first name\n");
        printf("Enter your choice: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Please enter a valid number.\n");
            while (getchar() != '\n');
            continue;
        }
    	
        if (choice < 1 || choice > 9) {
            printf("Hey, %d is not between 1 and 9, try again...\n", choice);
            continue;
        }

        switch(choice) {
            case 1:
                printDatabase(filename);
                break;
            case 2:
                lookupByID(filename);
                break;
            case 3:
                lookupByLastName(filename);
                break;
            case 4:
                addEmployee(filename);
                break;
            case 5:
                printf("good bye!\n");
                break;
	    case 6: 
    		removeEmployee(filename);
    		printf("(6) Remove an Employee\n");
    		break;
	    case 7: 
    		updateEmployee(filename);
    		break;
	    case 8:
    		printTopMSalaries(filename);
   		break;
   	     case 9:
        	findEmployeesByFirstName(filename);
        	break;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    } while(choice != 5);

    return 0;
}

