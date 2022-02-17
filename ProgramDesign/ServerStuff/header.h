#ifndef HEADER
#define HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define Type int
#define FormatString "%d\t"

typedef struct Item{
	int row ;
	int col ;
	Type element ;
}Item ; 


typedef struct Info{
	int number_of_elements_of_current_col ;
	int location;
}Info;


typedef struct CompressedMatrix{
    int row ;
    int column ;
    int NonZeroNum ;
    Item* location ;

}CompressedMatrix;



typedef enum Operations  { ADD , SUB } Operations ;


void Input(CompressedMatrix * cpmatrix , char * data );
void Destroy(CompressedMatrix * cpmatrix);
void Print(CompressedMatrix * cpmatrix);
CompressedMatrix* Transpose(CompressedMatrix* cpmatrix );
void Sort(CompressedMatrix * cpmatrix);
void p(CompressedMatrix * cpmatrix);

CompressedMatrix* Addsub(CompressedMatrix * a , CompressedMatrix * b  , Operations op);

CompressedMatrix* Multiply(CompressedMatrix * a , CompressedMatrix * b );
#endif 
