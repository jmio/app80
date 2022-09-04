/*
                         Generic APP Source File

                          Filename : COND85.C
                          Version  : 95/03/10
                            Written by Miu

                          �����W�����v�o�͏���
*/

#include <stdio.h>
#include <string.h>

#include "appdefs.h"
#include "appsub.h"
#include "apperr.h"

#include "cond80.h"
#include "code80.h"
#include "tools80.h"

/*-----------------------------------------------------------------*/
/* �����W�����v�o�� */
void jcc_output(char *cp_s,char *cp_n,char *cp_label,int i_num)
{
   /* �����i�l�o�̐��� 95/06/21	*/
   jflg_output(cp_s, cp_label, i_num) ;
}
/*-----------------------------------------------------------------*/
/* ���� �� ��� �� ���ׂ� */
void __chk_bw(char *cp,int *ip_type,int *ip_regs)
{
   int i_val ;

   *ip_regs = is_reg(cp) ;
   if (*ip_regs == 0) {			/* �e�[�u���ɂȂ�	*/
      *ip_type = cast_eval(cp) ;	/* BYTE or WORD		*/
      switch (*ip_type) {
      case NONEtype :			/* �C�~�f�B�G�C�g�H	*/
         *ip_regs = is_regptr(cp) ;	/* 21[HL],22[BC],23[DE]	*/
      break ;
      case BYTEtype :			/* (BYTE) (byte)	*/
         i_val = is_regptr(cp) ;
         *ip_regs = 30 + i_val;
      break ;
      case WORDtype :			/* (WORD) (word)	*/
         i_val = is_regptr(cp) ;
         *ip_regs = 40 + i_val;
      }
   } else if (*ip_regs < WORDreg) {	/* BYTE REG	*/
      *ip_type = BYTEtype ;
   } else if (*ip_regs < REGptr) {	/* WORD REG	*/
      *ip_type = WORDtype ;
   }

/*   printf("DEBUG_BW %s %d %d\n",cp,*ip_type,*ip_regs) ; */
}
/*-----------------------------------------------------------------*/
void _cmp_acc_any(int i_regs2,char *cp_2)
{
   if ((i_regs2 >= Acc) && (i_regs2 < WORDreg)) {
      if (i_cpu == ASMZ80) {			/* 95/06/17	*/
         OUTPUT2("CP",cp_2) ;
      } else {
         OUTPUT2("CMP",cp_2) ;			/* _CMP	A,r		*/
      }
   } else {
      switch (i_regs2) {
      case NONE :				/* _CMP	A,??????	*/
         if (i_cpu == ASMZ80) {
            OUTPUT2("CP",cp_2) ;
         } else if ((i_cpu == CPUZ80) || (i_cpu == CPU180) ) {
            OUTPUT1(".Z80") ;
            OUTPUT2("CP",cp_2) ;
            OUTPUT1(".8085") ;
         } else {
            OUTPUT2("CPI",cp_2) ;
         }
      break ;
      case BYTEptr :				/* _CMP	A,(byte)???	*/
         app_warning("�g�k���W�X�^�g�p����B\n") ;
         if (i_cpu == ASMZ80) {			/* 95/06/17	*/
            app_printf("\tLD\tHL,%s\n",cp_2) ;	/* 95/06/18	*/
            OUTPUT2("CP","(HL)") ;
         } else {
            OUTPUT3("LXI","H",cp_2) ;
            OUTPUT2("CMP","M") ;
         }
      break ;
      case BYTEHLptr :				/* _CMP	A,(byte)[HL]	*/
         if (i_cpu == ASMZ80) {			/* 95/06/17	*/
            OUTPUT2("CP","(HL)") ;
         } else {
            OUTPUT2("CMP","M") ;
         }
      break ;
      case BYTEBCptr :				/* _CMP	A,(byte)[BC]	*/
         app_warning("�g�k���W�X�^�g�p����B\n") ;
         move_2bytereg_to_2bytereg("B","C","H","L") ;	/* 95/06/23	*/
         if (i_cpu == ASMZ80) {			/* 95/06/17	*/
            OUTPUT2("CP","(HL)") ;
         } else {
            OUTPUT2("CMP","M") ;
         }
      break ;
      case BYTEDEptr :				/* _CMP	A,(byte)[DE]	*/
         if (i_cpu == ASMZ80) {			/* 95/06/17	*/
            OUTPUT3("EX","DE","HL") ;
            OUTPUT2("CP","(HL)") ;
            OUTPUT3("EX","DE","HL") ;
         } else {
            OUTPUT1("XCHG") ;
            OUTPUT2("CMP","M") ;
            OUTPUT1("XCHG") ;
         }
      break ;
      default :
         app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��		*/
      }
   }
}

