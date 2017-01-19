#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define HALT	0
#define PUSHC	1
#define ADD		2
#define SUB		3
#define MUL		4
#define DIV		5
#define MOD		6
#define RDINT	7
#define WRINT	8
#define RDCHR	9
#define WRCHR	10
#define PUSHG	11
#define POPG	12
#define ASF		13
#define RSF		14
#define PUSHL	15
#define POPL	16
#define EQ		17
#define NE		18
#define LT		19
#define LE		20
#define GT		21
#define GE		22
#define JMP		23
#define BRF		24
#define BRT		25
#define CALL	26
#define RET		27
#define DROP	28
#define PUSHR	29
#define POPR	30
#define DUP		31
#define NEW     32
#define GETF    33
#define PUTF    34
#define NEWA    35
#define GETFA   36
#define PUTFA   37
#define GETSZ   38
#define PUSHN   39
#define REFEQ   40
#define REFNE   41

#define TRUE	1
#define FALSE	0
#define IS_FAlSE	==0
#define IS_TRUE		!=0

#define SHIFT24 <<24
#define IMMEDIATE(x) ((x) & 0x00FFFFFF)
#define IMMEDIATE_CURRENT IMMEDIATE(program_memory[i])
#define SIGN_EXTEND(i) ((i) & 0x00800000 ? (i) | 0xFF000000 : ((i) & 0x00FFFFF))

#define MSB (1 << (8 * sizeof(unsigned int) - 1))
#define IS_PRIM(objRef) ((( objRef)->size & MSB) == 0)
#define GET_SIZE(objRef) ((objRef)->size & ~MSB)
#define GET_REFS(objRef) ((ObjRef *)(objRef)->data)

#define STACK_SIZE 1000
#define MEMORY_SIZE 300
#define REGISTER_SIZE 10

void error(char msg[]){
	printf("Error: %s\n",msg);
	exit(1);
}

void fatalError(char *msg) {
  error(msg);
}

ObjRef newPrimObject(int dataSize) {
  ObjRef objRef;

  objRef = malloc(sizeof(unsigned int) +
                  dataSize * sizeof(unsigned char));
  if (objRef == NULL) {
    fatalError("newPrimObject() got no memory");
  }
  objRef->size = dataSize;
  return objRef;
}

ObjRef getIndexedObjRef(ObjRef origin, int index){
	int i;
	ObjRef res;
	if(index >= GET_SIZE(origin)) error("Array out of bounds");
	res = (ObjRef)GET_REFS(origin);
	for(i = 0; i < index; i++){
		res = res + res -> size;
	}
	return res;
}

typedef unsigned int boolean;


typedef struct {
	boolean isObjRef;
	union {
		ObjRef objRef; 	/* used if isObjRef = TRUE*/
		int number;		/* used if isObjRef = FALSE*/
	} u;
} StackSlot;

StackSlot *newIntStackSlot(int value){
	StackSlot *result;
	result = malloc(sizeof(StackSlot));
	if (result == NULL) {
		error("no memory");
	}
	result -> isObjRef = FALSE;
	result -> u.number = value;
	return result;
}

StackSlot *newRefStackSlot(ObjRef objRef){
	StackSlot *result;
	result = malloc(sizeof(StackSlot));
	result -> isObjRef = TRUE;
	result -> u.objRef = objRef;
	if (result == NULL) {
		error("no memory");
	}
	
	return result;
}

StackSlot *global;
StackSlot stack[STACK_SIZE];
unsigned int program_memory[MEMORY_SIZE];
StackSlot return_register[REGISTER_SIZE];
int sp = 0;
int fp = 0;
int rp = 0;
int state = 0;

