#ifndef OPTIONS_H_
#define OPTIONS_H_

typedef struct {
} Options;

int options_parse_cmdline(int argc, char *argv[], Options *options);

#endif //OPTIONS_H_
