#include <stdio.h>
#include <ctype.h>

int main() {
    char s[1000];
    int i = 0;
    int num;
    int result = 0;
    int term = 0;
    char op = '+';
    
    printf("Enter expression: ");
    fgets(s, sizeof(s), stdin);

    while (s[i] != '\0') {
        if (isspace(s[i])) {
            i++;
            continue;
        }
        if (isdigit(s[i])) {
            num = 0;
            while (isdigit(s[i])) {
                num = num * 10 + (s[i] - '0');
                i++;
            }
            if (op == '+') {
                result = result + term;
                term = num;
            }
            else if (op == '-') {
                result = result + term;
                term = -num;
            }
            else if (op == '*') {
                term = term * num;
            }
            else if (op == '/') {
                if (num == 0) {
                    printf("Error: Division by zero.\n");
                    return 0;
                }
                term = term / num;
            }
            else {
                printf("Error: Invalid expression.\n");
                return 0;
            }
        }else if (s[i] == '+' || s[i] == '-' ||
                 s[i] == '*' || s[i] == '/') {

            op = s[i];
            i++;
        }else {
            printf("Error: Invalid expression.\n");
            return 0;
        }
    }

    result = result + term;
    printf("%d\n", result);
    return 0;
}
