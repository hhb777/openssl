/*
 * Copyright 2007-2019 The OpenSSL Project Authors. All Rights Reserved.
 * Copyright Nokia 2007-2019
 * Copyright Siemens AG 2015-2019
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

/* This demonstrates/tests cases where check-format.pl should complain */

/*
 * The '@'s after '*' are used for self-tests: they mark lines containing
 * a single flaw that should be reported. Normally it should be reported
 * while handling the given line, but in case of delayed checks there is a
 * following digit indicating the number of reports expected for this line.
 */

/* For each of the following set of lines the tool should complain */
/*@ tab character: 	 */
/*@ cr character:  */
/*@ non-printable ASCII character:  */
/*@ non-ascii character: ä */
/*@ space at EOL: */ 
// /*@ end-of-line comment style not allowed for C90 */
/*X */ /*@ no space after comment start, flagged unless sloppy-spc */
/* X*/ /*@ no space before comment end , flagged unless sloppy-spc */
/*@ comment start: /* inside intra-line comment */
/*@ multi-line comment with text on first line
 *@ comment start: /* inside multi-line comment
*@ indent off by -1 in multi-line comment
 *X *@ no space after leading * in multi-line comment, flagged unless sloppy-spc
 *@0 more than double space after .   in comment, flagged unless sloppy-spc
 *@2 non-empty comment text on last line of multi-line comment */
*/ /*@ comment end outside comment */
/*@ comment line is 4 columns toooooooooooooooooo wide, flagged unless sloppy-len */
/*@ comment line is 5 columns toooooooooooooooooooooooooooooooooooooooooooooo wide */
 #define X          /*@ indent of '#' before preprocessor directive off by 1 */
# define X          /*@ indent of preprocessor directive off by 1  */
typedef struct s  { /*@0 double space, flagged unless sloppy-spc */
    enum {          /*@2  double space in comment, flagged unless sloppy-spc */
           zz       /*@ hanging indent off by 1 or 3 */
         x = 1 +    /*@ hanging indent off by -1 or 1 */
           && 1,    /*@ hanging indent of '&&' off by 1 or 3 or -3 or -1, ... */
          y,z       /*@ no space after comma, flagged unless sloppy-spc */
    } type ;        /*@ space before ;, flagged unless sloppy-spc */
   union {          /*@ normal indent off by -1 */
        struct{} n; /*@ no space before {, flagged unless sloppy-spc */
    }p;             /*@ no space after, flagged unless sloppy-spc } */
    };              /*@ normal indent off by 4 */
fun() {             /*@ opening brace at end of function definition header */
    if (cond)) {    /*@ too many closing parens */
        stmt;       /*@0 single-line statement in braces */
}}}                 /*@2 too many closing braces */
#endif              /*@ unexpected #endif */
 int f(int a,       /*@ normal indent off by 1 */
      int b,        /*@ hanging indent off by -1 (or else 2 for sloppy-hang) */
        int c)      /*@ hanging indent off by 1 (or else 4 for sloppy-hang) */
{ int               /*@ text after opening brace */
    x = 1] +        /*@ too many closing brackets */
        2} -        /*@ too many closing braces within expr */
        3:;         /*@ ':' without preceding '?' within expr */
    s = {           /*@0 unclosed brace within initializer expression */
         (          /*@0 unclosed paren */
          a[        /*@0 unclosed bracket */
            ?;      /*@4 unclosed conditional expression */
   ggg(a,           /*@ normal indent off by -1 */
      b1,           /*@ hanging indent off by -1 (or else 2 for sloppy-hang) */
      b2,           /*@ hang.ind. again off by -1, flagged unless sloppy-hang */
   0,               /*@ hanging indent off by -4 (left of normal indent, .. */
       c1 + c3 + .. /*@ .. so reported also with sloppy-hang; this line too long */ c3
# define M(X) X     /*@ macro indent off by 1 */
        d ? 1       /*@ hanging indent off by 1 (or else 2 for sloppy-hang) */
           : 2);    /*@ hang.ind. further off by 1 (or else 5 f. sloppy-hang) */
    if (e+          /*@ no space before +, flagged unless sloppy-spc */
        (g*= 2) +   /*@ no space before *=, flagged unless sloppy-spc */
        ( 0) *      /*@ space after (, flagged unless sloppy-spc */
        h %2 -      /*@ no space after, flagged unless sloppy-spc % */
        0 +/* */    /*@ no space before comment, flagged unless sloppy-spc */
   /* */1 *         /*@ no space after comment, flagged unless sloppy-spc */
     shifted.. /*@ hanging indent off by -2, flagged unless sloppy-hang */ left)
       cmd;         /*@ single-statement indent off by -1 */
    do{ x = 3; }    /*@ no space before opening {, flagged unless sloppy-spc */
    while(e3);      /*@ no space after 'while', flagged unless sloppy-spc */
    switch (e ) {   /*@ space before ), flagged unless sloppy-spc */
   case 1:          /*@ case indent off by -1 */
     default:       /*@ default indent off by 1 */
}                   /*@ normal indent off by -4 */
  label:            /*@ label indent off by 1 */
    x; }            /*@ text before closing brace */
}                   /*@ extra closing brace */
/* Here the tool should stop complaining apart from the below issues at EOF */

{                   /*@0 unclosed brace outside expression */
#if                 /*@0 unclosed #if */
                    /*@0 space/empty line follows just before EOF: */

