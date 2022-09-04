/*
                         Generic APP Source File

                          Filename : FUNCTION.C
                          Version  : 95/03/09
                            Written by Mio

                              �֐��̏���
*/

#include <stdio.h>
#include <string.h>

#include "appmain.h"
#include "appdefs.h"
#include "appsub.h"
#include "apperr.h"
#include "superstr.h"
#include "proto.h"
#include "ifswitch.h"
#include "function.h"

/* function or procedure */
enum FUNCPRC  { FUNC , PROC } ;
enum NORMINTR { NORM , INTR } ;

/* �֐����\���� */
typedef struct FUNC_TBL {
   char *cp_fname ;             /* �֐��� */

   int  i_args ;                /* �������̐� */
   char **cpp_fargs ;           /* �������X�g�i�Ȃ��Ƃ��� NULLptr�j */

   int  i_save ;                /* �ۑ����W�X�^�� */
   char **cpp_fsave ;           /* �ۑ����W�X�^���X�g�i�Ȃ��Ƃ��� NULLptr�j */

   int  i_predef ;              /* �O�ɒ�`����Ă��邩 */
   int  i_called ;              /* �Ăяo���ꂽ�� */
   int  i_func_or_proc ;        /* _procedure , _function �̕� */
   struct FUNC_TBL *ftp_next ;  /* �e�[�u�������N */
} func_tbl ;

static func_tbl *FUNCTOP = NULL ; 
static func_tbl *fun_tmp = NULL ; /* �������� function ��� */
static int i_retfound ;           /* return �����������H */

#define KW_ENDF  "endf"           /* return ���� JMP �o�͔��f�p�L�[���[�h */

/* �֐��`���̃p�����[�^��݂Ƃ� */
static
func_tbl *get_funargs(char *cp)
{
   char ca_tmp[MAXARGLEN] ;
   int i ;
   func_tbl *ftp = app_malloc(sizeof(func_tbl)) ;

   ftp->cp_fname = NULL ;
   ftp->i_args   = 0 ;
   ftp->cpp_fargs = NULL ;
   ftp->i_save   = 0 ;
   ftp->cpp_fsave = NULL ;
   ftp->ftp_next = NULL ;

   /* �֐����̎��o�� */
   cp = gettoken(cp,WHITESPACE,FUNDELIMS,ca_tmp) ;
   cutspc(ca_tmp) ;
   i = app_strlen(ca_tmp) ;
   if ((i == 0) || (i > MAXFNLEN)) {
      app_error(FNAME_ERROR,1) ; /* �֐����s�� */
   }
   ftp->cp_fname = app_strdup(ca_tmp) ;
   cutspc(cp) ;

   /* �����������̎��o�� */
   if (cp[0] == '(') { /* �����������鎞���� */
      cp = gettoken(&cp[1],WHITESPACE,ARGDELIMS,ca_tmp) ;
      cutspc(ca_tmp) ; /* �O��̃X�y�[�X���J�b�g */
      if (app_strlen(ca_tmp) > 0) { /* ���e�������������� */
         ftp->i_args = strtocpa(ca_tmp,&(ftp->cpp_fargs),MAXARGS,ARGCUTDELIM) ;
      }
      cutspc(++cp) ; /* �����ʂ̎����� */
   }

   /* �ۑ����W�X�^�̎��o�� */
   if (cp[0] == ':') { /* �ۑ����W�X�^�w��̂��鎞���� */
      cp = gettoken(&cp[1],WHITESPACE,REGDELIMS,ca_tmp) ;
      cutspc(ca_tmp) ; /* �O��̃X�y�[�X���J�b�g */
      if (app_strlen(ca_tmp) > 0) { /* ���e�������������� */
         ftp->i_save = strtocpa(ca_tmp,&(ftp->cpp_fsave),MAXARGS,ARGCUTDELIM) ;
      }
   }

   return ftp ;
}

/* �֐��`���̈�̊J�� */
static
void ftp_release(func_tbl *ftp)
{
   if (ftp == NULL) {
      app_error(FUNFREE_NULL,1) ;
   }

   /* ���O�̈�̊J�� */
   app_free(ftp->cp_fname) ;

   /* �����̈�̊J�� */
   if (ftp->i_args > 0) {
      cpa_release(ftp->cpp_fargs) ;
   }

   /* �ۑ��ϐ��̈�̊J�� */
   if (ftp->i_save > 0) {
      cpa_release(ftp->cpp_fsave) ;
   }
}

