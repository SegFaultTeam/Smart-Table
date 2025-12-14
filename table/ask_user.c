///these functions ask user about frequency of sending notifications and frequency of stanging up
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

char *read_string(){
    size_t capacitanse = 5;
    size_t count = 0;
    char *string = malloc(capacitanse);
    if(!string) exit(EXIT_FAILURE);
    int c = getchar();
    while(c != '\n' && c != '\r' && c != EOF){
        if(capacitanse == count){
            capacitanse*=1.5;
            char *temp = realloc(string, capacitanse);
            if(!temp) exit(EXIT_FAILURE);
            string = temp;
        }
        putchar(c);
        string[count++] = c;
        c = getchar();
    }
    string[count] = '\0';
    return string;
}




char num_checkert(char *string){
    size_t count = 0;
    size_t count_dots = 0;
    for(int i = 0; string[i]; i++){
        if(!isdigit(string[i])){
            if(string[i] == '.' || string[i] == ','){
                if(count_dots == 1) return 'A'; //means abort/incorrect digit
                count_dots++;
            }else{
                return 'A';
            }

        } 
        if(count>9) return 'A';
        count++;
    }
    if(count_dots == 1){
        if(isdigit(string[0]) && isdigit(string[1]) && (string[2] == ',' || string[2] == '.')){
            string[2] = '.';
            float x = atof(string);
            if(x>50.0f){
                return 'h'; //mean to high
            }else if(x < 10.0f){
                return 'l'; //mean to low;
            }
            return 'f';
        }else{
            return 'A';
        }
    }else{
        int x = atoi(string);
        if(x < 10){
            return 'l';
        }
        else if(x>50){
            return 'h';
        }
        return 'i';
    }
}

char *minimum_temp(){
    printf("Please print minimum of comfort temperature(in celsius): ");
    char *buf = read_string();
    char check = num_checkert(buf);
    while(check == 'A' || check == 'h' || check == 'l'){
        free(buf);
        buf = NULL;
        switch(check){
            case('A'):
                printf("\nYour input is incorrect\n");
                break;
            case('h'):
                printf("\nYour input is too high🔥\n");
                break;
            case('l'):
                printf("\nYour input is too low❄\uFE0F\n");
                break;
        }
        printf("Please print minimum of comfort temperature(in celsius): ");
        buf = read_string();
        check = num_checkert(buf);
    }
    return buf;
}



bool num_checkerm(const char *nbuf){
    for(int i = 0; nbuf[i]; i++){
        if (!isdigit(nbuf[i])) return false;
        if(i>2) return false;
    }
    int x = atoi(nbuf);
    if(x > 360 || x < 2) return false;
    return true;
}

char *frequence_of_moving(){
    printf("\nHow often do you need to be reminded to move?\n");
    printf("Please write in minutes between 2 and 360 minutes:");
    char *nbuf = read_string();
    bool check = num_checkerm(nbuf);
    while(!check){
        free(nbuf);
        nbuf = NULL;
        printf("\nYour input was incorrect\n");
        printf("How often do you need to be reminded to move?\n");
        printf("Please write in minutes between 2 and 360 minutes:");
        nbuf = read_string();
        check = num_checkerm(nbuf);
    }
    return nbuf;
}
