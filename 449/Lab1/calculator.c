#include <stdio.h>

int main() {
    int n1;
    int n2;
    char op;
    int check1 = 1;

    while(check1 == 1) {
        
       printf("Enter your calculation: ");
       if (scanf("%d %c %d", &n1, &op ,&n2 ) != 3) {
        printf("\n");   
	printf("Invalid calculation! \"%d %c %d\"\n", n1,op,n2);
		check1 = 0;           
		break;
       }
       int res;
       int check = 1;
       switch (op) {
            case '+':
                res = n1 + n2;
                break;
            case '-':
                res = n1 - n2;
                break;
            case '*':
                res = n1 * n2;
                break;
            case '/':
                if(n2 == 0){
                    check = 0;
		    printf("\n");
                    printf("Invalid calculation! \"%d %c %d\"\n", n1,op,n2);
                    continue;
                }else{
                    res = n1 / n2;
                    break;
                }
            case '%':
                if(n2 == 0){
                    check = 0;
		    printf("\n");
                    printf("Invalid calculation! \"%d %c %d\"\n", n1,op,n2);
                    continue;
                }else{
                    res = n1 % n2;
                    break;
                }
            case '&':
                res = n1 & n2;
                break;
            default:
                check = 0;
		check1 = 0;
		printf("\n");
                printf("Invalid calculation! \"%d %c %d\"\n", n1,op,n2);
                break;
        }
        if(check == 1){
	    printf("\n");
            printf(  "> \%d %c %d = %d", n1,op,n2,res);
	    printf("\n");
            
        }
    }

    return 0;
}
