
/*
                         Generic APP Source File

                          Filename : CODE80.C
                          Version  : 95/07/08	K.T.
                          Version  : 95/06/17	K.T.
                          Version  : 95/03/17
                            Written by Miu

                          �f�[�^�]���o�͏���
                         ���ۂ� ASM �R�[�h�o��
                     �i�����W�����v�� COND86 �ɑ��݁j
*/

#include <stdio.h>
#include <string.h>

#include "appdefs.h"
#include "appsub.h"
#include "apperr.h"

#include "code80.h"
#include "cond80.h"
#include "tools80.h"

#define  FUNC_KW          "_func"            /* �֐��Ăяo�� */

#define  INCR             "++"               /* pre increment */
#define  DECR             "--"               /* pre decrement */
#define  ADD              "+="               /* pre decrement */
#define  SUB              "-="               /* pre decrement */

#define  BYTE_PTR_STR     "BYTE PTR "
#define  WORD_PTR_STR     "WORD PTR "

#define  RETVAL_REG       "A"                /* �߂�l�����郌�W�X�^ */

int i_cpu = ASMZ80 ; // �����l�ύX => 2021/3/1

/* �o�C�g���W�X�^�^���[�h���W�X�^�ϊ��e�[�u�� */
static char *cpa_repbregs[] =  { "PSW","BC","BC","DE","DE","HL","HL" } ;
static char *cpa_repwregs[] =  { "H","B","D","PC","SP","IX","IY" } ;
static char *cpa_repwbregs[] = { "H","L","B","C","D","E" } ;

/*-----------------------------------------------------------------*/
/* ���̕]�� */
void expr_eval(char *cp_expr,char *cp_dist)
{
/*      printf("DEBUG %s %s\n",cp_expr,cp_dist) ; */

   /* �֐��ďo���H */
   if (strstr(cp_expr,FUNC_KW) == cp_expr) {
      eval_func(&cp_expr[app_strlen(FUNC_KW)]) ; /* �֐��Ăяo�� */
      strcpy(cp_dist,RETVAL_REG) ;
      return ;
   }

   /* (byte),(word) �� BYTE PTR,WORD PTR �ɂ��� */
   cp_dist[0] = '\0' ;	/* clear	*/
/*   bwcast_eval(cp_expr,cp_dist) ;
*/
   /* ++ , -- �L�[���[�h�̌��� */
   incdec_eval(cp_dist,cp_expr,INCR,inc_output) ;
   incdec_eval(cp_dist,cp_expr,DECR,dec_output) ;

   /* += , -= �L�[���[�h�̌��� (for �̑����p) */
   addsub_eval(cp_dist,cp_expr,ADD,add_output) ;
   addsub_eval(cp_dist,cp_expr,SUB,sub_output) ;

   strcpy(cp_dist,cp_expr) ;
}
/*-----------------------------------------------------------------*/
/* ���x���̐��� */
void label_output(char *cp)
{
   app_printf("%s:\n",cp) ;
}
/*-----------------------------------------------------------------*/
/* ������ JMP �̏o�� */
void jmp_output(char *cp)
{
   if (i_cpu == ASMZ80) {
      app_printf("\tJP\t%s\n",cp) ;
   } else {
      app_printf("\tJMP\t%s\n",cp) ;
   }
}
/*-----------------------------------------------------------------*/
/* ������ JMP �̏o�́iSHORT�j */
void short_jmp_output(char *cp)
{
   if (i_cpu == ASMZ80) {
      if (ia_option['N']) {		/* �m�d�`�q �i�l�o �w��	*/
         app_printf("\tJP\t%s\n",cp) ;
      } else {
         app_printf("\tJR\t%s\n",cp) ;
      }
   } else {
      app_printf("\tJMP\t%s\n",cp) ;
   }
}
/*-----------------------------------------------------------------*/
/* �����i�l�o�̐��� 95/06/21	*/
void jflg_output(char *cp_flg, char *cp_label, int i_num)
{
   if (i_cpu == ASMZ80) {
      if (ia_option['N']) {		/* �m�d�`�q �i�l�o �w��	*/
         app_printf("\tJP\t%s,%s_%d\n",cp_flg,cp_label,i_num) ;
      } else {
         app_printf("\tJR\t%s,%s_%d\n",cp_flg,cp_label,i_num) ;
      }
   } else if ((i_cpu == CPU180) || (i_cpu == CPUZ80)) {
      if (ia_option['N']) {		/* �m�d�`�q �i�l�o �w��	*/
         app_printf("\tJ%s\t%s_%d\n",cp_flg,cp_label,i_num) ;
      } else {
         OUTPUT1(".Z80") ;
         app_printf("\tJR\t%s,%s_%d\n",cp_flg,cp_label,i_num) ;
         OUTPUT1(".8085") ;
      }
   } else {
      app_printf("\tJ%s\t%s_%d\n",cp_flg,cp_label,i_num) ;
   }
}
/*-----------------------------------------------------------------*/
/* �����i�l�o�̐��� 95/06/21	*/
void jflg_output_n(char *cp_flg)
{
   if (i_cpu == ASMZ80) {
      if (ia_option['N']) {		/* �m�d�`�q �i�l�o �w��	*/
         app_printf("\tJR\t%s,$ + 3\n",cp_flg) ;
      } else {
         app_printf("\tJR\t%s,$ + 2\n",cp_flg) ;
      }
   } else if ((i_cpu == CPU180) || (i_cpu == CPUZ80)) {
      OUTPUT1(".Z80") ;
      if (ia_option['N']) {		/* �m�d�`�q �i�l�o �w��	*/
         app_printf("\tJR\t%s,$ + 3\n",cp_flg) ;
      } else {
         app_printf("\tJR\t%s,$ + 2\n",cp_flg) ;
      }
      OUTPUT1(".8085") ;
   } else {
/*         app_printf("\tJ%s\t$ + 3\n",cp_flg) ;*/
         app_printf("\tJ%s\t$ + 6\n",cp_flg) ;	/* 96/07/04	*/
   }
}
/*-----------------------------------------------------------------*/
/* �ϐ��̕ۑ�(PUSH)�o�� */
void push_output(char *cp)
{
   int i ;
   
   /* �o�C�g���W�X�^�����[�h���W�X�^�ɕϊ� */
   i = is_reg(cp) ;
   if ((i > Acc) && (i < WORDreg)) {
      cp = cpa_repbregs[i - 1] ;
      i = is_reg(cp) ;
   }
   if ((i >= HLreg) && (i < REGptr)) {
      if (i_cpu == ASMZ80) {
         app_printf("\tPUSH\t%s\n",cp) ;
      } else if ((i_cpu == CPU180) || (i_cpu == CPUZ80)) {
         OUTPUT1(".Z80") ;
         app_printf("\tPUSH\t%s\n",cp) ;
         OUTPUT1(".8085") ;
      } else {
         cp = cpa_repwregs[i - HLreg] ;
         app_printf("\tPUSH\t%s\n",cp) ;
      }
   }
   if (strcmp(cp,"ALL") == 0) {
      if (i_cpu == ASMZ80) {
         app_outstr("\tPUSH\tHL\n") ;
         app_outstr("\tPUSH\tBC\n") ;
         app_outstr("\tPUSH\tDE\n") ;
         app_outstr("\tPUSH\tAF\n") ;
      } else {
         app_outstr("\tPUSH\tH\n") ;
         app_outstr("\tPUSH\tB\n") ;
         app_outstr("\tPUSH\tD\n") ;
         app_outstr("\tPUSH\tPSW\n") ;
      }
   } else if (i == 1) {
      if (i_cpu == ASMZ80) {
         app_outstr("\tPUSH\tAF\n") ;
      } else {
         app_outstr("\tPUSH\tPSW\n") ;
      }
   }
}
/*-----------------------------------------------------------------*/
/* �ϐ��̕ۑ�(POP)�o�� */
void pop_output(char *cp)
{
   int i ;
   
   /* �o�C�g���W�X�^�����[�h���W�X�^�ɕϊ� */
   i = is_reg(cp) ;
   if ((i > Acc) && (i < WORDreg)) {
      cp = cpa_repbregs[i - 1] ;
      i = is_reg(cp) ;
   }
   if ((i >= HLreg) && (i < REGptr)) {
      if (i_cpu == ASMZ80) {
         app_printf("\tPOP\t%s\n",cp) ;
      } else if ((i_cpu == CPU180) || (i_cpu == CPUZ80)) {
         OUTPUT1(".Z80") ;
         app_printf("\tPOP\t%s\n",cp) ;
         OUTPUT1(".8085") ;
      } else {
         cp = cpa_repwregs[i - HLreg] ;
         app_printf("\tPOP\t%s\n",cp) ;
      }
   }
   if (strcmp(cp,"ALL") == 0) {
      if (i_cpu == ASMZ80) {
         app_outstr("\tPOP\tAF\n") ;
         app_outstr("\tPOP\tDE\n") ;
         app_outstr("\tPOP\tBC\n") ;
         app_outstr("\tPOP\tHL\n") ;
      } else {
         app_outstr("\tPOP\tPSW\n") ;
         app_outstr("\tPOP\tD\n") ;
         app_outstr("\tPOP\tB\n") ;
         app_outstr("\tPOP\tH\n") ;
      }
   } else if (i == 1) {
      if (i_cpu == ASMZ80) {
         app_outstr("\tPOP\tAF\n") ;
      } else {
         app_outstr("\tPOP\tPSW\n") ;
      }
   }
}
/*-----------------------------------------------------------------*/
/* 95/07/08	*/
void xchg_output(void)
{
   if (i_cpu == ASMZ80) {
      OUTPUT3("EX","DE","HL") ;
   } else if ((i_cpu == CPUZ80) || (i_cpu == CPU180)) {
      OUTPUT1(".Z80") ;
      OUTPUT3("EX","DE","HL") ;
      OUTPUT1(".8085") ;
   } else {
         app_printf("\tXCHG\n") ;
   }
}
/*-----------------------------------------------------------------*/
void _move_any_acc(int i_regs1,char *cp_1)
{
   if (i_cpu == ASMZ80) {			/* 95/06/20	*/
      if (((i_regs1 > Acc) && (i_regs1 < WORDreg)) || (i_regs1 == NONE)) {
         OUTPUT3("LD",cp_1,"A") ;		/* _mov	r,A	*/
      } else if (i_regs1 != Acc) {		/* _mov	A,A �Ȃ� �������Ȃ�	*/
         switch (i_regs1) {
         case BYTEptr :				/* _MOV	A,(byte)???	*/
            app_printf("\tLD\t(%s),A\n",cp_1) ;	/* 95/06/20	*/
            break ;
         case BYTEHLptr :			/* _MOV	A,(byte)[HL]	*/
            app_printf("\tLD\t(HL),A\n") ;	/* 95/06/20	*/
            break ;
         case BYTEDEptr :			/* _MOV	A,(byte)[DE]	*/
            app_printf("\tLD\t(DE),A\n") ;	/* 95/06/20	*/
            break ;
         case BYTEBCptr :			/* _MOV	A,(byte)[BC]	*/
            app_printf("\tLD\t(BC),A\n") ;	/* 95/06/20	*/
            break ;
         case BYTEIXptr :			/* _MOV	A,(byte)[IX]	*/
            app_printf("\tLD\t(%s),A\n",cp_1) ;	/* 95/07/07	*/
            break ;
         default :
            app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��	*/
         }
      }
   /* i_cpu != ASMZ80	*/
   } else if ((i_regs1 > Acc) && (i_regs1 < WORDreg)) {
      OUTPUT3("MOV",cp_1,"A") ;		/* _mov	r,A	*/
   } else if (i_regs1 != Acc) {		/* _mov	A,A �Ȃ� �������Ȃ�	*/
      switch (i_regs1) {
      case NONE :			/* _MOV	??????,A	*/
         if ((i_cpu == CPU80) || (i_cpu == CPU85)) {
            app_warning("�W�O�W�T�ł͎g�p�ł��Ȃ�����\n") ;
         } else {
            OUTPUT1(".Z80") ;
            OUTPUT3("LD",cp_1,"A") ;
            OUTPUT1(".8085") ;
         }
      break ;
      case BYTEptr :			/* _MOV	(byte)???,A	*/
         OUTPUT2("STA",cp_1) ;
      break ;
      case BYTEHLptr :			/* _MOV	(byte)[HL],A	*/
         OUTPUT3("MOV","M","A") ;
      break ;
      case BYTEBCptr :
         OUTPUT2("STAX","B") ;
      break ;
      case BYTEDEptr :
         OUTPUT2("STAX","D") ;
      break ;
      default :
         app_error(LVALUE_ERROR,1) ;	/* ���ӂ��s�K��	*/
      }
   }
}

