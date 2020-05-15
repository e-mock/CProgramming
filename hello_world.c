#include <stdio.h>

int add(int a, int b){
   int sum = a + b;
   printf("Added %d and %d to get sum %d", a, b, sum);
        
   return sum;
}

int divide(int a, int b){
   return a / b;
}

void deadCodeFunction(){
   //function that is never called
   int sum = add(1, 3);
}

int main() {
   printf("Hello World!");
   printf("Hello again");
   
   int sum = add(2, 2);
   
   return 0;
}
