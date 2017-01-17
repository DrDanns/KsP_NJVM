#include <stdio.h>
#include <string.h>
#include <bigint.h>
#include "vm.c"

#define IMMEDIATE(x) ((x) & 0x00FFFFFF)

#define TEXT_LENGTH 80
#define INSTRUCTION_SIZE 30

#define VERSION 5

int i;
int version, instructionSize, sdaVariables;
unsigned int *p;
int header[3];
char format[4]; 
FILE *file;
int debug_active;
char option[2] = "--";

int main(int argc, char *argv[]){
	
	for(i=1; i < argc; i++){
		if(strcmp(argv[i],"--help") == 0){
			printf("Usage: ./njvm [options] <code file>\nOptions:\n  --debug\t   start virtual machine in debug mode\n  --version\t   show version and exit\n  --help\t   show this help and exit\n");
			exit(0);
		} else if(strcmp(argv[i],"--debug") == 0){
			if ( i == (argc-1)) {
				error("no code file specified");
			}
			else {
				debug_active = TRUE;
			}
		} else if (strcmp(argv[i],"--version") == 0){
			printf("Ninja Virtual Machine version %d (compiled %s, %s)\n",VERSION,__DATE__,__TIME__);
			exit(0);
		} else if(strncmp(argv[i], "--", 2) == 0) {
			printf("Error: unknown option '%s', try './njvm --help'\n",argv[i]);
			exit(1);
		}

	} 
	

	file = fopen(argv[argc-1], "r");

	if ( ( file ) == NULL ) {
	printf("Error: cannot open code file '%s'\n", argv[argc-1]);
	exit(1);
	}

	fread (& format [0], sizeof(char), 4, file);

	if(!(format[0] == 'N' && 
		format[1] == 'J' && 
		format[2] == 'B' && 
		format[3] == 'F')){
			printf("Error: file '%s' is not a Ninja binary\n",argv[argc-1]);
			exit(1);
	}


	fread(& header[0], sizeof(int), 3, file);

	version = header[0];
	instructionSize = header[1];
	sdaVariables = header[2];
	
	if(version != VERSION) {
		printf("Error: file '%s' has wrong version number.\n",argv[argc-1]);
		exit(99);
	}
	global = malloc(sdaVariables * sizeof(StackSlot));
	
	p = malloc(instructionSize * sizeof(int));


	fread (p, sizeof(int), instructionSize, file);
	fclose(file);
	
	if(debug_active) printf("DEBUG: file '%s' loaded (code size = %d, data size = %d)\n",argv[argc-1],instructionSize,sdaVariables);
	
	printf("Ninja Virtual Machine started\n");
	
	if(debug_active) {
		debug(instructionSize,p, sdaVariables);
	} else execute(instructionSize,p);
	free(p);
	printf("Ninja Virtual Machine stopped\n");
	
	return 0;
}






