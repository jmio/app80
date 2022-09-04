/*
                         Generic APP Source File

                          Filename : CPROTO.C
                          Version  : 95/03/03
                            Written by Mio

                  C �\�[�X�t�@�C���A�v���g�^�C�v���[�J
*/

#include <stdio.h>
#include <string.h>

/* �e CPU ���� INCLUDE */
#include "appdefs.h"
#include "appsub.h"
#include "apperr.h"

/* �e CPU �ˑ� INCLUDE */
#include "cproto.h"

/* �g�b�v�t�@�C���̕]�� */
int eval_file (char *cp_fname,int i_paramcount,char **cpp_paramstr)
{
   /* �J�n�i�R�}���h���C�� define ���j���� */
   eval_begin() ;

   /* �g�b�v�t�@�C���̕]�� */
   include_file(cp_fname) ;

   /* �������� */
   eval_exit() ;
   return 0 ;
}

/* �J�n���� */
int eval_begin(void)
{
   app_outstr("/*\n") ;
   app_outstr("    ���ӁF���̃t�@�C���͎����������ꂽ���̂ł�\n") ;
   app_outstr("*/\n") ;
   app_outstr("\n") ;
   
   return 0 ;
}

/* �G���[�������̏I���O���� (APPSUB.C ����R�[�������) */
int eval_errexit(int i_errcode)
{
    return i_errcode ;
}

/* �ʏ�̏I���O���� */
int eval_exit(void)
{
    return 0 ;
}


/* �s�̕]��  eval_line */
int eval_line(char *cp_line)
{
   static char caa_linebuf[LINEBUFS][LINEBUFSIZ] ;
   static long l_lineno = 0 ;

#define i_ml(l) ((int)((l) % LINEBUFS))

   char *cp ;
   int i_lineptr = i_ml(l_lineno) ;
   long l ;

   strcpy(caa_linebuf[i_lineptr],cp_line) ;

   /* �֐��̊J�n�s���������� */
   if ((cp_line[0] == '{') && (cp_line[1] == '\n')) {
      for (l = l_lineno-1 ; (l > 0) && (l > l_lineno-LINEBUFS) ; l--) {
         /* static �Ȋ֐��͏o�͂��Ȃ� */
         if (strstr(caa_linebuf[i_ml(l)],STATIC_KW) != NULL) {
            break ;
         }
         /* �R�����g�܂ł����̂ڂ��� */
         if (caa_linebuf[i_ml(l)][0] == '/') {
            /* �t���ŏo�� */
            for (;l < l_lineno-1;l++) {
               /* �v���v���Z�b�T�w�߈ȊO���o�� */
               if (caa_linebuf[i_ml(l)][0] != '#') {
                  app_outstr(caa_linebuf[i_ml(l)]) ;
               }
            }
            /* ��`�s�ɃZ�~�R�����̕t�� */
            if ((cp = strchr(caa_linebuf[i_ml(l)],'\n')) != NULL) {
               strcpy(cp," ;\n") ;
            }
            /* ��`�s�̏o�� */
            app_outstr(caa_linebuf[i_ml(l)]) ;
            app_outstr("\n") ; /* �󔒍s���J���� */
            break ;
         }
      }
      /* �s�o�b�t�@�I�[�o�[�t���[ */
      if (l <= l_lineno - LINEBUFS) {
         app_error(CPROTO_ERR1,1) ;
      }
   }

   l_lineno++ ;
   return ;

#undef i_ml
}
