#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include "header.h"


//function to implement pwd command
int pwd_gbsh ()
{
    system("pwd");
    return 1;

}

//function to implement clear command
int clear_gbsh ()
{
    system("clear");
    return 1;

}

//function to implement exit command
void exit_gbsh ()
{
    system("exit");
}

//function to implement ls command
int ls_gbsh(char **token)
{

	pid_t pid = fork();
	if (pid == 0)
	{
		//ls command implemented in child process
		//code to redirect ls command result to output file
	    	int fd1;
	    	if ((token[2]!=NULL)&&(strcmp(token[2],">")==0))
	    	{
	
	    		fd1=open(token[3],O_RDWR|O_CREAT|O_TRUNC,00700);
	    		dup2(1,fd1);

	    	}
	    	execl("/bin/ls","/bin/ls",token[1],(char*)0);
	    	close(fd1);
	}
    int status;
    wait(&status);
    return 1;
}
//function to implement cd command
int cd_gbsh(char **token){
if (token[1]==NULL)
{
int ret=chdir("/home");
}
else{

int ret=chdir(token[1]);
}
return 1;
}

//function to print environment variables
int environ_gbsh(){
    system("env");
    return 1;
}

//function to define environment variables
int setenv_gbsh(char **token){
    setenv(token[1],token[2],1);
    return 1;
};

//function to undefine environment variables
int unsetenv_gbsh(char **token){
    unsetenv(token[1]);

}




void stdout_gbsh(char *file)
{
	int fd1;
	// return a file descriptor for the newly created/ existing output file
	//close(1);
	fd1=open(file,O_WRONLY|O_CLOEXEC|O_APPEND|O_CREAT|O_TRUNC,00700);
	perror("open dup2 ");
	printf("%s :  ", file);
	//duplicate the file descriptor to stdout
	dup2(fd1,1);
	perror("write dup2 ");
	close(fd1);
}

+void stdin_gbsh(char *file)
+{
+	int fd;
+	// return a file descriptor for the input file
+	fd=open(file,O_RDONLY);
+	//duplicate the file descriptor as std input
+	dup2(fd,0);
+	perror("read dup2");
+	close(fd);
+	
+}
+char ** tokenformat_gbsh(char **tokens)
+{
+	int i=0;
+	int j=0;
+	int c=i;
+	output_cntr=0;
+	input_cntr=0;
	pipe_cntr=0;
+	while( tokens[i]!=NULL)
+	{
+		if (strcmp(tokens[i],">")==0)
+		{
+			output_cntr++;
+			process_strings[j]=i;
+			j=j+1;
+			c=i;
+			while( tokens[c]!=NULL)
+			{ tokens[c]=tokens[c+1];
+			c++;
			}
+			i=i-1;
+
+		}
+		if (strcmp(tokens[i],"<")==0)
+		{
+			input_cntr++;
+			process_strings[j]=i;
+			j=j+1;
+			c=i;
+			while( tokens[c]!=NULL)
+			{ tokens[c]=tokens[c+1];
+			c++;
			}
+			i=i-1;
+		}
		if (strcmp(tokens[i],"|")==0)
+		{
+			pipe_cntr++;
+			process_strings[j]=i;
+			j=j+1;
+			c=i;
+			while( tokens[c]!=NULL)
+			{ tokens[c]=tokens[c+1];
+			c++;
			}
+			i=i-1;
+		}
+		
+		i=i+1;	
+				
+	}
+	return tokens;
+
}



int fork_gbsh(char **tokens)

{
int fd[2];
pid_t child;
	int j=0;
	int status;
	int flag;

if(pipe_cntr==1)
	{
		if (pipe(fd==-1))
		{
		perror("piping");
		exit(1);
		}

		else if (pipe(fd==0))
		{
			child=fork();
  
			if(child<0)
			{
			perror("fork");
			exit(1);
			}  
    
			else if(child>0)
			{
		
			dup2(fd[1],1);////parent to child writing, fd[0] close  by parent
			execvp(token[0],token[0],NULL);
			close(fd[0]);

			}


			else
			{
			dup2(fd[0],0);//fd[1] close by child,child is reading
			execvp(token[1],token[1],NULL);
			close(fd[1]);

        		}
		}
	}
else 
	{
		
	child=fork();
    
    
	if(child>0)
		{
		//executed by parent process
		//parent process waits for the child process to terminate
		waitpid(child,&status,0);
	
		}

	else if(child<0)
		{
		perror("fork");
		exit(1);
		}
	else
		{
		char *curr_dir=malloc(sizeof(char)*256);
		getcwd(curr_dir,256);
		strcat(curr_dir,"/gbsh");
		char *PARENT;
		setenv(PARENT,curr_dir,1);
	
	
		if (output_cntr==1)
			{
			//output redirection
			stdout_gbsh(tokens[process_strings[j]]);
			j=j+1;
			}	
		if (input_cntr==1)
			{
			//input redirection
			stdin_gbsh(tokens[process_strings[j]]);	
			j=j+1;
			}

		flag = execvp(tokens[0],&tokens[0]);
		perror("execvp");
+	
+		if (flag<0)
+			{
+			printf("Execution error in exec");
+			}
		}
+	
	}	
}	



