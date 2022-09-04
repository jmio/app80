/*
                         Generic APP Source File

                          Filename : CODE86.C
                          Version  : 95/05/12
                            Written by Mio

                         ���ۂ� ASM �R�[�h�o��
                     �i�����W�����v�� COND86 �ɑ��݁j
*/

#include <stdio.h>
#include <string.h>

#include "appmain.h"
#include "appdefs.h"
#include "appsub.h"
#include "tools86.h"
#include "function.h"                        /* ���������� _func */
#include "superstr.h"

#include "code86.h"

#define  FUNC_KW          "_func"            /* �֐��Ăяo�� */

#define  INCR             "++"               /* pre increment */
#define  DECR             "--"               /* pre decrement */
#define  ADD              "+="               /* pre decrement */
#define  SUB              "-="               /* pre decrement */

#define  BYTE_PTR_STR     "BYTE PTR "
#define  WORD_PTR_STR     "WORD PTR "

#define  RETVAL_REG_OLD   "BP"               /* �߂�l�����郌�W�X�^(-O) */
#define  RETVAL_REG       "AX"               /* �߂�l�����郌�W�X�^     */

/* �o�C�g���W�X�^�^���[�h���W�X�^�ϊ��e�[�u�� */
static char *cpa_repbregs[] = { "AX","AX","BX","BX","CX","CX","DX","DX" } ;

/* �o�C�g�E���[�h�L�[���[�h�`�F�b�N */
void bwcast_eval (char *cp_expr,char *cp_dist)
{
   /* (byte)(word) �L�[���[�h�̌��� */
   switch(cast_eval(cp_expr)) {
   case BYTETYP1:
   case BYTETYP2:
      strcpy(cp_dist,BYTE_PTR_STR) ;
   break ;
   case WORDTYP1:
   case WORDTYP2:
      strcpy(cp_dist,WORD_PTR_STR) ;
   break ;
   default:
      cp_dist[0] = '\0' ; /* clear */
   }
}

/* ���̕]�� */
void expr_eval(char *cp_expr,char *cp_dist)
{
   /* �֐��ďo���H */
   if (strstr(cp_expr,FUNC_KW) == cp_expr) {
      eval_func(&cp_expr[app_strlen(FUNC_KW)]) ; /* �֐��Ăяo�� */
      strcpy(cp_dist,(ia_option['O'] ? RETVAL_REG_OLD : RETVAL_REG)) ;
      return ;
   }

   /* (byte),(word) �� BYTE PTR,WORD PTR �ɂ��� */
   bwcast_eval(cp_expr,cp_dist) ;

   /* ++ , -- �L�[���[�h�̌��� */
   incdec_eval(cp_dist,cp_expr,INCR,inc_output) ;
   incdec_eval(cp_dist,cp_expr,DECR,dec_output) ;

   /* += , -= �L�[���[�h�̌��� (for �̑����p) */
   addsub_eval(cp_dist,cp_expr,ADD,add_output) ;
   addsub_eval(cp_dist,cp_expr,SUB,sub_output) ;

   strcat(cp_dist,cp_expr) ;
}

/* ���x���̐��� */
void label_output(char *cp)
{
   app_printf("%s:\n",cp) ;
}

/* MOV �̏o�́i�]���O�j */
void mov_output(char *cp1,char *cp2)
{
   char ca1[MAXEXPRLEN],ca2[MAXEXPRLEN] ;

   bwcast_eval(cp1,ca1) ; /* ca1 �� �|�C���^������𓾂� */
   strcat(ca1,cp1) ;

   expr_eval(cp2,ca2) ;
   if (strcmp(ca1,ca2) != 0) { /* ����̎��̓L�����Z�� */
      app_printf("\tMOV\t%s,%s\n",ca1,ca2) ;
   }
}

/* LEA �̏o�́i�]���O�j */
void lea_output(char *cp1,char *cp2)
{
   char ca1[MAXEXPRLEN],ca2[MAXEXPRLEN] ;

   bwcast_eval(cp1,ca1) ; /* ca1 �� �|�C���^������𓾂� */
   strcat(ca1,cp1) ;

   expr_eval(cp2,ca2) ;
   if (strcmp(ca1,ca2) != 0) { /* ����̎��̓L�����Z�� */
      app_printf("\tLEA\t%s,%s\n",ca1,ca2) ;
   }
}

/* �֐��̖߂�l��� MOV �o�́i�]���O�j */
void retval_output(char *cp1)
{
   char ca1[MAXEXPRLEN] ;

   expr_eval(cp1,ca1) ;
   mov_output((ia_option['O'] ? RETVAL_REG_OLD : RETVAL_REG),ca1) ;
}