/*-----------------------------------------------------------------*/
int _cmp_any_bytereg(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if (i_regs2 == Acc) {		/* �E�ӂ� Acc	*/
      _cmp_acc_any(i_regs1,cp_1) ;
      return FALSE ;
   } else {				/* �E�ӂ� Acc �ȊO�� �޲�ڼ޽�	*/
      _move_acc_any(i_regs1,cp_1) ;	/* ���ӂ� Acc �Ƀ��[�h���Ă���	*/
      _cmp_acc_any(i_regs2,cp_2) ;	/* ��r����	*/
      return TRUE ;
   }
}

/*-----------------------------------------------------------------*/
int _cmp_any_byteptr(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if (i_regs1 == Acc) {		/* ���ӂ� Acc	*/
      _cmp_acc_any(i_regs2,cp_2) ;
      return TRUE ;
   } else if ((i_regs2 == BYTEHLptr) || (i_regs2 == BYTEDEptr)) {
      _move_acc_any(i_regs1,cp_1) ;	/* ���ӂ� Acc �Ƀ��[�h���Ă���	*/
      _cmp_acc_any(i_regs2,cp_2) ;
      return TRUE ;
   } else {
      _move_acc_any(i_regs2,cp_2) ;	/* �E�ӂ� Acc �Ƀ��[�h���Ă���	*/
      _cmp_acc_any(i_regs1,cp_1) ;	/* ��r����	*/
      return FALSE ;
   }
}

/*-----------------------------------------------------------------*/
int _cmp_any_none(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if (i_regs1 == Acc) {		/* ���ӂ� Acc	*/
      _cmp_acc_any(i_regs2,cp_2) ;
   } else {
      _move_acc_any(i_regs1,cp_1) ;	/* ���ӂ� Acc �Ƀ��[�h���Ă���	*/
      _cmp_acc_any(i_regs2,cp_2) ;
   }
   return TRUE ;
}

