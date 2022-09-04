/*
                         Generic APP Source File

                          Filename : APPERR.H
                          Version  : 95/03/01
                            Written by Mio

                          APP ���ʃG���[����
*/

#ifndef APPERR_H
#define APPERR_H

/* �G���[���b�Z�[�W�̏o�͐� */
#define _erout stdout

/* app_error �ɓn���G���[�R�[�h */
#define ERR_NULL_STR       1
#define FILE_ERROR         2
#define TOO_MANY_ARG       3
#define FILE_NOT_FOUND     4

#define IF_STK_OVER       10
#define IF_STK_EMPTY      11

#define SW_STK_OVER       12
#define SW_STK_EMPTY      13

#define LOOP_STK_OVER     14
#define LOOP_STK_EMPTY    15
#define NOT_IN_LOOP       16
#define FOR_ARGUMENT      17

#define NO_ENDIF          18
#define NO_ENDLOOP        19

#define QUOTE_ERROR       20
#define REP_ERROR         21

#define LANDOR_ERROR      30
#define LEXTRA_ERROR      31
#define LARG1MISS_ERROR   32
#define LARGCOND_ERROR    33
#define EXP_SYNTAX_ERROR  34

#define LVALUE_ERROR      40
#define ARGTYPE_ERROR     41
#define TYPE_NOT_MATCH    42

#define SELF_INCLUDE      50

#define FNAME_ERROR       70
#define FUNDUP_ERROR      71
#define FUNDEF_ERROR      72
#define ENDF_ERROR        73          /* �֐��O�� _endf,_return */
#define INFUNC_ERROR      74          /* �֐����� _function */
#define FUNARG_ERROR1     75          /* ��`�ƌĂяo���ň����̐����Ⴄ */
#define FUNARG_ERROR2     76          /* ��`�ƌĂяo���Ŋ֐��^�C�v���Ⴄ */
#define FUNUNDEF_ERROR    77          /* ����`�֐��̌Ăяo�� */
#define RETARG_ERROR      78          /* �߂�l�͕s�v */

#define IFD_STK_OVER      90
#define IFD_STK_EMPTY     91

#define FUNFREE_NULL      96
#define STRDUP_FAIL       97
#define FREE_NULL         98
#define MEMORY_NOT_ENOUGH 99

#define CPROTO_ERR1       100
#define MACRO_ERR         101
#define NOT_SUPPORTED     102

extern long l_err_lineno ;
extern char *cp_err_fname ;

#include "apperr.prt"

#endif /* of APPERR_H */