/*-----------------------------------------------------------------*/
void _move_acc_any(int i_regs2,char *cp_2)
{
   if (i_cpu == ASMZ80) {			/* 95/06/20	*/
      if (((i_regs2 > Acc) && (i_regs2 < WORDreg)) || (i_regs2 == NONE)) {
         OUTPUT3("LD","A",cp_2) ;		/* _mov	A,r	*/
      } else if (i_regs2 != Acc) {	/* _mov	A,A �Ȃ牽�����Ȃ�	*/
         switch (i_regs2) {
         case BYTEptr :				/* _MOV	A,(byte)???	*/
            app_printf("\tLD\tA,(%s)\n",cp_2) ;	/* 95/06/20	*/
            break ;
         case BYTEHLptr :			/* _MOV	A,(byte)[HL]	*/
            app_printf("\tLD\tA,(HL)\n") ;	/* 95/06/20	*/
            break ;
         case BYTEDEptr :			/* _MOV	A,(byte)[HL]	*/
            app_printf("\tLD\tA,(DE)\n") ;	/* 95/06/20	*/
            break ;
         case BYTEBCptr :			/* _MOV	A,(byte)[HL]	*/
            app_printf("\tLD\tA,(BC)\n") ;	/* 95/06/20	*/
            break ;
         default :
            app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��	*/
         }
      }
   /* i_cpu != ASMZ80	*/
   } else if ((i_regs2 > Acc) && (i_regs2 < WORDreg)) {
         OUTPUT3("MOV","A",cp_2) ;		/* _mov	A,r	*/
   } else if (i_regs2 != Acc) {			/* _mov	A,A �Ȃ牽�����Ȃ�	*/
      switch (i_regs2) {
      case NONE :				/* _MOV	A,?????	*/
         if ((i_cpu == CPU80) || (i_cpu == CPU85)) {
            OUTPUT3("MVI","A",cp_2) ;
         } else {
            OUTPUT1(".Z80") ;
            OUTPUT3("LD","A",cp_2) ;
            OUTPUT1(".8085") ;
         }
         break ;
      case BYTEptr :				/* _MOV	A,(byte)???	*/
         OUTPUT2("LDA",cp_2) ;
         break ;
      case BYTEHLptr :				/* _MOV	A,(byte)[HL]	*/
         OUTPUT3("MOV","A","M") ;
         break ;
      case BYTEBCptr :
         OUTPUT2("LDAX","B") ;			/* _mov	A,(byte)[BC]	*/
         break ;
      case BYTEDEptr :
         OUTPUT2("LDAX","D") ;			/* _mov	A,(byte)[DE]	*/
         break ;
      default :
         app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��	*/
      }
   }
}
/*-----------------------------------------------------------------*/
/* 95/07/07 ���ӂ��o�C�g���W�X�^�A�E�ӂ��o�C�g���W�X�^	*/
void _move_bytereg_bytereg(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if (i_cpu == ASMZ80) {
      app_printf("\tLD\t%s,%s\n",cp_1,cp_2) ;
   } else if ((i_cpu == CPU180) || (i_cpu == CPUZ80)) {
      OUTPUT1(".Z80") ;
      app_printf("\tLD\t%s,%s\n",cp_1,cp_2) ;
      OUTPUT1(".8085") ;
   } else {
      if ((i_regs1 <= Lreg) && (i_regs2 <= Lreg)) {
         app_printf("\tMOV\t%s,%s\n",cp_1,cp_2) ;
      } else {
         app_warning("�W�O�W�T�ł͎g�p�ł��Ȃ����W�X�^\n") ;
         app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��	*/
      }
   }
}
/*-----------------------------------------------------------------*/
/* 95/07/07 ���ӂ��o�C�g���W�X�^�A�E�ӂ��o�C�g���W�X�^�|�C���^	*/
void _move_bytereg_byteregptr(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if ((i_regs1 != Acc) && ((i_regs2 == BYTEBCptr) || (i_regs2 == BYTEDEptr))) {
      /* 95/07/07 �E�ӂ��o�C�g���W�X�a�b�c�d�^�|�C���^	*/
      app_warning("�`�������W�X�^���g�p����\n") ;
      _move_bytereg_byteregptr(Acc,i_regs2,"A",cp_2) ;
      _move_bytereg_bytereg(i_regs1,Acc,cp_1,"A") ;
   } else if ((i_regs1 == Ireg) || (i_regs1 == Rreg)) {
      app_warning("�`�������W�X�^���g�p����\n") ;
      _move_bytereg_byteregptr(Acc,i_regs2,"A",cp_2) ;
      _move_bytereg_bytereg(i_regs1,Acc,cp_1,"A") ;
   } else {
      if (i_cpu == ASMZ80) {
         app_printf("\tLD\t%s,(%s)\n",cp_1,cp_2) ;
      } else if ((i_cpu == CPU180) || (i_cpu == CPUZ80)) {
         OUTPUT1(".Z80") ;
         app_printf("\tLD\t%s,(%s)\n",cp_1,cp_2) ;
         OUTPUT1(".8085") ;
      } else {
         if (i_regs2 == BYTEHLptr) {
            app_printf("\tMOV\t%s,M\n",cp_1) ;
         } else if (i_regs2 == BYTEBCptr) {
            app_printf("\tLDAX\tB\n") ;
         } else if (i_regs2 == BYTEDEptr) {
            app_printf("\tLDAX\tD\n") ;
         } else {
            app_warning("�W�O�W�T�ł͎g�p�ł��Ȃ����W�X�^\n") ;
            app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��	*/
         }
      }
   }
}
/*-----------------------------------------------------------------*/
/* 95/07/07 ���ӂ��o�C�g���W�X�^�A�E�ӂ��o�C�g�|�C���^	*/
void _move_bytereg_byteptr(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if (i_regs1 != Acc) {
      app_warning("�`�������W�X�^���g�p����\n") ;
      _move_bytereg_byteptr(Acc,i_regs2,"A",cp_2) ;
      _move_bytereg_bytereg(i_regs1,Acc,cp_1,"A") ;
   } else {
      if (i_cpu == ASMZ80) {
         app_printf("\tLD\t%s,(%s)\n",cp_1,cp_2) ;
      } else if ((i_cpu == CPU180) || (i_cpu == CPUZ80)) {
         OUTPUT1(".Z80") ;
         app_printf("\tLD\t%s,(%s)\n",cp_1,cp_2) ;
         OUTPUT1(".8085") ;
      } else {
         app_printf("\tLDA\t%s\n",cp_2) ;
      }
   }
}
/*-----------------------------------------------------------------*/
/* 95/07/07 ���ӂ��o�C�g���W�X�^�A�E�ӂ����l	*/
void _move_bytereg_none(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if ((i_regs1 == Ireg) || (i_regs1 == Rreg)) {
      app_warning("�`�������W�X�^���g�p����\n") ;
      _move_bytereg_none(Acc,i_regs2,"A",cp_2) ;
      _move_bytereg_bytereg(i_regs1,Acc,cp_1,"A") ;
   } else {
      if (i_cpu == ASMZ80) {
         app_printf("\tLD\t%s,%s\n",cp_1,cp_2) ;
      } else if ((i_cpu == CPU180) || (i_cpu == CPUZ80)) {
         OUTPUT1(".Z80") ;
         app_printf("\tLD\t%s,%s\n",cp_1,cp_2) ;
         OUTPUT1(".8085") ;
      } else {
         app_printf("\tMVI\t%s,%s\n",cp_1,cp_2) ;
      }
   }
}
/*-----------------------------------------------------------------*/
/* 95/07/07 ���ӂ��o�C�g���W�X�^	*/
void _move_bytereg_any(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if ((i_regs2 >= Acc) && (i_regs2 < WORDreg)) {
      /* 95/07/07 �E�ӂ��o�C�g���W�X�^	*/
      _move_bytereg_bytereg(i_regs1,i_regs2,cp_1,cp_2) ;
   } else if ((i_regs2 > BYTEREGptr) && (i_regs2 < WORDREGptr)) {
      /* 95/07/07 �E�ӂ��o�C�g���W�X�^�|�C���^	*/
      _move_bytereg_byteregptr(i_regs1,i_regs2,cp_1,cp_2) ;
   } else if (i_regs2 == BYTEptr) {
      /* 95/07/07 �E�ӂ��o�C�g�|�C���^	*/
      _move_bytereg_byteptr(i_regs1,i_regs2,cp_1,cp_2) ;
   } else if (i_regs2 == NONE) {
      /* 95/07/07 �E�ӂ����l	*/
      _move_bytereg_none(i_regs1,i_regs2,cp_1,cp_2) ;
   } else {
      app_warning("�E�ӂ��s�K��\n") ;
      app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��	*/
   }
}
/*-----------------------------------------------------------------*/
/* 95/07/07 ���ӂ��o�C�g�|�C���^	�E�ӂ��o�C�g���W�X�^	*/
void _move_byteptr_bytereg(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if (i_regs2 != Acc) {
      app_warning("�`�������W�X�^���g�p����\n") ;
      _move_bytereg_bytereg(Acc,i_regs2,"A",cp_2) ;
      _move_byteptr_bytereg(i_regs1,Acc,cp_1,"A") ;
   } else {
      if (i_cpu == ASMZ80) {
         app_printf("\tLD\t(%s),%s\n",cp_1,cp_2) ;
      } else if ((i_cpu == CPU180) || (i_cpu == CPUZ80)) {
         OUTPUT1(".Z80") ;
         app_printf("\tLD\t(%s),%s\n",cp_1,cp_2) ;
         OUTPUT1(".8085") ;
      } else {
         app_printf("\tSTA\t%s\n",cp_1) ;
      }
   }
}
/*-----------------------------------------------------------------*/
/* 95/07/07 ���ӂ��o�C�g�|�C���^	�E�ӂ��o�C�g���W�X�^�|�C���^	*/
void _move_byteptr_byteregptr(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   app_warning("�`�������W�X�^���g�p����\n") ;
   _move_bytereg_byteregptr(Acc,i_regs2,"A",cp_2) ;
   _move_byteptr_bytereg(i_regs1,Acc,cp_1,"A") ;
}
/*-----------------------------------------------------------------*/
/* 95/07/07 ���ӂ��o�C�g�|�C���^	�E�ӂ��o�C�g�|�C���^	*/
void _move_byteptr_byteptr(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   app_warning("�`�������W�X�^���g�p����\n") ;
   _move_bytereg_byteptr(Acc,i_regs2,"A",cp_2) ;
   _move_byteptr_bytereg(i_regs1,Acc,cp_1,"A") ;
}
/*-----------------------------------------------------------------*/
/* 95/07/07 ���ӂ��o�C�g�|�C���^	�E�ӂ����l	*/
void _move_byteptr_none(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   app_warning("�`�������W�X�^���g�p����\n") ;
   _move_bytereg_none(Acc,i_regs2,"A",cp_2) ;
   _move_byteptr_bytereg(i_regs1,Acc,cp_1,"A") ;
}
/*-----------------------------------------------------------------*/
/* 95/07/07 ���ӂ��o�C�g�|�C���^	*/
void _move_byteptr_any(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if ((i_regs2 >= Acc) && (i_regs2 < WORDreg)) {
      /* 95/07/07 �E�ӂ��o�C�g���W�X�^	*/
      _move_byteptr_bytereg(i_regs1,i_regs2,cp_1,cp_2) ;
   } else if ((i_regs2 > REGptr) && (i_regs2 < BYTEptr)) {
      /* 95/07/07 �E�ӂ��o�C�g���W�X�^�|�C���^	*/
      _move_byteptr_byteregptr(i_regs1,i_regs2,cp_1,cp_2) ;
   } else if (i_regs2 == BYTEptr) {
      /* 95/07/07 �E�ӂ��o�C�g�|�C���^	*/
      _move_byteptr_byteptr(i_regs1,i_regs2,cp_1,cp_2) ;
   } else if (i_regs2 == NONE) {
      /* 95/07/07 �E�ӂ����l	*/
      _move_byteptr_none(i_regs1,i_regs2,cp_1,cp_2) ;
   } else {
      app_warning("�E�ӂ��s�K��\n") ;
      app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��	*/
   }
}
/*-----------------------------------------------------------------*/
/* ���ӂ����W�X�^�o�C�g�|�C���^	�E�ӂ��o�C�g���W�X�^	*/
void _move_byteregptr_bytereg(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if ((i_regs2 != Acc) && ((i_regs1 == BYTEBCptr) || (i_regs1 == BYTEDEptr))) {
      /* 95/07/07 �E�ӂ��o�C�g���W�X�a�b�c�d�^�|�C���^	*/
      app_warning("�`�������W�X�^���g�p����\n") ;
      _move_bytereg_bytereg(Acc,i_regs2,"A",cp_2) ;
      _move_byteregptr_bytereg(i_regs1,Acc,cp_1,"A") ;
   } else if ((i_regs2 == Ireg) || (i_regs2 == Rreg)) {
      app_warning("�`�������W�X�^���g�p����\n") ;
      _move_bytereg_bytereg(Acc,i_regs2,"A",cp_2) ;
      _move_byteregptr_bytereg(i_regs1,Acc,cp_1,"A") ;
   } else {
      if (i_cpu == ASMZ80) {
         app_printf("\tLD\t(%s),%s\n",cp_1,cp_2) ;
      } else if ((i_cpu == CPU180) || (i_cpu == CPUZ80)) {
         OUTPUT1(".Z80") ;
         app_printf("\tLD\t(%s),%s\n",cp_1,cp_2) ;
         OUTPUT1(".8085") ;
      } else {
         if (i_regs1 == BYTEHLptr) {
            app_printf("\tMOV\tM,%s\n",cp_2) ;
         } else if (i_regs1 == BYTEBCptr) {
            app_printf("\tSTAX\tB\n") ;
         } else if (i_regs1 == BYTEDEptr) {
            app_printf("\tSTAX\tD\n") ;
         } else {
            app_warning("�W�O�W�T�ł͎g�p�ł��Ȃ����W�X�^\n") ;
            app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��	*/
         }
      }
   }
}
/*-----------------------------------------------------------------*/
/* ���ӂ����W�X�^�o�C�g�|�C���^	�E�ӂ��o�C�g���W�X�^�|�C���^	*/
void _move_byteregptr_byteregptr(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   app_warning("�`�������W�X�^���g�p����\n") ;
   _move_bytereg_byteregptr(Acc,i_regs2,"A",cp_2) ;
   _move_byteregptr_bytereg(i_regs1,Acc,cp_1,"A") ;
}
/*-----------------------------------------------------------------*/
/* ���ӂ����W�X�^�o�C�g�|�C���^	 �E�ӂ��o�C�g�|�C���^	*/
void _move_byteregptr_byteptr(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   app_warning("�`�������W�X�^���g�p����\n") ;
   _move_bytereg_byteptr(Acc,i_regs2,"A",cp_2) ;
   _move_byteregptr_bytereg(i_regs1,Acc,cp_1,"A") ;
}
/*-----------------------------------------------------------------*/
/* ���ӂ����W�X�^�o�C�g�|�C���^	95/07/07 �E�ӂ����l	*/
void _move_byteregptr_none(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if ((i_regs1 != BYTEHLptr) && (i_regs1 != BYTEIXptr) && (i_regs1 != BYTEIYptr)) {
      app_warning("�`�������W�X�^���g�p����\n") ;
      _move_bytereg_none(Acc,i_regs2,"A",cp_2) ;
      _move_byteregptr_bytereg(i_regs1,Acc,cp_1,"A") ;
   } else {
      if (i_cpu == ASMZ80) {
         app_printf("\tLD\t(%s),%s\n",cp_1,cp_2) ;
      } else if ((i_cpu == CPU180) || (i_cpu == CPUZ80)) {
         OUTPUT1(".Z80") ;
         app_printf("\tLD\t(%s),%s\n",cp_1,cp_2) ;
         OUTPUT1(".8085") ;
      } else {
         if (i_regs1 == BYTEHLptr) {
            app_printf("\tMVI\tM,%s\n",cp_2) ;
         } else {
            app_warning("�W�O�W�T�ł͎g�p�ł��Ȃ����W�X�^\n") ;
            app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��	*/
         }
      }
   }
}
/*-----------------------------------------------------------------*/
/* ���ӂ����W�X�^�o�C�g�|�C���^	*/
void _move_byteregptr_any(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if ((i_regs2 >= Acc) && (i_regs2 < WORDreg)) {
      /* 95/07/07 �E�ӂ��o�C�g���W�X�^	*/
      _move_byteregptr_bytereg(i_regs1,i_regs2,cp_1,cp_2) ;
   } else if ((i_regs2 > REGptr) && (i_regs2 < BYTEptr)) {
      /* 95/07/07 �E�ӂ��o�C�g���W�X�^�|�C���^	*/
      _move_byteregptr_byteregptr(i_regs1,i_regs2,cp_1,cp_2) ;
   } else if (i_regs2 == BYTEptr) {
      /* 95/07/07 �E�ӂ��o�C�g�|�C���^	*/
      _move_byteregptr_byteptr(i_regs1,i_regs2,cp_1,cp_2) ;
   } else if (i_regs2 == NONE) {
      /* 95/07/07 �E�ӂ����l	*/
      _move_byteregptr_none(i_regs1,i_regs2,cp_1,cp_2) ;
   } else {
      app_warning("�E�ӂ��s�K��\n") ;
      app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��	*/
   }
}
/*-----------------------------------------------------------------*/
/* 95/07/08  ���ӂ��g�k���W�X�^ �E�ӂ����[�h���W�X�^	*/
void _move_HLreg_wordreg(int i_regs2,char *cp_2)
{
   if (i_regs2 == BCreg) {
      _move_bytereg_bytereg(Lreg,Creg,"L","C") ;
      _move_bytereg_bytereg(Hreg,Breg,"H","B") ;
   } else if (i_regs2 == DEreg) {
      _move_bytereg_bytereg(Lreg,Ereg,"L","E") ;
      _move_bytereg_bytereg(Hreg,Dreg,"H","D") ;
   } else {
      app_warning("�E�ӂ��s�K��\n") ;
      app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��	*/
   }
}
/*-----------------------------------------------------------------*/
/* 95/07/08  ���ӂ��c�d���W�X�^ �E�ӂ����[�h���W�X�^	*/
void _move_DEreg_wordreg(int i_regs2,char *cp_2)
{
   if (i_regs2 == BCreg) {
      _move_bytereg_bytereg(Ereg,Creg,"E","C") ;
      _move_bytereg_bytereg(Dreg,Breg,"D","B") ;
   } else if (i_regs2 == HLreg) {
      _move_bytereg_bytereg(Ereg,Lreg,"E","L") ;
      _move_bytereg_bytereg(Dreg,Hreg,"D","H") ;
   } else {
      app_warning("�E�ӂ��s�K��\n") ;
      app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��	*/
   }
}
/*-----------------------------------------------------------------*/
/* 95/07/08  ���ӂ��a�b���W�X�^ �E�ӂ����[�h���W�X�^	*/
void _move_BCreg_wordreg(int i_regs2,char *cp_2)
{
   if (i_regs2 == DEreg) {
      _move_bytereg_bytereg(Creg,Ereg,"C","E") ;
      _move_bytereg_bytereg(Breg,Dreg,"B","D") ;
   } else if (i_regs2 == HLreg) {
      _move_bytereg_bytereg(Creg,Lreg,"C","L") ;
      _move_bytereg_bytereg(Breg,Hreg,"B","H") ;
   } else {
      app_warning("�E�ӂ��s�K��\n") ;
      app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��	*/
   }
}
/*-----------------------------------------------------------------*/
/* 95/07/07  ���ӂ����[�h���W�X�^ �E�ӂ����[�h���W�X�^	*/
void _move_wordreg_wordreg(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if (i_regs1 == SPreg) {
      if ((i_regs2 == HLreg) || (i_regs2 == IXreg) || (i_regs2 == IYreg)) {
         if (i_cpu == ASMZ80) {
            app_printf("\tLD\t%s,%s\n",cp_1,cp_2) ;
         } else if ((i_cpu == CPU180) || (i_cpu == CPUZ80)) {
            OUTPUT1(".Z80") ;
            app_printf("\tLD\t%s,%s\n",cp_1,cp_2) ;
            OUTPUT1(".8085") ;
         } else {
            if (i_regs2 == HLreg) {
               app_printf("\tSPHL\n") ;
            } else {
               app_warning("�W�O�W�T�ł͎g�p�ł��Ȃ����W�X�^\n") ;
               app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��	*/
            }
         }
      } else {
         app_warning("�g�k���W�X�^���g�p����\n") ;
         _move_wordreg_wordreg(HLreg,i_regs2,"HL",cp_2) ;
         _move_wordreg_wordreg(i_regs1,HLreg,cp_1,"HL") ;
      }
   } else if ((i_regs1 == IXreg) || (i_regs1 == IYreg)) {
      push_output(cp_2) ;
      pop_output(cp_1) ;
   } else if ((i_regs2 == IXreg) || (i_regs2 == IYreg)) {
      push_output(cp_2) ;
      pop_output(cp_1) ;
   } else {
      if (i_regs1 == HLreg) {
         _move_HLreg_wordreg(i_regs2,cp_2) ;
      } else if (i_regs1 == BCreg) {
         _move_BCreg_wordreg(i_regs2,cp_2) ;
      } else if (i_regs1 == DEreg) {
         _move_DEreg_wordreg(i_regs2,cp_2) ;
      } else {
         app_warning("�g�p�ł��Ȃ����W�X�^\n") ;
         app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��	*/
      }
   }
}
/*-----------------------------------------------------------------*/
/* 95/07/08  ���ӂ����[�h���W�X�^  �E�ӂ����[�h�|�C���^	*/
void _move_wordreg_wordptr(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   char tmp[80] ;
   
   if (i_cpu == ASMZ80) {
      app_printf("\tLD\t%s,(%s)\n",cp_1,cp_2) ;
   } else if ((i_cpu == CPU180) || (i_cpu == CPUZ80)) {
      OUTPUT1(".Z80") ;
      app_printf("\tLD\t%s,(%s)\n",cp_1,cp_2) ;
      OUTPUT1(".8085") ;
   } else {
      if (i_regs1 == HLreg) {
         app_printf("\tLHLD\t%s\n",cp_2) ;
      } else if (i_regs1 == BCreg) {
         app_warning("�`�������W�X�^���g�p����\n") ;

         _move_bytereg_byteptr(Creg,BYTEptr,"C",cp_2) ;
/*
         app_printf("\tLDA\t%s\n",cp_2) ;
         app_printf("\tMOV\tC,A\n") ;
*/
         strcpy(tmp,cp_2) ;
         strcat(tmp," + 1") ;
         _move_bytereg_byteptr(Breg,BYTEptr,"B",tmp) ;
  /*       app_printf("\tLDA\t%s\n",tmp) ;
         app_printf("\tMOV\tB,A\n") ; */
      } else if (i_regs1 == DEreg) {
         app_printf("\tXCHG\n") ;
         app_printf("\tLHLD\t%s\n",cp_2) ;
         app_printf("\tXCHG\n") ;
      } else {
         app_warning("�W�O�W�T�ł͎g�p�ł��Ȃ����W�X�^\n") ;
         app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��	*/
      }
   }
}
/*-----------------------------------------------------------------*/
/* 95/07/08  ���ӂ����[�h���W�X�^	�E�ӂ����W�X�^���[�h�|�C���^	*/
void _move_wordreg_wordregptr(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if (i_regs1 == HLreg) {
      _move_bytereg_byteregptr(Lreg,i_regs2 - 10,"L",cp_2) ;
      inc_wordreg((i_regs2 - 50),cp_2) ;
      _move_bytereg_byteregptr(Hreg,i_regs2 - 10,"H",cp_2) ;
      dec_wordreg((i_regs2 - 50),cp_2) ;
   } else if (i_regs1 == BCreg) {
      _move_bytereg_byteregptr(Creg,i_regs2 - 10,"C",cp_2) ;
      inc_wordreg((i_regs2 - 50),cp_2) ;
      _move_bytereg_byteregptr(Breg,i_regs2 - 10,"B",cp_2) ;
      dec_wordreg((i_regs2 - 50),cp_2) ;
   } else if (i_regs1 == DEreg) {
      _move_bytereg_byteregptr(Ereg,i_regs2 - 10,"E",cp_2) ;
      inc_wordreg((i_regs2 - 50),cp_2) ;
      _move_bytereg_byteregptr(Dreg,i_regs2 - 10,"D",cp_2) ;
      dec_wordreg((i_regs2 - 50),cp_2) ;
   } else {
      app_warning("���ӂɎg�p�ł��Ȃ����W�X�^\n") ;
      app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��	*/
   }
}
/*-----------------------------------------------------------------*/
/* 95/07/08  ���ӂ����[�h���W�X�^	�E�ӂ����l	*/
void _move_wordreg_none(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if (i_cpu == ASMZ80) {
      app_printf("\tLD\t%s,%s\n",cp_1,cp_2) ;
   } else if ((i_cpu == CPU180) || (i_cpu == CPUZ80)) {
      OUTPUT1(".Z80") ;
      app_printf("\tLD\t%s,%s\n",cp_1,cp_2) ;
      OUTPUT1(".8085") ;
   } else {
      if (i_regs1 == HLreg) {
         app_printf("\tLXI\tH,%s\n",cp_2) ;
      } else if (i_regs1 == BCreg) {
         app_printf("\tLXI\tB,%s\n",cp_2) ;
      } else if (i_regs1 == DEreg) {
         app_printf("\tLXI\tD,%s\n",cp_2) ;
      } else if (i_regs1 == SPreg) {
         app_printf("\tLXI\tSP,%s\n",cp_2) ;
      } else if (i_regs1 == PCreg) {
         app_printf("\tJMP\t%s\n",cp_2) ;
      } else {
         app_warning("�W�O�W�T�ł͎g�p�ł��Ȃ����W�X�^\n") ;
         app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��	*/
      }
   }
}
/*-----------------------------------------------------------------*/
/* 95/07/07  ���ӂ����[�h���W�X�^	*/
void _move_wordreg_any(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if ((i_regs2 > WORDreg) && (i_regs2 < REGptr)) {
      /* 95/07/07 �E�ӂ����[�h���W�X�^	*/
      _move_wordreg_wordreg(i_regs1,i_regs2,cp_1,cp_2) ;
   } else if (i_regs2 == WORDptr) {
      /* �E�ӂ����[�h�|�C���^	*/
      _move_wordreg_wordptr(i_regs1,i_regs2,cp_1,cp_2) ;
   } else if ((i_regs2 >= WORDHLptr) && (i_regs2 <= WORDIYptr)) {
      /* �E�ӂ����W�X�^���[�h�|�C���^	*/
      _move_wordreg_wordregptr(i_regs1,i_regs2,cp_1,cp_2) ;
   } else if (i_regs2 == NONE) {
      /* 95/07/07 �E�ӂ����l	*/
      _move_wordreg_none(i_regs1,i_regs2,cp_1,cp_2) ;
   } else {
      app_warning("�E�ӂ��s�K��\n") ;
      app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��	*/
   }
}
/*-----------------------------------------------------------------*/
/* 95/07/08 ���ӂ����[�h�|�C���^	�E�ӂ����[�h���W�X�^	*/
void _move_wordptr_wordreg(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   char tmp[80] ;
   
   if (i_cpu == ASMZ80) {
      app_printf("\tLD\t(%s),%s\n",cp_1,cp_2) ;
   } else if ((i_cpu == CPU180) || (i_cpu == CPUZ80)) {
      OUTPUT1(".Z80") ;
      app_printf("\tLD\t(%s),%s\n",cp_1,cp_2) ;
      OUTPUT1(".8085") ;
   } else {
      if (i_regs2 == HLreg) {
         app_printf("\tSHLD\t%s\n",cp_1) ;
      } else if (i_regs2 == BCreg) {
         app_warning("�`�������W�X�^���g�p����\n") ;
         app_printf("\tMOV\tA,C\n") ;
         app_printf("\tSTA\t%s\n",cp_1) ;
         strcpy(tmp,cp_1) ;
         strcat(tmp," + 1") ;
         app_printf("\tMOV\tA,B\n") ;
         app_printf("\tSTA\t%s\n",tmp) ;
      } else if (i_regs2 == DEreg) {
         app_printf("\tXCHG\n") ;
         app_printf("\tSHLD\t%s\n",cp_1) ;
         app_printf("\tXCHG\n") ;
      } else {
         app_warning("�W�O�W�T�ł͎g�p�ł��Ȃ����W�X�^\n") ;
         app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��	*/
      }
   }
}
/*-----------------------------------------------------------------*/
/* 95/07/08 ���ӂ����[�h�|�C���^	�E�ӂ����W�X�^���[�h�|�C���^	*/
void _move_wordptr_wordregptr(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   char tmp[80] ;

   if (i_regs2 == WORDHLptr) {
      _move_byteptr_byteregptr(BYTEptr,(i_regs2 - 10),cp_1,cp_2) ;
      inc_wordreg(HLreg,cp_2) ;
      strcpy(tmp,cp_1) ;
      strcat(tmp," + 1") ;
      _move_byteptr_byteregptr(BYTEptr,(i_regs2 - 10),tmp,cp_2) ;
      dec_wordreg(HLreg,cp_2) ;
   }
   if (i_regs2 == WORDBCptr) {
      _move_byteptr_byteregptr(BYTEptr,(i_regs2 - 10),cp_1,cp_2) ;
      inc_wordreg(BCreg,cp_2) ;
      strcpy(tmp,cp_1) ;
      strcat(tmp," + 1") ;
      _move_byteptr_byteregptr(BYTEptr,(i_regs2 - 10),tmp,cp_2) ;
      dec_wordreg(BCreg,cp_2) ;
   }
   if (i_regs2 == WORDDEptr) {
      _move_byteptr_byteregptr(BYTEptr,(i_regs2 - 10),cp_1,cp_2) ;
      inc_wordreg(DEreg,cp_2) ;
      strcpy(tmp,cp_1) ;
      strcat(tmp," + 1") ;
      _move_byteptr_byteregptr(BYTEptr,(i_regs2 - 10),tmp,cp_2) ;
      dec_wordreg(DEreg,cp_2) ;
   }
}
/*-----------------------------------------------------------------*/
/* 95/07/08 ���ӂ����[�h�|�C���^	*/
void _move_wordptr_any(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if ((i_regs2 > WORDreg) && (i_regs2 < REGptr)) {
      /* �E�ӂ����[�h���W�X�^	*/
      _move_wordptr_wordreg(i_regs1,i_regs2,cp_1,cp_2) ;
   } else if (i_regs2 == WORDptr) {
      /* �E�ӂ����[�h�|�C���^	*/
      push_output("HL") ;	/* �ϐ��̕ۑ�(PUSH)�o��	*/
      _move_wordreg_wordptr(HLreg,i_regs2,"HL",cp_2) ;
      _move_wordptr_wordreg(i_regs1,HLreg,cp_1,"HL") ;
      pop_output("HL") ;	/* �ϐ��̕ۑ�(PUSH)�o��	*/
   } else if ((i_regs2 >= WORDHLptr) && (i_regs2 <= WORDIYptr)) {
      /* �E�ӂ����W�X�^���[�h�|�C���^	*/
      _move_wordptr_wordregptr(i_regs1,i_regs2,cp_1,cp_2) ;
   } else if (i_regs2 == NONE) {
      /* �E�ӂ����l	*/
      push_output("HL") ;	/* �ϐ��̕ۑ�(PUSH)�o��	*/
      _move_wordreg_none(HLreg,i_regs2,"HL",cp_2) ;
      _move_wordptr_wordreg(i_regs1,HLreg,cp_1,"HL") ;
      pop_output("HL") ;	/* �ϐ��̕ۑ�(PUSH)�o��	*/
   } else {
      app_warning("�E�ӂ��s�K��\n") ;
      app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��	*/
   }
}
/*-----------------------------------------------------------------*/
/* 95/07/09 ���ӂ����W�X�^���[�h�|�C���^�A�E�ӂ��Q�̃o�C�g���W�X�^	*/
void _move_wordregptr_2bytereg(int i_regs1,int i_regs2H,int i_regs2L,char *cp_1,char *cp_2H,char *cp_2L)
{
   _move_byteregptr_bytereg((i_regs1 - 10),i_regs2L,cp_1,cp_2L) ;
   if ((i_regs1 == WORDHLptr) && ((i_regs2H == Hreg) || (i_regs2H == Lreg))) {
      app_warning("�`�������W�X�^���g�p����\n") ;
      _move_bytereg_bytereg(Acc,i_regs2H,"A",cp_2H) ;
      inc_wordreg((i_regs1 - 50),cp_1) ;
      _move_byteregptr_bytereg((i_regs1 - 10),Acc,cp_1,"A") ;
   } else if (i_regs1 != WORDHLptr) {
      app_warning("�`�������W�X�^���g�p����\n") ;
      _move_bytereg_bytereg(Acc,i_regs2H,"A",cp_2H) ;
      inc_wordreg((i_regs1 - 50),cp_1) ;
      _move_byteregptr_bytereg((i_regs1 - 10),Acc,cp_1,"A") ;
   } else {
      inc_wordreg((i_regs1 - 50),cp_1) ;
      _move_byteregptr_bytereg((i_regs1 - 10),i_regs2H,cp_1,cp_2H) ;
   }
   dec_wordreg((i_regs1 - 50),cp_1) ;
}
/*-----------------------------------------------------------------*/
/* 95/07/09 ���ӂ����W�X�^���[�h�|�C���^	�E�ӂ����[�h���W�X�^	*/
void _move_wordregptr_wordreg(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if (i_regs2 == BCreg) {
      _move_wordregptr_2bytereg(i_regs1,Breg,Creg,cp_1,"B","C") ;
   } else if (i_regs2 == DEreg) {
      _move_wordregptr_2bytereg(i_regs1,Dreg,Ereg,cp_1,"D","E") ;
   } else if (i_regs2 == HLreg) {
      _move_wordregptr_2bytereg(i_regs1,Hreg,Lreg,cp_1,"H","L") ;
   } else {
      app_warning("�E�ӂ��s�K��\n") ;
      app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��	*/
   }
}
/*-----------------------------------------------------------------*/
/* 95/07/08 ���ӂ����W�X�^���[�h�|�C���^	�E�ӂ����l	*/
void _move_wordregptr_none(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if (i_regs1 != WORDDEptr) {
      push_output("DE") ;
      _move_wordreg_none(DEreg,i_regs2,"DE",cp_2) ;
      _move_wordregptr_wordreg(i_regs1,DEreg,cp_1,"DE") ;
      pop_output("DE") ;
   } else if (i_regs1 != WORDBCptr) {
      push_output("BC") ;
      _move_wordreg_none(BCreg,i_regs2,"BC",cp_2) ;
      _move_wordregptr_wordreg(i_regs1,BCreg,cp_1,"BC") ;
      pop_output("BC") ;
   }
}
/*-----------------------------------------------------------------*/
/* 95/07/08 ���ӂ����W�X�^���[�h�|�C���^	*/
void _move_wordregptr_any(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if ((i_regs2 > WORDreg) && (i_regs2 < REGptr)) {
      /* �E�ӂ����[�h���W�X�^	*/
      _move_wordregptr_wordreg(i_regs1,i_regs2,cp_1,cp_2) ;
   } else if (i_regs2 == WORDptr) {
      /* �E�ӂ����[�h�|�C���^	*/
      push_output("HL") ;	/* �ϐ��̕ۑ�(PUSH)�o��	*/
      _move_wordreg_wordptr(HLreg,i_regs2,"HL",cp_2) ;
      _move_wordregptr_wordreg(i_regs1,HLreg,cp_1,"HL") ;
      pop_output("HL") ;	/* �ϐ��̕ۑ�(PUSH)�o��	*/
   } else if ((i_regs2 >= WORDHLptr) && (i_regs2 <= WORDIYptr)) {
      /* �E�ӂ����W�X�^���[�h�|�C���^	*/
      push_output("HL") ;	/* �ϐ��̕ۑ�(PUSH)�o��	*/
      _move_wordreg_wordregptr(HLreg,i_regs2,"HL",cp_2) ;
      _move_wordregptr_wordreg(i_regs1,HLreg,cp_1,"HL") ;
      pop_output("HL") ;	/* �ϐ��̕ۑ�(PUSH)�o��	*/
   } else if (i_regs2 == NONE) {
      /* �E�ӂ����l	*/
      _move_wordregptr_none(i_regs1,i_regs2,cp_1,cp_2) ;
   } else {
      app_warning("�E�ӂ��s�K��\n") ;
      app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��	*/
   }
}
/*-----------------------------------------------------------------*/
/* 95/07/07	*/
void _move_any_any(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if ((i_regs1 >= Acc) && (i_regs1 < WORDreg)) {
      /* ���ӂ��o�C�g���W�X�^	*/
      _move_bytereg_any(i_regs1,i_regs2,cp_1,cp_2) ;
   } else if (i_regs1 == BYTEptr) {
      /* ���ӂ��o�C�g�|�C���^	*/
      _move_byteptr_any(i_regs1,i_regs2,cp_1,cp_2) ;
   } else if ((i_regs1 >= BYTEHLptr) && (i_regs1 <= BYTEIYptr)) {
      /* ���ӂ����W�X�^�o�C�g�|�C���^	*/
      _move_byteregptr_any(i_regs1,i_regs2,cp_1,cp_2) ;
   } else if ((i_regs1 > WORDreg) && (i_regs1 < REGptr)) {
      /* ���ӂ����[�h���W�X�^	*/
      _move_wordreg_any(i_regs1,i_regs2,cp_1,cp_2) ;
   } else if (i_regs1 == WORDptr) {
      /* ���ӂ����[�h�|�C���^	*/
      _move_wordptr_any(i_regs1,i_regs2,cp_1,cp_2) ;
   } else if ((i_regs1 >= WORDHLptr) && (i_regs1 <= WORDIYptr)) {
      /* ���ӂ����W�X�^���[�h�|�C���^	*/
      _move_wordregptr_any(i_regs1,i_regs2,cp_1,cp_2) ;
   } else {
      app_warning("���ӂ��s�K��\n") ;
      app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��	*/
   }
}

