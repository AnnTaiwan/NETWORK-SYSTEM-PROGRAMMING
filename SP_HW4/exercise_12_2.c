#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h> // For strtol()
#include <dirent.h> // For opendir and readdir
#include <string.h>
#include <stdbool.h>  // For boolean types
#ifndef BUF_SIZE 
#define BUF_SIZE 1024
#endif

#define MAX_CMD_SIZE 256
typedef struct Process_Node
{
    int pid; // process id
    int ppid; // parent id
    int num_child;
    char command_name[MAX_CMD_SIZE];
    struct Process_Node *child[BUF_SIZE]; // point to several childs
}Process_Node;

Process_Node *list; // record all the processes
int num_process = 0; // record total number of processes
int *tree_status; // record the status if existing in tree or not, if data is not zero , then it means it exists
Process_Node root; // root node points to child whose ppid is 0, in my environment exists systemd and kthreadd 


// Error handling function: Prints error message and exits the program
static void errExit(const char *msg) {
    perror(msg);  // Print a system error message corresponding to the last error
    exit(EXIT_FAILURE);  // Exit the program with a failure status
} 

void record_all_the_process()
{
    DIR *proc_dir; // needed for opendir, use this directory stream handle pointer to let the readdir can access this dir
    struct dirent *entry; // needed for readdir, which will return a pointer pointing to d_type, d_name,...

    // Open /proc directory
    proc_dir = opendir("/proc");
    if (proc_dir == NULL) {
        free(list);
        errExit("opendir /proc");
    }

    // Read each entry in /proc
    while ((entry = readdir(proc_dir)) != NULL) 
    {
        // Each entry in /proc corresponding to a PID is numeric
        if (entry->d_type == DT_DIR && atoi(entry->d_name) > 0) 
        { // if entry->d_name isn't a numeric string, atoi will return 0
            char path[270], line[256];
            // prepare this process's status file path
            snprintf(path, sizeof(path), "/proc/%s/status", entry->d_name);
            
            FILE *status_file = fopen(path, "r");
            if (status_file) // reading this status file
            {
                char process_name[256] = {0}; 
                char str_ppid[10] = {0};
                int ppid = -1;
                while (fgets(line, sizeof(line), status_file)) 
                {
                    if (strncmp(line, "Name:", 5) == 0) // found the command name
                    {
                        sscanf(line, "Name:\t%s", process_name);
                    } 
                    if (strncmp(line, "PPid:", 5) == 0) // found the ppid
                    {
                        sscanf(line, "PPid:\t%s", str_ppid);
                        ppid = atoi(str_ppid); // save it as integer
                    }
                    if (ppid != -1 && strlen(process_name) > 0)
                    {
                        break;  // We have what we need
                    }
                }
                fclose(status_file);
                // record it into list
                if(num_process < BUF_SIZE) // not exceed the max size
                {
                    // initialize this node
                    list[num_process].pid = atoi(entry->d_name);
                    list[num_process].ppid = ppid;
                    list[num_process].num_child = 0;
                    strcpy(list[num_process].command_name,  process_name);
                    num_process++;
                }
                else
                {
                    if ((list = (Process_Node *)realloc(list, (num_process + BUF_SIZE) * sizeof(Process_Node))) != NULL ) // Resize array
	            {
		        // initialize this node
                        list[num_process].pid = atoi(entry->d_name);
                        list[num_process].ppid = ppid;
                        list[num_process].num_child = 0;
                        strcpy(list[num_process].command_name,  process_name);
                        num_process++;
	            }
	            else 
	            {   /* realloc error */
	                free(list);
		        errExit("Out of storage");
	            }
                }
                
            }
            // if status_file == NULL, this file may already disappear, so don't access this file
        }
    }
    closedir(proc_dir);
}

int node_exist_in_tree(int pid)
{
    for(int i = 0; i < num_process; i++)
    {
        if(tree_status[i] == pid) // found it
          return i;
    }
    return -1;
}

