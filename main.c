#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>

char *buff[1024];
pid_t pid;
int in = 0, out = 0, app = 0, err = 0;

void sigHandler_int(int signo) {
	if ((signo == SIGINT)) {
		signal(SIGINT, SIG_IGN);
		kill(0, SIGINT);
	}

}
void sigHandler_stop(int signo) {
	if (signo == SIGSTOP) {
		signal(SIGSTOP, SIG_IGN);
		kill(0, SIGSTOP);
	}
}
void runIOE(char * args[]) {
	int fd;
	pid = fork();
	if (pid == -1) {
		printf("Child process could not be created\n");
		return;
	}
	if (pid == 0) {
		if (out == 1 && app == 0) {
			fd = open(args[0], O_RDWR | O_CREAT | O_TRUNC, 0644);
			if (fd < 0)
				fprintf(stderr, "Error in opening file for writing\n");
			if (err == 0) {
				dup2(fd, STDOUT_FILENO);
				close(fd);
			} else if (err == 1) {
				dup2(fd, STDERR_FILENO);
				close(fd);
			}
		}

		else if (out == 1 && app == 1) {
			fd = open(args[0], O_RDWR | O_CREAT | O_APPEND, 0644);
			if (fd < 0)
				fprintf(stderr, "Error in opening file for writing\n");
			if (err == 0) {
				dup2(fd, STDOUT_FILENO);
				close(fd);
			} else if (err == 1) {
				dup2(fd, STDERR_FILENO);
				close(fd);
			}
		}

		else if (in = 1) {
			fd = open(args[0], O_RDONLY, 0644);
			if (fd < 0)
				fprintf(stderr, "Error in opening file for reading\n");
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		if (execvp(args[0], args) == err) {
			printf("err");
			kill(getpid(), SIGTERM);
		} else {
			signal(SIGINT, sigHandler_int);
			signal(SIGSTOP, sigHandler_stop);
		}
	}
	waitpid(pid, NULL, 0);

}

void runAnd(char * args[]) {
	/*char *buf;
	 char newbuf[1024];
	 int i=0;
	 while(buf[0] = strtok(newbuf, "&&") != NULL) {
	 newbuf[i] = buff;
	 i++;
	 }

	 for (int k = 0; k < i; k++) {
	 execvp(newbuf[k], newbuf);
	 }
	 */
}

void runPipe(char * args[]) {
	int fd[2], fd1[2], i, j = 0, m = 0, n = 0, k = 0, last = 0;
	char *b[256];
	while (args[m] != NULL) {
		if (strcmp(args[m], "|") == 0) {
			n++;
		}
		m++;
	}
	n++;

	while (args[j] != NULL) {
		if (last != 1) {
			k = 0;
			while (strcmp(args[j], "|") != 0) {
				b[k] = args[j];
				j++;
				if (args[j] == NULL) {
					last = 1;
					k++;
					break;
				}
				k++;
			}
			j++;
			if (i % 2 != 0) {
				pipe(fd);
			} else {
				pipe(fd1);
			}

			pid = fork();

			if (pid == -1) {
				printf("Child process could not be created\n");
				return;
			}
			if (pid == 0) {
				if (i == 0) {
					dup2(fd1[1], STDOUT_FILENO);
					close(fd1[1]);
				} else if (i == n - 1) {
					if (n % 2 != 0) {
						dup2(fd[0], STDIN_FILENO);
						close(fd[0]);
					} else {
						dup2(fd1[0], STDIN_FILENO);
						close(fd1[0]);
					}
				} else {
					if (n % 2 != 0) {
						dup2(fd1[0], STDIN_FILENO);
						dup2(fd[1], STDOUT_FILENO);
						close(fd1[0]);
						close(fd[1]);
					} else {
						dup2(fd[0], STDIN_FILENO);
						dup2(fd1[1], STDOUT_FILENO);
						close(fd[0]);
						close(fd1[1]);
					}
				}
				execvp(b[0], b);
			}
			i++;
		}
	}
	waitpid(pid, NULL, 0);
}

int main(int argc, char **argv) {
	char buf[1024];
	int n, stat;
	int i = 0;
	int background = 0;
	char *b[1024];
	while (1) {
		fgets(buf, sizeof(buf), stdin);
		if ((buff[0] = strtok(buf, " \n")) == NULL)
			continue;
		n = 1;
		while ((buff[n] = strtok(NULL, " \n")) != NULL) {
			/*     if ((strchr(buff, '\\')) != NULL){
			 buff[strlen(buff)-1]=0;
			 buff[n]=strtok(NULL, " ");
			 }*/
			n++;
		}

		pid = fork();
		if (pid == -1) {
			printf("Child process could not be created\n");
		}
		if (pid == 0) {
			signal(SIGINT, SIG_IGN);
			signal(SIGSTOP, SIG_IGN);
			if (strcasecmp(buff[0], "cd") == 0)
				chdir(getenv("HOME"));
			else {
				while (buff[i] != NULL && background == 0) {
					b[i] = buff[i];
					if (!strcasecmp(buff[i], "&")) {
						background = 1;
					} else if (!strcasecmp(buff[i], "|")) {
						runPipe(buff);
						return 0;
					} else if (!strcasecmp(buff[i], ">")) {
						out = 1;
						runIOE(buff);
						return 0;
					} else if (!strcasecmp(buff[i], "<")) {
						in = 1;
						runIOE(buff);
						return 0;
					} else if (!strcasecmp(buff[i], ">>")) {
						out = 1;
						app = 1;
						runIOE(buff);
						return 0;
					} else if (!strcasecmp(buff[i], "&&")) {
						runAnd(buff);
						return 0;
					}
					i++;
					signal(SIGINT, sigHandler_int);
					signal(SIGSTOP, sigHandler_stop);
				}

				execvp(b[0], b);
				if (background == 0)
					waitpid(pid, NULL, 0);
				else {
					printf(
							"Parent not blocked, pid of background created process: %d\n",
							pid);
					waitpid(pid, &stat, WNOHANG);
				}

			}
		}

	}

	exit(0);
}