/*-----------------------------------------------------------------*/
/* cp_2 ���� cp_1 �ցA�f�[�^�]������j�[���j�b�N */
void mov_output(char *cp_1,char *cp_2)
{
   int i_regs1 ;
   int i_regs2 ;
   int i_type1 ;
   int i_type2 ;
   int i_retval ;
   
   if (strcmp(cp_1,cp_2) == 0) {	/* ����̎��̓L�����Z��	*/
      return ;
   }
   
   __chk_bw(cp_1,&i_type1,&i_regs1) ;	/* ���ӂ́H	*/
   __chk_bw(cp_2,&i_type2,&i_regs2) ;	/* �E�ӂ́H	*/
   
   if (i_type1 == NONEtype) {
      if ((i_regs1 >= HLptr) && (i_regs1 <= IYptr)) {
         i_type1 = i_type2 ;
         i_regs1 = i_regs1 + 20 + i_type1 * 10 ;
      }
   }
   if (i_type2 == NONEtype) {
      if ((i_regs2 >= HLptr) && (i_regs2 <= IYptr)) {
         i_type2 = i_type1 ;
         i_regs2 = i_regs2 + 20 + i_type2 * 10 ;
      }
   }
   /* �a�x�s�d�C�v�n�q�c �w�肪 �K�؂�	*/
   if ((i_type1 == NONEtype) && (i_type2 == NONEtype)) {
      app_error(ARGTYPE_ERROR,1) ;		/* �s�x�o�d�̎w�肪�K�v	*/
   }
   if ((i_type1 == BYTEtype) && (i_type2 == WORDtype)) {
      app_error(TYPE_NOT_MATCH,1) ;		/* �s�x�o�d�̎w�肪�s��v	*/
   }
   if ((i_type1 == WORDtype) && (i_type2 == BYTEtype)) {
      app_error(TYPE_NOT_MATCH,1) ;		/* �s�x�o�d�̎w�肪�s��v	*/
   }
   _move_any_any(i_regs1,i_regs2,cp_1,cp_2) ;	/* 95/07/07	*/
/*   _move(i_regs1,i_regs2,cp_1,cp_2) ;*/
}