+	
+	#define USERID_LENGTH 256
+	#define HOSTNAME_LENGTH 256
+	#define CWD_LENGTH 1024
+	#define MAX_INPUT_LENGTH 1024
+	#define TOKEN_DELIMITERS " \t\r\n"
+
+void gbsh_shell ()
+{
+		
+	//userid, hostname and current working directory variables initialised and memory assigned
+	    	
+	char *user_ID = malloc(sizeof(char)*USERID_LENGTH);
+	char *hostname = malloc(sizeof(char)*HOSTNAME_LENGTH);
+	char *cwd = malloc(sizeof(char)*CWD_LENGTH);
+	char *cmdline_input = malloc(sizeof(char)*MAX_INPUT_LENGTH);
+	int cmd_length;
+	//cmd_length = (int) malloc(sizeof(int)*1024);
+    
+    	 hostname[1023] = '\0';
+	//input_prompt(user_ID,hostname,cwd);
+ 	//struct passwd *pws;
+	//pws = getpwuid(geteuid());
+	while(1)
+    	{
+
+    		gethostname(hostname,HOSTNAME_LENGTH);
+    		getcwd(cwd,CWD_LENGTH);
+    		user_ID = getenv("USER");
+
+		//printing the prompt
+    		printf("%s@%s %s > ", user_ID, hostname, cwd);
+
+		//input from stdin
+    		fgets(cmdline_input,MAX_INPUT_LENGTH,stdin);
+
+
+    		//reference for removing trailing newline character
+    		//http://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
+
+    	if (cmdline_input != NULL)
+	{
+        	cmd_length = (int)strlen(cmdline_input);
+        	if (cmd_length>0 && cmdline_input[cmd_length-1] == '\n')
+		{
+        		cmdline_input[--cmd_length] = '\0';
+        	}
+    	}
+
+	 //parsing the command line input  
+    	int a=0;
+    	char *token;
+    	char **tokens=malloc(sizeof(char*)*64);
+    	token = strtok(cmdline_input, TOKEN_DELIMITERS);
+    	while (token!=NULL)
+	{
+        	tokens[a]=token;
+        	token=strtok(NULL, TOKEN_DELIMITERS);
+        	a++;
+		
+    	}
+    	tokens[a]=NULL;
+    	tokens=tokenformat_gbsh(tokens);
+	//parsed input string comparison with predefined functions
+	//then redirecting to specific functions as per the input command
+
+    		if (strcmp(tokens[0],"pwd")==0)
+		{
+    			pwd_gbsh();
+		}
+		else if (strcmp(tokens[0],"clear")==0)
+		{
+    			clear_gbsh();
+		}
+		else if (strcmp(tokens[0],"exit")==0)
+		{
+    			//exit_gbsh ();
+			exit(0);
+		}
+		//else if (strcmp(tokens[0],"ls")==0)
+		//{
+                //	ls_gbsh (tokens);
+		//}
+		else if (strcmp(tokens[0],"cd")==0)
+		{
+			cd_gbsh(tokens);
+		}
+		else if (strcmp(tokens [0],"setenv")==0)
+		{
+            		setenv_gbsh(tokens);
+       		}
+        	else if (strcmp(tokens [0],"unsetenv")==0)
+		{
+            		unsetenv_gbsh(tokens);
+		}
+		else if (strcmp(tokens[0],"environ")==0)
+		{
+            		environ_gbsh();
+		}
+      		else 
+		{
+			printf("forking!!!!\n");
+            		fork_gbsh(tokens);
+        	}
+    
+        }
+    }
+
+    
+
+
+
+
+
+int main(int argc, char *argv[])
+{
+    gbsh_shell();
+    return 0;
+}
