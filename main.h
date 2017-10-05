#ifndef MAIN_H
#define MAIN_H

#ifdef __MINGW32__
// Turns off globbing for MingW, this is the same as that
// CRT_noglob.o, but avoids having to locate CRT_nogob.o in the
// filesystem.
// http://mingw-users.1079350.n2.nabble.com/Problems-with-expanding-wildcards-to-a-program-td1358555.html
int _dowildcard = 0; // Magic variable to disable wildcards expansion in mingw
// in our case fixes param parser crashes on windows
#endif

#include "optionsparser.h"

int main(int argc, char *argv[]);

#endif // MAIN_H