/* CMP �̏o�́i�]���O�j */
void cmp_output(char *cp1,char *cp2)
{
   char ca1[MAXEXPRLEN],ca2[MAXEXPRLEN] ;

   expr_eval(cp1,ca1) ;
   expr_eval(cp2,ca2) ;
   app_printf("\tCMP\t%s,%s\n",ca1,ca2) ;
}

/* AND �̏o�́i�]���O�j */
void and_output(char *cp1,char *cp2)
{
   char ca1[MAXEXPRLEN],ca2[MAXEXPRLEN] ;

   expr_eval(cp1,ca1) ;
   expr_eval(cp2,ca2) ;
   app_printf("\tAND\t%s,%s\n",ca1,ca2) ;
}

/* TEST �̏o�́i�]���O�j */
void test_output(char *cp1,char *cp2)
{
   char ca1[MAXEXPRLEN],ca2[MAXEXPRLEN] ;

   expr_eval(cp1,ca1) ;
   expr_eval(cp2,ca2) ;
   app_printf("\tTEST\t%s,%s\n",ca1,ca2) ;
}

/* ������ JMP �̏o�� */
void jmp_output(char *cp)
{
   app_printf("\tJMP\t%s\n",cp) ;
}

/* ������ JMP �̏o�́iSHORT�j */
void short_jmp_output(char *cp)
{
   if (ia_option['N']) {
      app_printf("\tJMP\t%s\n",cp) ;
   } else {
      app_printf("\tJMP\tSHORT %s\n",cp) ;
   }
}

/* JCXZ �̏o�� */
void jcxz_output(char *cp)
{
   app_printf("\tJCXZ\t%s\n",cp) ;
}

/* LOOP �̏o�� */
void loop_output(char *cp)
{
   app_printf("\tLOOP\t%s\n",cp) ;
}

/* �T�u���[�`�� CALL �̏o�� */
void call_output(char *cp)
{
   app_printf("\tCALL\t%s\n",cp) ;
}

/* �T�u���[�`�� RET �̏o�� */
void ret_output(void)
{
   app_outstr("\tRET\n") ;
}

/* ���荞�� IRET �̏o�� */
void iret_output(void)
{
   app_outstr("\tIRET\n") ;
}

/* �ϐ��̕ۑ�(PUSH)�o�� */
void push_output(char *cp)
{
   int i ;

   /* �o�C�g���W�X�^�����[�h���W�X�^�ɕϊ� */
   if ((i = is_bytereg(cp)) != 0) {
      cp = cpa_repbregs[i-1] ;
   }
   if (strcmp(cp,"ALL") == 0) {
      app_outstr("\tPUSHA\n") ;
   } else if (strcmp(cp,"FLAGS") == 0) {
      app_outstr("\tPUSHF\n") ;
   } else {
      app_printf("\tPUSH\t%s\n",cp) ;
   }
}

/* �ϐ��̕ۑ�(POP)�o�� */
void pop_output(char *cp)
{
   int i ;

   /* �o�C�g���W�X�^�����[�h���W�X�^�ɕϊ� */
   if ((i = is_bytereg(cp)) != 0) {
      cp = cpa_repbregs[i] ;
   }
   if (strcmp(cp,"ALL") == 0) {
      app_outstr("\tPOPA\n") ;
   } else if (strcmp(cp,"FLAGS") == 0) {
      app_outstr("\tPOPF\n") ;
   } else {
      app_printf("\tPOP\t%s\n",cp) ;
   }
}

/* �ϐ��̃C���N�������g�o�́i�]�����݁j */
void inc_output(char *cp)
{
   app_printf("\tINC\t%s\n",cp) ;
}

/* �ϐ��̃f�N�������g�o�́i�]�����݁j */
void dec_output(char *cp)
{
   app_printf("\tDEC\t%s\n",cp) ;
}

/* �ϐ��̉��Z�o�� */
void add_output(char *cp1,char *cp2)
{
   app_printf("\tADD\t%s,%s\n",cp1,cp2) ;
}

/* �ϐ��̌��Z�o�� */
void sub_output(char *cp1,char *cp2)
{
   app_printf("\tSUB\t%s,%s\n",cp1,cp2) ;
}

/* PUBLIC �錾 */
void public_output(char *cp)
{
   app_printf("\tPUBLIC\t%s\n",cp) ;
}

/* EXTERNAL �錾 */
void extern_output(char *cp)
{
   app_printf("\tEXTRN\t%s:NEAR\n",cp) ;
}
