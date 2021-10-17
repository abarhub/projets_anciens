
/* parent -"myshell to create child #1 . Child#1 fork()'s child #2 

(before redirections, and execlp(),

Child#1 to run "more"

Child#2 to run "man ls" */

 

int main()

{

int pid1, pid2, pipe_fd[2], sts;

pid1 = fork(); /* create child#1 */

if (pid1 == -1 ) { /* could not fork(), error, exit */

printf ("main: unable to fork()'ing exiting..\n");

exit(1);

}

if (pid1 == 0) {/* child#1 */

/* step #1 in child#1: (to run "more")--create pipes */

sts = pipe (pipe_fd); /*pipe between child#1-child#2 */

/* or, sts = pipe (&pipe_fd[0]); */

if (sts == -1 ) {

printf ("child#1: unable to pipe, exiting..\n");

exit(1);

}

 

/* step #2 in child#1: create child#2 */

pid2 = fork(); /* create child#2 */

if (pid1 == -1 ) { /* could not fork(), error, exit */

printf ("child#1: unable to fork()'ing exiting..\n");

exit(1);

}

if (pid2 == 0) { /* child#2: to run "man ls" */

/* needs STDIN terminal, 

STDOUT re-directed to pipe */

close (1); /* closing STDOUT for redirection */

dup (pipe_fd[1]); /* write descr of pipe ([1])

dup's to STDOUT */

/* close unwanted decriptors */

close (pipe_fd[1]);

close (pipe_fd[1]);

execlp ("man", "man", "ls", (char *) 0);

exit (1); /* safety, if execlp() failed */

} /* end of child#2 */

else { /* child #1 to run "more"*/

close (0); /* closing STDIN for redirection */

dup (pipe_fd[0]); /* write descr of pipe ([0])

dup's to STDIN */

/* close unwanted decriptors */

close (pipe_fd[0]);

close (pipe_fd[1]);

execlp ("more", "more", (char *) 0);

exit (1); /* safety, if execlp() failed */

} /* end of child#1 */
}
else /* parent, "myshell" */

wait ( (int *) 0);

}

 
