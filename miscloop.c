/*
                         Generic APP Source File

                          Filename : MISCLOOP.C
                          Version  : 95/03/01
                            Written by Mio

                            �e�탋�[�v����
*/

#include <stdio.h>
#include <string.h>

#include "appdefs.h"
#include "apperr.h"
#include "appsub.h"
#include "proto.h"
#include "condexp.h"
#include "superstr.h"
#include "miscloop.h"

int i_loopnum = 0 ;              /* loop ���x���E�J�E���^ */
int i_looplev = 0 ;              /* loop �u���b�N�l�X�e�B���O���x�� */
char *cp_lcondstk[LPNESTMAX] ,   /* loop �����X�^�b�N1 */
     *cp_lcondstk2[LPNESTMAX] ;  /* loop �����X�^�b�N2 (for) */
int ia_loopn[LPNESTMAX] ,        /* loop ���x���ԍ��X�^�b�N */
    ia_contf[LPNESTMAX] ,        /* continue �t���O�X�^�b�N */
    ia_exitf[LPNESTMAX] ;        /* exit �t���O�X�^�b�N */

/* _while �̏��� */
int eval_while(char *cp_line)
{
   if (++i_looplev >= LPNESTMAX) {
      app_error(LOOP_STK_OVER,1) ;       /* loop �̃l�X�g���[������ */
   }
   i_loopnum++ ;

   ncjmp_output(TMPL_CONTINUE,i_loopnum) ; /* _continue ���̃��x�� */
   tmplabel_output(TMPL_BEGIN,i_loopnum) ; /* ���[�v�̓��̃��x�� */

   ia_loopn[i_looplev] = i_loopnum ;
   ia_contf[i_looplev] = TRUE ;
   ia_exitf[i_looplev] = FALSE ;

   cp_lcondstk[i_looplev]  = app_strdup(cp_line) ; /* �����̓W�J�͂��Ƃ� */
   cp_lcondstk2[i_looplev] = NULL ;                /* �����̓W�J�͂��Ƃ� */

   return 0 ;
}

/* _wend �̏��� */
#pragma argsused
int eval_wend(char *cp_line)
{
   if ((i_looplev == 0) || (cp_lcondstk[i_looplev] == NULL)) {
      app_error(LOOP_STK_EMPTY,1) ;     /* ���[�v�O�� _wend */
   }

   tmplabel_output(TMPL_CONTINUE,ia_loopn[i_looplev]) ; /* continue ���x�� */
   eval_cond(cp_lcondstk[i_looplev],FALSE,TMPL_BEGIN,
             ia_loopn[i_looplev],0,TRUE); /* �����W�����v�̐��� */
   app_free(cp_lcondstk[i_looplev]) ;     /* ����������̊J�� */
   if (ia_exitf[i_looplev] == TRUE) {
      tmplabel_output(TMPL_EXIT,ia_loopn[i_looplev]) ; /* exit ���x�� */
   }
   i_looplev-- ;

   return 0 ;
}

/* _repeat �̏��� */
#pragma argsused
int eval_repeat(char *cp_line)
{
   if (++i_looplev >= LPNESTMAX) {
      app_error(LOOP_STK_OVER,1) ;       /* loop �̃l�X�g���[������ */
   }
   i_loopnum++ ;

   tmplabel_output(TMPL_BEGIN,i_loopnum) ;    /* ���[�v�̓��̃��x�� */

   ia_loopn[i_looplev] = i_loopnum ;
   ia_contf[i_looplev] = FALSE ;
   ia_exitf[i_looplev] = FALSE ;
   cp_lcondstk[i_looplev]  = NULL ; /* �����͂����ɂȂ� */
   cp_lcondstk2[i_looplev] = NULL ;

   return 0 ;
}

/* _until �̏��� */
int eval_until(char *cp_line)
{
   if ((i_looplev == 0) || (cp_lcondstk[i_looplev] != NULL)) {
      app_error(LOOP_STK_EMPTY,1) ;     /* ���[�v�O�� _wend */
   }

   if (ia_contf[i_looplev] == TRUE) {
      tmplabel_output(TMPL_CONTINUE,ia_loopn[i_looplev]); /* continue ���x�� */
   }
   /* �����W�����v�̐��� */
   eval_cond(cp_line,TRUE,TMPL_BEGIN,ia_loopn[i_looplev],0,FALSE);
   if (ia_exitf[i_looplev] == TRUE) {
      tmplabel_output(TMPL_EXIT,ia_loopn[i_looplev]) ; /* exit ���x�� */
   }
   i_looplev-- ;

   return 0 ;
}

