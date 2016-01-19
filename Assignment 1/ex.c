/* strtok example */
#include <stdio.h>
#include <string.h>

int main ()
{
  char str[] ="/home/srishti/Documents/Security Engineering/MoiFS/Assignment 1/simple_slash/newfile|u2|u1,u3|u4";
  char * pch;
  printf ("Splitting string \"%s\" into tokens:\n",str);
  pch = strtok (str,"|");
  while (pch != NULL)
  {
    printf ("%s\n",pch);
    pch = strtok (NULL, "|");
  }

  printf("-----%s\n", str);
  return 0;
}