/*-----------------------------------------------------------------*/
/* cp_1 �� cp_2 �� ��r�����j�[���j�b�N�𐶐�����B */
int _cmp(char *cp1,char *cp2)
{
   int i_regs1 ;
   int i_regs2 ;
   int i_type1 ;
   int i_type2 ;
   int i_retval ;
   char cp_1[MAXEXPRLEN],cp_2[MAXEXPRLEN] ;
   
   expr_eval(cp1,cp_1) ;
   expr_eval(cp2,cp_2) ;
   
   __chk_bw(cp_1,&i_type1,&i_regs1) ;	/* ���ӂ́H	*/
   __chk_bw(cp_2,&i_type2,&i_regs2) ;	/* �E�ӂ́H	*/
   
   if (i_type1 == NONEtype) {
      if ((i_regs1 >= HLptr) && (i_regs1 <= DEptr)) {
         i_type1 = i_type2 ;
         i_regs1 = i_regs1 + 20 + i_type1 * 10 ;
      }
   }
   if (i_type2 == NONEtype) {
      if ((i_regs2 >= HLptr) && (i_regs2 <= DEptr)) {
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
   
   /* �E�ӂ��o�C�g���W�X�^	*/
   if ((i_regs2 >= Acc) && (i_regs2 < WORDreg)) {
      i_retval = _cmp_any_bytereg(i_regs1,i_regs2,cp_1,cp_2) ;
   }
   /* �E�ӂ��o�C�g�|�C���^	*/
   if (i_regs2 == BYTEptr) {
      i_retval = _cmp_any_byteptr(i_regs1,i_regs2,cp_1,cp_2) ;
   }
   /* �E�ӂ����W�X�^�o�C�g�|�C���^	*/
   if ((i_regs2 >= BYTEHLptr) && (i_regs2 <= BYTEIXptr)) {
      i_retval = _cmp_any_byteptr(i_regs1,i_regs2,cp_1,cp_2) ;
   }
   /* �E�ӂ��C�~�f�B�G�C�g	*/
   if ((i_regs2 == NONE)) {
      i_retval = _cmp_any_none(i_regs1,i_regs2,cp_1,cp_2) ;
   }
   return i_retval ;
}

/*-----------------------------------------------------------------*/
int _and_acc_any(int i_regs2,char *cp_2)
{
   if ((i_regs2 >= Acc) && (i_regs2 < WORDreg)) {
      if (i_cpu == ASMZ80) {			/* 95/06/17	*/
         OUTPUT2("AND",cp_2) ;
      } else {
         OUTPUT2("ANA",cp_2) ;			/* _AND	A,r		*/
      }
      return TRUE ;
   } else {
      switch (i_regs2) {
      case NONE :				/* _CMP	A,??????	*/
         if (i_cpu == ASMZ80) {			/* 95/06/17	*/
            OUTPUT2("AND",cp_2) ;
         } else {
            OUTPUT2("ANI",cp_2) ;
         }
      break ;
      case BYTEptr :				/* _CMP	A,(byte)???	*/
         app_warning("�g�k���W�X�^�g�p����B\n") ;
         if (i_cpu == ASMZ80) {			/* 95/06/17	*/
            app_printf("\tLD\tHL,%s\n",cp_2) ;	/* 95/06/18	*/
            OUTPUT2("AND","(HL)") ;
         } else {
            OUTPUT3("LXI","H",cp_2) ;
            OUTPUT2("ANA","M") ;
         }
      break ;
      case BYTEHLptr :				/* _CMP	A,(byte)[HL]	*/
         if (i_cpu == ASMZ80) {			/* 95/06/17	*/
            OUTPUT2("AND","(HL)") ;
         } else {
            OUTPUT2("ANA","M") ;
         }
      break ;
      case BYTEBCptr :				/* _CMP	A,(byte)[BC]	*/
         app_warning("�g�k���W�X�^�g�p����B\n") ;
         move_2bytereg_to_2bytereg("B","C","H","L") ;	/* 95/06/23	*/
         if (i_cpu == ASMZ80) {				/* 95/06/17	*/
            OUTPUT2("AND","(HL)") ;
         } else {
            OUTPUT2("ANA","M") ;
         }
      break ;
      case BYTEDEptr :				/* _CMP	A,(byte)[DE]	*/
         if (i_cpu == ASMZ80) {			/* 95/06/17	*/
            OUTPUT3("EX","DE","HL") ;
            OUTPUT2("AND","(HL)") ;
            OUTPUT3("EX","DE","HL") ;
         } else {
            OUTPUT1("XCHG") ;
            OUTPUT2("ANA","M") ;
            OUTPUT1("XCHG") ;
         }
      break ;
      default :
         app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��		*/
      }
      return TRUE ;
   }
}

/*-----------------------------------------------------------------*/
int _and_any_bytereg(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if (i_regs1 == Acc) {		/* ���ӂ� Acc		*/
      _and_acc_any(i_regs2,cp_2) ;
   } else {
      _move_acc_any(i_regs1,cp_1) ;	/* ���ӂ� Acc �Ƀ��[�h���Ă���	*/
      _and_acc_any(i_regs2,cp_2) ;
      _move_any_acc(i_regs1,cp_1) ;
   }
   return TRUE ;
}

/*-----------------------------------------------------------------*/
int _and_any_byteptr(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if (i_regs1 == Acc) {		/* ���ӂ� Acc	*/
      _and_acc_any(i_regs2,cp_2) ;
   } else {
      _move_acc_any(i_regs1,cp_1) ;	/* ���ӂ� Acc �Ƀ��[�h���Ă���	*/
      _and_acc_any(i_regs2,cp_2) ;
      _move_any_acc(i_regs1,cp_1) ;
   }
   return TRUE ;
}

/*-----------------------------------------------------------------*/
int _and_any_byteregptr(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if (i_regs1 == Acc) {		/* ���ӂ� Acc	*/
      _and_acc_any(i_regs2,cp_2) ;
   } else if (i_regs1 < WORDreg) {
      _move_acc_any(i_regs2,cp_2) ;	/* �E�ӂ� Acc �Ƀ��[�h���Ă���	*/
      _and_acc_any(i_regs1,cp_1) ;
      _move_any_acc(i_regs1,cp_1) ;
   } else {
      _move_acc_any(i_regs1,cp_1) ;	/* ���ӂ� Acc �Ƀ��[�h���Ă���	*/
      _and_acc_any(i_regs2,cp_2) ;
      _move_any_acc(i_regs1,cp_1) ;
   }
   return TRUE ;
}

/*-----------------------------------------------------------------*/
int _and_any_none(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if (i_regs1 == Acc) {		/* ���ӂ� Acc	*/
      _and_acc_any(i_regs2,cp_2) ;
   } else {
      _move_acc_any(i_regs1,cp_1) ;	/* ���ӂ� Acc �Ƀ��[�h���Ă���	*/
      _and_acc_any(i_regs2,cp_2) ;
      _move_any_acc(i_regs1,cp_1) ;
   }
   return TRUE ;
}

/*-----------------------------------------------------------------*/
int _and(char *cp1,char *cp2)
{
   int i_regs1 ;
   int i_regs2 ;
   int i_type1 ;
   int i_type2 ;
   int i_retval ;
   char cp_1[MAXEXPRLEN],cp_2[MAXEXPRLEN] ;
   
   expr_eval(cp1,cp_1) ;
   expr_eval(cp2,cp_2) ;
   
   __chk_bw(cp_1,&i_type1,&i_regs1) ;	/* ���ӂ́H	*/
   __chk_bw(cp_2,&i_type2,&i_regs2) ;	/* �E�ӂ́H	*/
   
   if (i_type2 == NONEtype) {
      if ((i_regs2 >= HLptr) && (i_regs2 <= DEptr)) {
         i_type2 = i_type1 ;
         i_regs2 = i_regs2 + 20 + i_type2 * 10 ;
      }
   }
   /* �a�x�s�d�C�v�n�q�c �w�肪 �K�؂�	*/
   if (i_type1 == NONEtype) {
      app_error(ARGTYPE_ERROR,1) ;		/* �s�x�o�d�̎w�肪�K�v	*/
   }
   if ((i_type1 == BYTEtype) && (i_type2 == WORDtype)) {
      app_error(TYPE_NOT_MATCH,1) ;		/* �s�x�o�d�̎w�肪�s��v	*/
   }
   if ((i_type1 == WORDtype) && (i_type2 == BYTEtype)) {
      app_error(TYPE_NOT_MATCH,1) ;		/* �s�x�o�d�̎w�肪�s��v	*/
   }
   if ((i_type1 != Acc) && (i_type2 == Acc)) {
      app_error(LVALUE_ERROR,1) ;		/* �E�ӂ� Acc �͕s�K��	*/
   }
   
   /* �E�ӂ��o�C�g���W�X�^	*/
   if ((i_regs2 >= Acc) && (i_regs2 < WORDreg)) {
      i_retval = _and_any_bytereg(i_regs1,i_regs2,cp_1,cp_2) ;
   }
   /* �E�ӂ��o�C�g�|�C���^	*/
   if (i_regs2 == BYTEptr) {
      i_retval = _and_any_byteptr(i_regs1,i_regs2,cp_1,cp_2) ;
   }
   /* �E�ӂ����W�X�^�o�C�g�|�C���^	*/
   if ((i_regs2 >= BYTEHLptr) && (i_regs2 <= BYTEIXptr)) {
      i_retval = _and_any_byteregptr(i_regs1,i_regs2,cp_1,cp_2) ;
   }
   /* �E�ӂ��C�~�f�B�G�C�g	*/
   if (i_regs2 == NONE) {
      i_retval = _and_any_none(i_regs1,i_regs2,cp_1,cp_2) ;
   }
   return i_retval ;
}

/*-----------------------------------------------------------------*/
int _test_acc_any(int i_regs2,char *cp_2)
{
   if ((i_regs2 >= Acc) && (i_regs2 < WORDreg)) {
      if (i_cpu == CPU180) {			/* 95/06/30	*/
         OUTPUT2("TST",cp_2) ;
      } else if (i_cpu == ASMZ80) {
         OUTPUT2("AND",cp_2) ;			/* 95/06/30	*/
      } else {
         OUTPUT2("ANA",cp_2) ;			/* _AND	A,r		*/
      }
      return TRUE ;
   } else {
      switch (i_regs2) {
      case NONE :				/* _CMP	A,??????	*/
         if (i_cpu == CPU180) {			/* 95/06/30	*/
            OUTPUT2("TST",cp_2) ;
         } else if (i_cpu == ASMZ80) {			/* 95/06/18	*/
            OUTPUT2("AND",cp_2) ;
         } else {
            OUTPUT2("ANI",cp_2) ;
         }
      break ;
      case BYTEptr :				/* _CMP	A,(byte)???	*/
         app_warning("�g�k���W�X�^�g�p����B\n") ;
         if (i_cpu == CPU180) {			/* 95/06/30	*/
            OUTPUT3("LXI","H",cp_2) ;
            OUTPUT2("TST","(HL)") ;
         } else if (i_cpu == ASMZ80) {		/* 95/06/30	*/
            app_printf("\tLD\tHL,%s\n",cp_2) ;	/* 95/06/18	*/
            OUTPUT2("AND","(HL)") ;
         } else {
            OUTPUT3("LXI","H",cp_2) ;
            OUTPUT2("ANA","M") ;
         }
      break ;
      case BYTEHLptr :				/* _CMP	A,(byte)[HL]	*/
         if (i_cpu == CPU180) {			/* 95/06/30	*/
            OUTPUT2("TST","(HL)") ;
         } else if (i_cpu == ASMZ80) {		/* 95/06/30	*/
            OUTPUT2("AND","(HL)") ;
         } else {
            OUTPUT2("ANA","M") ;
         }
      break ;
      case BYTEBCptr :				/* _CMP	A,(byte)[BC]	*/
         app_warning("�g�k���W�X�^�g�p����B\n") ;
         move_2bytereg_to_2bytereg("B","C","H","L") ;	/* 95/06/23	*/
         if (i_cpu == CPU180) {				/* 95/06/30	*/
            OUTPUT2("TST","(HL)") ;
         } else if (i_cpu == ASMZ80) {			/* 95/06/30	*/
            OUTPUT2("AND","(HL)") ;
         } else {
            OUTPUT2("ANA","M") ;
         }
      break ;
      case BYTEDEptr :				/* _CMP	A,(byte)[DE]	*/
         if (i_cpu == CPU180) {			/* 95/06/30	*/
            OUTPUT1("XCHG") ;
            OUTPUT2("TST","(HL)") ;
            OUTPUT1("XCHG") ;
         } else if (i_cpu == ASMZ80) {		/* 95/06/30	*/
            OUTPUT3("EX","DE","HL") ;
            OUTPUT2("AND","(HL)") ;
            OUTPUT3("EX","DE","HL") ;
         } else {
            OUTPUT1("XCHG") ;
            OUTPUT2("ANA","M") ;
            OUTPUT1("XCHG") ;
         }
      break ;
      default :
         app_error(LVALUE_ERROR,1) ;		/* ���ӂ��s�K��		*/
      }
      return TRUE ;
   }
}

/*-----------------------------------------------------------------*/
int _test_any_bytereg(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if (i_regs1 == Acc) {		/* ���ӂ� Acc	*/
      _test_acc_any(i_regs2,cp_2) ;	/* 95/06/18	*/
   } else {
      app_warning("�`���W�X�^�g�p����B\n") ;
      _move_acc_any(i_regs1,cp_1) ;	/* ���ӂ� Acc �Ƀ��[�h���Ă���	*/
      _test_acc_any(i_regs2,cp_2) ;
   }
   return TRUE ;
}

/*-----------------------------------------------------------------*/
int _test_any_byteptr(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if (i_regs1 == Acc) {		/* ���ӂ� Acc	*/
      _test_acc_any(i_regs2,cp_2) ;	/* 95/06/18	*/
   } else {
      app_warning("�`���W�X�^�g�p����B\n") ;
      _move_acc_any(i_regs1,cp_1) ;	/* ���ӂ� Acc �Ƀ��[�h���Ă���	*/
      _test_acc_any(i_regs2,cp_2) ;
   }
   return TRUE ;
}

/*-----------------------------------------------------------------*/
int _test_any_byteregptr(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if (i_regs1 == Acc) {		/* ���ӂ� Acc	*/
      _test_acc_any(i_regs2,cp_2) ;	/* 95/06/18	*/
   } else if (i_regs1 < WORDreg) {
      app_warning("�`���W�X�^�g�p����B\n") ;
      _move_acc_any(i_regs2,cp_2) ;	/* �E�ӂ� Acc �Ƀ��[�h���Ă���	*/
      _test_acc_any(i_regs1,cp_1) ;
   } else {
      app_warning("�`���W�X�^�g�p����B\n") ;
      _move_acc_any(i_regs1,cp_1) ;	/* ���ӂ� Acc �Ƀ��[�h���Ă���	*/
      _test_acc_any(i_regs2,cp_2) ;
   }
   return TRUE ;
}

/*-----------------------------------------------------------------*/
int _test_any_none(int i_regs1,int i_regs2,char *cp_1,char *cp_2)
{
   if (i_regs1 == Acc) {		/* ���ӂ� Acc	*/
      _test_acc_any(i_regs2,cp_2) ;	/* 95/06/18	*/
   } else {
      app_warning("�`���W�X�^�g�p����B\n") ;
      _move_acc_any(i_regs1,cp_1) ;	/* ���ӂ� Acc �Ƀ��[�h���Ă���	*/
      _and_acc_any(i_regs2,cp_2) ;
   }
   return TRUE ;
}

/*-----------------------------------------------------------------*/
int _test(char *cp1,char *cp2)
{
   int i_regs1 ;
   int i_regs2 ;
   int i_type1 ;
   int i_type2 ;
   int i_retval ;
   char cp_1[MAXEXPRLEN],cp_2[MAXEXPRLEN] ;
   
   expr_eval(cp1,cp_1) ;
   expr_eval(cp2,cp_2) ;
   
/*   printf("%s %s\n",cp_1,cp_2) ; 95/06/19	*/
   
   __chk_bw(cp_1,&i_type1,&i_regs1) ;	/* ���ӂ́H	*/
   __chk_bw(cp_2,&i_type2,&i_regs2) ;	/* �E�ӂ́H	*/
   
   if (i_type2 == NONEtype) {
      if ((i_regs2 >= HLptr) && (i_regs2 <= DEptr)) {
         i_type2 = i_type1 ;
         i_regs2 = i_regs2 + 20 + i_type2 * 10 ;
      }
   }
   /* �a�x�s�d�C�v�n�q�c �w�肪 �K�؂�	*/
   if (i_type1 == NONEtype) {
      app_error(ARGTYPE_ERROR,1) ;		/* �s�x�o�d�̎w�肪�K�v	*/
   }
   if ((i_type1 == BYTEtype) && (i_type2 == WORDtype)) {
      app_error(TYPE_NOT_MATCH,1) ;		/* �s�x�o�d�̎w�肪�s��v	*/
   }
   if ((i_type1 == WORDtype) && (i_type2 == BYTEtype)) {
      app_error(TYPE_NOT_MATCH,1) ;		/* �s�x�o�d�̎w�肪�s��v	*/
   }
   if ((i_type1 != Acc) && (i_type2 == Acc)) {
      app_error(LVALUE_ERROR,1) ;		/* �E�ӂ� Acc �͕s�K��	*/
   }
   
   /* �E�ӂ��o�C�g���W�X�^	*/
   if ((i_regs2 >= Acc) && (i_regs2 < WORDreg)) {
      i_retval = _test_any_bytereg(i_regs1,i_regs2,cp_1,cp_2) ;
   }
   /* �E�ӂ��o�C�g�|�C���^	*/
   if (i_regs2 == BYTEptr) {
      i_retval = _test_any_byteptr(i_regs1,i_regs2,cp_1,cp_2) ;
   }
   /* �E�ӂ����W�X�^�o�C�g�|�C���^	*/
   if ((i_regs2 >= BYTEHLptr) && (i_regs2 <= BYTEIXptr)) {
      i_retval = _test_any_byteregptr(i_regs1,i_regs2,cp_1,cp_2) ;
   }
   /* �E�ӂ��C�~�f�B�G�C�g	*/
   if (i_regs2 == NONE) {
      i_retval = _test_any_none(i_regs1,i_regs2,cp_1,cp_2) ;
   }
   return i_retval ;
}

/*-----------------------------------------------------------------*/
/*-----------------------------------------------------------------*/
/* cp_1 �� cp_2 �� ��r������Aequal �����ŃW�����v����j�[���j�b�N */
void cond_equal(char *cp_1,char *cp_2,char *cp_label,int i_num)
{
   _cmp(cp_1,cp_2) ;
   /* �����i�l�o�̐��� 95/06/21	*/
   jflg_output("Z",cp_label,i_num) ;
}
/*-----------------------------------------------------------------*/
/* cp_1 �� cp_2 �� ��r������Anot equal �����ŃW�����v����R�[�h */
void cond_not_e(char *cp_1,char *cp_2,char *cp_label,int i_num)
{
   _cmp(cp_1,cp_2) ;
   /* �����i�l�o�̐��� 95/06/21	*/
   jflg_output("NZ",cp_label,i_num) ;
}
/*-----------------------------------------------------------------*/
/* cp_1 �� cp_2 �� ��r������Abelow �����ŃW�����v����R�[�h */
void cond_below(char *cp_1,char *cp_2,char *cp_label,int i_num)
{
   if (_cmp(cp_1,cp_2) == TRUE) {
      /* �����i�l�o�̐��� 95/06/21	*/
      jflg_output("C",cp_label,i_num) ;
   } else {
      /* �����i�l�o�̐��� 95/06/21	*/
      jflg_output_n("Z") ;
      jflg_output("NC",cp_label,i_num) ;
   }
}
/*-----------------------------------------------------------------*/
/* cp_1 �� cp_2 �� ��r������Abelow or equal �����ŃW�����v����R�[�h */
void cond_b_or_e(char *cp_1,char *cp_2,char *cp_label,int i_num)
{
   if (_cmp(cp_1,cp_2) == TRUE) {
      /* �����i�l�o�̐��� 95/06/21	*/
      jflg_output("C",cp_label,i_num) ;
      jflg_output("Z",cp_label,i_num) ;
   } else {
      jflg_output("NC",cp_label,i_num) ;
   }
}
/*-----------------------------------------------------------------*/
/* cp_1 �� cp_2 �� ��r������Aabove �����ŃW�����v����R�[�h */
void cond_above(char *cp_1,char *cp_2,char *cp_label,int i_num)
{
   if (_cmp(cp_1,cp_2) == TRUE) {
      jflg_output_n("Z") ;
      jflg_output("NC",cp_label,i_num) ;
   } else {
      jflg_output("C",cp_label,i_num) ;
   }
}
/*-----------------------------------------------------------------*/
/* cp_1 �� cp_2 �� ��r������Aabove or equal �����ŃW�����v����R�[�h */
void cond_a_or_e(char *cp_1,char *cp_2,char *cp_label,int i_num)
{
   if (_cmp(cp_1,cp_2) == TRUE) {
      jflg_output("NC",cp_label,i_num) ;
   } else {
      jflg_output("C",cp_label,i_num) ;
      jflg_output("Z",cp_label,i_num) ;
   }
}
/*-----------------------------------------------------------------*/
/* cp_1 �� cp_2 �� test ������Azero �����ŃW�����v����R�[�h */
void cond_test(char *cp_1,char *cp_2,char *cp_label,int i_num)
{
   _test(cp_1,cp_2) ;
   jflg_output("Z",cp_label,i_num) ;	/* 95/06/21	*/
}

/*------------------------------------------------------------------*/
/* cp_1 �� cp_2 �� test ������Anot zero �����ŃW�����v����R�[�h */
void cond_not_t(char *cp_1,char *cp_2,char *cp_label,int i_num)
{
   _test(cp_1,cp_2) ;
   jflg_output("NZ",cp_label,i_num) ;	/* 95/06/21	*/
}

/*-----------------------------------------------------------------*/
/* cp_1 �� cp_2 �� and ������Azero �����ŃW�����v����R�[�h */
void cond_and(char *cp_1,char *cp_2,char *cp_label,int i_num)
{
   _and(cp_1,cp_2) ;
   jflg_output("Z",cp_label,i_num) ;	/* 95/06/21	*/
}

/*-----------------------------------------------------------------*/
/* cp_1 �� cp_2 �� and ������Anot zero �����ŃW�����v����R�[�h */
void cond_not_a(char *cp_1,char *cp_2,char *cp_label,int i_num)
{
   _and(cp_1,cp_2) ;
   jflg_output("NZ",cp_label,i_num) ;	/* 95/06/21	*/
}

/*-----------------------------------------------------------------*/
/* �t���O�ɂ��W�����v */
void cond_flags(char *cp_1,int i_tf,char *cp_label,int i_num)
{
   int i ;
   char ca_nfn[256] ;
   static char *cpa_flagname[] = { "PO","PE",NULL } ;
   static char *cpa_flagnot[]  = { "PE","PO",NULL } ;
   
   cutspc(cp_1) ;                              /* ���݂���菜�� */
   gettoken(cp_1,WHITESPACE,LDELIM,ca_nfn) ;   /* ���������̐؂�o�� */
   i = app_scantbl(cpa_flagname,ca_nfn) ;
   
   if (i_tf == ON) {
      Jnn_OUTPUT(cp_1,cp_label,i_num) ;
   } else {
      if (i == 0) {
         JNn_OUTPUT(cp_1,cp_label,i_num) ;
      } else {
         Jnn_OUTPUT(cpa_flagnot[i -1],cp_label,i_num) ;
      }
   }
}

/*-----------------------------------------------------------------*/