void printLine(int index, int n){
	if(n==4) printf("%04d:\t",index);
	else printf("%03d:\t",index);
		switch(program_memory[index] & 0xFF000000){
			case (HALT SHIFT24): printf("halt\n"); break;
			case (PUSHC SHIFT24): printf("pushc\t%d\n", SIGN_EXTEND(program_memory[index])); break;
			case (ADD SHIFT24): printf("add\n"); break;
			case (SUB SHIFT24): printf("sub\n"); break;
			case (MUL SHIFT24): printf("mul\n"); break;
			case (DIV SHIFT24): printf("div\n"); break;
			case (MOD SHIFT24): printf("mod\n"); break;
			case (RDINT SHIFT24): printf("rdint\n"); break;
			case (WRINT SHIFT24): printf("wrint\n"); break;
			case (RDCHR SHIFT24): printf("rdchr\n"); break;
			case (WRCHR SHIFT24): printf("wrchr\n"); break;
			case (PUSHG SHIFT24): printf("pushg\t%d\n", IMMEDIATE(program_memory[index])); break;
			case (POPG SHIFT24): printf("popg\t%d\n", IMMEDIATE(program_memory[index])); break;
			case (ASF SHIFT24): printf("asf\t%d\n", IMMEDIATE(program_memory[index])); break;
			case (RSF SHIFT24): printf("rsf\n"); break;
			case (PUSHL SHIFT24): printf("pushl\t%d\n", SIGN_EXTEND(program_memory[index])); break;
			case (POPL SHIFT24): printf("popl\t%d\n", SIGN_EXTEND(program_memory[index])); break;
			case (EQ SHIFT24): printf("eq\n"); break;
			case (NE SHIFT24): printf("ne\n"); break;
			case (LT SHIFT24): printf("lt\n"); break;
			case (LE SHIFT24): printf("le\n"); break;
			case (GT SHIFT24): printf("gt\n"); break;
			case (GE SHIFT24): printf("ge\n"); break;
			case (JMP SHIFT24): printf("jmp\t%d\n", IMMEDIATE(program_memory[index])); break;
			case (BRF SHIFT24): printf("brf\t%d\n", IMMEDIATE(program_memory[index])); break;
			case (BRT SHIFT24): printf("brt\t%d\n", IMMEDIATE(program_memory[index])); break;
			case (CALL SHIFT24): printf("call\t%d\n", IMMEDIATE(program_memory[index])); break;
			case (RET SHIFT24): printf("ret\n"); break;
			case (DROP SHIFT24): printf("drop\t%d\n", IMMEDIATE(program_memory[index])); break;
			case (PUSHR SHIFT24): printf("pushr\n"); break;
			case (POPR SHIFT24): printf("popr\n"); break;
			case (DUP SHIFT24): printf("dup\n"); break;
			case (NEW SHIFT24): printf("new\t%d\n", IMMEDIATE(program_memory[index])); break;
			case (GETF SHIFT24): printf("getf\t%d\n", IMMEDIATE(program_memory[index])); break;
			case (PUTF SHIFT24): printf("putf\t%d\n", IMMEDIATE(program_memory[index])); break;
			case (NEWA SHIFT24): printf("newa\n"); break;
			case (GETFA SHIFT24): printf("getfa\n"); break;
			case (PUTFA SHIFT24): printf("putfa\n"); break;
			case (GETSZ SHIFT24): printf("getsz\n"); break;
			case (PUSHN SHIFT24): printf("pushn\n"); break;
			case (REFEQ SHIFT24): printf("refeq\n"); break;
			case (REFNE SHIFT24): printf("refne\n"); break;
		}
}

void pushInt(int element){
	if(sp < STACK_SIZE){
		stack[sp] = *newIntStackSlot(element);
		sp++;
	} else {
		error("Stack Overflow");
	}
}


void pushRef(ObjRef element){
	if(sp < STACK_SIZE){
		stack[sp] = *newRefStackSlot(element);
		sp++;
	} else {
		error("Stack Overflow");
	}
}

void pushRefIndex(ObjRef element, int index){
	if(index < STACK_SIZE){
		stack[index] = *newRefStackSlot(element);
	} else {
		error("Stack Overflow");
	}
}

int popInt(void){
	if(sp!=0){
		sp--;
		if (stack[sp].isObjRef IS_FAlSE){
			return stack[sp].u.number;
		}
		error("Stack element is no Integer");
	}	
	error("Stack empty");
	exit(1);
	
}

ObjRef popRef(void){
	if(sp!=0){
		sp--;
		if (stack[sp].isObjRef IS_TRUE){
			return stack[sp].u.objRef;
		}
		error("Top Stack element is no Object Reference");
	}
	error("Stack empty");	
	exit(1);
}

ObjRef popRefIndex(int index){
	if(index!=0){
		if (stack[index].isObjRef IS_TRUE){
			return stack[index].u.objRef;
		}
		error("Indexed Stack element is no Object Reference");
	}
	error("Stack empty");
	exit(1);
}

void loadBip(void){
	ObjRef o1, o2;
	o2 = popRef();
	o1 = popRef();
	bip.op1 = o1;
	bip.op2 = o2;
}

void loadBipDiv(void){
	ObjRef o1, o2;
	o2 = popRef();
	o1 = popRef();
	bip.op1 = o2;
	if(bigToInt() == 0) error("division by zero");
	bip.op1 = o1;
	bip.op2 = o2;
}