/* _continue �̏��� */
#pragma argsused
int eval_continue(char *cp_line)
{
   if (i_looplev == 0) {
      app_error(NOT_IN_LOOP,1) ;
   }
   ia_contf[i_looplev] = TRUE ;
   ncjmp_output(TMPL_CONTINUE,ia_loopn[i_looplev]) ;
   return 0 ;
}

/* _exit �̏��� */
#pragma argsused
int eval_exit(char *cp_line)
{
   if (i_looplev == 0) {
      app_error(NOT_IN_LOOP,1) ;
   }
   ia_exitf[i_looplev] = TRUE ;
   ncjmp_output(TMPL_EXIT,ia_loopn[i_looplev]) ;
   return 0 ;
}

/* _let �̏��� (_for �̑��p�����[�^��) */
int eval_let(char *cp_line)
{
   char ca_arg1[ARGSTRMAX],         /* ������̍��� */
        ca_cond[ARGSTRMAX],         /* ������Z�q */
        ca_arg2[ARGSTRMAX] ;        /* ������̉E�� */

   cp_line = get_args(cp_line,ca_arg1,ca_cond,ca_arg2,NULL) ;

   if (strcmp(ca_cond,OP_LET) != 0) {
      app_error(LARGCOND_ERROR,1) ;
   }
   cutspc(ca_arg1) ;
   cutspc(ca_arg2) ;
   mov_output(ca_arg1,ca_arg2) ;

   return 0 ;
}

/* _for �̏��� */
int eval_for(char *cp_line)
{
   char **cpp_tmp ;                       /* �R�̈����̕����p */
   int i ;

   if (++i_looplev >= LPNESTMAX) {
      app_error(LOOP_STK_OVER,1) ;       /* loop �̃l�X�g���[������ */
   }
   i_loopnum++ ;

   i = strtocpa(cp_line,&cpp_tmp,3,",;") ;
   if (i != 3) {
      if (i != 0) {
         cpa_release(cpp_tmp) ;
      }
      app_error(FOR_ARGUMENT,1) ;
   }

   /* mov �̏o�� */
   if (app_strlen(cpp_tmp[0]) > 0) {
      eval_let(cpp_tmp[0]) ;
   }

   /* �����������邩 */
   if (app_strlen(cpp_tmp[1]) > 0) {
      cp_lcondstk[i_looplev] = app_strdup(cpp_tmp[1]) ;
      ncjmp_output(TMPL_FORBEGIN,i_loopnum) ; /* _continue ���̃��x�� */
   } else {
      cp_lcondstk[i_looplev] = NULL ;
   }

   /* �����w�肪���邩 */
   if (app_strlen(cpp_tmp[2]) > 0) {
      cp_lcondstk2[i_looplev] = app_strdup(cpp_tmp[2]) ;
   } else {
      cp_lcondstk2[i_looplev] = NULL ;
   }

   tmplabel_output(TMPL_BEGIN,i_loopnum) ; /* ���[�v�̓��̃��x�� */
   ia_loopn[i_looplev] = i_loopnum ;
   ia_contf[i_looplev] = FALSE ;
   ia_exitf[i_looplev] = FALSE ;

   cpa_release(cpp_tmp) ;
   return 0 ;
}

/* _next �̏��� */
#pragma argsused
int eval_next(char *cp_line)
{
   char ca_tmp[128] ;

   if (i_looplev == 0) {
      app_error(LOOP_STK_EMPTY,1) ;     /* ���[�v�O�� _next */
   }

   tmplabel_output(TMPL_CONTINUE,ia_loopn[i_looplev]) ; /* continue ���x�� */

   /* �����̏��� */
   if (cp_lcondstk2[i_looplev] != NULL) {
      expr_eval(cp_lcondstk2[i_looplev],ca_tmp) ;
      app_free(cp_lcondstk2[i_looplev]) ; /* ����������̊J�� */
   }

   tmplabel_output(TMPL_FORBEGIN,ia_loopn[i_looplev]) ; /* for_bg ���x�� */

   /* �����̏��� */
   if (cp_lcondstk[i_looplev] != NULL) {
      eval_cond(cp_lcondstk[i_looplev],FALSE,TMPL_BEGIN,
                ia_loopn[i_looplev],0,TRUE); /* �����W�����v�̐��� */
      app_free(cp_lcondstk[i_looplev]) ;     /* ����������̊J�� */
   } else { /* �������Ȃ��Ƃ� */
      ncjmp_output(TMPL_BEGIN,ia_loopn[i_looplev]) ;
   }

   if (ia_exitf[i_looplev] == TRUE) {
      tmplabel_output(TMPL_EXIT,ia_loopn[i_looplev]) ; /* exit ���x�� */
   }

   i_looplev-- ;

   return 0 ;
}

/* �� loop �u���b�N�̒��� ? */
int in_loopblkp(void)
{
   return i_looplev ;
}
