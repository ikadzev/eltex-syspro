#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include "phonebook.h"

// Constants specific to main.c
#define MAXCOMMAND 256

// CLI function declarations
int startCLI(PhoneBook* book);
int mainLoop(PhoneBook* book);
void addCLI(char* args, PhoneBook* book);
void editCLI(char* args, PhoneBook* book);
void deleteCLI(char* args, PhoneBook* book);
void entryCLI(char* args, PhoneBook* book);

#endif // MAIN_H