/* �֐���񃊃X�g�̊J�� */
static
void ftp_relall(func_tbl *ftp)
{
   if (ftp != NULL) {
      ftp_relall(ftp->ftp_next) ;
      ftp_release(ftp) ;
   }
}

/* �֐����ȑO��`����Ă��Ȃ����A�܂��������X�g�����ꂩ�`�F�b�N */
static 
void check_and_define(func_tbl *ftp_t,int i_defp)
{
   func_tbl *ftp = FUNCTOP ;

   /* �������O�̒�`�����݂��邩�H */
   while (ftp != NULL) {
      if (strcmp(ftp->cp_fname,ftp_t->cp_fname) == 0) { /* ���O����v */
         if ((ftp->i_predef == TRUE) && (i_defp == TRUE)) {
            app_error(FUNDUP_ERROR,1) ; /* �������O�̊֐������ɂ��� */
         }
         if (ftp->i_args == ftp_t->i_args) {
            if (ftp->i_args != 0) {
               if (cpa_equalp(ftp->cpp_fargs,ftp_t->cpp_fargs) == FALSE) {
                  app_error(FUNDEF_ERROR,1) ; /* �w�b�_�Ǝ��̂̒�`���Ⴄ */
               }
            }
            /* �^�����S�Ɉ�v���� */
            if (i_defp == TRUE) {
               ftp->i_predef = TRUE ; /* ��`�ς݂̈� */
            }
            return ;
         }
         app_error(FUNDEF_ERROR,1) ; /* �w�b�_�Ǝ��̂̒�`���Ⴄ */
      }
      ftp = ftp->ftp_next ; /* ���̃����N */
   }

   /* ������Ȃ�������A���X�g�ɐV���ɒǉ����� */
   ftp_t->i_predef = i_defp ;   /* �w�b�_�̎� FALSE (�I���W�i���ł̋t) */
   ftp_t->ftp_next = FUNCTOP ;
   FUNCTOP         = ftp_t ;
}

/* �֐��ďo���̕ϐ��ۑ��A���n������ */
static
void funpush_output(char *cp_from,char *cp_to)
{
   int i_pushf = FALSE ,
       i_leaf  = FALSE ;

   if (*cp_to == PUSH_CHAR) {
      cp_to++ ;
      i_pushf = TRUE ;
   }
   if (*cp_to == LEA_CHAR) {
      cp_to++ ;
      i_leaf  = TRUE ;
   }

   /* �ϐ��̕ۑ����s�� */
   if (i_pushf == TRUE) {
      push_output(cp_to) ;
   }

   /* ������n�� */
   if (i_leaf == TRUE) {
      lea_output(cp_to,cp_from) ;
   } else {
      mov_output(cp_to,cp_from) ;
   }
}

/* �֐��ďo���̕ۑ��ϐ����A���� */
static
void funpop_output(char *cp)
{
   int i_popf = FALSE ;

   if (*cp == PUSH_CHAR) {
      cp++ ;
      i_popf = TRUE ;
   }
   if (*cp == LEA_CHAR) {
      cp++ ;
   }

   /* �ϐ��̕ۑ����s�� */
   if (i_popf == TRUE) {
      pop_output(cp) ;
   }
}

/* �֐��ďo���̈����̓W�J���� */
static 
func_tbl *check_and_movout(func_tbl *ftp_t)
{
   int i ;
   func_tbl *ftp = FUNCTOP ;

   /* �������O�̒�`�����݂��邩�H */
   while (ftp != NULL) {
      if (strcmp(ftp->cp_fname,ftp_t->cp_fname) == 0) { /* ���O����v */
         if (ftp->i_args != ftp_t->i_args) {
            app_error(FUNARG_ERROR1,1) ; /* ��`�ƌĂяo���ň����̐����Ⴄ */
         }
         if (ftp->i_func_or_proc != ftp_t->i_func_or_proc) {
            app_error(FUNARG_ERROR2,1) ; /* ��`�ƌĂяo���Ń^�C�v���Ⴄ */
         }
         for (i = 0; i < ftp->i_args ; i++) { /* ������n�� */
            char *cp_to   = ftp->cpp_fargs[i] ,
                 *cp_from = ftp_t->cpp_fargs[i] ;
            funpush_output(cp_from,cp_to) ;       /* PUSH output */
         }
         return ftp ;
      }
      ftp = ftp->ftp_next ; /* ���̃����N */
   }

   /* ������Ȃ�������G���[ */
   app_error(FUNUNDEF_ERROR,1) ;
   return NULL ;
}