void connect(int c_idx, int ppid) // c_idx is the current child node's idx in list, and ppid is this child's ppid
{
    if(ppid == 0) // c_idx node is near root
    {
        tree_status[c_idx] = list[c_idx].pid; // updat root's tree_status
        root.child[root.num_child] = &list[c_idx]; // let root point to child
        root.num_child++;
        return;
    }
    int p_idx = node_exist_in_tree(ppid);
    if(p_idx != -1) // parent already exists in the tree
    {
        if (list[p_idx].num_child < BUF_SIZE) 
        {
            tree_status[c_idx] = list[c_idx].pid; // updat child's tree_status
            list[p_idx].child[list[p_idx].num_child] = &list[c_idx]; // let parent point to child
            list[p_idx].num_child++;
        } 
        else 
        {
            fprintf(stderr, "Error: Exceeded maximum child processes for PID: %d\n", list[p_idx].pid);
        }
        
    }
    else // parent not in the tree, so have to look up until reaching one node which already is in tree
    {
        for(int i = 0; i < num_process; i++)
        { // find the parent's idx in list
            if(list[i].pid == ppid) // find it
            {
               p_idx = i;
               break;
            }
        }
        // connect parent and child
        tree_status[c_idx] = list[c_idx].pid; // updat child's tree_status
        list[p_idx].child[list[p_idx].num_child] = &list[c_idx]; // let parent point to child
        list[p_idx].num_child++;
        connect(p_idx, list[p_idx].ppid);
    }
}

void build_tree() // start to build a tree from the list
{
    int temp;
    for(int i = 0; i < num_process; i++) 
    {
        temp = node_exist_in_tree(list[i].pid);
        if(temp != -1) // this node is already in the tree
            continue;
        else
            connect(i, list[i].ppid); // start to find the parent to move this child node into tree
    }
}
void print_tree(Process_Node *node, int level, bool is_last, char *prefix) {
    // Print current node with the appropriate formatting
    if (level > 0) 
    {
        printf("%s", prefix);

        // Print └─ or ├─ based on whether the node is the last child
        if (is_last)
            printf("└─");
        else
            printf("├─");
    }

    // Print the process ID and name
    printf("%s(%d)\n", node->command_name, node->pid);

    // Prepare prefix for the next level
    if (level > 0) 
    {
        char temp[BUF_SIZE];
        snprintf(temp, sizeof(temp), "%s%s", prefix, is_last ? "    " : "│   ");

        // Recursively print each child process
        for (int i = 0; i < node->num_child; i++) 
        {
            bool is_last_child = (i == node->num_child - 1);
            print_tree(node->child[i], level + 1, is_last_child, temp);
        }
    } 
    else 
    {
        // If we are at the root, recursively print each child
        for (int i = 0; i < node->num_child; i++) 
        {
            bool is_last_child = (i == node->num_child - 1);
            char temp[BUF_SIZE];
            snprintf(temp, sizeof(temp), "%s", prefix);
            print_tree(node->child[i], level + 1, is_last_child, temp);
        }
    }
}

int main()
{
    // initialize root node
    root.pid = 0;
    root.ppid = -1;
    root.num_child = 0;
    strcpy(root.command_name, "init");
    
    list = (Process_Node*)malloc(BUF_SIZE * sizeof(Process_Node)); // first create a big array to store the info
    if (list == NULL) 
    {
        errExit("malloc failed");
    }
    // First, search all the process and record each's info
    record_all_the_process();
    // build an array to record the status if existing in tree or not
    tree_status = (int*)malloc(num_process * sizeof(int));
    if (tree_status == NULL) 
    {
        // Handle malloc failure
        free(list);
        errExit("malloc failed");
    }
    // Initialize all elements to -1
    for (int i = 0; i < num_process; i++) 
    {
        tree_status[i] = -1;
    }
    // Secondly, build the whole tree
    build_tree();
    // Finally, print the tree
    // Check if the root process exists
    if (root.num_child != 0) 
    {
        // Call the print_tree function, starting from the root process
        char initial_prefix[BUF_SIZE] = "";
        print_tree(&root, 0, true, initial_prefix);
    }
    else 
    {
        fprintf(stderr, "Root process not found!\n");
    }
    
    // free resource
    free(list);
    free(tree_status);
    return 0;
}
