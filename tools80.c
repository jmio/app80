/*
                         Generic APP Source File

                          Filename : TOOLS80.C
                          Version  : 95/07/05
                            Written by Mio

                           APP80 �p�c�[���W
*/

#include <stdio.h>
#include <string.h>

#include "appdefs.h"
#include "appsub.h"
#include "apperr.h"
#include "tools80.h"

#define MAXVARLEN 80                 /* ���A�ϐ����ő咷 */

/* �o�C�g���W�X�^���ǂ������ׂ� */
int is_reg(char *cp)
{
   static char *cpa_regs80word[] = {"HL","BC","DE","PC","SP","IX","IY","AF",NULL} ;
   static char cpa_regsbyte[] = "ABCDEHLIR" ;
   int i ;

   /* NULL ������ ? */
   if (cp[0] == '\0') {
      return 0 ;
   }
   /* �o�C�g���W�X�^���H */
   if (cp[1] == '\0') {
      char *cp1 = strchr(cpa_regsbyte,cp[0]) ;
      if (cp1 != NULL) {
         return (int)(cp1-cpa_regsbyte)+1 ;
      }
      return 0 ;
   }
   /* ���[�h���W�X�^���H */
   if ((i = app_scantbl(cpa_regs80word,cp)) != 0) {
      return i + WORDreg ;
   }
   return 0 ;
}
/*-----------------------------------------------------------------*/
/* ���W�X�^�|�C���^���H */
int is_regptr(char *cp)
{
   char tmp[80] ;
   int i ;
   static char *cpa_regs80word2[] = {"HL","BC","DE","PC","SP","IX","IY",NULL} ;

   strcpy(tmp,cp) ;
   if (cp[0] == '\(') {
      i = 0 ;
      while ((cp[i] != '\0') && (cp[i + 1] != '\)')) {
         tmp[i] = cp[i + 1] ;
         i = i + 1 ;
      }
      tmp[i] = '\0' ;
   } else if (cp[0] == '\[') {
      i = 0 ;
      while ((cp[i] != '\0') && (cp[i + 1] != '\]')) {
         tmp[i] = cp[i + 1] ;
         i = i + 1 ;
      }
      tmp[i] = '\0' ;
   }
/*   printf("DEBUG_ptr %s %s\n",cp,tmp) ; */
   /* ���[�h���W�X�^���H */
   if ((i = app_scantbl(cpa_regs80word2,tmp)) != 0) {
      strcpy(cp,tmp) ;
      return i + REGptr ;
   } else if (strstr(tmp,"IX") != NULL) {
      strcpy(cp,tmp) ;
      return 6 + REGptr ;
   } else if (strstr(tmp,"IY") != NULL) {
      strcpy(cp,tmp) ;
      return 7 + REGptr ;
   }
   return 0 ;
}
/*-----------------------------------------------------------------*/
/* (byte),(word) �L�[���[�h�̌���
   (byte) ... "(byte)"��������폜���� 1 ��Ԃ�
   (word) ... "(word)"��������폜���� 2 ��Ԃ�
   �ǂ����������Ȃ��� 0 ��Ԃ�   
*/
int cast_eval(char *cp)
{
   int i_ans ;
   static char *cpa_wordbyte[] = {"(byte)","(word)","(BYTE)","(WORD)",NULL} ;
   i_ans = (int)app_scantbl_with_cut(cpa_wordbyte,cp) ;
   if (i_ans > 2) {
      i_ans = i_ans - 2 ;	/* 95/06/27	*/
   }
   return i_ans ;
}
/*-----------------------------------------------------------------*/
/* ++,-- �L�[���[�h�̌��� */
int incdec_eval(char *cp_ptr,char *cp_var,char *cp_incdec,void (*outfunc)())
{
   char *cp_i ;
   char ca_buf[MAXVARLEN] ;

   /* ++,-- �̈ʒu�𒲂ׂ� */
   cp_i = strstr(cp_var,cp_incdec) ;
   if (cp_i != NULL) {
      if ((cp_i != cp_var) && (cp_i[app_strlen(cp_incdec)] != '\0')) { 
         /* �擪�łȂ��A�����ł��Ȃ��Ƃ��A�h���X�w��Ƃ݂Ȃ� e.g. [++BX] �� */
         gettoken(cp_i+2,WHITESPACE,"])",ca_buf) ;
         str_replace(cp_var,cp_incdec,"") ; /* ++,-- �̍폜 */
      } else {
         str_replace(cp_var,cp_incdec,"") ; /* ++,-- �̍폜 */
         strcpy(ca_buf,cp_ptr) ;
         strcat(ca_buf,cp_var) ;
      }
      outfunc(ca_buf) ;
   }

   return 0 ;
}
/*-----------------------------------------------------------------*/
/* += , -= �L�[���[�h�̌��� */
int addsub_eval(char *cp_ptr,char *cp_var,char *cp_addsub,void (*outfunc)())
{
   char *cp_i ;
   char ca_buf1[MAXVARLEN] ,
        ca_buf2[MAXVARLEN] ;

   /* +=,-= �̈ʒu�𒲂ׂ� */
   cp_i = strstr(cp_var,cp_addsub) ;
   if (cp_i != NULL) {
      /* �ϐ��̐؂�o�� */
      cp_var = gettoken(cp_var,WHITESPACE,cp_addsub,ca_buf2) ;
      cutspc(ca_buf2) ;
      if (app_strlen(ca_buf2) == 0) {
         app_error(EXP_SYNTAX_ERROR,1) ;
      }
      strcpy(ca_buf1,cp_ptr) ; /* �|�C���^������ */
      strcat(ca_buf1,ca_buf2) ; /* �ϐ��{�� */

      /* �����̐؂�o�� */
      gettoken(cp_i+2,WHITESPACE,LDELIM,ca_buf2) ;
      cutspc(ca_buf2) ;
      if (app_strlen(ca_buf2) == 0) {
         app_error(EXP_SYNTAX_ERROR,1) ;
      }
      outfunc(ca_buf1,ca_buf2) ;

      /* += , -= �̍폜 */
      str_replace(cp_var,cp_addsub,"") ;
   }

   return 0 ;
}
