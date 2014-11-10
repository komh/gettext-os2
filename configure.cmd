extproc sh

n=configure
test -f "./$n." || { echo "\`./$n' not found !!!"; exit 1; }

opts="
  --enable-shared
  --enable-static
  --disable-relocatable
  --with-included-gettext
"

lt_cv_sys_max_cmd_len=-1 \
"./$n" $opts "$@" 2>&1 | tee "$n.log"
