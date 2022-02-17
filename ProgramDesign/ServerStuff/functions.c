#include "header.h"

void Input(CompressedMatrix * cpmatrix , char * data ){

    int index = 0;
    char line [50];
    char * ptr = data ;

    while(*ptr != '\n' && *ptr != '\0'){
        line[index++] = *ptr++;
    }
    line[index] = '\0';
    sscanf(line , "%d %d %d" , &(cpmatrix->row)  , &(cpmatrix->column)  , &(cpmatrix->NonZeroNum));

    cpmatrix->location = (Item*)malloc(sizeof(Item) * cpmatrix->NonZeroNum);
    int end = cpmatrix->NonZeroNum ;
    Item* start = cpmatrix->location;

    for(int i = 0 ; i < end ; i++){

        index = 0 ;
        ptr++;
        while(*ptr != '\n' && *ptr != '\0'){
            line[index++] = *ptr++;
        }
        line[index] = '\0';
        
        sscanf( line , "%d %d %d" , &(start[i].row), &(start[i].col), &(start[i].element));
    }
}


/* ------------------- sort ------------------------ */

inline static int key(Item it , int rowlen ){
   return  it.row * rowlen + it.col;
}

inline static void swap(Item * a, Item * b){
    Item temp = *a;
    *a = *b;
    *b = temp;
}

static int partition(Item * array , int low , int high , int rowlen ){
    Item pivot = array[low];
    int pkey = key(pivot , rowlen);
    int i = low , j = low + 1;
    while( j <= high ){
        if ( key(array[j] , rowlen) <= pkey ){
            swap(&array[++i], &array[j]);
        }
        j++;
    }
    swap(&array[low], &array[i]);
    return i ;
}

static void quicksort(Item * array , int start , int end , int rowlen ){
    if (end <= start ) return; 
    int position = partition(array, start , end  , rowlen);
    quicksort(array, start, position -1 ,rowlen);
    quicksort(array, position+1, end ,rowlen);
}

void Sort(CompressedMatrix * cpmatrix ){
    Item * start = cpmatrix->location;
    quicksort(start, 0, cpmatrix->NonZeroNum -1 , cpmatrix->row);
}

/* ------------------- sort ------------------------ */

/* ------------------- print ------------------------ */

void Print(CompressedMatrix * cpmatrix){
    int r = cpmatrix->row;
    int c = cpmatrix->column;
    int size = c * r ;

    Type* temp = (Type*)calloc(size , sizeof(Type) );

    int end = cpmatrix->NonZeroNum ;

    Item* start = cpmatrix->location;

    for(int i = 0 ; i < end ; i++){
        temp[ start[i].row * c + start[i].col ] = start[i].element;
    }

    for(int i = 0 ; i < r ; i++){
        printf("[");
        for (int j = 0 ; j < c ; j++){
           printf(FormatString , temp[i*c + j]); 
        }
        printf("]\n");
    }
    free(temp);
}


void p(CompressedMatrix * cpmatrix){
    int end = cpmatrix->NonZeroNum ;
    Item * start = cpmatrix->location;
    for(int i = 0 ; i < end ; i++){
        printf("[%d\t,%d\t]:%d\n" , start[i].row , start[i].col , start[i].element);
    }
}


/* ------------------- print ------------------------ */

static void transpose(Item* compressed ,Item * transposed ,  int columnLength , int Enum){
    Info * information = (Info*)calloc(sizeof(Info) , columnLength);

	//loop over compressed to get number of elements of current column
	for (int i = 0 ; i < Enum ; i++){
		information[compressed[i].col].number_of_elements_of_current_col++;
	}
	//loop over information to generate usable location
	for (int i = 1 ; i < columnLength ; i++){
		information[i].location = information[i-1].location + information[i-1].number_of_elements_of_current_col;
	}
	//loop over compressed & transposed to do the transposing
	for (int i = 0 ; i < Enum ; i++){
		int index = information[compressed[i].col].location++;
		transposed[index].row = compressed[i].col;
		transposed[index].col = compressed[i].row;
		transposed[index].element = compressed[i].element;
	}

    free(information);
}


CompressedMatrix* Transpose(CompressedMatrix* cpmatrix ){

    CompressedMatrix* res = (CompressedMatrix*)malloc(sizeof(CompressedMatrix));

    res->row = cpmatrix->column;
    res->column = cpmatrix->row;
    res->NonZeroNum = cpmatrix->NonZeroNum;
    res->location = (Item*)calloc(cpmatrix->NonZeroNum  , sizeof(Item));

    transpose(cpmatrix->location, res->location, cpmatrix->column , cpmatrix->NonZeroNum);
    return res;
}