void executeLine(int i){
	int x;
	char c;
	StackSlot stackslot;
	ObjRef objRef,objRefVal;
	switch(program_memory[i] & 0xFF000000){
			case (PUSHC SHIFT24): 
				bigFromInt(IMMEDIATE_CURRENT);
				objRef = bip.res;
				pushRef(objRef);
			break;
			case (ADD SHIFT24): 
				loadBip();
				bigAdd();
				pushRef(bip.res);
				break;
			case (SUB SHIFT24): 
				loadBip();
				bigSub();
				pushRef(bip.res);
				break;
			case (MUL SHIFT24): 
				loadBip();
				bigMul();
				pushRef(bip.res);
				break;
			case (DIV SHIFT24):
				loadBipDiv();
				bigDiv();	
				pushRef(bip.res);	
				break;
			case (MOD SHIFT24): 
				loadBipDiv();
				bigDiv();
				pushRef(bip.rem);
				break;
			case (RDINT SHIFT24): 
				bigRead(stdin);
				objRef = bip.res;
				pushRef(objRef);
				break;
			case (WRINT SHIFT24):
				bip.op1 = popRef();
				bigPrint(stdout);
				break;
			case (RDCHR SHIFT24): 
				scanf("%s", &c); 
				bigFromInt(c);
				pushRef(bip.res); 
				break;
			case (WRCHR SHIFT24):
				bip.op1 = popRef();
				c = (char)bigToInt();
				printf("%c", c);
				break;
			case (HALT SHIFT24): state = -99; break;
			case (PUSHG SHIFT24): {
				pushRef(global[IMMEDIATE_CURRENT].u.objRef);
			} break;
			case (POPG SHIFT24): global[IMMEDIATE_CURRENT] = *newRefStackSlot(popRef()); break;
			case (ASF SHIFT24): {
				pushInt(fp);
				fp = sp;
				sp = sp + IMMEDIATE_CURRENT;
			} break;
			case (RSF SHIFT24): {
				sp = fp;
				fp = popInt();
			} break;
			case (PUSHL SHIFT24): 
				if ((fp + SIGN_EXTEND(program_memory[i])) < STACK_SIZE) {
					objRef = popRefIndex(fp + SIGN_EXTEND(program_memory[i]));
					pushRef(objRef);
				} else{
					printf("Out of stack bounds.\n");
					exit(99);
				} break;
			case (POPL SHIFT24):
				objRef = popRef();
				pushRefIndex(objRef, fp + SIGN_EXTEND(program_memory[i]));
				break;
			case (EQ SHIFT24):{
				loadBip();
				if (bigCmp() == 0){
					bigFromInt(TRUE);
				} else {
					bigFromInt(FALSE);
				}
				pushRef(bip.res);
			} break;
			case (NE SHIFT24):{
				loadBip();
				if (bigCmp() != 0){
					bigFromInt(TRUE);
				} else {
					bigFromInt(FALSE);
				}
				pushRef(bip.res);
			} break;
			case (LT SHIFT24):
				loadBip();
				if (bigCmp() < 0){
					bigFromInt(TRUE);
				} else {
					bigFromInt(FALSE);
				}
				pushRef(bip.res);
			 break;
			case (LE SHIFT24):{
				loadBip();
				if (bigCmp() <= 0){
					bigFromInt(TRUE);
				} else {
					bigFromInt(FALSE);
				}
				pushRef(bip.res);
			} break;
			case (GT SHIFT24):{
				loadBip();
				if (bigCmp() > 0){
					bigFromInt(TRUE);
				} else {
					bigFromInt(FALSE);
				}
				pushRef(bip.res);
			} break;
			case (GE SHIFT24):{
				loadBip();
				if (bigCmp() >= 0){
					bigFromInt(TRUE);
				} else {
					bigFromInt(FALSE);
				}
				pushRef(bip.res);
			} break;
			case (JMP SHIFT24): state = IMMEDIATE_CURRENT -1; break;
			case (BRF SHIFT24): if(*(boolean *)popRef()->data IS_FAlSE) state = IMMEDIATE_CURRENT -1; break;
			case (BRT SHIFT24): if(*(boolean *)popRef()->data IS_TRUE) state = IMMEDIATE_CURRENT -1; break;
			case (CALL SHIFT24): 
				pushInt(state + 1);  state = IMMEDIATE_CURRENT -1; break;
			case (RET SHIFT24):  state = popInt()-1; break;
			case (DROP SHIFT24): sp -= IMMEDIATE_CURRENT; break;
			case (PUSHR SHIFT24): 
				if(rp > 0) {
					rp--;
					pushRef(return_register[rp].u.objRef);
				} else {
					error("Register empty");
				}
				break;
			case (POPR SHIFT24): 
			if(rp < REGISTER_SIZE) {
				return_register[rp] = *newRefStackSlot(popRef()); rp++; 
			} else {
				error("Register overflow");
			}
				break;
			case (DUP SHIFT24): 
				stackslot = stack[sp-1]; 
				if(stackslot.isObjRef){
					objRef = popRef();
					pushRef(objRef);
					pushRef(objRef);
				} else {
					x = popInt();
					pushInt(x);
					pushInt(x);
				}
				break;
			case (NEW SHIFT24):

				break;
			case (GETF SHIFT24):
				objRef = popRef();
				pushRef(getIndexedObjRef(objRef,IMMEDIATE_CURRENT));
                break;
			case (PUTF SHIFT24):
                objRefVal = popRef();
				objRef = popRef();
				*(ObjRef *)getIndexedObjRef(objRef,IMMEDIATE_CURRENT) -> data = objRefVal;
				break;
			case (NEWA SHIFT24):
                break;
			case (GETFA SHIFT24):
                break;
			case (PUTFA SHIFT24):
                break;
			case (GETSZ SHIFT24):
                break;
			case (PUSHN SHIFT24):
                break;
			case (REFEQ SHIFT24):
                break;
			case (REFNE SHIFT24):
                break;

		}
}

