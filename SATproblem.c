//Aishwarya Afzulpurkar

#include <stdio.h>

int max(int a, int b){ //for positive ints only
    if (a >b)
        return a;
    else return b;
}


int main(){
    //scan the number of variables n and constraints
    int n, constraints;
    scanf("%d %d", &n, &constraints);
    
    // create an nxn matrix A
    // i denotes the number of Esets (strongly connected components)
    // maximum allocation horizontally means no equality constraints
    // j denotes the number of members in a given Eset
    // maximum allocation vertically means no inequality constraints
    int A[n][n]; // i,j
    int i = 0;
    int j = 0;
    // Each (i,j) value is associated with a variable a
    //
    // Functions used:
    //
    // to assign new (i,j) value to a:
    // -point a to A[i,j]
    //
    // if (j of a) == (j of b)
    //    a and b are in same Eset
    //
    // add a to Eset of b
    //    assign new (i,j) value to a
    //    (i of a) = (i of b)
    //    (j of a) = (j of b) + 1
    
    //define INDEX to keep track of which variables have been seen before
    //INDEX is the max(x,y) the integer value of the input variables x and y
    int INDEX = 0;
    
    // fill up matrix according to equality/nonequality constraints
    // ie. if x==y, they are in the same Eset
    
    // with every scanf it detects:
    // x_index: index of the input vars x
    int x_index;
    // y_index: and y
    int y_index;
    // op: == or !=
    char* op;
    
    //scanning through the constraints
    while (constraints != 0){ // !=0 or =! -1 ?
        
        
        //scan input and separate out the variables and operator
        scanf("%d %s %d", &x_index, op, &y_index);
        //if both x and y were not seen before
        if ((x_index > INDEX) && (y_index > INDEX)){
            //add x_index and y_index to a new set
            
            char* x;
            char* y;
            sprintf(x, "%d", x_index); //cast x_index as string
            sprintf(y, "%d", y_index); //cast x_index as string
            
            x = &A[i+1,1];
            y = &A[i+2,1];
            //increment pointer i to end of used horizontal space
            i=i+2;
        }
        
        //if both x and y were seen before
        else if ((x_index < INDEX) && (y_index < INDEX)){
            printf("NOT SATISFIABLE");
            return 0;

        }else{
            //check based on operator
            if (strcmp(op, "==") == 0){ //if this is an equality constraint
                //if x was seen before, but y was not
                if ((x_index < INDEX) && (y_index > INDEX)){
                    int i_x;
                    int j_x;
                    //add y to the Eset of x
                    
                    //cast x_index into string x
                    char x[n];
                    sprintf(x, "%d", x_index); //cast x_index as string
                    //compare string x at A[i,j] till A[i, j-1] till A[0,j] till A[0,0]
                    for(int a=0; a<i+1; a++){
                        for (int b=0; b<j+1; b++){
                            if (strcmp(x, A[a,b]) == 0){ //A[i,j] of var found!
                                i_x = a;
                            }else if (strcmp (A[i_x, b], "0") == 0){
                                j_x = b-1;
                                break;
                            }
                        }break;
                    }
                    char y[n];
                    sprintf(y, "%d", y_index); //cast y_index as string
                    y= &A[i_x,j_x+1];
                }
                //if y was seen before, but x was not
                else if ((x_index > INDEX) && (y_index < INDEX)){
                    //add x to the Eset of y
                    int i_y;
                    int j_y;
                    //add y to the Eset of x
                    
                    //cast x_index into string x
                    char y[n];
                    sprintf(y, "%d", y_index); //cast x_index as string
                    //compare string y at A[i,j] till A[i, j-1] till A[0,j] till A[0,0]
                    for(int a=0; a<i+1; a++){
                        for (int b=0; b<j+1; b++){
                            if (strcmp(y, A[a,b]) == 0){ //A[i,j] of var found!
                                i_y = a;
                            }else if (strcmp (A[i_y, b], "0") == 0){
                                j_y = b-1;
                                break;
                            }
                        }break;
                    }
                    
                    char x[n];
                    sprintf(x, "%d", x_index); //cast x_index as string
                    x = &A[i_y,j_y+1];
                }
            }
            else if (strcmp(op, "!=") == 0){ //if this is an inequality constraint
                //if x was seen before and y was not
                else if ((x_index < INDEX) && (y_index > INDEX)){
                    //assign y_index new (i,j) value (i+1, 1)
                    char y[n];
                    sprintf(y, "%d", y_index); //cast y_index as string
                    y = &A[i+1,1];
                    i = i+1;
                }
                //if y was seen before and x was not
                else if ((x_index > INDEX) && (y_index < INDEX)){
                    //assign x_index new (i,j) value (i+1, 1)
                    char x[n];
                    sprintf(x, "%d", x_index); //cast x_index as string
                    x = &A[i+1,1];
                    i = i+1;
                }
            }
        }
        
        //increment INDEX value
        INDEX = max(x_index, y_index);
        constraints = constraints -1;
    } //exit while loop
    //end of file is assumed to be reached
    
    //no contradictions detected so system satisfiable
    printf("SATISFIABLE");
    return 0;
    
}