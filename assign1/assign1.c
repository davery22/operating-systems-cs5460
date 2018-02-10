#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/syscall.h> 
#include <unistd.h>
#include <fcntl.h>

/*********************************************************************
 *
 * These C functions use patterns and functionality often found in
 * operating system code. Your job is to implement them. Of course you
 * should write test cases. However, do not hand in your test cases
 * and (especially) do not hand in a main() function since it will
 * interfere with our tester.
 *
 * Additional requirements on all functions you write:
 *
 * - you may not refer to any global variables
 *
 * - you may not call any functions except those specifically
 *   permitted in the specification
 *
 * - your code must compile successfully on CADE lab Linux
 *   machines when using:
 *
 * /usr/bin/gcc -O2 -fmessage-length=0 -pedantic-errors -std=c99 -Werror -Wall -Wextra -Wwrite-strings -Winit-self -Wcast-align -Wcast-qual -Wpointer-arith -Wstrict-aliasing -Wformat=2 -Wmissing-include-dirs -Wno-unused-parameter -Wshadow -Wuninitialized -Wold-style-definition -c assign1.c 
 *
 * NOTE 1: Some of the specifications below are specific to 64-bit
 * machines, such as those found in the CADE lab.  If you choose to
 * develop on 32-bit machines, some quantities (the size of an
 * unsigned long and the size of a pointer) will change. Since we will
 * be grading on 64-bit machines, you must make sure your code works
 * there.
 *
 * NOTE 2: You should not need to include any additional header files,
 * but you may do so if you feel that it helps.
 *
 * HANDIN: submit your finished file, still called assign1.c, in Canvas
 *
 *
 *********************************************************************/

/*********************************************************************
 *
 * byte_sort()
 *
 * specification: byte_sort() treats its argument as a sequence of
 * 8 bytes, and returns a new unsigned long integer containing the
 * same bytes, sorted numerically, with the smaller-valued bytes in
 * the lower-order byte positions of the return value
 * 
 * EXAMPLE: byte_sort (0x0403deadbeef0201) returns 0xefdebead04030201
 *
 *********************************************************************/

unsigned long byte_sort (unsigned long arg)
{
    /* Pardon, wanted to try something different, ie sorting networks */
    //unsigned char *bytes = (unsigned char*)&arg; -- hmm looks like super-safety mode won't let us be this clever
    union longbytes {
        unsigned char b[sizeof(unsigned long)];
        unsigned long l;
    };
    union longbytes bytes;
    bytes.l = arg; // so here's something even worse! >:)
#define min(x,y) (x<y?x:y)
#define max(x,y) (x<y?y:x)
#define SWAP(x,y) { const unsigned char a = min(bytes.b[x], bytes.b[y]); \
                    const unsigned char b = max(bytes.b[x], bytes.b[y]); \
                    bytes.b[x] = a; bytes.b[y] = b; }
    SWAP(0, 1);
    SWAP(2, 3);
    SWAP(0, 2);
    SWAP(1, 3);
    SWAP(1, 2);
    SWAP(4, 5);
    SWAP(6, 7);
    SWAP(4, 6);
    SWAP(5, 7);
    SWAP(5, 6);
    SWAP(0, 4);
    SWAP(1, 5);
    SWAP(1, 4);
    SWAP(2, 6);
    SWAP(3, 7);
    SWAP(3, 6);
    SWAP(2, 4);
    SWAP(3, 5);
    SWAP(3, 4);
#undef min
#undef max
#undef SWAP
    return bytes.l;
}

/*********************************************************************
 *
 * nibble_sort()
 *
 * specification: nibble_sort() treats its argument as a sequence of 16 4-bit
 * numbers, and returns a new unsigned long integer containing the same nibbles,
 * sorted numerically, with smaller-valued nibbles towards the "small end" of
 * the unsigned long value that you return
 *
 * the fact that nibbles and hex digits correspond should make it easy to
 * verify that your code is working correctly
 * 
 * EXAMPLE: nibble_sort (0x0403deadbeef0201) returns 0xfeeeddba43210000
 *
 *********************************************************************/

unsigned long nibble_sort (unsigned long arg)
{
    /* Okay, back to old insertion sort (with some evil) */
#define EXTRACT(x) ((arg >> ((x) << 2)) & 0xf)
#define PLACE(x,y) { unsigned long mask = (~0UL >> ((15-(x)) << 2)) & (~0UL << (((y)+1) << 2)); \
                     arg = ((arg << 4) & mask) | (EXTRACT(x) << ((y) << 2)) | (arg & ~mask & ~(0xfL << ((y) << 2))); }
    int i, j;
    for (i = 1; i < 16; i++) {
        for (j = i-1; j >= 0 && EXTRACT(j) > EXTRACT(i); j--) {}
        PLACE(i,j+1)
    }
#undef EXTRACT
#undef PLACE
    return arg;
}

