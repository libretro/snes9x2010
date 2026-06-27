#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libretro/libretro-common/streams/file_stream.h"

START_TEST(test_filestream_gets_bounds)
{
    // Invariant: filestream_gets never writes beyond the allocated buffer size
    const struct {
        size_t buffer_size;
        const char *test_data;
    } test_cases[] = {
        {5, "ABCDEFGHIJ\n"},      // Exploit case: buffer too small for data + newline
        {1, "A\n"},               // Boundary case: minimal buffer
        {10, "Hello\n"},          // Valid input: fits with room
        {3, "AB\n"},              // Boundary: exact fit with newline
        {4, "NoNewlineEOF"}       // Case without newline before EOF
    };
    
    int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);
    
    for (int i = 0; i < num_cases; i++) {
        size_t buffer_size = test_cases[i].buffer_size;
        const char *test_data = test_cases[i].test_data;
        
        // Create a temporary file with test data
        FILE *tmp = tmpfile();
        fwrite(test_data, 1, strlen(test_data), tmp);
        rewind(tmp);
        
        // Convert to RFILE
        RFILE *stream = filestream_open_from_stdio(tmp);
        
        // Allocate buffer with guard pages
        char *buffer = malloc(buffer_size);
        char *guard = malloc(1);
        *guard = 0xAA;
        
        // Call the actual production function
        char *result = filestream_gets(stream, buffer, buffer_size);
        
        // Check guard byte wasn't corrupted
        ck_assert_msg(*guard == 0xAA, 
                     "Buffer overflow detected: guard byte corrupted at buffer_size=%zu, data='%s'",
                     buffer_size, test_data);
        
        // Verify null termination within bounds
        if (result != NULL) {
            size_t len = strlen(buffer);
            ck_assert_msg(len < buffer_size,
                         "String length %zu exceeds buffer size %zu for data='%s'",
                         len, buffer_size, test_data);
        }
        
        // Cleanup
        filestream_close(stream);
        fclose(tmp);
        free(buffer);
        free(guard);
    }
}
END_TEST

Suite *security_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Security");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_filestream_gets_bounds);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = security_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}