#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "fmt.h"
#include "seq.h"
#include "makeTests.h"

/* The array `tests` contains all unit tests for the lab. */

extern void haltTest(Seq_T stream);
extern void haltAfterTest(Seq_T stream);
extern void addTest(Seq_T stream);
extern void printSizeTest(Seq_T stream);

extern void add_twoZerosTest(Seq_T stream);
extern void add_twoNonzerosTest(Seq_T stream);
extern void add_threeNumsTest(Seq_T stream);
extern void add_toSelfTest(Seq_T stream);
//extern void add_overflowTest(Seq_T stream);

extern void mult_twoZerosTest(Seq_T stream);
extern void mult_twoNonzerosTest(Seq_T stream);
extern void mult_threeNumsTest(Seq_T stream);
extern void mult_toSelfTest(Seq_T stream);
//extern void mult_overflowTest(Seq_T stream);

extern void div_evenIdentityTest(Seq_T stream);
extern void div_oddIdentityTest(Seq_T stream);
extern void div_evenByTwoTest(Seq_T stream);
extern void div_oddByTwoTest(Seq_T stream);
extern void div_bySelfTest(Seq_T stream);

extern void printAllCharactersTest(Seq_T stream);
extern void printGivenCharsTest(Seq_T stream);

static struct test_info {
        const char *name;
const char *test_input; /* NULL if no input */
        /* writes instructions into sequence */
        void (*build_test)(Seq_T stream);
} tests[] = {
        /* {"(NAME)", (FUNCTION(Seq_T stream))} */
        {"HaltTest"           , NULL, haltTest},
        {"HaltTestExtra"      , NULL, haltAfterTest},
        {"AddTest"            , NULL, addTest},
        {"PrintSizeTest"      , NULL, printSizeTest},
        {"add_twoZerosTest"   , NULL, add_twoZerosTest},
        {"add_twoNonzerosTest", NULL, add_twoNonzerosTest},
        {"add_threeNumsTest"  , NULL, add_threeNumsTest},
        {"add_toSelfTest", NULL, add_toSelfTest},
        //{"add_overflowTest", NULL, add_overflowTest},
        {"mult_twoZerosTest"   , NULL, mult_twoZerosTest},
        {"mult_twoNonzerosTest", NULL, mult_twoNonzerosTest},
        {"mult_threeNumsTest"  , NULL, mult_threeNumsTest},
        {"mult_toSelfTest", NULL, mult_toSelfTest},
        //{"mult_overflowTest", NULL, mult_overflowTest},
        {"div_evenIdentityTest", NULL, div_evenIdentityTest},
        {"div_oddIdentityTest", NULL, div_oddIdentityTest},
        {"div_evenByTwoTest", NULL, div_evenByTwoTest},
        {"div_oddByTwoTest", NULL, div_oddByTwoTest},
        {"div_bySelfTest", NULL, div_bySelfTest},
        {"printAllCharsTest"  , NULL, printAllCharactersTest},
        {"printURandom"       , NULL, printGivenCharsTest},
        /* copy in files */
};

  
#define NTESTS (sizeof(tests)/sizeof(tests[0]))

/*
 * open file 'path' for writing, then free the pathname;
 * if anything fails, checked runtime error
 */
static FILE *open_and_free_pathname(char *path);

/*
 * if contents is NULL or empty, remove the given 'path', 
 * otherwise write 'contents' into 'path'.  Either way, free 'path'.
 */
static void write_or_remove_file(char *path, const char *contents);

static void write_test_files(struct test_info *test);


int main (int argc, char *argv[])
{
        bool failed = false;
        if (argc == 1)
                for (unsigned i = 0; i < NTESTS; i++) {
                        printf("***** Writing test '%s'.\n", tests[i].name);
                        write_test_files(&tests[i]);
                }
        else
                for (int j = 1; j < argc; j++) {
                        bool tested = false;
                        for (unsigned i = 0; i < NTESTS; i++)
                                if (!strcmp(tests[i].name, argv[j])) {
                                        tested = true;
                                        write_test_files(&tests[i]);
                                }
                        if (!tested) {
                                failed = true;
                                fprintf(stderr,
                                        "***** No test named %s *****\n",
                                        argv[j]);
                        }
                }
        return failed; /* failed nonzero == exit nonzero == failure */
}


static void write_test_files(struct test_info *test)
{
        FILE *binary = open_and_free_pathname(Fmt_string("%s.um", test->name));
        Seq_T instructions = Seq_new(0);
        test->build_test(instructions);
        Um_write_sequence(binary, instructions);
        Seq_free(&instructions);
        fclose(binary);
         
        write_or_remove_file(Fmt_string("%s.0", test->name),
                             test->test_input);
/*
        write_or_remove_file(Fmt_string("%s.1", test->name),
                             test->expected_output);
         */
}

static void write_or_remove_file(char *path, const char *contents)
{
        if (contents == NULL || *contents == '\0') {
                remove(path);
        } else {
                FILE *input = fopen(path, "wb");
                assert(input != NULL);

                fputs(contents, input);
                fclose(input);
        }
        free(path);
}
 
static FILE *open_and_free_pathname(char *path)
{
        FILE *fp = fopen(path, "wb");
        assert(fp != NULL);

        free(path);
        return fp;
}
