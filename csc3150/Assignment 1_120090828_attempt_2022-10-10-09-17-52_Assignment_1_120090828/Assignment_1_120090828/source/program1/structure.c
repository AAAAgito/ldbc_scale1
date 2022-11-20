int main(int argc, char *argv[]){

	/* fork a child process */
	pid_t pid;
	int status;

	printf("Process start to fork \n");
	pid = fork();

	if (pid == -1){//cannot fork successfully
	}

	else {
		//child process
		if (pid == 0){
			//deal with the argc, argv of child process
            //print related things
	
		/* execute test program */
		execve(arg[0], arg,NULL);
		perror("execve");//avoid error
		exit(EXIT_FAILURE);
		}
		else {//parent process
			printf("I'm Parent Process, my pid = %d\n", getpid());

		/* wait for child process terminates */
			waitpid(-1, &status, WUNTRACED);
			printf("Parent process receives SIGCHLD signal \n");

		/* check child process'  termination status */
			if (WIFEXITED(status)){//normal
			}
			else if (WIFSTOPPED(status)) {//stop
			}
			else if (WIFSIGNALED(status)){//exit because of a signal
				 if (WTERMSIG(status) == 13) {
                    //print different things according to the signal received
                    }
			}
			else {//avoid there is other case
			}
			exit (0);//exit normally
		}
	
	}
	
	 
	
	
	
}
