extproc sh

(cd gettext-runtime; libtoolize -f)
(cd gettext-tools; libtoolize -f)
libtoolize -f

COMSPEC=/bin/sh \
    ./autogen.sh --skip-gnulib
