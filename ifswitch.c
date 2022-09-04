/*
                         Generic APP Source File

                          Filename : IFSWITCH.C
                          Version  : 95/03/01
                            Written by Mio

                             �������򏈗�
*/

#include <stdio.h>
#include <string.h>

#include "appmain.h"
#include "appdefs.h"
#include "appsub.h"
#include "apperr.h"
#include "proto.h"
#include "condexp.h"
#include "ifswitch.h"

static int i_iflev = 0 ;           /* �����u���b�N�l�X�e�B���O���x�� */
static int i_ifnum = 0 ;           /* ��������郉�x���ɕt�������ԍ� */
static int i_swlev = 0 ;           /* _switch �u���b�N�l�X�e�B���O���x�� */

static int i_delayflag = FALSE ;   /* �W�����v�̐�����x�点�邽�߂̃t���O */

static int ia_rifn[IFNESTMAX]  ;   /* _if ���x���ԍ��X�^�b�N */
static int ia_felse[IFNESTMAX] ;   /* _else ���o�X�^�b�N */
static int ia_cactr[SWNESTMAX] ;   /* _switch �l�X�g�X�^�b�N */

/* _if �̏��� */
int eval_if(char *cp_line)
{
   if (++i_iflev >= IFNESTMAX) {
      app_error(IF_STK_OVER,1) ;  /* _if �l�X�e�B���O�I�[�o�[ */
   }
   ++i_ifnum ;
   ia_rifn[i_iflev]  = i_ifnum ;
   ia_felse[i_iflev] = FALSE ;
   eval_cond(cp_line,TRUE,TMPL_ELSE,i_ifnum,0,FALSE) ;

   return 0 ;
}
/* _else �̏��� */
#pragma argsused
int eval_else(char *cp_line)
{
   if (i_iflev == 0) {
      app_error(IF_STK_EMPTY,1) ;             /* _if ���Ȃ��̂� _else ���o */
   }
   ia_felse[i_iflev] = TRUE ;
   if (i_delayflag == FALSE) {
      ncjmp_output(TMPL_ENDIF,ia_rifn[i_iflev]) ; /* _endif �ւ̃W�����v */
   }
   tmplabel_output(TMPL_ELSE,ia_rifn[i_iflev]) ; /* ���x���o�� */

   return 0 ;
}

/* _endif �̏��� */
#pragma argsused
int eval_endif(char *cp_line)
{
   if (i_iflev == 0) {
      app_error(IF_STK_EMPTY,1) ;        /* _if ���Ȃ��̂� _endif ���o */
   }

   if (ia_felse[i_iflev] == TRUE) {
      tmplabel_output(TMPL_ENDIF,ia_rifn[i_iflev]) ; /* ���x���o�� */
   } else {
      tmplabel_output(TMPL_ELSE,ia_rifn[i_iflev]) ; /* ���x���o�� */
   }
   i_iflev-- ;

   return 0 ;
}

/* _switch �̏��� */
#pragma argsused
int eval_switch(char *cp_line)
{
   if (++i_swlev >= SWNESTMAX) {
      app_error(SW_STK_OVER,1) ; /* _switch �̃l�X�g���[������ */
   }
   ia_cactr[i_swlev] = 0 ; /* _case �J�E���^�̃N���A */
   return 0 ;
}

/* _case �̏��� */
int eval_case(char *cp_line)
{
   if (i_swlev == 0) {
      app_error(SW_STK_EMPTY,1) ; /* _switch �̂Ȃ� _case */
   }
   if (ia_cactr[i_swlev] > 0) {
      (void)eval_else(cp_line) ; /* �Q��ڈȍ~�� _elseif */
   }
   ia_cactr[i_swlev]++ ; /* case �J�E���^++ */
   return eval_if(cp_line) ; /* _if �̏��� */
}

/* _default �̏��� */
int eval_default(char *cp_line)
{
   if (i_swlev == 0) {
      app_error(SW_STK_EMPTY,1) ; /* _switch �̂Ȃ� _default */
   }
   return eval_else(cp_line) ; /* _else �̏��� */
}

/* _endsw �̏��� */
int eval_endsw(char *cp_line)
{
   if (i_swlev == 0) {
      app_error(SW_STK_EMPTY,1) ; /* _switch �̂Ȃ� _endsw */
   }
   while (ia_cactr[i_swlev]-- > 0) {
      (void)eval_endif(cp_line) ;
   }
   i_swlev-- ;
   return 0 ;
}

/* �� if �u���b�N�̒��� ? */
int in_ifblkp(void)
{
   return i_iflev ;
}