/*********************************************************************
 *
 * name_list()
 *
 * specification: allocate and return a pointer to a linked list of
 * struct elts
 *
 * - the first element in the list should contain in its "val" field the first
 *   letter of your first name; the second element the second letter, etc.;
 *
 * - the last element of the linked list should contain in its "val" field
 *   the last letter of your first name and its "link" field should be a null
 *   pointer
 *
 * - each element must be dynamically allocated using a malloc() call
 *
 * - if any call to malloc() fails, your function must return NULL and must also
 *   free any heap memory that has been allocated so far; that is, it must not
 *   leak memory when allocation fails
 *  
 *********************************************************************/

struct elt {
  char val;
  struct elt *link;
};

struct elt *name_list (void)
{
    /* *Unsafe* version (requires contiguous memory)
    const char* name = "Daniel";
    int len = 6;
    struct elt *list = malloc(len*sizeof(struct elt));

    if (list) {
        int i;
        for (i = 0; i < len; i++) {
            (list + i)->val = name[i];
            (list + i)->link = (i < len-1) ? (list + i + 1) : NULL;
        }
    }

    return list;*/


    /* *Safe* version (does not require contiguous memory) */
    const char *name = "Daniel";
    struct elt *head = NULL, *cur = NULL;

    int i;
    for (i = 0; name[i]; i++) {
        struct elt *next = malloc(sizeof(struct elt));
        if (!next) {
            // Malloc failure -- clean up and return
            while (head) {
                next = head->link;
                free(head);
                head = next;
            }
            return NULL;
        }

        if (!i) {
            head = cur = next;
        }
        else {
            cur->link = next;
            cur = cur->link;
        }

        cur->val = name[i]; 
        cur->link = NULL;
    }

    return head;
}

/*********************************************************************
 *
 * convert()
 *
 * specification: depending on the value of "mode", print "value" as
 * octal, binary, or hexidecimal to stdout, followed by a newline
 * character
 *
 * extra requirement 1: output must be via putc()
 *
 * extra requirement 2: print nothing if "mode" is not one of OCT,
 * BIN, or HEX
 *
 * extra requirement 3: all leading/trailing zeros should be printed
 *
 * EXAMPLE: convert (HEX, 0xdeadbeef) should print
 * "00000000deadbeef\n" (including the newline character but not
 * including the quotes)
 *
 *********************************************************************/

enum format_t {
  OCT = 66, BIN, HEX
};

void convert (enum format_t mode, unsigned long value)
{
    char mask, shift;
    switch (mode) {
        case BIN:
            mask = 0x1; shift = 1;
            break;
        case OCT:
            mask = 0x3; shift = 2;
            break;
        case HEX:
            mask = 0xf; shift = 4;
            break;
        default:
            return;
    }

    char cur_shift;
    for (cur_shift = 64-shift; cur_shift >= 0; cur_shift -= shift) {
        char next_val = (value >> cur_shift) & mask;
        next_val = (next_val < 10) ? next_val + '0' : next_val - 10 + 'a';
        putc(next_val, stdout);
    }

    putc('\n', stdout);
}

/*********************************************************************
 *
 * draw_me()
 *
 * this function creates a file called me.txt which contains an ASCII-art
 * picture of you (it does not need to be very big). the file must (pointlessly,
 * since it does not contain executable content) be marked as executable.
 * 
 * extra requirement 1: you may only call the function syscall() (type "man
 * syscall" to see what this does)
 *
 * extra requirement 2: you must ensure that every system call succeeds; if any
 * fails, you must clean up the system state (closing any open files, deleting
 * any files created in the file system, etc.) such that no trash is left
 * sitting around
 *
 * you might be wondering how to learn what system calls to use and what
 * arguments they expect. one way is to look at a web page like this one:
 * http://blog.rchapman.org/post/36801038863/linux-system-call-table-for-x86-64
 * another thing you can do is write some C code that uses standard I/O
 * functions to draw the picture and mark it as executable, compile the program
 * statically (e.g. "gcc foo.c -O -static -o foo"), and then disassemble it
 * ("objdump -d foo") and look at how the system calls are invoked, then do
 * the same thing manually using syscall()
 *
 *********************************************************************/

