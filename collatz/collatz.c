#include <stdio.h>
#include <cs50.h>

int collatz(int s, int c);

int main(void){
    // Get the number the user wants to recurse;
    printf("Collatz conjecture, for every positive integer value, it will always return to 1\n");
    int n;
    do{
        printf("n: ");
        n = get_int();
    } while(n < 1);

    int steps = collatz(0, n);
    printf("It took %i steps to return to 1 using Collatz Conjecture\n", steps);
    return 0;
}

// Returns number of steps to get to 1, note, works on every positive integer only
int collatz(int s, int c){
    // Return number of steps tranversed, if returned to 1
    // Base case of recursion
    if (c == 1){
        return s;
    } else {
        // Recursive case of function
        // If c is even
        if((c % 2) == 0){
            return collatz((s + 1), (c / 2));
        } else if ((c % 2) != 0){
            // if c is odd
            return collatz((s + 1), ((3 * c) + 1));
        }
    }

    // To make the compiler happy, my inputs are sanitized
    return 0;
}