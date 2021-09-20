#include "shell.h"

#include "lib.h"
#include "uart.h"

char buf[SHELL_BUFLEN];       /* Input line (large enough for */
int32_t len;                  /* Length of line read    */
char tokbuf[SHELL_BUFLEN +    /* Buffer to hold a set of  */
            SHELL_MAXTOK];    /* Contiguous null-terminated */
int32_t tlen;                 /* Current length of all data */
                              /*   in array tokbuf    */
int32_t tok[SHELL_MAXTOK];    /* Index of each token in */
int32_t toktyp[SHELL_MAXTOK]; /* Type of each token in tokbuf */
int32_t ntok;                 /* Number of tokens on line */
// pid32 child;      /* Process ID of spawned child  */
bool backgnd;           /* Run command in background? */
char *outname, *inname; /* Pointers to strings for file */
                        /*   names that follow > and <  */
// did32 stdinput, stdoutput;  /* Descriptors for redirected */
/*   input and output   */
int32_t i;                /* Index into array of tokens */
int32_t j;                /* Index into array of commands */
int32_t msg;              /* Message from receive() for */
                          /*   child termination    */
int32_t tmparg;           /* Address of this var is used  */
                          /*   when first creating child  */
                          /*   process, but is replaced */
char *src, *cmp;          /* Pointers used during name  */
                          /*   comparison     */
bool diff;                /* Was difference found during  */
char *args[SHELL_MAXTOK]; /* Argument vector passed to  */
                          /*   builtin commands   */
// did32 dev = 0;    /* ID of tty device from which  */

int32_t lexan(char *line,      /* Input line terminated with */
                               /*   NEWLINE or NULLCH    */
              int32_t len,     /* Length of the input line,  */
                               /*   including NEWLINE    */
              char *tokbuf,    /* Buffer into which tokens are */
                               /*   stored with a null   */
                               /*   following each token */
              int32_t *tlen,   /* Place to store number of */
                               /*   chars in tokbuf    */
              int32_t tok[],   /* Array of pointers to the */
                               /*   start of each token  */
              int32_t toktyp[] /* Array that gives the type  */
                               /*   of each token    */
) {
  char quote;      /* Character for quoted string  */
  uint32_t ntok;   /* Number of tokens found */
  char *p;         /* Pointer that walks along the */
                   /*   input line     */
  int32_t tbindex; /* Index into tokbuf    */
  char ch;         /* Next char from input line  */

  /* Start at the beginning of the line with no tokens */

  ntok = 0;
  p = line;
  tbindex = 0;

  /* While not yet at end of line, get next token */

  while ((*p != NULLCH) && (*p != SH_NEWLINE)) {
    /* If too many tokens, return error */

    if (ntok >= SHELL_MAXTOK) {
      return SYSERR;
    }

    /* Skip whitespace before token */

    while ((*p == SH_BLANK) || (*p == SH_TAB)) {
      p++;
    }

    /* Stop parsing at end of line (or end of string) */

    ch = *p;
    if ((ch == SH_NEWLINE) || (ch == NULLCH)) {
      *tlen = tbindex;
      return ntok;
    }

    /* Set next entry in tok array to be an index to the  */
    /*   current location in the token buffer   */

    tok[ntok] = tbindex; /* the start of the token */

    /* Set the token type */

    switch (ch) {
      case SH_AMPER:
        toktyp[ntok] = SH_TOK_AMPER;
        tokbuf[tbindex++] = ch;
        tokbuf[tbindex++] = NULLCH;
        ntok++;
        p++;
        continue;

      case SH_LESS:
        toktyp[ntok] = SH_TOK_LESS;
        tokbuf[tbindex++] = ch;
        tokbuf[tbindex++] = NULLCH;
        ntok++;
        p++;
        continue;

      case SH_GREATER:
        toktyp[ntok] = SH_TOK_GREATER;
        tokbuf[tbindex++] = ch;
        tokbuf[tbindex++] = NULLCH;
        ntok++;
        p++;
        continue;

      default:
        toktyp[ntok] = SH_TOK_OTHER;
    };

    /* Handle quoted string (single or double quote) */

    if ((ch == SH_SQUOTE) || (ch == SH_DQUOTE)) {
      quote = ch; /* remember opening quote */

      /* Copy quoted string to arg area */

      p++; /* Move past starting quote */

      while (((ch = *p++) != quote) && (ch != SH_NEWLINE) && (ch != NULLCH)) {
        tokbuf[tbindex++] = ch;
      }
      if (ch != quote) { /* string missing end quote */
        return SYSERR;
      }

      /* Finished string - count token and go on  */

      tokbuf[tbindex++] = NULLCH; /* terminate token  */
      ntok++;                     /* count string as one token  */
      continue;                   /* go to next token   */
    }

    /* Handle a token other than a quoted string    */

    tokbuf[tbindex++] = ch; /* put first character in buffer*/
    p++;

    while (((ch = *p) != SH_NEWLINE) && (ch != NULLCH) && (ch != SH_LESS) &&
           (ch != SH_GREATER) && (ch != SH_BLANK) && (ch != SH_TAB) &&
           (ch != SH_AMPER) && (ch != SH_SQUOTE) && (ch != SH_DQUOTE)) {
      tokbuf[tbindex++] = ch;
      p++;
    }

    /* Report error if other token is appended */

    if ((ch == SH_SQUOTE) || (ch == SH_DQUOTE) || (ch == SH_LESS) ||
        (ch == SH_GREATER)) {
      return SYSERR;
    }

    tokbuf[tbindex++] = NULLCH; /* terminate the token  */

    ntok++; /* count valid token  */
  }
  *tlen = tbindex;
  return ntok;
}
