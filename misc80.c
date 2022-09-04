/*
                         Generic APP Source File

                          Filename : MISC80.C
                          Version  : 95/03/14
                            Written by Mio

                            APP80 �ˑ�����
*/

#include <stdio.h>
#include <string.h>

#include "appmain.h"
#include "appdefs.h"
#include "appsub.h"
#include "apperr.h"
#include "condexp.h"
#include "miscloop.h"
#include "proto.h"
#include "code80.h"
#include "misc80.h"

extern int i_cpu ;

/* _near �̏��� */
#pragma argsused
int eval_near(char *cp_line)
{
   ia_option['N'] = TRUE ;

   return 0 ;
}

/* _short �̏��� */
#pragma argsused
int eval_short(char *cp_line)
{
   ia_option['N'] = FALSE ;

   return 0 ;
}

/* �b�o�t�^�C�v�̎w�� */
int eval_cpu(char *cp_line)
{
   int i ;
   char ca_nfn[256] ;
   static char *cpa_cpuname[] = { "CPU80","CPU85","CPUZ80","CPU180","ASMZ80",NULL } ;
   static int ia_cputable[] = { 0, CPU80 , CPU85 , CPUZ80,  CPU180 , ASMZ80 } ;

   cutspc(cp_line) ;                              /* ���݂���菜�� */
   gettoken(cp_line,WHITESPACE,LDELIM,ca_nfn) ;   /* ���������̐؂�o�� */
   i = app_scantbl(cpa_cpuname,ca_nfn) ;
   if (i == 0) {
         app_error(999,1) ;		/* CPU_MODE ���s�K��		*/
   }
   i_cpu = ia_cputable[i] ;
   if (i_cpu == ASMZ80) {		/* 95/03/29	*/
      app_outstr("\t.Z80\n") ;
   } else {
      app_outstr("\t.8085\n") ;
   }
   return 0 ;
}

/* _loop �̏��� */
int eval_loop(char *cp_line)
{
   char ca_buf[LBLBFLEN] ,
        ca_arg1[ARGSTRMAX],         /* ������̍��� */
        ca_cond[ARGSTRMAX],         /* ������Z�q */
        ca_arg2[ARGSTRMAX] ;        /* ������̉E�� */

   if (++i_looplev >= LPNESTMAX) {
      app_error(LOOP_STK_OVER,1) ;         /* loop �̃l�X�g���[������ */
   }
   i_loopnum++ ;

   ia_loopn[i_looplev] = i_loopnum ;
   ia_contf[i_looplev] = FALSE ;
   ia_exitf[i_looplev] = FALSE ;

   /* ������̐؂�o�� */
   cp_line = get_args(cp_line,ca_arg1,ca_cond,ca_arg2,NULL) ;

   if (strcmp(ca_cond,OP_LET) == 0) { /* ������Z�q���H */
      cutspc(ca_arg1) ;
      cutspc(ca_arg2) ;
      mov_output(ca_arg1,ca_arg2) ;
   } else if (app_strlen(ca_cond) != 0) { /* �����łȂ��Ƃ� */
      app_error(LARGCOND_ERROR,1) ;
   }

   tmplabel_output(TMPL_BEGIN,i_loopnum) ; /* ���[�v�̓��̃��x�� */

   cp_lcondstk[i_looplev] = strdup(ca_arg1) ; /* �����̓W�J�͂��Ƃ� */
   cp_lcondstk2[i_looplev] = NULL ;

   return 0 ;
}

/* _endl �̏��� */
#pragma argsused
int eval_endl(char *cp_line)
{
   if ((i_looplev == 0) || (cp_lcondstk[i_looplev] == NULL)) {
      app_error(LOOP_STK_EMPTY,1) ;     /* ���[�v�O�� _endl */
   }

   tmplabel_output(TMPL_CONTINUE,ia_loopn[i_looplev]) ; /* continue ���x�� */
   loop80_output(cp_lcondstk[i_looplev],TMPL_BEGIN,ia_loopn[i_looplev]) ;
   app_free(cp_lcondstk[i_looplev]) ;     /* ���[�v�ϐ�������̊J�� */

   if (ia_exitf[i_looplev] == TRUE) {
      tmplabel_output(TMPL_EXIT,ia_loopn[i_looplev]) ; /* exit ���x�� */
   }
   i_looplev-- ;

   return 0 ;
}
