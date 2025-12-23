#include "phonebook.h"

int compare_entries(const Entry* a, const Entry* b) { return strcmp(a->second_name, b->second_name); }

TreeNode* insert_node(TreeNode* root, Entry* e) {
    if (!root) {
        TreeNode* n = malloc(sizeof(TreeNode));
        n->data = *e;
        n->left = n->right = NULL;
        return n;
    }

    if (compare_entries(e, &root->data) < 0)
        root->left = insert_node(root->left, e);
    else
        root->right = insert_node(root->right, e);

    return root;
}

TreeNode* find_by_ident(TreeNode* root, int ident) {
    if (!root) return NULL;
    if (root->data.ident == ident) return root;

    TreeNode* l = find_by_ident(root->left, ident);
    if (l) return l;

    return find_by_ident(root->right, ident);
}

TreeNode* find_min(TreeNode* n) {
    while (n->left) n = n->left;
    return n;
}

TreeNode* delete_node(TreeNode* root, int ident) {
    if (!root) return NULL;

    if (root->data.ident == ident) {
        if (!root->left) {
            TreeNode* r = root->right;
            free(root);
            return r;
        }
        if (!root->right) {
            TreeNode* l = root->left;
            free(root);
            return l;
        }

        TreeNode* min = find_min(root->right);
        root->data = min->data;
        root->right = delete_node(root->right, min->data.ident);
        
        return root;
    }

    root->left = delete_node(root->left, ident);
    root->right = delete_node(root->right, ident);
    return root;
}

void search_tree(TreeNode* root, const char* filter, int* found, int* count, int max) {
    if (!root || *count >= max) return;

    search_tree(root->left, filter, found, count, max);

    Entry* e = &root->data;
    if (filter == NULL || strstr(e->name, filter) || strstr(e->second_name, filter) ||
            strstr(e->last_name, filter) || strstr(e->job, filter) || strstr(e->job_post, filter)) {
            found[(*count)++] = e->ident;
        }

    search_tree(root->right, filter, found, count, max);
}

int height(TreeNode* n) {
    if (!n) return 0;
    int l = height(n->left);
    int r = height(n->right);
    return (l > r ? l : r) + 1;
}

void tree_to_array(TreeNode* root, Entry* arr, int* idx) {
    if (!root) return;
    tree_to_array(root->left, arr, idx);
    arr[(*idx)++] = root->data;
    tree_to_array(root->right, arr, idx);
}

TreeNode* build_balanced(Entry* arr, int l, int r) {
    if (l > r) return NULL;
    int m = (l + r) / 2;

    TreeNode* n = malloc(sizeof(TreeNode));
    n->data = arr[m];
    n->left = build_balanced(arr, l, m - 1);
    n->right = build_balanced(arr, m + 1, r);
    return n;
}

int entryPB(PhoneBook* book, const char* filter, int* found_indices, int max_found) {
    int found = 0;
    search_tree(book->root, filter, found_indices, &found, max_found);
    return found;
}

int addPB(PhoneBook* book, const char* name, const char* surname) {
    Entry e = {0};
    e.ident = book->next_ident++;
    strcpy(e.name, name);
    strcpy(e.second_name, surname);
    book->root = insert_node(book->root, &e);
    book->count++;
    if (book->count % 10 == 0) {
        Entry* arr = malloc(sizeof(Entry) * book->count);
        int idx = 0;
        tree_to_array(book->root, arr, &idx);
        book->root = build_balanced(arr, 0, book->count - 1);
        free(arr);
    }
    return e.ident;
}

int deletePB(PhoneBook* book, int ident) {
    TreeNode* node = find_by_ident(book->root, ident);
    if (!node) return NO_ENTRY;
    
    book->root = delete_node(book->root, ident);
    book->count--;
    return ident;
}

int editPB(PhoneBook* book, int ident, char* args, ...) {
    TreeNode* node = find_by_ident(book->root, ident);
    if (!node) return NO_ENTRY;

    Entry* profile = &node->data;
    va_list argument_list;
    va_start(argument_list, args);

    while (*args) {
        switch (*args++) {
        case 'n': strcpy(profile->name, va_arg(argument_list, char*)); break;
        case 'l': strcpy(profile->last_name, va_arg(argument_list, char*)); break;
        case 'j': strcpy(profile->job, va_arg(argument_list, char*)); break;
        case 'p': strcpy(profile->job_post, va_arg(argument_list, char*)); break;
        case 'e':
            if (profile->emails_count == MAXEMAIL) return OVERFLOW_E;
            strcpy(profile->emails[profile->emails_count++], va_arg(argument_list, char*));
            break;
        case 'f':
            if (profile->phones_count == MAXPHONE) return OVERFLOW_P;
            strcpy(profile->phones[profile->phones_count++], va_arg(argument_list, char*));
            break;
        case 't': strcpy(profile->profiles.TG, va_arg(argument_list, char*)); break;
        case 'w': strcpy(profile->profiles.WA, va_arg(argument_list, char*)); break;
        case 'i': strcpy(profile->profiles.LI, va_arg(argument_list, char*)); break;
        case 'g': strcpy(profile->profiles.IG, va_arg(argument_list, char*)); break;
        case 'E':
            if (profile->emails_count <= 0) {
                profile->emails_count = 0;
                return OVERFLOW_E;
            }
            profile->emails_count--;
            break;
        case 'F':
            if (profile->phones_count <= 0) {
                profile->phones_count = 0;
                return OVERFLOW_P;
            }
            profile->phones_count--;
            break;
        default:
            return UNKNOWN_EXPRESSION;
        }
    }
    return ident;
}

