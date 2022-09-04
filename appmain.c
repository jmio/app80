/*
                         Generic APP Source File

                          Filename : APPMAIN.C
                          Version  : 95/03/01
                            Written by Mio

                    Generic APP ���C�����[�`���t�@�C��
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "appmain.h"
#include "appsub.h"
#include "appdefs.h"
#include "helpmsg.h"

/* �O���[�o���ϐ� */
FILE *fp_out ;                      /* �o�̓t�@�C�� */
int ia_option[256] ;                 /* �I�v�V���� on/off �t���O */

#ifdef __MSDOS__
/* HELP���b�Z�[�W�i�{���͊֐��ł͂Ȃ��j */
extern far int HELPMSG() ;
#else
#define far
#endif

/* �t�@�C���̕]�� (EVALxx.C �ɑ���) */
int eval_file (char *cp_fname,int i_paramconut,char **cpp_paramstr) ;

/* ���C�����[�`�� */
int main(int argc,char **argv)
{
   char *cpa_fname[MAXFNAMES] ,        /* �t�@�C�����p�����[�^ */
        *cpa_paramstr[MAXPARAMS] ;     /* �t�@�C�����ȊO�̃p�����[�^ */
   int  i ,
        i_fnames ,                     /* �t�@�C�����p�����[�^�� */
        i_paramcount ;                 /* �t�@�C�����ȊO�̃p�����[�^�� */

   /* �^�C�g���\�� */
   printf("%s %s %s\n",GAPP_TITLE,GAPP_VERSION,GAPP_COPYRIGHT) ;

   /* �t�@�C�����p�Œ蒷�̈�̊m�� */
   for (i = 0 ; i < MAXFNAMES ; i++) {
      cpa_fname[i] = app_malloc(PNLENGTH) ;
   }

   /* �R�}���h���C���p�����[�^�̏��� */
   i_fnames = getparam(argc,argv,cpa_fname,cpa_paramstr,&i_paramcount) ;

#ifdef __MSDOS__
   /* �������c�ʂ̕\�� */
   printf("Available memory %ld bytes\n",(long)app_avmem()) ;
#endif

   /* �t�@�C�������w�肳��Ă��Ȃ����̏��� */
   if (i_fnames == 0) {
      strcpy(cpa_fname[0],"CON") ;
      strcpy(cpa_fname[1],"CON") ;
   } else {
      /* �g���q�̎�舵�� */
      if (getdotpos(cpa_fname[0]) == NULL) { 
         /* ���̓t�@�C�����Ɋg���q���Ȃ��Ƃ� */
         strcat(cpa_fname[0],DEFAULT_IN_EXT) ;
      }
      /* �o�̓t�@�C�����̏ȗ� */
      if (i_fnames == 1) {
         char *cp ;
         /* �g���q�܂Ő؂�o���� .ASM ��ǉ� */
         strcpy(cpa_fname[1],cpa_fname[0]) ;
         cp = getdotpos(cpa_fname[1]) ;
         if (cp != NULL) {
            *cp = '\0' ; /* �g���q�ȍ~���J�b�g */
         }
         if (ia_option['H']) {
            strcat(cpa_fname[1],DEFAULT_HD_EXT) ;
         } else {
            strcat(cpa_fname[1],DEFAULT_OUT_EXT) ;
         }
      }
   }

   /* �o�̓t�@�C���̃I�[�v�� */
   fp_out = open_file(cpa_fname[1],"wt") ;

   /* ���̓t�@�C���̕]�� */
   eval_file(cpa_fname[0],i_paramcount,cpa_paramstr) ;

   /* �t�@�C�����p�Œ蒷�̈�̊J�� */
   for (i = 0 ; i < MAXFNAMES ; i++) {
      app_free(cpa_fname[i]) ;
   }

   /* �I�v�V�����p�ϒ��̈�̊J�� */
   for (i = 0 ; i < i_paramcount ; i++) {
      app_free(cpa_paramstr[i]) ;
   }

   /* �o�̓t�@�C���̃N���[�Y */
   fclose(fp_out) ;
   return 0 ;
}

/* �R�}���h���C���E�p�����[�^�̉��� */
int getparam(int argc,char **argv,char **cpp,char **cpp_o,int *i_opc)
{
   int i,ii ,
       i_fpc = 0,           /* �I�v�V�����ȊO�̃p�����[�^�J�E���^ */
       i_result = 0;        /* too many args �`�F�b�N�p�p�����[�^�J�E���^ */
   char c ;

   /* �I�v�V�����z��̃N���A */
   for (i = '\0' ; i <= '\xff' ; i++) {
      ia_option[i] = OFF ;
   }

   /* �I�v�V�����p�����[�^�J�E���^�N���A */
   *i_opc = 0 ;

   /* �p�����[�^�̃X�L���� */
   for (i = 1;i < argc;i++) {
      if (argv[i][0] == '-') {

         for (ii = 1;ii < app_strlen(argv[i]);ii++) {
            c = toupper(argv[i][ii]) ;
            switch(c) {
            case HELP_OPTION:
               printf("%s\n",(char far *)HELPMSG) ;
               exit(0) ;
            case INCLUDE_OPTION:
            case DEFINE_OPTION:
               if (*i_opc < MAXPARAMS) {
                  /* ������I�v�V�����͂��̏�ŗ̈���m�ۂ��ăR�s�[ */
                  cpp_o[(*i_opc)++] = app_strdup(&(argv[i][ii])) ;
                  ii = app_strlen(argv[i]) ; /* break of for */
                  continue ; /* break */
               }
            break ;
            default:
               ia_option[c] = ON ;
            }
         }
      } else {
         i_result++ ;
         if (i_fpc < MAXFNAMES) { 
            /* �t�@�C�����̓t�@�C�����p�Œ蒷�̈�ɃR�s�[ */
            strcpy(cpp[i_fpc++],argv[i]) ;
         }
      }
   }
   return i_result ;
}
