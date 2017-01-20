#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MSB (1 << (8 * sizeof(unsigned int) - 1))
#define IS_PRIM(objRef) ((( objRef)->size & MSB) == 0)
#define GET_SIZE(objRef) ((objRef)->size & ~MSB)
#define GET_REFS(objRef) ((ObjRef *)(objRef)->data)
#define OBJ_REF(i) *(ObjRef *)getIndexedObjRef(objRef,i)


typedef struct {
    unsigned int size;			/* byte count of payload data */
    unsigned char data[1];		/* payload data, size as needed */
} *ObjRef;

ObjRef getIndexedObjRef(ObjRef origin, int index){
    int i;
    ObjRef *res;
    if(IS_PRIM(origin)) exit(1);
    if(index >= GET_SIZE(origin)) exit(1);
    res = (ObjRef *)GET_REFS(origin);
    for(i = 0; i < index; i++){
        printf("%d", ((ObjRef)res)->size);
        res = (res + ((ObjRef)res) -> size);
    }
    return (ObjRef)res;
}

int main(int argc, char *argv[]){

    int dataSize;
    ObjRef objRef;
    ObjRef objRefdata1;
    ObjRef objRefdata2;
    ObjRef objReff;
    int i;

    objRef = malloc(sizeof(unsigned int) + 2 * sizeof(ObjRef));
    /*objRef = malloc(sizeof(unsigned int) + 2 * sizeof(int)); */
    objRefdata1 = malloc(sizeof(unsigned int) + sizeof(unsigned char));
    objRefdata2 = malloc(sizeof(unsigned int) + sizeof(unsigned char));

    printf("%p\n", (void *)objRefdata1);
    printf("%p\n", (void *)objRefdata2);

    objRef->size = sizeof(int);
    /**(int *)objRef->data = 5; */
    /**(int *)(objRef->data+1) = 11; */

    *(ObjRef *)objRef->data = objRefdata1;
    *(ObjRef *)(objRef->data+1) = objRefdata2;

    int x[] = {1, 2, 3};
    char *p = &x;
    /*printf("%d", *(p+8)); */
    printf("%p\n", (void *)(objRef->data));
    printf("%p\n", (void *)(objRef->data+1));


    int *a, b;


    b = 17;

    a = &b;

    printf("Inhalt der Variablen b:    %i\n", b);

    printf("Inhalt des Speichers der Adresse auf die a zeigt:    %i\n", *a);

    printf("Adresse der Variablen b:   %p\n", (void*)&b);

    printf("Adresse auf die die Zeigervariable a verweist:   %p\n", (void*)a);

    /* Aber */

    printf("Adresse der Zeigervariable a: %p\n", &a);

}