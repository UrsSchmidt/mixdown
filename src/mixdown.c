#include "ast.h"
#include "functions.h"
#include "logger.h"
#include "output.h"
#include "shared.h"
#include "y.tab.h"

#include <argp.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_8  127
#define MAX_16 32767
#define MAX_24 8388607
#define MAX_32 2147483647

extern FILE *yyin;

const char *argp_program_version = "mixdown 1.0";

static char doc[] = "mixdown -- a sound generator for the command line\n"
                    "Example: echo 'sin(A4*t*tau)*fade(t,0,1,l,0);' | mixdown | aplay -q";

static char args_doc[] = "[[input=FILE] output=FILE]";

static struct argp_option options[] = {
    { "bps",        'b', "NUM",  0, "Bit per sample (8, 16, 24, 32) [default=16]" },
    { "channels",   'c', "NUM",  0, "Channels (1, 2) [default=2]" },
    { "format",     'f', "FMT",  0, "File format (AIFF, WAVE) [default=WAVE]" },
    { "length",     'l', "NUM",  0, "Length in seconds [default=1.0]" },
    { "reverse",    'r', 0,      0, "Reverse samples" },
    { "samplerate", 's', "NUM",  0, "Sample rate in Hz [default=44100]" },
    { "verbose",    'v', 0,      0, "Produce verbose output" },
    { "nowarn",     'w', 0,      0, "Suppress some warnings" },
    { 0 }
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    uint32_t i;
    double d;
    switch (key) {
    case 'b':
        i = atoi(arg);
        if (i != 8 && i != 16 && i != 24 && i != 32)
            argp_usage(state);
        arguments->bps = i;
        break;
    case 'c':
        i = atoi(arg);
        if (i != 1 && i != 2)
            argp_usage(state);
        arguments->channels = i;
        break;
    case 'f':
        if (!strcmp(arg, "AIFF"))
            arguments->format = AIFF;
        else if (!strcmp(arg, "WAVE"))
            arguments->format = WAVE;
        else
            argp_usage(state);
        break;
    case 'l':
        d = atof(arg);
        if (d <= 0.0)
            argp_usage(state);
        arguments->length = d;
        break;
    case 'r':
        arguments->reverse = true;
        break;
    case 's':
        i = atoi(arg);
        if (i <= 0)
            argp_usage(state);
        arguments->samplerate = i;
        break;
    case 'v':
        arguments->verbose = true;
        break;
    case 'w':
        arguments->nowarn = true;
        break;
    case ARGP_KEY_ARG:
        if (state->arg_num >= 2)
            argp_usage(state);
        if (state->arg_num == 0) {
            arguments->output = arg;
        } else if (state->arg_num == 1) {
            arguments->input = arguments->output;
            arguments->output = arg;
        }
        break;
    case ARGP_KEY_END:
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

void print_node(uint32_t depth, struct ast_node *node) {
    if (!node)
        return;
    if (node->node_type != NT_ARGUMENT_LIST)
        for (uint32_t i = 0; i < depth; i++)
            fprintf(stderr, "  ");
    switch (node->node_type) {
    case NT_NEG:
        fprintf(stderr, "-\n");
        print_node(depth + 1, ((struct unary_node*) node)->right);
        break;
    case NT_ADD:
    case NT_SUB:
    case NT_MUL:
    case NT_DIV:
    case NT_MOD:
    case NT_POW:
        switch (node->node_type) {
        case NT_ADD: fprintf(stderr, "+\n"); break;
        case NT_SUB: fprintf(stderr, "-\n"); break;
        case NT_MUL: fprintf(stderr, "*\n"); break;
        case NT_DIV: fprintf(stderr, "/\n"); break;
        case NT_MOD: fprintf(stderr, "%%\n"); break;
        case NT_POW: fprintf(stderr, "^\n"); break;
        default: break;
        }
        print_node(depth + 1, ((struct binary_node*) node)->left);
        print_node(depth + 1, ((struct binary_node*) node)->right);
        break;
    case NT_FUNCTION_CALL:
        fprintf(stderr, "%s()\n", ((struct function_call_node*) node)->identifier);
        break;
    case NT_FUNCTION_CALL_ARGUMENT_LIST:
        fprintf(stderr, "%s()\n", ((struct function_call_argument_list_node*) node)->identifier);
        print_node(depth + 1, ((struct function_call_argument_list_node*) node)->argument_list);
        break;
    case NT_ARGUMENT_LIST:
        print_node(depth, ((struct argument_list_node*) node)->argument);
        print_node(depth, ((struct argument_list_node*) node)->argument_list);
        break;
    case NT_IDENTIFIER:
        fprintf(stderr, "%s\n", ((struct identifier_node*) node)->identifier);
        break;
    case NT_NUMBER:
        fprintf(stderr, "%f\n", ((struct number_node*) node)->number);
        break;
    default:
        exit(EXIT_FAILURE);
        return;
    }
}

double calculate_node(double l, double t, struct ast_node *node) {
    if (!node)
        return 0.0;
    switch (node->node_type) {
    case NT_NEG:
        return -calculate_node(l, t, ((struct unary_node*) node)->right);
    case NT_ADD:
    case NT_SUB:
    case NT_MUL:
    case NT_DIV:
    case NT_MOD:
    case NT_POW: {
        const double left = calculate_node(l, t, ((struct binary_node*) node)->left);
        const double right = calculate_node(l, t, ((struct binary_node*) node)->right);
        switch (node->node_type) {
        case NT_ADD: return left + right;
        case NT_SUB: return left - right;
        case NT_MUL: return left * right;
        case NT_DIV:
            if (right == 0.0) {
                warn_once(DIVISION_BY_ZERO, NULL);
                return left;
            }
            return left / right;
        case NT_MOD:
            if (right == 0.0) {
                warn_once(DIVISION_BY_ZERO, NULL);
                return left;
            }
            return fmod(left, right);
        case NT_POW: return pow(left, right);
        default: return 0.0;
        }
        return 0.0;
    }
    case NT_FUNCTION_CALL: {
        const char *identifier = ((struct function_call_node*) node)->identifier;
        warn_once(UNDEFINED_FUNCTION, identifier);
        return 0.0;
    }
    case NT_FUNCTION_CALL_ARGUMENT_LIST: {
        struct function_call_argument_list_node *function_call_argument_list_node = (struct function_call_argument_list_node*) node;
        const char *identifier = function_call_argument_list_node->identifier;
        struct ast_node *an = function_call_argument_list_node->argument_list;
        struct argument_list_node *aln = NULL;
        double argv[MAX_FUNCTION_ARGS];
        for (uint32_t argc = 1; argc <= MAX_FUNCTION_ARGS; argc++) {
            if (an->node_type != NT_ARGUMENT_LIST) {
                argv[argc - 1] = calculate_node(l, t, an);
                if (!strcmp(identifier, "arp")) {
                    if (argc <= 2) {
                        warn_once(UNDEFINED_FUNCTION, identifier);
                        return 0.0;
                    }
                    const size_t n_count = argc - 2;
                    double n_arr[n_count];
                    for (size_t i = 0; i < n_count; i++) {
                        n_arr[i] = argv[i + 2];
                    }
                    return arp(argv[0], argv[1], n_count, n_arr);
                }
                if (!strcmp(identifier, "fade")) {
                    if (argc <= 4 || argc % 2 == 0) {
                        warn_once(UNDEFINED_FUNCTION, identifier);
                        return 0.0;
                    }
                    const size_t n_count = (argc - 1) / 2;
                    double t_arr[n_count];
                    double y_arr[n_count];
                    for (size_t i = 0; i < n_count; i++) {
                        t_arr[i] = argv[i * 2 + 1];
                        y_arr[i] = argv[i * 2 + 2];
                    }
                    return fade(argv[0], n_count, t_arr, y_arr);
                }
                if (!strcmp(identifier, "min")) {
                    if (argc <= 0) {
                        warn_once(UNDEFINED_FUNCTION, identifier);
                        return 0.0;
                    }
                    return min(argc, argv);
                }
                if (!strcmp(identifier, "max")) {
                    if (argc <= 0) {
                        warn_once(UNDEFINED_FUNCTION, identifier);
                        return 0.0;
                    }
                    return max(argc, argv);
                }
                switch (argc) {
                case 1: {
                    const double a = argv[0];
                    if (!strcmp(identifier, "dB"))
                        return dB(a);
                    if (!strcmp(identifier, "deg"))
                        return deg(a);
                    if (!strcmp(identifier, "phi"))
                        return phi(a);
                    if (!strcmp(identifier, "circ"))
                        return circ(a);
                    if (!strcmp(identifier, "exptl"))
                        return exptl(a);
                    if (!strcmp(identifier, "moog"))
                        return moog(a);
                    if (!strcmp(identifier, "noise"))
                        return noise(a);
                    if (!strcmp(identifier, "sawf"))
                        return sawf(a);
                    if (!strcmp(identifier, "sawr"))
                        return sawr(a);
                    if (!strcmp(identifier, "squ"))
                        return squ(a);
                    if (!strcmp(identifier, "tri"))
                        return tri(a);
                    if (!strcmp(identifier, "sin"))
                        return sin(a);
                    if (!strcmp(identifier, "cos"))
                        return cos(a);
                    if (!strcmp(identifier, "tan"))
                        return tan(a);
                    if (!strcmp(identifier, "asin"))
                        return asin(a);
                    if (!strcmp(identifier, "acos"))
                        return acos(a);
                    if (!strcmp(identifier, "atan"))
                        return atan(a);
                    if (!strcmp(identifier, "exp"))
                        return exp(a);
                    if (!strcmp(identifier, "ln"))
                        return log(a);
                    if (!strcmp(identifier, "log10"))
                        return log10(a);
                    if (!strcmp(identifier, "sqrt"))
                        return sqrt(a);
                    if (!strcmp(identifier, "ceil"))
                        return ceil(a);
                    if (!strcmp(identifier, "floor"))
                        return floor(a);
                    if (!strcmp(identifier, "abs"))
                        return fabs(a);
                    if (!strcmp(identifier, "sgn"))
                        return sgn(a);
                    warn_once(UNDEFINED_FUNCTION, identifier);
                    return 0.0;
                }
                case 2: {
                    const double a = argv[0];
                    const double b = argv[1];
                    if (!strcmp(identifier, "symm")) {
                        if (!(-1.0 <= b && b <= 1.0))
                            warn_once(CONSTRAINT_VIOLATION, identifier);
                        return symm(a, b);
                    }
                    if (!strcmp(identifier, "sync"))
                        return sync(a, b);
                    if (!strcmp(identifier, "parab")) {
                        if (!(1.0 <= b))
                            warn_once(CONSTRAINT_VIOLATION, identifier);
                        return parab(a, b);
                    }
                    if (!strcmp(identifier, "pulse")) {
                        if (!(-1.0 <= b && b <= 1.0))
                            warn_once(CONSTRAINT_VIOLATION, identifier);
                        return pulse(a, b);
                    }
                    warn_once(UNDEFINED_FUNCTION, identifier);
                    return 0.0;
                }
                case 3: {
                    const double a = argv[0];
                    const double b = argv[1];
                    const double c = argv[2];
                    if (!strcmp(identifier, "filterlp2"))
                        return filterlp2(a, b, c);
                    warn_once(UNDEFINED_FUNCTION, identifier);
                    return 0.0;
                }
                case 6: {
                    const double a = argv[0];
                    const double b = argv[1];
                    const double c = argv[2];
                    const double d = argv[3];
                    const double e = argv[4];
                    const double f = argv[5];
                    if (!strcmp(identifier, "filter2"))
                        return filter2(a, b, c, d, e, f);
                    warn_once(UNDEFINED_FUNCTION, identifier);
                    return 0.0;
                }
                case 7: {
                    const double a = argv[0];
                    const double b = argv[1];
                    const double c = argv[2];
                    const double d = argv[3];
                    const double e = argv[4];
                    const double f = argv[5];
                    const double g = argv[6];
                    if (!strcmp(identifier, "adsr")) {
                        if (!(0.0 <= f && f <= 1.0))
                            warn_once(CONSTRAINT_VIOLATION, identifier);
                        return adsr(a, b, c, d, e, f, g);
                    }
                    if (!strcmp(identifier, "sfs")) {
                        if (!(0.0 <= f && f <= 1.0))
                            warn_once(CONSTRAINT_VIOLATION, identifier);
                        return sfs(a, b, c, d, e, f, g);
                    }
                    warn_once(UNDEFINED_FUNCTION, identifier);
                    return 0.0;
                }
                case 9: {
                    const double a = argv[0];
                    const double b = argv[1];
                    const double c = argv[2];
                    const double d = argv[3];
                    const double e = argv[4];
                    const double f = argv[5];
                    const double g = argv[6];
                    const double h = argv[7];
                    const double i = argv[8];
                    if (!strcmp(identifier, "dahdsr")) {
                        if (!(0.0 <= h && h <= 1.0))
                            warn_once(CONSTRAINT_VIOLATION, identifier);
                        return dahdsr(a, b, c, d, e, f, g, h, i);
                    }
                    warn_once(UNDEFINED_FUNCTION, identifier);
                    return 0.0;
                }
                default:
                    warn_once(UNDEFINED_FUNCTION, identifier);
                    return 0.0;
                }
            }
            aln = (struct argument_list_node*) an;
            argv[argc - 1] = calculate_node(l, t, aln->argument);
            an = aln->argument_list;
        }
        warn_once(UNDEFINED_FUNCTION, identifier);
        return 0.0;
    }
    case NT_ARGUMENT_LIST:
        exit(EXIT_FAILURE);
        return 0.0;
    case NT_IDENTIFIER: {
        const char *identifier = ((struct identifier_node*) node)->identifier;
        if (!strcmp(identifier, "l"))
            return l;
        if (!strcmp(identifier, "t"))
            return t;
        if (!strcmp(identifier, "e"))
            return M_E;
        if (!strcmp(identifier, "H"))
            return M_H;
        if (!strcmp(identifier, "pi"))
            return M_PI;
        if (!strcmp(identifier, "tau"))
            return M_TAU;
        /* is it a key? */
        const char first = identifier[0];
        if ('A' <= first && first <= 'G') {
            const size_t len = strlen(identifier);
            const char last = identifier[len - 1];
            if ((len == 2 || len == 3) &&
                ('0' <= last && last <= '8')) {
                /* it is a key */
                static const int32_t keys[] = { 1, 3, -8, -6, -4, -3, -1 };
                int32_t n = keys[first - 'A'];
                if (len == 3) {
                    const char mid = identifier[1];
                    if (mid == '#')
                        n++;
                    else if (mid == 'b')
                        n--;
                    else {
                        warn_once(UNDEFINED_IDENTIFIER, identifier);
                        return 0.0;
                    }
                }
                n += 12 * (last - '0');
                return pow(M_H, n - 49) * 440.0;
            }
        }
        warn_once(UNDEFINED_IDENTIFIER, identifier);
        return 0.0;
    }
    case NT_NUMBER:
        return ((struct number_node*) node)->number;
    default:
        exit(EXIT_FAILURE);
        return 0.0;
    }
}

void free_node(struct ast_node *node) {
    if (!node)
        return;
    switch (node->node_type) {
    case NT_NEG:
        free_node(((struct unary_node*) node)->right);
        break;
    case NT_ADD:
    case NT_SUB:
    case NT_MUL:
    case NT_DIV:
    case NT_MOD:
    case NT_POW:
        free_node(((struct binary_node*) node)->left);
        free_node(((struct binary_node*) node)->right);
        break;
    case NT_FUNCTION_CALL:
        break;
    case NT_FUNCTION_CALL_ARGUMENT_LIST:
        free_node(((struct function_call_argument_list_node*) node)->argument_list);
        break;
    case NT_ARGUMENT_LIST:
        free_node(((struct argument_list_node*) node)->argument);
        free_node(((struct argument_list_node*) node)->argument_list);
        break;
    case NT_IDENTIFIER:
    case NT_NUMBER:
        break;
    default:
        exit(EXIT_FAILURE);
        return;
    }
    free(node);
}

int main(int argc, char **argv) {
    /* argp */
    struct arguments arguments;
    arguments.bps = 16;
    arguments.channels = 2;
    arguments.format = WAVE;
    arguments.length = 1.0;
    arguments.reverse = false;
    arguments.samplerate = 44100;
    arguments.verbose = false;
    arguments.nowarn = false;
    arguments.input = NULL; /* stdin */
    arguments.output = NULL; /* stdout */
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    if (arguments.verbose) {
        fprintf(stderr, "bps = %i\n", arguments.bps);
        fprintf(stderr, "channels = %i\n", arguments.channels);
        fprintf(stderr, "format = %s\n", format_tostring(arguments.format));
        fprintf(stderr, "length = %f s\n", arguments.length);
        fprintf(stderr, "reverse = %s\n", arguments.reverse ? "yes" : "no");
        fprintf(stderr, "samplerate = %i Hz\n", arguments.samplerate);
        fprintf(stderr, "verbose = %s\n", arguments.verbose ? "yes" : "no");
        fprintf(stderr, "nowarn = %s\n", arguments.nowarn ? "yes" : "no");
        fprintf(stderr, "input = %s\n", arguments.input ? arguments.input : "stdin");
        fprintf(stderr, "output = %s\n", arguments.output ? arguments.output : "stdout");
    }

    /* yyparse */
    if (arguments.input) {
        yyin = fopen(arguments.input, "r");
        if (!yyin) {
            error(FOPEN_FAILED, arguments.input);
            return 1;
        }
    }
    struct ast_node *root = NULL;
    const int retval_yyparse = yyparse(&root);
    if (arguments.input && yyin)
        fclose(yyin);
    if (retval_yyparse || !root) {
        error(YYPARSE_FAILED, NULL);
        return 1;
    }
    if (arguments.verbose)
        print_node(0, root);

    /* init functions */
    init_functions((double) arguments.samplerate);

    /* main calculation */
    const uint32_t bytepersample = arguments.bps / 8;
    const uint32_t blockalign = arguments.channels * bytepersample;
    const uint32_t bytepersecond = arguments.samplerate * blockalign;
    const uint32_t datasize = arguments.length * bytepersecond;
    uint8_t data[datasize];

    const bool swap = swap_byteorder(arguments.format);
    uint32_t o1, o2, o3, o4;
    switch (arguments.bps) {
    case 8:
        break;
    case 16:
        o1 = swap ? 1 : 0;
        o2 = swap ? 0 : 1;
        break;
    case 24:
        o1 = swap ? 2 : 0;
        o2 = 1;
        o3 = swap ? 0 : 2;
        break;
    case 32:
        o1 = swap ? 3 : 0;
        o2 = swap ? 2 : 1;
        o3 = swap ? 1 : 2;
        o4 = swap ? 0 : 3;
        break;
    }

    for (uint32_t i = 0; i < datasize; i += blockalign) {
        const double t = ((double) i) / (double) bytepersecond;
        double v = calculate_node(arguments.length, t, root);
        /* clipping */
        if (v > 1.0) {
            v = 1.0;
            if (!arguments.nowarn)
                warn_once(SAMPLE_CLIPPING, NULL);
        } else if (v < -1.0) {
            v = -1.0;
            if (!arguments.nowarn)
                warn_once(SAMPLE_CLIPPING, NULL);
        }
        for (uint32_t c = 0; c < arguments.channels; c++) {
            const uint32_t offset = (arguments.reverse ? datasize - i - blockalign : i) + c * bytepersample;
            switch (arguments.bps) {
            case 8:
                data[offset] = (uint8_t) ((arguments.format == WAVE) ? 127.5 * (v + 1.0) : (MAX_8 * v));
                break;
            case 16: {
                const int16_t sample = MAX_16 * v;
                data[offset + o1] = sample & 0xFF;
                data[offset + o2] = sample >> 8;
            }   break;
            case 24: {
                const int32_t sample = MAX_24 * v;
                data[offset + o1] = sample & 0xFF;
                data[offset + o2] = (sample >> 8) & 0xFF;
                data[offset + o3] = sample >> 16;
            }   break;
            case 32: {
                const int32_t sample = MAX_32 * v;
                data[offset + o1] = sample & 0xFF;
                data[offset + o2] = (sample >> 8) & 0xFF;
                data[offset + o3] = (sample >> 16) & 0xFF;
                data[offset + o4] = sample >> 24;
            }   break;
            }
        }
    }

    /* free memory */
    free_node(root);

    /* print remaining optional warnings */
    if (!arguments.nowarn) {
        const uint8_t zero = (arguments.bps == 8 && arguments.format == WAVE) ? 0x7F : 0x00;
        for (uint32_t i = 0; i < blockalign; i++) {
            if (data[i] != zero) {
                warn(STARTS_ON_NON_ZERO_SAMPLE, NULL);
                break;
            }
        }
        for (uint32_t i = datasize - blockalign; i < datasize; i++) {
            if (data[i] != zero) {
                warn(ENDS_ON_NON_ZERO_SAMPLE, NULL);
                break;
            }
        }
    }

    /* write audio data */
    FILE *file;
    if (arguments.output) {
        /* to file */
        file = fopen(arguments.output, "wb");
        if (!file) {
            error(FOPEN_FAILED, arguments.output);
            return 1;
        }
    } else {
        /* to stdout */
        file = stdout;
    }
    switch (arguments.format) {
    case AIFF:
        fwrite_aiff(&arguments, datasize, data, file);
        break;
    case WAVE:
        fwrite_wave(&arguments, datasize, data, file);
        break;
    }
    if (arguments.output)
        fclose(file);

    /* print number of samples written */
    if (arguments.verbose)
        fprintf(stderr, "%u samples written\n", datasize / blockalign);

    return 0;
}
