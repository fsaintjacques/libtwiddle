% tw_bitmap(2)
% François Saint-Jacques
% 2015-09-03

# NAME

tw_bitmap – libtwiddle basic bitmap implementation

# SYNOPSIS

```c
#include <libtwiddle/bitmap.h>

struct tw_bitmap * tw_bitmap_new(uint32_t nbits);
void tw_bitmap_free(struct tw_bitmap *bitmap);

void tw_bitmap_set(struct tw_bitmap *bitmap, uint32_t pos);
void tw_bitmap_clear(struct tw_bitmap *bitmap, uint32_t pos);

bool tw_bitmap_test(struct tw_bitmap *bitmap, uint32_t pos);
```

# DESCRIPTION

**tw_bitmap** is a simple struct

# RETURN VALUE

# SEE ALSO
