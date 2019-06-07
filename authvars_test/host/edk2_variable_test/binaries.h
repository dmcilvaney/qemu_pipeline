/*
 * For a file "file.bin", EXTERNAL_BIN(file) maps the linker symbol
 *  '_binary_file_bin_start' ->  static const char *file;
 *  '_binary_file_bin_size'  ->  static const unsigned int file_size;
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#define EXTERNAL_BIN(name)   extern unsigned char _binary_##name##_bin_start; \
                                    extern unsigned char _binary_ ##name##_bin_size; \
                                    static const char const * name = (char *)(& _binary_##name##_bin_start); \
                                    static const unsigned int name##_size = (unsigned int)(& _binary_##name##_bin_size);

EXTERNAL_BIN(test);
EXTERNAL_BIN(Test_db);
EXTERNAL_BIN(Test_KEK);
#pragma GCC diagnostic pop