#include <stdio.h>
#include <stdlib.h> 

struct Person {
    char *name;
    int age;
    int role; // 1: Student, 2: Teacher
};

int main()
{
    struct Person *person;
    struct Person staff;
    char* role_name;

    staff.name = "Xiao Ming";
    staff.age = 35;
    staff.role = 2;
    switch(staff.role) {
        case 1:
            role_name = "Student";
            break;
        case 2:
            role_name = "Teacher";
            break;
        default:
            role_name = "Unknown";
    }
    printf("=== Staff %s Role: %s\n", staff.name, role_name);

    person = malloc(sizeof(struct Person));
    person->name = "John Doe";
    person->age = 28;
    person->role = 1;
    switch(person->role) {
        case 1:
            role_name = "Student";
            break;
        case 2:
            role_name = "Teacher";
            break;
        default:
            role_name = "Unknown";
    }
    printf("=== Person %s Role: %s\n", person->name, role_name);

    free(person);
    return 0;
}