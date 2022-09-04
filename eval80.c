/*
                         Generic APP Source File

                          Filename : EVAL80.C
                          Version  : 95/03/01
                            Written by Mio

                  APP80 �����A�]�����[�`���x�[�X�t�@�C��
*/

#include <stdio.h>
#include <string.h>

#include "appmain.h"
#include "appdefs.h"
#include "appsub.h"

#include "ifdefs.h"              /* ���� Pre Processing ���� */
#include "ifswitch.h"            /* �������򏈗� */
#include "miscloop.h"            /* �e�탋�[�v���� */
#include "function.h"            /* �֐����� */
#ifdef APP86
# include "misc86.h"             /* x86 �ˑ����߂̏��� */
#else
# include "misc80.h"             /* x80 �ˑ����߂̏��� */
#endif

#include "eval80.h"

/* �W�����v�e�[�u���̍\�� */
typedef int (*intfunc_ptr)(char *) ;
typedef struct {
   char        *cp ; /* �\��ꕶ���� */
   intfunc_ptr ifp ; /* �������[�`�� */
   int         idf ; /* ifdef �ɂ�� disable �t���O */
   int         ihf ; /* �w�b�_�쐬���R�[���t���O */
} jmptable ;

/*
   �W�����v�e�[�u���{�� :
   �ǉ�����ۂ́A�\���A�������[�`���̑g���ȉ��̃e�[�u���̖���{NULL,NULL}
   ����O�̈ʒu�ɁA�����`���Œǉ����邱�ƁB
*/
const jmptable JMPTBL[] = {
  /* �\��ꖼ , �֐��� , ifdef�t���O , �w�b�_�쐬���[�h�t���O */
  /*  ifdef �t���O           ON = ifdef �ɂ�� disable �t���O�ɍ��E����Ȃ� */
  /*  �w�b�_�쐬���[�h�t���O ON = �w�b�_�쐬���ɂ��R�[������� */
  { "ifdef"       , eval_ifdef       , ON  , OFF } ,
  { "ifndef"      , eval_ifndef      , ON  , OFF } ,
  { "elsedef"     , eval_elsedef     , ON  , OFF } ,
  { "endifdef"    , eval_endifdef    , ON  , OFF } ,
  { "define"      , eval_define      , OFF , OFF } ,
  { "include"     , eval_include     , OFF , ON  } ,
  { "header"      , eval_include     , OFF , OFF } ,
  { "warning"     , eval_warning     , OFF , OFF } ,
#ifdef IFSWITCH_H
  { "if"          , eval_if          , OFF , OFF } ,
  { "else"        , eval_else        , OFF , OFF } ,
  { "endif"       , eval_endif       , OFF , OFF } ,
  { "switch"      , eval_switch      , OFF , OFF } ,
  { "case"        , eval_case        , OFF , OFF } ,
  { "default"     , eval_default     , OFF , OFF } ,
  { "endsw"       , eval_endsw       , OFF , OFF } ,
#endif
#ifdef MISCLOOP_H
  { "while"       , eval_while       , OFF , OFF } ,
  { "wend"        , eval_wend        , OFF , OFF } ,
  { "repeat"      , eval_repeat      , OFF , OFF } ,
  { "until"       , eval_until       , OFF , OFF } ,
  { "continue"    , eval_continue    , OFF , OFF } ,
  { "exit"        , eval_exit        , OFF , OFF } ,
  { "let"         , eval_let         , OFF , OFF } ,
  { "for"         , eval_for         , OFF , OFF } ,
  { "next"        , eval_next        , OFF , OFF } ,
# ifdef MISC86_H
  { "loopcx"      , eval_loopcx      , OFF , OFF } ,
  { "endl"        , eval_endl        , OFF , OFF } ,
# endif
# ifdef MISC80_H
  { "loop"        , eval_loop        , OFF , OFF } ,
  { "endl"        , eval_endl        , OFF , OFF } ,
# endif
#endif
#ifdef FUNCTION_H
  { "function"    , eval_function    , OFF , ON  } ,
  { "function_h"  , eval_function_h  , OFF , OFF } ,
  { "procedure"   , eval_procedure   , OFF , ON  } ,
  { "procedure_h" , eval_procedure_h , OFF , OFF } ,
  { "endf"        , eval_endf        , OFF , OFF } ,
  { "endp"        , eval_endp        , OFF , OFF } ,
  { "endpi"       , eval_endpi       , OFF , OFF } ,
  { "func"        , eval_func        , OFF , OFF } ,
  { "proc"        , eval_proc        , OFF , OFF } ,
  { "return"      , eval_return      , OFF , OFF } ,
  { "public"      , eval_public      , OFF , OFF } ,
  { "extrn"       , eval_extrn       , OFF , OFF } ,
  { "extern"      , eval_extrn       , OFF , OFF } ,
#endif
#ifdef MISC86_H
  { "short"       , eval_short       , OFF , OFF } ,
  { "near"        , eval_near        , OFF , OFF } ,
  { "signed"      , eval_signed      , OFF , OFF } ,
  { "unsigned"    , eval_unsigned    , OFF , OFF } ,
#endif
#ifdef MISC80_H
  { "short"       , eval_short       , OFF , OFF } ,
  { "near"        , eval_near        , OFF , OFF } ,
  { "cpu"         , eval_cpu         , OFF , OFF } ,
#endif
  { NULL          , NULL             , OFF , OFF } ,
} ;