/*-----------------------------------------------------------------*/
/* �֐��̖߂�l��� MOV �o�́i�]���O�j */
void retval_output(char *cp1)
{
   mov_output("A",cp1) ;
}
/*-----------------------------------------------------------------*/
/* LEA �̏o�́i�]���O�j */
void lea_output(char *cp1,char *cp2)
{
      app_error(TYPE_NOT_MATCH,1) ;		/* �s�x�o�d�̎w�肪�s��v */
	/*      app_error(NOT_SUPPORTED,1) ; */
}
/*-----------------------------------------------------------------*/
/* �T�u���[�`�� CALL �̏o�� */
void call_output(char *cp)
{
   app_printf("\tCALL\t%s\n",cp) ;
}
/*-----------------------------------------------------------------*/
/* �T�u���[�`�� RET �̏o�� */
void ret_output(void)
{
   app_outstr("\tRET\n") ;
}
/*-----------------------------------------------------------------*/
/* ���荞�� IRET �̏o�� */
void iret_output(void)
{
   if ((i_cpu == CPU180) || (i_cpu == CPUZ80)) {
      app_outstr("\tRETI\n") ;
   } else {
      app_outstr("\tRET\n") ;
   }
}
/*-----------------------------------------------------------------*/
/* PUBLIC �錾 */
void public_output(char *cp)
{
   app_printf("\tPUBLIC\t%s\n",cp) ;
}

