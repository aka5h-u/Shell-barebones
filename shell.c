#include<stdio.h>
#include<stdlib.h> //exit ,malloc free realloc
#include<string.h> //strtok strcmp
#include<unistd.h> //exec fork pid_t
#include<sys/wait.h> //wait pid

//Function declarations

int shell_cd(char **args);
int shell_help(char **args);
int shell_exit(char **args);

//char builtin strings
char *builtin_str[]={
    "cd",
    "help",
    "exit"

};

//array of function pointer ,to add more builtin functions edit here
int (*builtin_func[]) (char **) = {
  &shell_cd,
  &shell_help,
  &shell_exit
};

int built_in_number()
{
    return sizeof(builtin_str) /sizeof(char *);
}
//contants
#define BUFFER_SIZE 1024 //as per tut
#define SPLIT_SIZE 64  //as per tut
#define DEMLIMITER " \r\t\n\a"
/**************************************************************
 * 
 * BUILT IN FUNCTION IMPLMENTATIONS
 * *********************************************************/
//1. cd
int shell_cd(char **args)
{
    if(args[1]== NULL)
        fprintf(stderr,"Expected arguemnt to cd \n");
    else
    {
        if(chdir(args[1])!=0)
            perror("In changing directory: ");

    }
    return 1 ;

}

//2. help
int shell_help(char **args)
{
    printf("This is the shell help page \n");
    printf("Following are the builtin functions");
    
    for(int i = 0 ;i < built_in_number(); i++)
    {
        printf("%s \n",builtin_str[i]);
    }
    printf("Use the man command for information on other programs.\n");
    
    return 1;

}

//3.exit
int shell_exit(char **args)
{
    return 0;
}
//read from stdin function 

char *shell_read_line(void)
{
    int buffer_size = BUFFER_SIZE ;
    int c ;
    int position =0 ;
    char *buffer = malloc(sizeof(char)*buffer_size);
    if(!buffer)
        {
            fprintf(stderr,"Allocation error \n");
            exit(EXIT_FAILURE);
        }
    while(1)
    {
        c = getchar() ;
        if(c == EOF || c == '\n')
        {
            buffer[position] = '\0';
            return buffer ;

        }
        else
            buffer[position]=c ;
        
        position++ ;

    
  
        //if we exceed the buffer limit allocated on the heap
        if(position >= buffer_size)
        {
            buffer_size += BUFFER_SIZE ;
            buffer = realloc(buffer, buffer_size);
            if(!buffer)
            {
                fprintf(stderr,"Allocation error \n");
                exit(EXIT_FAILURE);
             }
        }
    }

}
// Parsing the given line into args but the delimiter is only whitespaces ;no quotes etc
char **shell_split_line(char *line)
{
    int bufspl = SPLIT_SIZE ;
    int position = 0;
    
    char **tokens = malloc(bufspl*sizeof(char *));
    char *token ;

    if(!tokens)
    {
        fprintf(stderr,"Allocation failure");
        exit(EXIT_FAILURE);
    }
    token = strtok(line,DEMLIMITER);
    while(token != NULL)
    {
        tokens[position] =token ;
        position++;

        if(position >= bufspl)
        {
            bufspl += SPLIT_SIZE ;
            tokens = realloc(tokens,bufspl);

            if(!tokens)
            {
                fprintf(stderr,"Allocation error");
                exit(EXIT_FAILURE) ;
            }
        }
        token = strtok(NULL, DEMLIMITER);
    }
    tokens[position]= NULL;
    return tokens ;


}
int shell_init(char **args)
{
    pid_t pid ,wpid ;
    int status ;
    pid = fork() ;
    
    // fork returns 0 to child and pid of child to the parent process
    if(pid == 0)
    {
        //child process
        if(execvp(args[0],args)== -1) //child process executes thd cmd parsed earlier with execvp
        //where v is vector of string arg and p is os find the path of the cmd to be executed
        {
            perror("shell");
        }
           
        
        exit(EXIT_FAILURE);
    }
    else if(pid < 0)
        perror("shell");
    else
    {
        do  //parent process waits for state of the child to change either ended by a kill sginal
        {
            wpid = waitpid(pid, &status, WUNTRACED);
        
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
 

    }
    
   
    return 1;
   

}


//Shell execute
int shell_execute_cmd(char **args)
{
    int i;
    if(args[0] == NULL)
        return 1 ;

    for(i = 0;i < built_in_number(); i++)
    {
        if(strcmp(args[0],builtin_str[i])==0)
            return (*builtin_func[i])(args);
       
           
    }
     return shell_init(args);
}


void shell_loop(void)
{
    //loop has 3 wwork to do
    char *line;
    char **args;
    int status ;
   do{
    printf("> "); //prompt symbol
    // 1.read the command
    line = shell_read_line();
    //2.split string to arguments
    args = shell_split_line(line) ;
    //3. execute the command
    status = shell_execute_cmd(args) ;
    //free up space after execution

    free(line);
    free(args);
   } while (status);
}




int main(int argc, char *argv[])
{
    shell_loop() ;

    return EXIT_SUCCESS ;
}