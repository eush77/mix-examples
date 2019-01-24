#include "Format.h"

#include <string.h>

void format(__stage(1) char *Out, const struct Char *Fmt,
            __stage(1) const char **Args) __stage(1) {
  do {
    const struct Char *Subst = Fmt;

    while (Subst[0].Ch && (Subst[0].Ch != '{' || Subst[1].Ch < '0' ||
                           Subst[1].Ch > '9' || Subst[2].Ch != '}'))
      Subst += 1;

    if (Subst != Fmt) {
      memcpy(Out, Fmt, Subst - Fmt);
      Out += Subst - Fmt;
    }

    if (Subst->Ch) {
      unsigned NSub = Subst[1].Ch - '0';
      size_t Len = strlen(Args[NSub]);

      memcpy(Out, Args[NSub], Len);
      Out += Len;
      Fmt = Subst + 3;
    } else {
      Fmt = Subst;
    }
  } while (Fmt->Ch);
}

__attribute__((mix(format))) void *mixFormat(void *, const struct Char *);
