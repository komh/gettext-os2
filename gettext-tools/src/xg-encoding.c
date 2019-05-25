/* Keeping track of the encoding of strings to be extracted.
   Copyright (C) 2001-2019 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

/* Specification.  */
#include "xg-encoding.h"

#include <stdio.h>
#include <stdlib.h>

#include "msgl-ascii.h"
#include "msgl-iconv.h"
#include "po-charset.h"
#include "xalloc.h"
#include "xerror.h"
#include "xvasprintf.h"

#include "gettext.h"
#define _(str) gettext (str)


/* Canonicalized encoding name for all input files.  */
const char *xgettext_global_source_encoding;

#if HAVE_ICONV
/* Converter from xgettext_global_source_encoding to UTF-8 (except from
   ASCII or UTF-8, when this conversion is a no-op).  */
iconv_t xgettext_global_source_iconv;
#endif

/* Canonicalized encoding name for the current input file.  */
const char *xgettext_current_source_encoding;

#if HAVE_ICONV
/* Converter from xgettext_current_source_encoding to UTF-8 (except from
   ASCII or UTF-8, when this conversion is a no-op).  */
iconv_t xgettext_current_source_iconv;
#endif


/* Error message about non-ASCII character in a specific lexical context.  */
char *
non_ascii_error_message (lexical_context_ty lcontext,
                         const char *file_name, size_t line_number)
{
  char buffer[21];
  char *errmsg;

  if (line_number == (size_t)(-1))
    buffer[0] = '\0';
  else
    sprintf (buffer, ":%ld", (long) line_number);

  switch (lcontext)
    {
    case lc_outside:
      errmsg =
        xasprintf (_("Non-ASCII character at %s%s."), file_name, buffer);
      break;
    case lc_comment:
      errmsg =
        xasprintf (_("Non-ASCII comment at or before %s%s."),
                   file_name, buffer);
      break;
    case lc_string:
      errmsg =
        xasprintf (_("Non-ASCII string at %s%s."), file_name, buffer);
      break;
    default:
      abort ();
    }
  return errmsg;
}

/* Convert the given string from xgettext_current_source_encoding to
   the output file encoding (i.e. ASCII or UTF-8).
   The resulting string is either the argument string, or freshly allocated.
   The file_name and line_number are only used for error message purposes.  */
char *
from_current_source_encoding (const char *string,
                              lexical_context_ty lcontext,
                              const char *file_name, size_t line_number)
{
  if (xgettext_current_source_encoding == po_charset_ascii)
    {
      if (!is_ascii_string (string))
        {
          multiline_error (xstrdup (""),
                           xasprintf ("%s\n%s\n",
                                      non_ascii_error_message (lcontext,
                                                               file_name,
                                                               line_number),
                                      _("Please specify the source encoding through --from-code.")));
          exit (EXIT_FAILURE);
        }
    }
  else if (xgettext_current_source_encoding != po_charset_utf8)
    {
#if HAVE_ICONV
      struct conversion_context context;

      context.from_code = xgettext_current_source_encoding;
      context.to_code = po_charset_utf8;
      context.from_filename = file_name;
      context.message = NULL;

      string = convert_string_directly (xgettext_current_source_iconv, string,
                                        &context);
#else
      /* If we don't have iconv(), the only supported values for
         xgettext_global_source_encoding and thus also for
         xgettext_current_source_encoding are ASCII and UTF-8.
         convert_string_directly() should not be called in this case.  */
      abort ();
#endif
    }

  return (char *) string;
}
