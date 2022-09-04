/*
                         Generic APP Source File

                          Filename : MISC86.C
                          Version  : 95/03/14
                            Written by Mio

                            APP86 �ˑ�����
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
#include "misc86.h"

static int i_loopcxlev = 0 ;            /* loopcx �u���b�N�l�X�e�B���O���x�� */

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

/* _signed �̏��� */
#pragma argsused
int eval_signed(char *cp_line)
{
   ia_option['S'] = TRUE ;

   return 0 ;
}

/* _short �̏��� */
#pragma argsused
int eval_unsigned(char *cp_line)
{
   ia_option['S'] = FALSE ;

   return 0 ;
}

/* _loopcx �̏��� */
int eval_loopcx(char *cp_line)
{
   char ca_buf[LBLBFLEN] ;

   if (++i_looplev >= LPNESTMAX) {
      app_error(LOOP_STK_OVER,1) ;         /* loop �̃l�X�g���[������ */
   }
   i_loopnum++ ;

   ia_loopn[i_looplev] = i_loopnum ;
   ia_contf[i_looplev] = FALSE ;
   ia_exitf[i_looplev] = FALSE ;

   if (i_loopcxlev++ > 0) {
      push_output("CX") ;                  /* ���x���P�ȏ�� CX ���v�b�V�� */
   }

   cutspc(cp_line) ;                              /* ���݂���菜�� */
   gettoken(cp_line,WHITESPACE,LDELIM,ca_buf) ;   /* �V���{�����̐؂�o�� */

   if (app_strlen(ca_buf) != 0) {          /* �J�E���g���̑�� */
      mov_output("CX",ca_buf) ;
   }

   if (app_isnum(ca_buf) == FALSE) {       /* �͂����萔���łȂ��Ȃ� */
      char ca_tmp[LBLBFLEN] ;
      sprintf(ca_tmp,"%s_%d",TMPL_EXIT,i_loopnum) ;
      jcxz_output(ca_tmp) ;
      ia_exitf[i_looplev] = TRUE ;
   }

   tmplabel_output(TMPL_BEGIN,i_loopnum) ; /* ���[�v�̓��̃��x�� */

   cp_lcondstk[i_looplev] = NULL ; /* �����̓W�J�͂��Ƃ� */
   cp_lcondstk2[i_looplev] = NULL ; /* �����̓W�J�͂��Ƃ� */

   return 0 ;
}

/* _endl �̏��� */
#pragma argsused
int eval_endl(char *cp_line)
{
   char ca_tmp[LBLBFLEN] ;

   if ((i_looplev == 0) || (i_loopcxlev == 0)) {
      app_error(LOOP_STK_EMPTY,1) ;     /* ���[�v�O�� _endl */
   }

   tmplabel_output(TMPL_CONTINUE,ia_loopn[i_looplev]) ; /* continue ���x�� */
   sprintf(ca_tmp,"%s_%d",TMPL_BEGIN,ia_loopn[i_looplev]) ;
   loop_output(ca_tmp) ;

   if (--i_loopcxlev > 0) {
      pop_output("CX") ;
   }
   if (ia_exitf[i_looplev] == TRUE) {
      tmplabel_output(TMPL_EXIT,ia_loopn[i_looplev]) ; /* exit ���x�� */
   }
   i_looplev-- ;

   return 0 ;
}
