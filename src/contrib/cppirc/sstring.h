#ifndef _SSTRING_H_
#define	_SSTRING_H_

#include <sys/types.h>

size_t	 strlcat(char *, const char *, size_t);
size_t	 strlcpy(char *, const char *, size_t);
char	*strsep(char **, const char *);

#endif /* _SSTRING_H_ */
