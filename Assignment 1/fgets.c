#include <stdio.h>
#include <string.h>

int main(){
	char str[80];
	int i;

	printf("Enter string: ");
	fgets(str, 80, stdin);

	 /*removing newline*/
    i = strlen(str) -1;
    if(str[i] == '\n')
      str[i] = '\0';

  printf("String is: %s", str);

	return 0;
}