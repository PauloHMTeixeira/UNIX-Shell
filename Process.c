#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>

#define SIZE 80
#define H_SIZE 1000

void split(char** argv, char* string, char* token, int* haveCommercial);
void printParameters(char** argv);
int getMaiorIndex(char* string);
int vmmq(char** matrix);
void getCommand(char* string);

int main() {

	int should_run = 1;
	int haveCommercial;

	int paipe[2];

	pid_t pid;

	char history[H_SIZE][SIZE/2+1];
	int tail = 0;

	char string[SIZE/2+1];
	char aux[SIZE/2+1];

	char* argv[SIZE/2+1];
	char* token;

	while(should_run) {

		printf("%s", "ejpr> ");
		fflush(stdout);

		getCommand(string);
		strcpy(aux, string);


		if(strcmp("!!", aux) == 0){
		

			printf("%s\n", history[tail-1]);
			strcpy(string, history[tail-1]);
		}


		split(argv, string, token, &haveCommercial);

		strcpy(history[tail], aux);
		tail++;

		pid = fork();

			if(pid < 0) {
				printf("%s", "Erro ao criar o processo");
				exit(1);
			}

			if(pid == 0) {

				int valor = vmmq(argv);

				if(valor == 1){
					
					int index = getMaiorIndex(aux);
					int fp;

					fp = open(&aux[index+1], O_RDONLY);

					dup2(fp, STDIN_FILENO);

					for(int i = 0; argv[i] != NULL; i++){
						if(strcmp(argv[i], "<") == 0){
							argv[i] = NULL;
							break;
						}
					}

				}
				if(valor == 2){
					
					int index = getMaiorIndex(aux);
					int fp;

					fp = open(&aux[index+1], O_WRONLY);

					dup2(fp, STDOUT_FILENO);

					for(int i = 0; argv[i] != NULL; i++){
						if(strcmp(argv[i], ">") == 0){
							argv[i] = NULL;
							break;

						}
					}
				}

				if(valor == 3){
					char** c2;

					for(int i = 0; argv[i] != NULL; i++){
						if(strcmp(argv[i], "|") == 0){
							argv[i] = NULL;
							c2 = &argv[i+1];
							break;
						}
					}

					pipe(paipe);

					pid_t cpid = fork();

					if(cpid < 0) {
						printf("%s\n", "ERROR");
						exit(0);
					}

					if(cpid == 0){

						close(paipe[0]);
						dup2(paipe[1], STDOUT_FILENO);
						execvp(argv[0], argv);
						exit(0);
					}

					else {
						close(paipe[1]);
						wait(NULL);

						dup2(paipe[0], STDIN_FILENO);
						execvp(c2[0], c2);
						exit(0);
					}
				}
				
				execvp(argv[0], argv);
				exit(0);
			}


			if(pid > 0) {

				if(!haveCommercial)
					wait(NULL);
			}
	}
			return 0;
	}
	

	void printParameters(char** argv) {
		for(int i = 0; argv[i] != NULL; i++) {
			printf("%s\n", argv[i]);
		}
	}

	int vmmq(char** matrix) {

		for(int i = 0; matrix[i] != NULL; i++){
			if(strcmp(matrix[i], "<") == 0) return 1;
			if(strcmp(matrix[i], ">") == 0) return 2;
			if(strcmp(matrix[i], "|") == 0) return 3;
		}

		return 0;
	}

	int getMaiorIndex(char* string){

		for(int i = 0; i < strlen(string); i++){
			if(string[i] == '>') return (i+1);
			if(string[i] == '<') return (i+1);
			if(string[i] == '|') return (i+1);
		}
		return 0;
	} 

	void getCommand(char* string){

		fgets(string,SIZE/2+1,stdin);

		for(int i = 0; string[i] != '\0'; i++){
			if(string[i] == '\n') string[i] = '\0';
		}
	}

	void split(char** argv, char* string, char* token, int* haveCommercial){

		token = strtok(string, " ");
		int i = 0;
		while(token != NULL) {
			argv[i] = malloc(sizeof(strlen(token)+1) * sizeof(char));
			
			if(strcmp(token, "&") == 0) {
				argv[i] = NULL;
				*haveCommercial = 1;
			}

			else strcpy(argv[i], token);
				token = strtok(NULL, " ");
				i++;
			}
			argv[i] = NULL;
	}