/*-----------------------------------------------------------------*/
/* EXTERNAL �錾 */
void extern_output(char *cp)
{
   app_printf("\tEXTRN\t%s\n",cp) ;
}
/*-----------------------------------------------------------------*/
/* �ϐ��̉��Z�o�� */
void add_output(char *cp1,char *cp2)
{
   app_printf("\tADD\t%s,%s\n",cp1,cp2) ;
}
/*-----------------------------------------------------------------*/
/* �ϐ��̌��Z�o�� */
void sub_output(char *cp1,char *cp2)
{
   int i_regs1 ;
   int i_type1 ;
   int i_retval ;
   char cp_1[MAXEXPRLEN],cp_2[MAXEXPRLEN] ;
   
   expr_eval(cp1,cp_1) ;
   __chk_bw(cp_1,&i_type1,&i_regs1) ;	/* ���ӂ́H	*/
   if (i_regs1 != Acc) {
      _move_acc_any(i_regs1,cp_1) ;	/* ���ӂ� Acc �Ƀ��[�h���Ă���	*/
   }
   if (i_cpu == ASMZ80) {
      app_printf("\tSUB\t%s\n",cp2) ;
   } else {
      app_printf("\tSUB\t%s\n",cp2) ;
   }
}
/*-----------------------------------------------------------------*/
void inc_bytereg(char *cp)
{
   if (i_cpu == ASMZ80) {
      app_printf("\tINC\t%s\n",cp) ;
   } else if ((i_cpu == CPUZ80) || (i_cpu == CPU180)) {
      OUTPUT1(".Z80") ;
      app_printf("\tINC\t%s\n",cp) ;
      OUTPUT1(".8085") ;
   } else {
      app_printf("\tINR\t%s\n",cp) ;
   }
}
/*-----------------------------------------------------------------*/
void inc_wordreg(int i_regs,char *cp)
{
   if (i_cpu == ASMZ80) {
      app_printf("\tINC\t%s\n",cp) ;
   } else if ((i_cpu == CPUZ80) || (i_cpu == CPU180)) {
      OUTPUT1(".Z80") ;
      app_printf("\tINC\t%s\n",cp) ;
      OUTPUT1(".8085") ;
   } else {
      switch (i_regs) {
      case BCreg :
         app_printf("\tINX\tB\n") ;
      break ;
      case DEreg :
         app_printf("\tINX\tD\n") ;
      break ;
      case HLreg :
         app_printf("\tINX\tH\n") ;
      break ;
      default :
         app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��		*/
      }
   }
}
/*-----------------------------------------------------------------*/
void inc_byteptr(int i_regs,char *cp)
{
   app_warning("�`�������W�X�^���g�p����\n") ;
   _move_bytereg_byteptr(Acc,i_regs,"A",cp) ;
   inc_bytereg("A") ;
   _move_byteptr_bytereg(i_regs,Acc,cp,"A") ;
}
/*-----------------------------------------------------------------*/
void inc_byteregptr(int i_regs,char *cp)
{
   if (i_regs == BYTEHLptr) {
      if (i_cpu == ASMZ80) {
         app_printf("\tINC\t(%s)\n",cp) ;
      } else if ((i_cpu == CPUZ80) || (i_cpu == CPU180)) {
         OUTPUT1(".Z80") ;
         app_printf("\tINC\t(%s)\n",cp) ;
         OUTPUT1(".8085") ;
      } else {
         app_printf("\tINR\tM\n") ;
      }
   } else if (i_regs == BYTEDEptr) {
      xchg_output() ;
      inc_byteregptr(BYTEHLptr,"HL") ;
      xchg_output() ;
   } else {
      push_output("HL") ;
      _move_wordreg_wordreg(HLreg,(i_regs - 40),"HL",cp) ;
      inc_byteregptr(BYTEHLptr,"HL") ;
      pop_output("HL") ;
   }
}
/*-----------------------------------------------------------------*/
/* �ϐ��̃C���N�������g�o�́i�]�����݁j */
void inc_output(char *cp)
{
   int i_type ;
   int i_regs ;

   __chk_bw(cp,&i_type,&i_regs) ;

   if (i_regs < WORDreg) {		/* BYTEreg < 10	*/
      inc_bytereg(cp) ;
   } else if (i_regs < REGptr) {	/* WORDreg < 20	*/
      inc_wordreg(i_regs,cp) ;
   } else if (i_regs == BYTEptr) {	/* BYTEptr == 30	*/
      inc_byteptr(i_regs,cp) ;
   } else if (i_regs == WORDptr) {	/* WORDptr == 40	*/
      app_error(TYPE_NOT_MATCH,1) ;	/* �s�x�o�d�̎w�肪�s��v */
   } else if (i_regs < WORDREGptr) {	/* BYTEREGptr < 60	*/
      inc_byteregptr(i_regs,cp) ;
   } else {
      app_error(TYPE_NOT_MATCH,1) ;	/* �s�x�o�d�̎w�肪�s��v */
   }
}
/*-----------------------------------------------------------------*/
void dec_bytereg(char *cp)
{
   if (i_cpu == ASMZ80) {
      app_printf("\tDEC\t%s\n",cp) ;
   } else if ((i_cpu == CPUZ80) || (i_cpu == CPU180)) {
      OUTPUT1(".Z80") ;
      app_printf("\tDEC\t%s\n",cp) ;
      OUTPUT1(".8085") ;
   } else {
      app_printf("\tDCR\t%s\n",cp) ;
   }
}
/*-----------------------------------------------------------------*/
void dec_wordreg(int i_regs,char *cp)
{
   if (i_cpu == ASMZ80) {
      app_printf("\tDEC\t%s\n",cp) ;
   } else if ((i_cpu == CPUZ80) || (i_cpu == CPU180)) {
      OUTPUT1(".Z80") ;
      app_printf("\tDEC\t%s\n",cp) ;
      OUTPUT1(".8085") ;
   } else {
      switch (i_regs) {
      case BCreg :
         app_printf("\tDCX\tB\n") ;
      break ;
      case DEreg :
         app_printf("\tDCX\tD\n") ;
      break ;
      case HLreg :
         app_printf("\tDCX\tH\n") ;
      break ;
      default :
         app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��		*/
      }
   }
}
/*-----------------------------------------------------------------*/
/* 95/07/09	*/
void dec_byteptr(int i_regs,char *cp)
{
   app_warning("�`�������W�X�^���g�p����\n") ;
   _move_bytereg_byteptr(Acc,i_regs,"A",cp) ;
   dec_bytereg("A") ;
   _move_byteptr_bytereg(i_regs,Acc,cp,"A") ;
}
/*-----------------------------------------------------------------*/
void dec_byteregptr(int i_regs,char *cp)
{
   if (i_regs == BYTEHLptr) {
      if (i_cpu == ASMZ80) {
         app_printf("\tDEC\t(%s)\n",cp) ;
      } else if ((i_cpu == CPUZ80) || (i_cpu == CPU180)) {
         OUTPUT1(".Z80") ;
         app_printf("\tDEC\t(%s)\n",cp) ;
         OUTPUT1(".8085") ;
      } else {
         app_printf("\tDCR\tM\n") ;
      }
   } else if (i_regs == BYTEDEptr) {
      xchg_output() ;
      dec_byteregptr(BYTEHLptr,"HL") ;
      xchg_output() ;
   } else {
      push_output("HL") ;
      _move_wordreg_wordreg(HLreg,(i_regs - 40),"HL",cp) ;
      dec_byteregptr(BYTEHLptr,"HL") ;
      pop_output("HL") ;
   }
}
/*-----------------------------------------------------------------*/
/* �ϐ��̃f�N�������g�o�́i�]�����݁j */
void dec_output(char *cp)
{
   int i_type ;
   int i_regs ;

   __chk_bw(cp,&i_type,&i_regs) ;
/*     printf("DEBUG %s %d\n",cp,i_regs) ;	*/

   if (i_regs < WORDreg) {		/* BYTEreg < 10	*/
      dec_bytereg(cp) ;
   } else if (i_regs < REGptr) {	/* WORDreg < 20	*/
      dec_wordreg(i_regs,cp) ;
   } else if (i_regs == BYTEptr) {	/* BYTEptr == 30	*/
      dec_byteptr(i_regs,cp) ;
   } else if (i_regs == WORDptr) {	/* WORDptr == 40	*/
      app_error(TYPE_NOT_MATCH,1) ;	/* �s�x�o�d�̎w�肪�s��v */
   } else if (i_regs < WORDREGptr) {	/* BYTEREGptr < 60	*/
      dec_byteregptr(i_regs,cp) ;
   } else {
      app_error(TYPE_NOT_MATCH,1) ;	/* �s�x�o�d�̎w�肪�s��v */
   }
}
/*-----------------------------------------------------------------*/
void loop80_output(char *cp_1,char *cp_label,int i_num)
{
   dec_output(cp_1) ;
   jcc_output("NZ","Z",cp_label,i_num) ;
}
/*-----------------------------------------------------------------*/
/* 95/06/23 dr <= sr	*/
void move_bytereg_to_bytereg(char *cp_sr,char *cp_dr)
{
   if (i_cpu == ASMZ80) {
      if (*cp_sr == 'M') {		/* MOV	r,M	*/
         OUTPUT3("LD",cp_dr,"(HL)") ;
      } else if (*cp_dr == 'M') {	/* MOV	M,r	*/
         OUTPUT3("LD","(HL)",cp_sr) ;
      } else {				/* MOV	r,r	*/
         OUTPUT3("LD",cp_dr,cp_sr) ;
      }
   } else {
      OUTPUT3("MOV",cp_dr,cp_sr) ;
   }
}
/*-----------------------------------------------------------------*/
/* 95/06/23	*/
void move_2bytereg_to_2bytereg(char *cp_sh,char *cp_sl,char *cp_dh,char *cp_dl)
{
   move_bytereg_to_bytereg(cp_sh,cp_dh) ;	/* sh => dh	*/
   move_bytereg_to_bytereg(cp_sl,cp_dl) ;
}
/*-----------------------------------------------------------------*/
