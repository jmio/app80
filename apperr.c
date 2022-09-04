/*
                         Generic APP Source File

                          Filename : APPERR.C
                          Version  : 95/03/01
                            Written by Mio

                          APP ���ʃG���[����
*/

#include <stdio.h>
#include <string.h>

#include "appdefs.h"
#include "appsub.h"
#include "apperr.h"

long l_err_lineno ;
char *cp_err_fname ;

/* ----------------------------------------------------------- */

/* �e CPU �ʁA�G���[�������㏈�����[�`�� (EVALxx.C �ɑ���) */
int eval_errexit(int) ;

/* ----------------------------------------------------------- */

/* �G���[���� */
void app_error(int i_errcode,int i_isfatal)
{
   if (l_err_lineno > 0) {
      fprintf(_erout,"%s(%ld):",cp_err_fname,(long)l_err_lineno) ;
   }

   switch (i_errcode) {
   case ERR_NULL_STR:
      fputs("�������A���P�[�V�����E�G���[(Null Pointer => strlen)\n",_erout) ;
   break ;
   case FILE_NOT_FOUND:
      fputs("�C���N���[�h�E�t�@�C����������܂���:",_erout) ;
   break ;
   case TOO_MANY_ARG:
      fputs("�R�}���h���C���������������܂�\n",_erout) ;
   break ;

   case IF_STK_OVER:
      fputs("if �̃l�X�e�B���O���[�����܂�\n",_erout) ;
   break ;
   case IF_STK_EMPTY:
      fputs("if �� endif �̐��������܂���\n",_erout) ;
   break ;

   case SW_STK_OVER:
      fputs("switch �̃l�X�e�B���O���[�����܂�\n",_erout) ;
   break ;
   case SW_STK_EMPTY:
      fputs("switch �u���b�N�����߂��u���b�N�O�ɂ���܂�\n",_erout) ;
   break ;

   case LOOP_STK_OVER:
      fputs("���[�v�̃l�X�e�B���O���[�����܂�\n",_erout) ;
   break ;
   case LOOP_STK_EMPTY:
      fputs("���[�v�̑Ή������Ă��܂���\n",_erout) ;
   break ;
   case NOT_IN_LOOP:
      fputs("���[�v�u���b�N�O�Ƀ��[�v�����߂�����܂�\n",_erout) ;
   break ;
   case FOR_ARGUMENT:
      fputs("for �̈����̐����������܂�\n",_erout) ;
   break ;

   case NO_ENDIF:
      fputs("�Ή��̎��Ă��Ȃ� if/switch ��������܂�\n",_erout) ;
   break ;
   case NO_ENDLOOP:
      fputs("�Ή��̎��Ă��Ȃ����[�v��������܂�\n",_erout) ;
   break ;

   case REP_ERROR:
      fputs("���ʂ̑Ή������Ă��܂���\n",_erout) ;
   break ;
   case QUOTE_ERROR:
      fputs("�N�I�[�g�������Ă��܂���\n",_erout) ;
   break ;

   case LANDOR_ERROR:
      fputs(" || �� && �̍��݂͂ł��܂���\n",_erout) ;
   break ;
   case LEXTRA_ERROR:
   case LARG1MISS_ERROR:
   case LARGCOND_ERROR:
      fputs("�������̕��@������������܂���\n",_erout) ;
   break ;
   case EXP_SYNTAX_ERROR:
      fputs("���̕��@������������܂���\n",_erout) ;
   break ;

   case LVALUE_ERROR:
      fputs("�������̍��ӂɓK�؂łȂ��l������܂�\n",_erout) ;
   break ;
   case ARGTYPE_ERROR:
      fputs("�������̗��ӂ̌^���w�肵�Ă�������\n",_erout) ;
   break ;
   case TYPE_NOT_MATCH:
      fputs("�������̗��ӂ̌^���قȂ��Ă��܂�\n",_erout) ;
   break ;

   case SELF_INCLUDE:
      fputs("�������g���C���N���[�h���悤�Ƃ��Ă��܂�\n",_erout) ;
   break ;

   case FNAME_ERROR:
      fputs("�֐���������������܂���\n",_erout) ;
   break ;
   case FUNDUP_ERROR:
      fputs("���ɓ����̊֐�����`�ς݂ł�\n",_erout) ;
   break ;
   case FUNDEF_ERROR:
      fputs("�����`���̈قȂ铯���̒�`������܂�\n",_erout) ;
   break ;
   case ENDF_ERROR:
      fputs("�֐��̊O�Ɋ֐������߂�����܂�\n",_erout) ;
   break ;
   case INFUNC_ERROR:
      fputs("�֐������q�ɂ��邱�Ƃ͂ł��܂���\n",_erout) ;
   break ;
   case FUNARG_ERROR1:
      fputs("�����̐����Ⴂ�܂�\n",_erout) ;
   break ;
   case FUNARG_ERROR2:
      fputs("�֐��̌^(func,proc)���Ⴂ�܂�\n",_erout) ;
   break ;
   case FUNUNDEF_ERROR:
      fputs("����`�̊֐����Ăяo�����Ƃ��Ă��܂�\n",_erout) ;
   break ;
   case RETARG_ERROR:
      fputs("procedure �ɖ߂�l�͕s�v�ł�\n",_erout) ;
   break ;

   case IFD_STK_OVER:
      fputs("if(n)def �̃l�X�e�B���O���[�����܂�\n",_erout) ;
   break ;
   case IFD_STK_EMPTY:
      fputs("if(n)def �� endifdef �̐��������܂���\n",_erout) ;
   break ;

   case STRDUP_FAIL:
      fputs("�������A���P�[�V�����E�G���[(Fixlen Str too short)\n",_erout) ;
   break ;
   case FREE_NULL:
      fputs("�������A���P�[�V�����E�G���[(Null Pointer => free)\n",_erout) ;
   break ;
   case MEMORY_NOT_ENOUGH:
      fputs("�������A���P�[�V�����E�G���[(Not Enough Memory)\n",_erout) ;
   break ;

   case CPROTO_ERR1:
      fputs("�s�o�b�t�@�ɂ����܂�Ȃ���`������܂�\n",_erout) ;
   break ;
   case NOT_SUPPORTED:
      fputs("���̑���̓T�|�[�g����Ă��܂���\n",_erout) ;
   break ;

   default:
      if (i_isfatal > 0) {
        fprintf(_erout,"����`�̃G���[���������܂���(errcode=%d)\n",i_errcode);
      }
   }

   eval_errexit(i_errcode) ; /* EVALxx.C ���̃G���[�㏈�����Ăяo�� */
   if (i_isfatal == 0) {
      return ; /* ��̃��b�Z�[�W�̕\���� perror ���ɔC����Ƃ� */
   }
   exit(i_errcode) ;
}

/* ���[�j���O���� */
void app_warning(char *cp)
{
   if (l_err_lineno > 0) {
      fprintf(_erout,"%s(%ld):",cp_err_fname,(long)l_err_lineno) ;
   }
   fputs(cp,_erout) ;
   return ;
}

