#!/usr/bin/env python3


from random import randint as rint
from random import uniform as rfloat

import argparse



def printInfo(row , column , Enum , tp ):

    print("Generate random data for sparse matrix")
    print("Number of rows              : {}".format(row))
    print("Number of columns           : {}".format(column))
    print("Number of non-zero element  : {}".format(Enum))
    print("Type of elements            : {}".format(tp))

def generate(row , column , Enum , tp ):
    
    bd = None 
    func = 0
    needRound = False

    if tp == 'int':
        func = rint
        bd = 0,10000
    elif tp == 'float':
        func = rfloat
        bd = 0.0 , 10000.0
        needRound = True

    print(row , column , Enum)

    s = {}

    while(len(s) != Enum):
        rowpos = rint(0 , row - 1)
        colpos = rint(0, column - 1 )
        value = func(*bd)
        if needRound:
            value = round(value , 6)
        s[(rowpos , colpos)] = value

    for k ,v in s.items():
        print("{} {} {}".format(k[0],k[1], v))



if __name__ == '__main__':

    parser = argparse.ArgumentParser(description='Test data generator')
    parser.add_argument('-M' , '--row' , type=int , help='row number of input matrix',default=100)
    parser.add_argument('-N' , '--column' , type=int , help='column number of input matrix',default=200)
    parser.add_argument('-E' , '--Enum' , type=int , help='number of elements',default=5)
    parser.add_argument('-T' , '--type' , type=str , help='data type of matrix element',default='int')
    parser.add_argument('-V' , '--verbose' , help='Be verbose for debuging purpose',default=False , action='store_true')
    parser.add_argument('-R' , '--random' ,help='Use random data for M and N as well',default=False , action='store_true')
    
    args = parser.parse_args()

    UpperBound = 100

    datatype = args.type
    verbose = args.verbose
    randomAll = args.random
    Enumber = args.Enum

    row , column = 0,0

    if randomAll :
        row = rint(1,UpperBound)
        column = rint(1,UpperBound)
        Enumber = rint(0 , (row*column)//2 - 1 )
    else:
        row = args.row
        column = args.column


    if verbose:
        printInfo(row , column ,Enumber , datatype )

    generate(row , column ,Enumber , datatype )

    if verbose:
        print("Done.")

