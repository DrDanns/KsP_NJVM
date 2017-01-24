#include <bigint.h>
#include "vm.c"

#define VERSION 8

int i;
int version, instructionSize, sdaVariables;
char *rest;
unsigned int *p;
int header[3];
char format[4]; 
FILE *file;
int debug_active;
char option[2] = "--";

int main(int argc, char *argv[]){
	boolean manualStackSize = FALSE;
	boolean manualHeapSize = FALSE;
	StackSlot *tempStackSlot;
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
		} else if (strcmp(argv[i],"--stack") == 0){
			i++;
			stacksize = strtol(argv[i], &rest, 10);
			if(strcmp(rest,"") != 0) error("illegal stack size");
			if(stacksize <= 0){
				error("illegal stack size");
			} else {
				if(!manualStackSize) setStacksize(stacksize);
				manualStackSize = TRUE;
			}
		} else if (strcmp(argv[i],"--gcpurge") == 0){
			gcpurge = TRUE;
		} else if (strcmp(argv[i],"--gcstats") == 0){
			gcstats = TRUE;
		} else if (strcmp(argv[i],"--heap") == 0){
			i++;
			heapsize = strtol(argv[i], &rest, 10);
			if(strcmp(rest,"") != 0) error("illegal heap size");
			if(heapsize <= 0){
				error("illegal heap size");
			} else {
				if(!manualHeapSize) setHeapsize(heapsize);
				manualHeapSize = TRUE;
			}
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
	tempStackSlot = global;
	for(i = 0; i < sdaVariables; i++){
		tempStackSlot[i].u.objRef = NULL;
	}
	
	p = malloc(instructionSize * sizeof(int));
	
	if(!manualStackSize) setStacksize(64);
	if(!manualHeapSize) setHeapsize(8192);

	fread (p, sizeof(int), instructionSize, file);
	fclose(file);
	
	if(debug_active) {
		printf("DEBUG:\tfile\t:\t'%s'\n\tcode\t:\t%d instructions\n\tdata\t:\t%d objects\n\tstack\t:\t%d slots\n\theap\t:\t2 * %d bytes\n",argv[argc-1],instructionSize,sdaVariables, stacksize/16 /*sizeof(StackSlot)*/ ,heapsize/2);
	}
	
	printf("Ninja Virtual Machine started\n");
	
	if(debug_active) {
		debug(instructionSize,p, sdaVariables);
	} else execute(instructionSize,p);
	free(p);
	if(gcstats) collectGarbageStats();
	printf("Ninja Virtual Machine stopped\n");
	
	return 0;
}






