/*
                         Generic APP Source File

                          Filename : IFDEFS.C
                          Version  : 95/03/01
                            Written by Mio

            �e�@������A�V���{����`�^�C���N���[�h�֌W����������
*/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "appdefs.h"
#include "appsub.h"
#include "apperr.h"
#include "proto.h"
#include "ifdefs.h"

/* _define �Œ�`����� APP �����V���{�����L�����Ă������߂�
   �V���{�����X�g�̍\�� */
typedef struct SYMLIST {
   char *cp_symbol ;           /* �V���{��������(malloc �����) */
   void *cp_value ;            /* �V���{���̎��l(�����̊g���p) */
   struct SYMLIST *sp_next ;   /* ���������|�C���^ */
} symlist ;

static int i_out_enable_flag  = ON ;    /* ifdef �ɂ��o�� enable �t���O */

static int ia_ifdstk[IFDSTKSIZ] ;       /* ifdef �l�X�e�B���O�X�^�b�N */
static int i_ifdlev           = 0 ;     /* ifdef �̃l�X�e�B���O���x�� */

static char *cpa_ipath[MAXINCPATH] ;    /* �C���N���[�h�p�X�z�� */
static int i_incpaths = 0 ;

static symlist *SYMTOP = NULL ;  /* �V���{�����X�g�̐擪 */

#define SELF_NAME  "$SELF$"     /* �������g�� basename �ƒu�����镶���� */

/* ifdef,ifndef �𔭌������Ƃ��A�v�b�V������ */
static
void ifdefpush(int i_onoff)
{
   if (i_ifdlev >= IFDSTKSIZ) {
      app_error(IFD_STK_OVER,1) ;
   }
   ia_ifdstk[i_ifdlev++] = i_onoff ;
}

/* endifdef �Ń|�b�v���� */
static
int ifdefpop(void)
{
   if (i_ifdlev == 0) {
      app_error(IFD_STK_EMPTY,1) ;
   }
   return (int)(ia_ifdstk[--i_ifdlev]) ;
}

/* �V���{������`����Ă��邩�ǂ����𒲂ׂ� */
static
int symdefine_p(char *cp)
{
   symlist *slp = SYMTOP ;
   while (slp != NULL) {
      if (strcmp((slp->cp_symbol),cp) == 0) {
         return TRUE ;
      }
      slp = slp->sp_next ;
   }
   return FALSE ;
}

/* ���݂� ifdef �o�͐����Ԃ𓾂� */
int output_enable(void)
{
   return i_out_enable_flag ;
}

/* �����V���{�����`���� */
void sympush(char *cp)
{
   symlist *slp ;

   /* ���X�g�\���̂̍쐬 */
   slp            = (symlist *)app_malloc(sizeof(symlist)) ;

   /* �V���{��������̋L������m�ۂ��A�R�s�[ */
   slp->cp_symbol = app_strdup(cp) ;

   /* �V���{���̒l�i���g�p�j */
   slp->cp_value  = (void *)NULL ;

   /* ���X�g�������N������ */
   slp->sp_next   = SYMTOP ;
   SYMTOP         = slp ;
}

/* �����V���{���e�[�u���̃N���A : ��n���� call ����� */
void symclear(void)
{
   symlist *slp = SYMTOP ;

   while (slp != NULL) {
      app_free(slp->cp_symbol) ;
      /* app_free(slp->cp_value) ; */ /* value �g�����g�p */
      slp = slp->sp_next ;
   }
}

/* _define �̏��� */
int eval_define(char *cp_buf)
{
   char ca_symname[MAXSYMLEN] ;

   cutspc(cp_buf) ;                                  /* ���݂���菜�� */
   gettoken(cp_buf,WHITESPACE,LDELIM,ca_symname) ;   /* �V���{�����̐؂�o�� */
   if (symdefine_p(ca_symname) == FALSE) {
      sympush(ca_symname) ;                          /* �V���{���̒�` */
   }
   return 0 ;
}