/* �g�b�v�t�@�C���̕]�� */
int eval_file (char *cp_fname,int i_paramcount,char **cpp_paramstr)
{
   /* �J�n�i�R�}���h���C�� define ���j���� */
   eval_begin(i_paramcount,cpp_paramstr) ;

   /* �g�b�v�t�@�C���̕]�� */
   include_file(cp_fname) ;

   /* �������� */
   eval_normalexit() ;
   return 0 ;
}

/* �J�n���� */
int eval_begin(int i_paramcount,char **cpp_paramstr)
{
   int i ;

   for (i = 0;i < i_paramcount;i++) {
      char c = toupper(cpp_paramstr[i][0]) ;
      switch(c) {
      case INCLUDE_OPTION:
         add_incpath(&cpp_paramstr[i][1]) ;
      break ;
      case DEFINE_OPTION:
         sympush(&cpp_paramstr[i][1]) ;
      break ;
      }
   }
   return 0 ;
}

/* �G���[�������̏I���O���� (APPERR.C ����R�[�������) */
int eval_errexit(int i_errcode)
{
    /* ������`�V���{���e�[�u���̊J�� */
    symclear() ;

#ifdef FUNCTION_H
    /* �֐���`�e�[�u���̊J�� */
    exit_function() ;
#endif

    return i_errcode ;
}

/* �ʏ�̏I���O���� */
int eval_normalexit()
{
    /* ������`�V���{���e�[�u���̊J�� */
    symclear() ;

    /* �C���N���[�h�p�X�̈�̊J�� */
    clear_incpath() ;

#ifdef FUNCTION_H
    /* �֐���`�e�[�u���̊J�� */
    exit_function() ;
#endif

    return 0 ;
}

/* APP �s�ȊO�̏o�� (ifdef ���䍞��) */
static
void normal_output(char *cp)
{
   if (ia_option['H'] == FALSE) {
#if defined(IFSWITCH_H) && defined(MISCLOOP_H)
      if ((in_ifblkp() > 0) || (in_loopblkp() > 0)) {
         if (*cp == '\t') {
            do { cp++ ; } while (*cp == '\t') ;
            cp -- ;
         }
      }
#endif
      if (output_enable() == ON) {
         app_outstr(cp) ;
      }
   }
}

/* APP �\���̃R�����g�A�E�g */
static
void comment_output(char *cp)
{
   if (ia_option['H'] == FALSE) {
      if (*cp == '\t') {
         do { cp++ ; } while (*cp == '\t') ;
         cp -- ;
      }
      app_printf("\t;%s",cp) ;
   }
}

/* �s�̕]��  eval_line */
int eval_line(char *cp_line)
{
   char ca_resw[RESWBUFSIZ] ; /* �؂�o�����\��� */
   char *cp_orig = cp_line ;  /* �s�o�b�t�@�ʒu�̃Z�[�u�p */

   int i ;

   /* �\���炵�����̂����邩�H */
   if ((cp_line = get_appresw(cp_line,ca_resw)) == NULL) {
      normal_output(cp_orig) ; /* �Ȃ���΂��̂܂܏o�� */
      return ;
   }

   /* �W�����v�e�[�u���̃X�L�����A�y�щ��� */
   for (i = 0 ; JMPTBL[i].cp != NULL ; i++ ) {
      /* �w�b�_�쐬���[�h���A�֌W�����R�}���h�͖��� */
      if ((ia_option['H'] == TRUE) && (JMPTBL[i].ihf == OFF)) {
         continue ;
      }
      /* ifdef �� disable ���Aifdef �n�̃R�}���h�ȊO�̎����� */
      if ((output_enable() == OFF) && (JMPTBL[i].idf == OFF)) {
         continue ;
      }
      /* �e�[�u���̕�����ƈ�v���邩�H */
      if (strcmp((JMPTBL[i].cp),ca_resw) == 0) { /* �����񂪈�v������ */
         comment_output(cp_orig) ;               /* �R�����g�o�� */
         (void)JMPTBL[i].ifp(cp_line) ;          /* �W�����v */
         break ;
      }
   }

   /* �A���_�[�o�[�̂��Ƃ͗\���ȊO������? */
   if (JMPTBL[i].cp == NULL) {
      normal_output(cp_orig) ; /* ���̂܂܏o�� */
      return ;
   }
   return ;
}