void Destroy(CompressedMatrix * cpmatrix){
    free(cpmatrix->location);
    free(cpmatrix);
}


// res = a op b
CompressedMatrix* Addsub(CompressedMatrix * a , CompressedMatrix * b  , Operations op){
   if (a->row != b->row || a->column != b->column ){
        fprintf(stderr , "Size error in Addsub\n");
        return NULL ;
   }

   CompressedMatrix * res = (CompressedMatrix*)malloc(sizeof(CompressedMatrix));
   res->row = a->row;
   res->column = a->column;
   int bufsize = a->NonZeroNum + b->NonZeroNum;
   Item * buffer = (Item*)calloc(bufsize, sizeof(Item));

   Item * buf = buffer;
   int walka = 0 ;
   int enda = a->NonZeroNum ; 
   Item * starta = a->location ;
   int walkb = 0 ;
   int endb = b->NonZeroNum ; 
   Item * startb = b->location ;

   int rowlen = a->row ;

   int same = 0;


#define  PlaceA    *buf = starta[walka]
#define  PlaceB    *buf = startb[walkb]

#define  OperationA  PlaceA;walka++
#define OperationB \
           PlaceB; \
           if (op == SUB){ \
           buf->element = -(buf->element); \
           } \
           walkb++
   
   while(walka < enda  && walkb < endb){
       int ka = key(starta[walka], rowlen);
       int kb = key(startb[walkb], rowlen);

       if (ka < kb){
           OperationA;
       }
       else if (ka == kb){
           same++ ;
           PlaceA ;
           if (op == ADD){
               buf->element = starta[walka].element  + startb[walkb].element;
           }
           else if (op == SUB){
               buf->element = starta[walka].element  - startb[walkb].element;
           }
           else{
               fprintf(stderr , "Operation error\n"); return  NULL;
           }
           walka++ ; walkb++;
       }
       else{
           OperationB;
       }
       buf++;
   }

   while(walka < enda){
       OperationA;
       buf++;
   }

   while(walkb < endb){
       OperationB;
       buf++;
   }

   int ressize = a->NonZeroNum + b->NonZeroNum - same ;
   res->NonZeroNum = ressize ;

   res->location = (Item * )malloc(sizeof(Item) * ressize);

   memcpy(res->location, buffer, sizeof(Item) * ressize );

   free(buffer);

   return  res;
}


/* ----------------------- multiplication ------------------------ */

//return the number of element in a row
//start : the beginning of a row
static int getrowEnd(Item * start , int size ){
    int cot = 1;
    int r = start[0].row;
    while(cot < size){
        if(start[cot].row == r){
            cot++;
        }
        else{
            break;
        }
    }
    return  cot ;
}



//return a new matrix which is the result of a*b

CompressedMatrix* Multiply(CompressedMatrix * a, CompressedMatrix * b ){
    if (a->column != b->row){
        fprintf(stderr, "Dimension error in Multiply\n");
        return  NULL ;
    }

    CompressedMatrix* temp = Transpose(b);
    CompressedMatrix* res = (CompressedMatrix*)malloc(sizeof(CompressedMatrix));
    res->row = a->row;
    res->column = b->column;

    int bufsize = a->NonZeroNum * b->NonZeroNum;
    Item * buffer = (Item*)malloc(sizeof(Item) * bufsize );
    
    Item* starta = a->location;
    int enda = a->NonZeroNum;

    Item* buf = buffer;

    int stepa = 0 ;
    int stepb = 0 ;

    while(enda){

        Item* startb = temp->location;
        int endb = temp->NonZeroNum;

        stepa = getrowEnd(starta, enda);
        
        while(endb){
            stepb = getrowEnd(startb, endb);

            Type sum = 0;
            int flag = 0;
            for (int i = 0 ; i < stepa ; i++){
                for(int j = 0 ; j < stepb ; j++){
                    if (starta[i].col == startb[j].col){
                        sum += starta[i].element * startb[j].element;
                        flag = 1;
                        break;
                    }
                }
            }
            if (flag){
                buf->row = starta[0].row;
                buf->col = startb[0].row;
                buf->element = sum;
                buf++;
            }

            startb += stepb;
            endb -= stepb;
        }


        starta += stepa;
        enda -= stepa;
        
        //debug
        //printf("\n%d\n" , enda);
    }


    int cot = buf - buffer;
    res->NonZeroNum = cot ;
    res->location = (Item*)malloc(sizeof(Item) * cot );
    memcpy(res->location , buffer , sizeof(Item) * cot );


    free(buffer);
    free(temp);

    return  res ;
}


/* ----------------------- multiplication ------------------------ */
