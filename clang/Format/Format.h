#ifndef MIX_EXAMPLES_CLANG_FORMAT_FORMAT_H
#define MIX_EXAMPLES_CLANG_FORMAT_FORMAT_H

#ifdef __cplusplus
extern "C" {
#endif

struct Char {
  char Ch;
} __attribute((packed,staged));

void format(char *Out, const struct Char *Fmt, const char **Args);
void *mixFormat(void *Ctx, const struct Char *Fmt);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // MIX_EXAMPLES_CLANG_FORMAT_FORMAT_H