/* _ifdef,_ifndef �̏��� */
static 
int _eval_ifdef(char *cp_buf,int i_condition)
{
   char ca_symname[MAXSYMLEN] ;

   cutspc(cp_buf) ;                                  /* ���݂���菜�� */
   gettoken(cp_buf,WHITESPACE,LDELIM,ca_symname) ;   /* �V���{�����̐؂�o�� */
   ifdefpush(i_out_enable_flag) ;                    /* ���݂̏����� PUSH */

   /* �����ɍ���Ȃ��u���b�N���ł́A�l�X�g�̂݃J�E���g���� */
   if (i_out_enable_flag == ON) {
      i_out_enable_flag = (symdefine_p(ca_symname) == i_condition ? ON : OFF);
   }
   return 0 ;
}

/* _ifdef �̏��� */
int eval_ifdef(char *cp_buf)
{
    return _eval_ifdef(cp_buf,TRUE) ;
}

/* _ifndef �̏��� */
int eval_ifndef(char *cp_buf)
{
    return _eval_ifdef(cp_buf,FALSE) ;
}

/* _elsedef �̏��� */
#pragma argsused
int eval_elsedef(char *cp_buf)
{
   if (i_ifdlev == 0) {
      app_error(IFD_STK_EMPTY,1) ;
   }
   /* �P���x���O�� OFF �̂Ƃ��������] */
   if (ia_ifdstk[i_ifdlev-1] == ON) {
      i_out_enable_flag = (i_out_enable_flag == ON ? OFF : ON ) ;
   }
   return 0 ;
}

/* _endifdef �̏��� */
#pragma argsused
int eval_endifdef(char *cp_buf)
{
   i_out_enable_flag = ifdefpop() ;
   return 0 ;
}

/* �C���N���[�h�t�@�C���̌��� */
static
int search_include_file(char *cp_fname)
{
   int i = -1,
       i_count = 0 ;
   char ca_buf[PNLENGTH] ;

   strcpy(ca_buf,cp_fname) ;
   while (((i = open(ca_buf,O_RDONLY)) < 0) && (i_count < i_incpaths)) {
      strcpy(ca_buf,cpa_ipath[i_count]) ; /* �C���N���[�h�p�X�̈������ */
      i = app_strlen(ca_buf) ;
      if (ca_buf[i-1] != PATH_SEPL) {
         ca_buf[i] = PATH_SEPL ; /* �p�X��؂�L���ȊO�ŏI����Ă���ǉ� */
         ca_buf[i+1]   = '\0' ;
      }
      strcat(ca_buf,cp_fname) ; /* �t���p�X���𓾂� */
      i_count++ ;
   }

   if (i < 0) { /* ������Ȃ������� */
      app_error(FILE_NOT_FOUND,0) ;
      fprintf(_erout,"%s\n",cp_fname) ;
      exit(FILE_NOT_FOUND) ;
   }

   close(i) ;
   return include_file(ca_buf) ;
}

/* _include,_header �̏��� */
int eval_include(char *cp_buf)
{
   char ca_nfn[PNLENGTH] ;

   cutspc(cp_buf) ;                              /* ���݂���菜�� */
   gettoken(cp_buf,WHITESPACE,LDELIM,ca_nfn) ;   /* �t�@�C�����̐؂�o�� */

   if (strstr(ca_nfn,SELF_NAME) != NULL) {
      char ca_mynam[PNLENGTH] ;
      get_ownbasename(ca_mynam) ;                /* �x�[�X�l�[���𓾂� */
      str_replace(ca_nfn,SELF_NAME,ca_mynam) ;   /* $SELF$ �ƒu�� */
   }

   return search_include_file(ca_nfn) ; /* �C���N���[�h */
}

/* �C���N���[�h�p�X�̒ǉ� */
int add_incpath(char *cp_path)
{
   if (i_incpaths >= MAXINCPATH) {
      app_error(TOO_MANY_ARG,1) ;
   }
   cutspc(cp_path) ;
   if (app_strlen(cp_path) > 0) {
      cpa_ipath[i_incpaths++] = app_strdup(cp_path) ;
   }
   return 0 ;
}

/* �C���N���[�h�p�X�̈�̊J�� */
void clear_incpath(void)
{
   int i ;
   for (i = 0 ; i < i_incpaths ; i++) {
      app_free(cpa_ipath[i]) ;
   }
   i_incpaths = 0 ;
}

/* _warning �̏��� */
int eval_warning(char *cp_buf)
{
   cutspc(cp_buf) ;
   strcat(cp_buf,"\n") ;
   app_warning(cp_buf) ;

   return 0 ;
}
