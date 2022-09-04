/*
                         Generic APP Source File

                          Filename : TOOLS86.C
                          Version  : 95/03/14
                            Written by Mio

                           APP86 �p�c�[���W
*/

#include <stdio.h>
#include <string.h>

#include "appdefs.h"
#include "appsub.h"
#include "apperr.h"
#include "tools86.h"

#define MAXVARLEN 80                 /* ���A�ϐ����ő咷 */

/* ���[�h���W�X�^���ǂ������ׂ� */
int is_wordreg(char *cp)
{
   static char *cpa_regs86word[] = {"AX","BX","CX","DX",
                                    "SI","DI","BP", NULL } ;
   int i ;

   /* NULL ������ ? */
   if (cp[0] == '\0') {
      return 0 ;
   }

   /* ���[�h���W�X�^���H */
   if ((i = app_scantbl(cpa_regs86word,cp)) != 0) {
      return i + WORDreg ;
   }
   return 0 ;
}

/* �o�C�g���W�X�^���ǂ������ׂ� */
int is_bytereg(char *cp)
{
   static char *cpa_regs86byte[] = {"AL","AH","BL","BH",
                                    "CL","CH","DL","DH", NULL } ;
   int i ;

   /* NULL ������ ? */
   if (cp[0] == '\0') {
      return 0 ;
   }

   /* ���[�h���W�X�^���H */
   if ((i = app_scantbl(cpa_regs86byte,cp)) != 0) {
      return i ;
   }
   return 0 ;
}

/* (byte),(word) �L�[���[�h�̌���
   (byte) ... "(byte)"��������폜���� 1 ��Ԃ�
   (word) ... "(word)"��������폜���� 2 ��Ԃ�
   �ǂ����������Ȃ��� 0 ��Ԃ�   
*/
int cast_eval(char *cp)
{
   static char *cpa_wordbyte[] = {"(byte)","(word)","(BYTE)","(WORD)",NULL} ;
   return (int)app_scantbl_with_cut(cpa_wordbyte,cp) ;
}

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
