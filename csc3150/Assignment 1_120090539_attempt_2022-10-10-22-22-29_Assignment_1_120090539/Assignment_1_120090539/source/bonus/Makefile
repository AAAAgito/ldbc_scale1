
#[TAB]instruction

#The first object file is the final object
OBJ=DuLinkList.o Qsort.o pstree.o
FT=test
${FT}:${OBJ}
	gcc ${OBJ} -o test
DuLinkList.o:DuLinkList.c DuLinkList.h
	gcc -c DuLinkList.c -o DuLinkList.o
Qsort.o:Qsort.c Qsort.h
	gcc -c Qsort.c -o Qsort.o
pstree.o:pstree.c main.h
	gcc -c pstree.c -o pstree.o
	
#Pseudo target
.PHONY:
clean:
	rm -rf ${OBJ}
	rm -rf ${FT}
	
run:
	make
	make clean
	./${FT}