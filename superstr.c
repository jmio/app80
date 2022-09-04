/*
                         Generic APP Source File

                          Filename : SUPERSTR.C
                          Version  : 95/03/13
                            Written by Mio

                      ��₱���������񑀍�̉�����
                        �ichar* �z�񑀍�֐��Q�j
*/

#include <stdio.h>
#include <string.h>

#include "appdefs.h"
#include "appsub.h"

#include "superstr.h"

#define MAXSTRLEN 256

/* �J���}�ŋ�؂�ꂽ�����񂩂�Achar *[] �^�z����쐬���ĕԂ�
   �i�A���A������|�C���^�z��̃T�C�Y�͌Œ蒷�j */
int strtocpa(char *cp,char ***cppp_list,int i_maxlen,char *cp_delim)
{
    char ca_tmp[MAXSTRLEN] ;
    int i = 0 ;

    /* �|�C���^�z��̊m�� */
    *cppp_list = (char **)app_malloc(sizeof(void *)*(i_maxlen+1)) ;

    /* �������z��̍쐬 */
    while ((app_strlen(cp) > 0) && (i < i_maxlen)) {
       cp = gettoken(cp,WHITESPACE,cp_delim,ca_tmp) ;
       cutspc(ca_tmp) ;
       if (app_strlen(ca_tmp) > 0) {
          (*cppp_list)[i++] = app_strdup(ca_tmp) ; /* �z��v�f�̍쐬 */
       } else {
          (*cppp_list)[i++] = "" ;                 /* �k�������� */
       }
       if ((*cp != '\0') && (strchr(cp_delim,(int)*cp) != NULL)) {
          cp++ ; /* �J���}�̎����� */
       }
    }

    if (i == 0) {
       app_free(*cppp_list) ; /* �v�f���Ȃ��Ƃ� */
       *cppp_list = NULL ;
    } else {
       (*cppp_list)[i] = NULL ; /* �f���~�^ */
    }

    return i ; /* �v�f�̌� */
}

/* char *[] �^�������X�g�̕\���i��Ƀf�o�b�O�p�j */
void cpa_print(char *cpa_list[])
{
   int i ;
   for (i = 0 ;cpa_list[i] != NULL;i++) {
      printf("ARG %d:[%s]\n",i,cpa_list[i]) ;
   }
}

/* char *[] �^�������X�g�i�������g�A�y�єz��v�f�j�̊J�� */
void cpa_release(char *cpa_list[])
{
   int i ;
   if (cpa_list != NULL) {
      for (i = 0 ;cpa_list[i] != NULL;i++) {
         if (app_strlen(cpa_list[i]) > 0) { /* �k��������łȂ��Ƃ� */
            app_free(cpa_list[i]) ; /* ���X�g�̗v�f�̊J�� */
         }
      }
   }
   app_free(cpa_list) ; /* �������g�̊J�� (NULL �̎��� ���̊֐����� error) */
}

/* char *[] �^�������X�g�̓��e�̓��ꐫ�`�F�b�N */
int cpa_equalp(char *cpa_list1[],char *cpa_list2[])
{
   int i ;

   /* �e�v�f�̂����A��ł��قȂ��Ă���΃y�P */
   for (i = 0 ; cpa_list1[i] != NULL ; i++) {
      if (strcmp(cpa_list1[i],cpa_list2[i]) != 0) {
         return FALSE ;
      }
   }
   /* �Q�Ԗڂ̃��X�g�̕������� */
   if (cpa_list2[i] != NULL) {
      return FALSE ;
   }
   /* ���S�Ɉ�v */
   return TRUE ;
}