void draw_me (void)
{
    const char *me = "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n"
                     "&&&&&&&&&&&&&&&&&&&&&&&&&%(*.                     .*(%&&&&&&&&&&&&&&&&&&&&&&&&&\n"
                     "&&&&&&&&&&&&&&&&&&&&(,        . ........,,.......        ,(&&&&&&&&&&&&&&&&&&&&\n"
                     "&&&&&&&&&&&&&&&&%,            ...,,//((((#%%#(((/*,....      ,%&&&&&&&&&&&&&&&&\n"
                     "&&&&&&&&&&&&&/         .   ..,*#%%%&&&&@@@@@@@@&&&&%(,......     /&&&&&&&&&&&&&\n"
                     "&&&&&&&&&&/     .      . ./#&&&&&&&&@@@@@@@@@@@@@&@@@@*.......    /&&&&&&&&&&&&\n"
                     "&&&&&&&&/       ..   ...,#%#%&@@@@@@@@@@@@@@@@@@@@@@&@@@@#*.......    /&&&&&&&&\n"
                     "&&&&&%           . .*(####%%&&&&@@@@@@@@@@@@@@@@@@@@@&@@@@,........    %&&&&&&&\n"
                     "&&&&.          . .*##%%&%&&&&&@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%*.........   .&&&&\n"
                     "&&(   .    ..  .,/##%%&&%%&&&@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@,..........   (&&&&\n"
                     "           .,/####%%%%%%%%&%&&&&&@@@@@@@@@@@@@@@@@@@@@@@@@@&(,...........   #&&\n"
                     "            .,((((##%%%%##((#####%%&&&&&&&@@@@@@@@@@@@@@@@@@@@&%,...........   \n"
                     "            .(#(((#((###(//////(((##%%%&&&&&&&&&&&@@@@@@@@@@@@@%*,............ \n"
                     "    .       ,#((/#(/(#((//**,***//((##%%&%&&%&&&&&&&&@@@@@@@@@@%#*............ \n"
                     "           ./(/((((###(/*,.....,,**/((##%%%%%%%%%&&&&&&@@@@@@@@@%(*............\n"
                     "           *((/((##%%(,......,,,,,,**/((##%%%%%%%%%&&&&&&@@@@@@&&%((,..........\n"
                     "          ./((/(##%&/....,///((///***//((######%%%%%%%%%&@@@@@@@(/*............\n"
                     "        ,((((#%&&*....,*//#%&@@&&&%(((//((((####%%%%%%%&@@@@@%%%(/,............\n"
                     "          *(###%%%/..  ..,***//(##%%&%&%%((((((##########%@@@@@&(**,...........\n"
                     "        *(#(####,.. .*/#%&((%@@@&%((###((((###%%%%%%%##%@@@@@(,................\n"
                     "         ,(/(((#,.....,*/*,,/%&&%%&%(***,*/(#%&@@@@@@@&&&@@@&@(**,.............\n"
                     "          .*/(//*.......,,*////(##%#/*,...,*(%&@@@@@&&@@@&@@@%(/,..............\n"
                     "           ,*(/*,........,,*//(((//**,,...,*(#&@@@@@@@%%%&@(/,.................\n"
                     "        ....**..    ...,,**//(((/*,,,,,,*/(#%&&&@&&@@#&%#/*,,..................\n"
                     "        .......   .....,,**/((((/*,,,,,,,*(###%&&%%###*,,,.....................\n"
                     "          .,*,.   .....,**/(##%%#*,,,,,,,*(#(#%%####((.........................\n"
                     "             ,/..  ....,,*/(#%%&%#(/,,***,*/###%%%###((,.......................\n"
                     "             *... .....,*(%%&&%(/////////*/#&%%&%%###(*........................\n"
                     "             ,.........*(%&(/****///(##%&&&&&&&%%##(...........................\n"
                     "            ., .......,*(%%%&&((/////(###%%%%%%&&%%##*.........................\n"
                     "            ..  ......,,(##(/(%//.,,*#%&&%%#%%&&&%##/..........................\n"
                     "            .   ......,,*(/*,,*//((//((#%&@@@&&%%##/...........................\n"
                     "           ..   ......,.,***,,,,,*/(####%%%%%%%###*............................\n"
                     "           (*.  .........,,,*****//(##%%%####%##(,.............................\n"
                     "            *.......,,....,,,***//(####%#######/.......... ....................\n"
                     "             .,,..,,,,,,,...,,**///(((######((,............................... \n"
                     "           .    .**,*,,,,,,,,,**//(((######(*................................. \n"
                     "     ..            ,(//********//((##(#####*.................... ...........   \n"
                     "    ,,,,..          .,##////((((((###%%%#/...................... ..........    \n"
                     "     .%/,            .**#%#(((#%%%%####&%........................ .......      \n"
                     "       ,/,.      ...,,.***#%&%%%%####%@@#,.............................        \n"
                     "         ..,    ..,*,.,,,***/%&&%%%%&@@#//(/*,.......................          \n"
                     "            ,*,.**/,,..,***,,,*#%&&@@//(#(/(((##/,............,,.              \n"
                     "              .(/*/*,..,*,,...*(#%%&//(##(/*(####(###*........                 \n"
                     "                 .****.,,....,//(#%&&%(//((%(/**/(##(#%%%%#,.                  \n"
                     "                      ......,**,##%%@%((((((%(//(#&&%(##*                      \n"
                     "                            . ,(%%&@@@&//((##%%#/*.                            \n";

    int len = 3760;
    const char *filename = "me.txt";

    int fd = syscall(SYS_open, filename, O_WRONLY | O_CREAT, S_IRWXU); // S_IRWXU -- user has read/write/execute permissions on new file
    if (fd < 0) {
        // Open failed -- just walk away
        return;
    }

    if (syscall(SYS_write, fd, me, len) != len) {
        // Write failed -- best effort close/delete the file and walk away
        syscall(SYS_close, fd);
        syscall(SYS_unlink, filename);
        return;
    }

    // Best effort close
    syscall(SYS_close, fd);
}