/* _function_h,_procedure_h �̏��� */
static
int _eval_function_h(char *cp_line,int i_forp)
{
   func_tbl *ftp ;

   ftp = get_funargs(cp_line) ;     /* �֐��̑����̎擾 */
   ftp->i_func_or_proc = i_forp ;
   check_and_define(ftp,FALSE) ;    /* �֐����e�[�u���ɓo�^ */

   return 0 ;
}

/* _function,_procedure �̏��� */
static
int _eval_function(char *cp_line,int i_forp)
{
   int i ;
   func_tbl *ftp ;

/* ���`���̃w�b�_��F�����邽�߂̏�� */
#define HEADCHARPOS (i_forp == FUNC ? -10 : -11)
#define HEADCHAR    '@'

   if (cp_line[HEADCHARPOS] == HEADCHAR) { /* ���`���̃w�b�_�H */
      _eval_function_h(cp_line,i_forp) ;
      return 0 ;
   }

   if (fun_tmp != NULL) { /* ���łɊ֐��̒��H */
      app_error(INFUNC_ERROR,1) ;
   }

   ftp = get_funargs(cp_line) ;       /* �֐��̑����̎擾 */
   ftp->i_func_or_proc = i_forp ;
   check_and_define(ftp,TRUE) ;       /* �֐����e�[�u���ɓo�^ */
   label_output(ftp->cp_fname) ;      /* �֐������x���o�� */

   /* PUSH ���ߏo�� */
   for (i = 0 ; i < ftp->i_save ; i++) {
      push_output(ftp->cpp_fsave[i]) ;
   }

   fun_tmp = ftp ;         /* �������̊֐��̏��ւ̃|�C���^�ۑ� */
   i_retfound = FALSE ;

   return 0 ;
}


/* _endf,_endp �̏��� */
#pragma argsused
static
void _eval_endf(char *cp_line,int i_forp,int i_intr)
{
   char ca_label[LBLBFLEN] ;
   int i ;

   if (fun_tmp == NULL) { /* ���A�֐��̒��ł͂Ȃ� */
      app_error(ENDF_ERROR,1) ;
   }

   if (fun_tmp->i_func_or_proc != i_forp) { /* procedure �ɑ΂��� endf */
      app_error(ENDF_ERROR,1) ;             /* �܂��͂��̋t */
   }

   /* �֐��o���̃��x�� */
   if ((i_retfound == TRUE) || (ia_option['R'] == TRUE)) {
      sprintf(ca_label,LABEL_FEND,fun_tmp->cp_fname) ; /* ���x�������񐶐� */
      label_output(ca_label) ;
   }

   /* POP ���ߏo�� */
   for (i = (fun_tmp->i_save)-1 ; i >= 0 ; i--) {
      pop_output(fun_tmp->cpp_fsave[i]) ;
   }

   /* RET ���ߏo�� */
   if (i_intr == TRUE) { /* ���荞�݃T�u���[�`����? */
      iret_output() ;
   } else {
      ret_output() ;
   }

   if (in_ifblkp() > 0) { /* �����Ă��Ȃ� if �����݂��� */
      app_error(NO_ENDIF,1) ;
   }

   if (in_loopblkp() > 0) { /* �����Ă��Ȃ����[�v�����݂��� */
      app_error(NO_ENDLOOP,1) ;
   }

   fun_tmp = NULL ; /* �֐��u���b�N����̒E�o */
}

/* _func,_proc �̏��� */
static
int _eval_func(char *cp_line,int i_forp)
{
   int i ;
   func_tbl *ftp,*ftp_d ;

   ftp = get_funargs(cp_line) ;   /* �֐��̑����̎擾 */
   ftp->i_func_or_proc = i_forp ; /* �`�F�b�N�̂��ߑ�����^���� */

   ftp_d = check_and_movout(ftp) ;         /* ���� mov �̏o�� */
   /* ������ ftp_d �͒�`���̃e�[�u���|�C���^ */
   call_output(ftp_d->cp_fname) ;          /* �֐��� call */
   for (i = (ftp_d->i_args)-1 ; i >= 0 ; i--) {
      funpop_output(ftp_d->cpp_fargs[i]) ; /* �ۑ��ϐ��̕��A */
   }
   (ftp_d->i_called)++ ;                   /* �Ă΂ꂽ��++ */

   ftp_release(ftp) ;             /* �e���|�����G���A�̊J�� */

   return 0 ;
}

