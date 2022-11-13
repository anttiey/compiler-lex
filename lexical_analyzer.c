#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_LINE 256
#define MAX_NUM 1024
#define OPERATOR 1
#define ASSIGN 2
#define PLUS 3
#define MINUS 4
#define MULTI 5
#define DIVISION 6
#define COLON 8
#define SEMICOLON 9
#define QUOTE 10
#define BLANK 11

struct value_table
{
    int table_index;
    char values[MAX_LINE][MAX_NUM];
} value_table;

int isDelimiter(char c)
{
    if ((c == '.' || isalnum(c))) { return 0; } 
    return 1;
}

int delimiterType(char c) {

    if (c == ' ' || c == '\t' || c == '\n' || c == '\r') { return BLANK; }
    if (c == '=' || c == '+' || c == '-' || c == '*' || c == '/' || c == ':') { return OPERATOR; }
    if (c == '"') { return QUOTE; }
    if (c == ';') { return SEMICOLON; }

    return 0;
}

int isInteger(char *s) {
    while (*s) {
        if (!isdigit(*s)) {
            return 0;
        }
        s++;
    }
    return 1;
}

int isDouble(char *s) {
    while (*s) {
        if ((!isdigit(*s) && *s != '.')) {
            return 0;
        }
        s++;
    }
    return 1;
}

int isValid(char *s) {

    int digitCount = 0;
    int errorCount = 0;

    while (*s) {

        if (isdigit(*s)) { 
            digitCount++;
        }

        if (isalpha(*s)) {
            if (digitCount > 0) {
                return errorCount;
            }
        }

        errorCount++;
        s++;
    }

    return 0;
}

int operatorType(char c) {
    if(c == '=') { return ASSIGN; }
    if(c == '+') { return PLUS; }
    if(c == '-') { return MINUS; }
    if(c == '*') { return MULTI; }
    if(c == '/') { return DIVISION; }
    if(c == ':') { return COLON; }

    return 0;
}

void printOperator(char c) {

    int type = operatorType(c);

    switch(type) {
        case ASSIGN:
            printf("<ASSIGN, >\t %c\n", c);
            break;
        case PLUS:
            printf("<PLUS, >\t %c\n", c);
            break;
        case MINUS:
            printf("<MINUS, >\t %c\n", c);
            break;
        case MULTI:
            printf("<MULTI, >\t %c\n", c);
            break;
        case DIVISION:
            printf("<DIVISION, >\t %c\n", c);
            break;
        case COLON:
            printf("<COLON, >\t %c\n", c);
            break;
    }

}

void table_init(struct value_table *table) {
    for(int i = 0; i < MAX_LINE; i++) {
        table->values[i][0] = 0;
    }
    table->table_index = 0;
}

int table_add(struct value_table *table, char *value) {

    for(int i = 1; i <= table->table_index; i++) {
        if (strcmp(value, table->values[i]) == 0) {
            return i;
        }
    }

    table->table_index++;
    strcpy(table->values[table->table_index], value);

    return table->table_index;

}

void table_print(struct value_table *table) {

    for(int i = 1; i <= table->table_index; i++) {
        printf("%d\t\t %s\n", i, table->values[i]);
    }

}


int main(int argc, char *argv[])
{

    // FILE *input = fopen("ex1.txt", "r");
    // if (input == NULL)
    // {
    //     perror("ERROR");
    //     exit(EXIT_FAILURE);
    // }
    
    printf("===========================\n");
    printf("\tTOKEN LIST\n");
    printf("===========================\n");

    char line[MAX_LINE];
    char *str;
    char token[MAX_LINE] = "";

    int lineCount = 1;

    int resultCount = 0;
    int quoteCount = 0;

    struct value_table symbol_table;
    struct value_table string_table;

    table_init(&symbol_table);
    table_init(&string_table);

    while ((str = fgets(line, 1024, stdin)) != NULL)
    {
        int len = (int)strlen(str);

        for (int i = 0; i < len; i++)
        {
            
            if (isDelimiter(str[i]) != 0) {

                if (delimiterType(str[i]) == QUOTE) {

                    char string_token[MAX_LINE] = "";

                    quoteCount = 1;
                    strncat(token, &str[i], 1);

                    for (int j = i + 1; j < len; j++) {
                        if (delimiterType(str[j]) == QUOTE) {
                            quoteCount = 0;
                            strncat(token, &str[i], 1);
                            i = j;
                            break;
                        }

                        if(j == len - 1 && (delimiterType(str[j]) == BLANK)) { continue; }

                        strncat(string_token, &str[j], 1);
                        strncat(token, &str[j], 1);
                    }

                    if(quoteCount) {
                        printf("Error: Line %d\t %s\n", lineCount, token);
                    } else {
                        int string_index = table_add(&string_table, string_token);
                        printf("<STRING, %d>\t %s\n", string_index, token);
                    }

                    strcpy(token, "");
                    continue;

                }

                if(strlen(token) > 0) {

                    if(isInteger(token)) {
                        if(strlen(token) > 1 && token[0] == '0') {
                            printf("Error: Line %d\t %s\n", lineCount, token);
                        } else {
                            printf("<INT, %s>\t %s\n", token,  token);
                        }
                    }
                    else if(isDouble(token)) {
                        printf("<DOUBLE, %s>\t %s\n", token, token);
                    }
                    else {

                        while(1) {

                            int error = isValid(token);
                            if(!error) { break; }

                            char tmp[MAX_LINE];
                            strncpy(tmp, token, error);
                            printf("Error: Line %d\t %s\n", lineCount, tmp);

                            strcpy(token, token + error);

                        }

                        if (strlen(token) > 10) {
                            token[10] = 0;
                        }

                        int symbol_index = table_add(&symbol_table, token);

                        if (symbol_index < 10) {
                            printf("<ID, %d>\t\t %s\n", symbol_index, token);
                        } else {
                            printf("<ID, %d>\t %s\n", symbol_index, token);
                        }

                    }

                }

                if (!delimiterType(str[i])) {
                    printf("Error: Line %d\t %c\n", lineCount, str[i]);
                } else if(delimiterType(str[i]) == SEMICOLON) {
                    printf("<SEMICOLON, >\t %c\n", str[i]);
                } else if(delimiterType(str[i]) != BLANK) {
                    printOperator(str[i]);
                }

                strcpy(token, "");
                continue;

            }

            strncat(token, &str[i], 1);

        }

        lineCount++;

    }

    printf("\n===========================\n");
    printf("\tSYMBOL TABLE\n");
    printf("===========================\n");
    printf("INDEX\t\t SYMBOL\n");

    table_print(&symbol_table);

    printf("\n===========================\n");
    printf("\tSTRING TABLE\n");
    printf("===========================\n");
    printf("INDEX\t\t STRING\n");

    table_print(&string_table);

}