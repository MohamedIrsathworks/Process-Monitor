#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#define USERNAME "Mohamed"
#define PASSWORD "@Irsath06"

struct ProcessNode {
int pid;
char name[256];
char status[256];
struct ProcessNode* left;
struct ProcessNode* right;
};

void add_process_to_bst(struct ProcessNode** root, int pid, const char* name, const char* status) {
if (*root == NULL) {
struct ProcessNode* newProcess = (struct ProcessNode*)malloc(sizeof(struct ProcessNode));
newProcess->pid = pid;
strncpy(newProcess->name, name, sizeof(newProcess->name) - 1);
newProcess->name[sizeof(newProcess->name) - 1] = '\0';
strncpy(newProcess->status, status, sizeof(newProcess->status) - 1);
newProcess->status[sizeof(newProcess->status) - 1] = '\0';
newProcess->left = newProcess->right = NULL;
*root = newProcess;
} else {
if (pid < (*root)->pid) {
add_process_to_bst(&(*root)->left, pid, name, status);

11

}
else {
add_process_to_bst(&(*root)->right, pid, name, status);
}
}
}

void search_process_by_name_in_bst(struct ProcessNode* root, const char* targetName) {
if (root != NULL) {
search_process_by_name_in_bst(root->left, targetName);
if (strstr(root->name, targetName) != NULL) {
printf("PID: %d, Name: %s, Status: %s\n", root->pid, root->name, root->status);
}
search_process_by_name_in_bst(root->right, targetName);
}
}

void kill_process_in_bst(struct ProcessNode** root, int dpid) {
if (*root == NULL) {
return;
}

if (dpid < (*root)->pid) {
kill_process_in_bst(&(*root)->left, dpid);
} else if (dpid > (*root)->pid) {
kill_process_in_bst(&(*root)->right, dpid);
} else {
struct ProcessNode* temp = *root;
if ((*root)->left == NULL) {
*root = (*root)->right;
} else if ((*root)->right == NULL) {
*root = (*root)->left;
} else {
struct ProcessNode* minRightSubtree = (*root)->right;

12

while (minRightSubtree->left != NULL) {
minRightSubtree = minRightSubtree->left;
}
(*root)->pid = minRightSubtree->pid;
strcpy((*root)->name, minRightSubtree->name);
strcpy((*root)->status, minRightSubtree->status);
kill_process_in_bst(&(*root)->right, minRightSubtree->pid);
}
free(temp);
}
}

void print_processes_in_bst(struct ProcessNode* root) {
if (root != NULL) {
print_processes_in_bst(root->left);
printf("PID: %d, Name: %s, Status: %s\n", root->pid, root->name, root->status);
print_processes_in_bst(root->right);
}
}

int create_process(char* cmd) {
pid_t pid = fork();
if (pid == 0) {
if (cmd != NULL) {
execlp(cmd, cmd, NULL);
exit(0);
}
} else if (pid > 0) {
printf("Process %d created successfully.\n", pid);
return pid;
} else {
printf("Error creating process.\n");
}
}

13

int authenticate_user() {
char username[256];
char password[256];
printf("Enter username: ");
scanf("%255s", username);
printf("Enter password: ");
scanf("%255s", password);
if (strcmp(username, USERNAME) == 0 && strcmp(password, PASSWORD) == 0) {
printf("Authentication successful!\n");
return 1;
} else {
printf("Authentication failed. Access denied.\n");
return 0;
}
}

void free_bst(struct ProcessNode* root) {
if (root != NULL) {
free_bst(root->left);
free_bst(root->right);
free(root);
}
}

int main() {
DIR* dir;
struct dirent* entry;
struct ProcessNode* bstRoot = NULL;
char cmd[256];
int dpid, newpid;

dir = opendir("/proc");
if (dir == NULL) {

14

perror("opendir");
exit(1);
}

while ((entry = readdir(dir)) != NULL) {
if (entry->d_type == DT_DIR) {
char path[300];
int pid = atoi(entry->d_name);
if (pid != 0) {
snprintf(path, sizeof(path), "/proc/%s/status", entry->d_name);
FILE* file = fopen(path, "r");
if (file) {
char line[256];
char* name = NULL;
char* status = NULL;
while (fgets(line, sizeof(line), file)) {
if (strstr(line, "Name:")) {
name = strdup(line + 6);
} else if (strstr(line, "State:")) {
status = strdup(line + 7);
}
}
if (name != NULL && status != NULL) {
add_process_to_bst(&bstRoot, pid, name, status);
}
free(name);
free(status);
fclose(file);
}
}
}
}
closedir(dir);

15

if (authenticate_user()) {
while (1) {
printf("\nMenu:\n");
printf("1. Create a new process (c)\n");
printf("2. Delete a process (d)\n");
printf("3. Display the list of processes (p)\n");
printf("4. Search a Process (s)\n");
printf("5. Quit (q)\n");
printf("Enter your choice:");

char choice;
scanf(" %c", &choice);

switch (choice) {
case 'c':
printf("Enter the command to run: ");
scanf("%255s", cmd);
newpid = create_process(cmd);
add_process_to_bst(&bstRoot, newpid, cmd, "Running");
break;
case 'd':
printf("Enter the PID of the process to delete: ");
scanf("%d", &dpid);
kill_process_in_bst(&bstRoot, dpid);
break;
case 'p':
print_processes_in_bst(bstRoot);
break;
case 's':
printf("Enter the name of the process to search: ");
char targetName[256];
scanf("%255s", targetName);
search_process_by_name_in_bst(bstRoot, targetName);
break;

16

case 'q':
free_bst(bstRoot);
return 0;
default:
printf("Invalid choice. Please enter 'c'/'d'/'p'/'s'/'q'\n");
}
}
}

free_bst(bstRoot);
return 0;
}