/* _function �̏��� */
int eval_function(char *cp_line)
{
   if (ia_option['H']) { /* �w�b�_�����H */
      app_outstr(ia_option['O'] ? OLD_FUNC_H : NEW_FUNC_H ) ;
      app_outstr(cp_line) ;
   } else {
      _eval_function(cp_line,FUNC) ;
   }
   return 0 ;
}

/* _procedure �̏��� */
int eval_procedure(char *cp_line)
{
   if (ia_option['H']) { /* �w�b�_�����H */
      app_outstr(ia_option['O'] ? OLD_PROC_H : NEW_PROC_H ) ;
      app_outstr(cp_line) ;
   } else {
      _eval_function(cp_line,PROC) ;
   }
   return 0 ;
}

/* _function_h �̏��� */
int eval_function_h(char *cp_line)
{
   _eval_function_h(cp_line,FUNC) ;
   return 0 ;
}

/* _procedure_h �̏��� */
int eval_procedure_h(char *cp_line)
{
   _eval_function_h(cp_line,PROC) ;
   return 0 ;
}

/* _endf �̏��� */
int eval_endf(char *cp_line)
{
   _eval_endf(cp_line,FUNC,NORM) ;
   return 0 ;
}

/* _endp �̏��� */
int eval_endp(char *cp_line)
{
   _eval_endf(cp_line,PROC,NORM) ;
   return 0 ;
}

/* _endpi �̏��� */
int eval_endpi(char *cp_line)
{
   _eval_endf(cp_line,PROC,INTR) ;
   return 0 ;
}

/* _func �̏��� */
int eval_func(char *cp_line)
{
   _eval_func(cp_line,FUNC) ;
   return 0 ;
}

/* _proc �̏��� */
int eval_proc(char *cp_line)
{
   _eval_func(cp_line,PROC) ;
   return 0 ;
}

/* _return �̏��� */
int eval_return(char *cp_line)
{
   char ca_buf[LINEBUFSIZ],
        ca_resw[RESWBUFSIZ],
        ca_label[LBLBFLEN] ;

   int i ;

   if (fun_tmp == NULL) { /* ���A�֐��̒��ł͂Ȃ� */
      app_error(ENDF_ERROR,1) ;
   }

   /* �߂�l�̑�� */
   {
      char ca_eval[MAXARGLEN] ;
      cutspc(cp_line) ; /* ���݂���菜�� */
      gettoken(cp_line,WHITESPACE,LDELIM,ca_eval) ; /* �߂�l�̎��o�� */
      if (app_strlen(ca_eval) > 0) { /* �߂�l������Ƃ� */
         if (fun_tmp->i_func_or_proc == FUNC) { /* �߂�l�̑�� */
            retval_output(ca_eval) ; /* �߂�l�� mov */
         } else {
            app_error(RETARG_ERROR,1) ; /* procedure �ɖ߂�l�͕s�v */
         }
      }
   }

   sprintf(ca_label,LABEL_FEND,fun_tmp->cp_fname) ; /* ���x�������񐶐� */

   /* ���s�̐�ǂ݂ɂ�� JMP ���ߏo�͔��f */
   prefetch_line(ca_buf) ;
   get_appresw(ca_buf,ca_resw) ;
   if (strcmp(ca_resw,KW_ENDF) != 0) {
      jmp_output(ca_label) ;
   }

   i_retfound = TRUE ;

   return 0 ;
}

/* _public �̏��� */
#pragma argsused
int eval_public(char *cp_line)
{
   func_tbl *ftp = FUNCTOP ;

   /* ���݂܂ł̒�`�֐��� PUBLIC �錾 */
   while (ftp != NULL) {
      if (ftp->i_predef == TRUE) {
         public_output(ftp->cp_fname) ;
         (ftp->i_called)++ ;
      }
      ftp = ftp->ftp_next ; /* ���̃����N */
   }

   return 0 ;
}

/* _extrn �̏��� */
#pragma argsused
int eval_extrn(char *cp_line)
{
   func_tbl *ftp = FUNCTOP ;

   /* ���݂܂ł̖���`�֐��� EXTERN �錾 */
   while (ftp != NULL) {
      if (ftp->i_predef == FALSE) { /* �w�b�_ = FALSE (original �Ƌt) */
         extern_output(ftp->cp_fname) ;
         ftp->i_predef = TRUE ;
      }
      ftp = ftp->ftp_next ; /* ���̃����N */
   }

   return 0 ;
}

/* �������� */
int exit_function(void)
{
   ftp_relall(FUNCTOP) ;

   return 0 ;
}
