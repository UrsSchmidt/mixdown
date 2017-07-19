#ifndef LOGGER_HEADER_FILE
#define LOGGER_HEADER_FILE

/* errors */
#define FOPEN_FAILED "fopen failed `%s'"
#define YYPARSE_FAILED "yyparse failed"

/* warnings (cannot be suppressed with -w) */
#define CONSTRAINT_VIOLATION "constraint violation in function `%s()'"
#define DIVISION_BY_ZERO "division by zero"
#define UNDEFINED_FUNCTION "undefined function `%s()'"
#define UNDEFINED_IDENTIFIER "undefined identifier `%s'"

/* optional warnings (can be suppressed with -w) */
#define ENDS_ON_NON_ZERO_SAMPLE "ends on non-zero sample"
#define SAMPLE_CLIPPING "sample clipping occured"
#define STARTS_ON_NON_ZERO_SAMPLE "starts on non-zero sample"

void error(const char* msg, const char* arg);

void warn(const char* msg, const char* arg);

void warn_once(const char* msg, const char* arg);

#endif
