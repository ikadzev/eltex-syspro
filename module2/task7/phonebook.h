#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#define MAXSTRING 100
#define MAXEMAIL  10
#define MAXPHONE  10
#define MAXENTRY  50

#define OVERFLOW             -1
#define OVERFLOW_E           -2
#define OVERFLOW_P           -3
#define NO_ENTRY             -4
#define UNKNOWN_EXPRESSION   -5

typedef struct IM {
    char TG[MAXSTRING];
    char WA[MAXSTRING];
    char LI[MAXSTRING];
    char IG[MAXSTRING];
} IM;

typedef struct Entry {
    int ident;
    char name[MAXSTRING];
    char second_name[MAXSTRING];
    char last_name[MAXSTRING];
    char job[MAXSTRING];
    char job_post[MAXSTRING];
    char emails[MAXEMAIL][MAXSTRING];
    int  emails_count;
    char phones[MAXPHONE][12];
    int  phones_count;
    IM profiles;
} Entry;

typedef struct TreeNode {
    Entry data;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

typedef struct PhoneBook {
    int count;
    int next_ident;
    TreeNode* root;
} PhoneBook;

void initPB(PhoneBook* book);
int addPB(PhoneBook* book, const char* name, const char* surname);
int deletePB(PhoneBook* book, int ident);
int editPB(PhoneBook* book, int ident, char* args, ...);
int entryPB(PhoneBook* book, const char* filter, int* found_indices, int max_found);

Entry* get_entryPB(PhoneBook* book, int ident);
void rebalancePB(PhoneBook* book);
void freePB(PhoneBook* book);
int compare_entries(const Entry* a, const Entry* b);// { return strcmp(a->last_name, b->last_name); }
TreeNode* insert_node(TreeNode* root, Entry* e);
TreeNode* find_by_ident(TreeNode* root, int ident);
TreeNode* delete_node(TreeNode* root, int ident);
TreeNode* find_min(TreeNode* node);
void search_tree(TreeNode* root, const char* filter, int* found, int* count, int max);
int height(TreeNode* node);
int is_unbalanced(TreeNode* root);
void tree_to_array(TreeNode* root, Entry* arr, int* index);
TreeNode* build_balanced(Entry* arr, int left, int right);

#endif