void outputList(int argn, unsigned int program[]){
	int i;
	for(i = 0; i < argn; i++){
		printLine(i,3);
	}
}
void execute(int argn, unsigned int program[]){
	int i;

	if(argn < MEMORY_SIZE){
		for(i = 0; i < argn; i++){
			program_memory[i] = program[i];
		}
	} else {
		error("System ran out of memory.");
	}
	
	for(state = 0; state < argn; state++){
		if(state >= 0) executeLine(state);
		else break;
	}
	
	
	
}

void debug(int argn, unsigned int program[], int globaln){
	
	int i;
	
	int breakpoint = -1;
	
	int input_int;
	char input[10];
	
	void *pointer;
	StackSlot stackslot;
	
	if(argn < MEMORY_SIZE){
		for(i = 0; i < argn; i++){
			program_memory[i] = program[i];
		}
	} else {
		error("System ran out of memory");
	}
	
	while(state <= argn && state >= 0){
		printLine(state, 4);
		printf("DEBUG: inspect, list, breakpoint, step, run, quit?\n");
		scanf("%s", &input[0]);
		if((strcmp(input,"list") == 0 || strcmp(input,"l") == 0)){
			outputList(argn, program);
			printf("\t--- end of code ---\n");
		} else if(strcmp(input,"quit") == 0|| strcmp(input,"q") == 0){
			exit(0);
		} else if(strcmp(input,"inspect") == 0|| strcmp(input,"i") == 0){
			printf("DEBUG [inspect]: stack, data, register, object?\n");
			scanf("%s", &input[0]);
			if(strcmp(input,"stack") == 0|| strcmp(input,"s") == 0){
				if(sp == fp) printf("sp, fp");
				else printf("sp");
				printf("\t--->\t%03d:\tXXXX\n",sp);
				for(i = sp-1; i >= 0; i--){
					if(i == fp) printf("fp\t--->");
					else printf("\t");
					printf("\t%03d:\t",i);
					if(stack[i].isObjRef){
						printf("Ref: %p\n", (void*)&stack[i]);
					}
					else {
						printf("Int: %d\n",stack[i].u.number);
					}
				}
			printf("\t\t --- bottom of stack ---\n");
			} else if(strcmp(input,"data") == 0|| strcmp(input,"d") == 0) {
				for(i = 0; i < globaln; i++){
					printf("data[%04d]:\t%d\n", i, *(int *)global[i].u.objRef -> data);
				}
				printf("\t --- bottom of data ---\n");
			} else if(strcmp(input,"register") == 0|| strcmp(input,"r") == 0) {
				for(i = 0; i < REGISTER_SIZE; i++){
					printf("register[%02d]:\t%d\n", i, *(int *)return_register[i].u.objRef);
				}
				printf("\t --- bottom of register ---\n");
			} else if(strcmp(input,"object") == 0|| strcmp(input,"o") == 0) {
				printf("Object reference?\n");
				if(scanf("%p", &pointer) IS_TRUE){
					stackslot = *(StackSlot *) pointer;
					printf("value : ");
					bip.op1 = stackslot.u.objRef;
					bigPrint(stdout);
					printf("\n");
				}
			}
		} else if(strcmp(input,"breakpoint") == 0|| strcmp(input,"b") == 0){
			printf("DEBUG [breakpoint]: ");
			printf(breakpoint >= 0 ? "set at %d\n" : "cleared\n", breakpoint);
			printf("DEBUG [breakpoint]: address to set, -1 to clear?\n");
			if(scanf("%d", &input_int) IS_TRUE){
				if(input_int == -1) {
					breakpoint = -1;
					printf("DEBUG [breakpoint]: now cleared\n");
				} else {
					breakpoint = input_int;
					printf("DEBUG [breakpoint]: now set at %d\n", input_int);
				}
				
			}
		} else if(strcmp(input,"step") == 0|| strcmp(input,"s") == 0){
				executeLine(state);
				state++;
		} else if(strcmp(input,"run") == 0|| strcmp(input,"r") == 0){
			for(state = state; state != (breakpoint >= 0 ? breakpoint : argn); state++){
				if(state <= argn && state >= 0) executeLine(state);
				else break;
			}
		}	
	}	
}
