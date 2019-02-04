#include <stdio.h>
#include <string.h>
#include <ctype.h>

using namespace std;

int main()
{
    char buffer[256] = {0};
    char password[] = "password";
    char c;
    int pos = 0;
    
    printf("%s", "Enter password: ");
    do {
        scanf ("%c", &c);
        
        if( isprint(c) ) 
        {
            buffer[ pos++ ] = c;
            printf("%c", '*');
        }
        else if( c == 8 && pos )
        {
            buffer[ pos-- ] = '\0';
            printf("%s", "\b \b");
        }
    } while( c != 13 );
    
    if( !strcmp(buffer, password) )
        printf("\n%s\n", "Logged on succesfully!");
    else
        printf("\n%s\n", "Incorrect login!");

}
