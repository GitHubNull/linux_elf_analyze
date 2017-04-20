/*
 * A test program for GeekOS user mode
 */

#include <conio.h>

int main(int argc, char* argv[])
{
    int i;
    Print_String("I am a baby\tb\n");
    for (i = 0; i < argc; ++i) {
	Print("Arg %d is %s\n", i,argv[i]);
    }
    return 1;
}
