#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<unistd.h>
#include<dirent.h>

// Define the maximum path length
#define MAX_PATH_LENGTH 1024

// Node structure for the stack
typedef struct Node
{
    char path[MAX_PATH_LENGTH];
    struct Node* next;
} Node;

// Stack structure
typedef struct Stack 
{
    Node* top;
} Stack;

void printstack(Stack*);//printfun declaration

// Initialize the stack
void initStack(Stack* stack) 
{
    stack->top = NULL;
}


// Save the directory stack to a file
void saveStack(const Stack* stack)
{
    FILE* file = fopen("stack_file.txt", "w");
    if (!file) {
        perror("fopen() error");
        return;
    }

    Node* current = stack->top;
    while(current != NULL) {
        fprintf(file, "%s\n", current->path);
        current = current->next;
    }

    fclose(file);
}



// Load the directory stack from a file
void loadStack(Stack* stack) 
{
    FILE* file = fopen("stack_file.txt", "r");
    if (file==NULL) 
    {
        // If the file does not exist,return
        return;
    }

    char path[1024];
    
    while (fgets(path, sizeof(path), file)) 
    {
        path[strcspn(path,"\n")] = '\0';  // Remove newline character

        Node* newNode = (Node*)malloc(sizeof(Node));
        if (!newNode)
       	{
            perror("malloc() error");
            fclose(file);
            return;
        }

        strncpy(newNode->path, path, sizeof(newNode->path));
        newNode->path[sizeof(newNode->path) - 1] = '\0';  // Ensure null termination
        newNode->next = stack->top;
        stack->top = newNode;
    }

 
    fclose(file);

}
char cwd[PATH_MAX];
static int i=0;
// Push a directory onto the stack
void pushd(Stack* stack, const char* path) 
{
        int flag=0;
       if(stack->top!=NULL)
       {
	       Node *n=stack->top;

	       strncpy(cwd,n->path,sizeof(n->path));
       }
       else
       {
          if(!i)
         { 
             if((getcwd(cwd,sizeof(cwd)))==NULL)
	    {
               perror("getcwd");
	       return;
	     }
            i=1; 
         }
       }

    
        
       DIR *dp;//checking newdirecoty in current directory path
	struct dirent *p;

	dp = opendir(cwd);
	if(dp==NULL)
		perror("opendir");
        while(p=readdir(dp))
	{
		if(strcmp(p->d_name,path+1)==0)//we have give new dir with: //newdir_name
		{
			//printf("directory found:%s\n",path);
			flag=1;
			break;
		}
             flag=0;
	}
        if(flag==0)
	{
		printf("given directory is not found\n");
		exit(0);
	}

	strncat(cwd,path,sizeof(cwd));
           cwd[sizeof(cwd)-1]='\0';	
     
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) 
    {
        perror("Failed to push directory onto stack");
        return;
    }
    strncpy(newNode->path,cwd,MAX_PATH_LENGTH);
    newNode->path[MAX_PATH_LENGTH - 1] = '\0'; // Ensure null-termination
    
  /***adding node to stack(SLL)***/
    if(stack->top==NULL)
    {
    newNode->next = stack->top;
    stack->top = newNode;
    }
    else
    {
	  Node *last = stack->top;

	  while(last->next!=NULL)
	   last = last->next;

  	  newNode->next = last->next;
	   last->next = newNode;

    }

    saveStack(stack);//after pushing data saving into file

  /**changing directory**/        
    if(chdir(cwd)!=0)
    {
       perror("chdir()");
    }
    else
    {
      printf("changed directory is:%s\n",cwd);
    }    

    printstack(stack);
    
}

// Pop a directory from the stack
char* popd(Stack* stack) 
{
	char pwd[PATH_MAX];

    if (stack->top == NULL) 
    {
	    if( getcwd(pwd,sizeof(pwd))==NULL)
	    {
                perror("getcwd");	
	    }
	    printf("current directory is:%s\n",pwd);
        fprintf(stderr, "Stack is empty, cannot pop\n");
        return NULL;
    }
    Node* temp = stack->top;
    stack->top = stack->top->next;
      

    char* path = strdup(temp->path);
    
   

    printstack(stack);
    saveStack(stack);//after popping saving into file
    
    free(temp);

    return path;
}

void printstack(Stack *stack)
{

      Node *temp = stack->top;
      if(temp!=NULL)
      { 
         printf("path:");

         while(temp!=0)
         {
           printf("%s",temp->path);
           temp = temp->next;
         }
       printf("\n");
      }

}
/****main function****/
int main(int argc, char* argv[]) 
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s [pushd <dir> | popd]\n", argv[0]);
        return 1;
    }

    static Stack stack;
    static int initialized = 0;

    if (!initialized) 
    {
        initStack(&stack);
        initialized = 1;
    }
    
    loadStack(&stack);

    if (strcmp(argv[1],"pushd") == 0)
    {
        if (argc < 3) 
	{
            fprintf(stderr, "Usage: %s pushd <dir>\n", argv[0]);
            return 1;
        }
        pushd(&stack, argv[2]);
        printf("Pushed Directory: %s\n", argv[2]);
    
    } 
    else if (strcmp(argv[1], "popd") == 0) 
    {
        char* dir = popd(&stack);
        if (dir) 
	{
            printf("Popped Directory: %s\n", dir);
            free(dir);
        }
    } 
    else 
    {
        fprintf(stderr, "Unknown command: %s\n", argv[1]);
        return 1;
    }

    return 0;
}

