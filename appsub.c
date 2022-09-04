/*
                         Generic APP Source File

                          Filename : APPSUB.C
                          Version  : 95/03/01
                            Written by Mio

                    APP �]�����[�`���p�T�u���[�`���Q
*/

#include <stdio.h>
#include <string.h>

#ifdef __MSDOS__
# include <alloc.h>
#else
# include <malloc.h>
#endif

#include <stdarg.h>
#include <ctype.h>

#include "appmain.h"
#include "appdefs.h"
#include "apperr.h"
#include "appsub.h"

/* ----------------------------------------------------------- */

/* �e CPU �ʁA�s�]�����[�`���̃R�[�� (EVALxx.C �ɑ���) */
int eval_line(char *) ;

/* ----------------------------------------------------------- */

/* �s�̃v���t�F�b�`�o�b�t�@�A����уt���O */
static FILE *fp_in_cpy = NULL ;
static int i_pftch = FALSE ;
static char ca_pftch[LINEBUFSIZ] ;

/* �������c�� */
long app_avmem(void)
{
#ifdef __MSDOS__
   return (long)coreleft() ;
#else
   return (long)0 ;
#endif
}

/* �������̊m�� */
void *app_malloc(size_t st)
{
   void *v_result = malloc(st) ;

   if (v_result == NULL) {
      app_error(MEMORY_NOT_ENOUGH,1) ;
   }
   return v_result ;
}

/* �������̊J�� */
void app_free(void *vp)
{
   if (vp == NULL) {
      app_error(FREE_NULL,0) ;
   } else {
      free(vp) ;
   }
   return ;
}

/* �V���Ɋm�ۂ����������ɕ�������R�s�[ : �ϒ��o�b�t�@ */
char *app_strdup(char *cp_orig)
{
   char *cp_new = (char *)app_malloc(app_strlen(cp_orig)+1) ;

   strcpy(cp_new,cp_orig) ;
   return cp_new ;
}

/* �V���Ɋm�ۂ����������ɕ�������R�s�[ : �Œ蒷�o�b�t�@ */
char *app_strdup2(char *cp_orig,size_t s)
{
   char *cp_new ;

   if ((app_strlen(cp_orig)+1) < s) {
      app_error(STRDUP_FAIL,1) ;
   } ;
   cp_new = app_malloc(s) ;
   strcpy(cp_new,cp_orig) ;
   return cp_new ;
}

/* �t�@�C���̃I�[�v�� */
FILE *open_file(const char *cp_fname,const char *cp_mode)
{
   FILE *fp_result = fopen(cp_fname,cp_mode) ; /* ���� */

   if (fp_result == NULL) {
      app_error(FILE_ERROR,0) ;
      perror(cp_fname) ; /* �G���[���b�Z�[�W�o�� */
      exit(1) ;
   }
   return fp_result ;
}

/* �t�@�C������̂P�s�ǂ݂��� */
static
int read_line(char *cp_line,FILE *fp_in)
{
   /* �o�b�t�@�N���A */
   cp_line[0] = '\0' ;

   /* �t�@�C����ǂ݂��s���Ă��邩�H */
   if (i_pftch == TRUE) {
      strcpy(cp_line,ca_pftch) ;
      i_pftch = FALSE ;
      return TRUE ;
   }

   /* EOF ? */
   if (feof(fp_in)) {
      return FALSE ;
   }

   /* �s���Ă��Ȃ���Βʏ�ɓǂ݂��� */
   if (fgets(cp_line,LINEBUFSIZ,fp_in) != NULL) {
      return TRUE ;
   }

   return FALSE ;
}

/* �t�@�C������̂P�s��ǂ� */
int prefetch_line(char *cp_buf)
{
   /* �o�b�t�@�N���A */
   cp_buf[0] = '\0' ;

   /* �t�@�C����ǂ݂��s���Ă��邩�H */
   if ((i_pftch == TRUE) || (fp_in_cpy == NULL)) {
      return FALSE ; /* ���s */
   }

   /* EOF ? */
   if (feof(fp_in_cpy)) {
      return FALSE ;
   }

   /* �s���Ă��Ȃ���Βʏ�ɓǂ݂��� */
   if (fgets(ca_pftch,LINEBUFSIZ,fp_in_cpy) != NULL) {
      strcpy(cp_buf,ca_pftch) ;
      i_pftch = TRUE ;
      return TRUE ;
   }

   return FALSE ;
}

/* �t�@�C���̃C���N���[�h */
int include_file(char *cp_fname)
{
   FILE *fp_in,*fp_in_old ;   /* ���̓t�@�C�� */
   long l_lineno = 0 ;        /* ���̓t�@�C���s�ԍ� */
   char *cp_old_fname ;       /* �C���N���[�h���鑤�̃t�@�C�����|�C���^ */
   char ca_line[LINEBUFSIZ] ; /* �s�o�b�t�@ */

   /* �t�@�C���̃I�[�v�� */
   fp_in = open_file(cp_fname,"rt") ;

   /* �v���t�F�b�`�p�̃R�s�[�쐬 */
   fp_in_old = fp_in_cpy ;
   fp_in_cpy = fp_in ;

   /* �t�@�C�������G���[�������[�`������Q�Ɖ\�ɂ��� */
   cp_old_fname = cp_err_fname ;
   cp_err_fname = cp_fname ;

   /* �t�@�C���̕]�� */
   while(read_line(ca_line,fp_in) == TRUE) {
      l_err_lineno = ++l_lineno ; /* �s�ԍ����t�@�C�����X�^�e�B�b�N�� */
      (void)eval_line(ca_line) ;  /* �]�� (EVALxx.C) */
   }

   fclose(fp_in) ;

   fp_in_cpy = fp_in_old ;
   cp_err_fname = cp_old_fname ;

   return 0 ;
}

/* �o�̓t�@�C���ɕ�������������� */
void app_outstr(char *cp_outstr)
{
   if (fputs(cp_outstr,fp_out) == EOF) {
      app_error(FILE_ERROR,0) ;
      fclose(fp_out) ;
      perror("app_outstr") ;
      exit(1) ;
   }
}

/* �o�̓t�@�C���� printf �`���ŏ������� */
void app_printf(char *cp_format , ...)
{
   va_list v_argptr;

   va_start(v_argptr, cp_format);
   if (vfprintf(fp_out,cp_format,v_argptr) == EOF) {
      app_error(FILE_ERROR,0) ;
      perror("app_printf") ;
      exit(1) ;
   }

   va_end(v_argptr);
}

/* �������g�̃t�@�C�����𓾂� */
void get_ownname(char *cp)
{
   strcpy(cp,cp_err_fname) ;
   return ;
}

/* �������g�̃t�@�C�����𓾂� */
void get_ownbasename(char *cp)
{
   get_ownname(cp) ;
   cp = getdotpos(cp) ;
   if (cp != NULL) { /* �g���q����������A�����Ő؂� */
      *cp = '\0' ;
   }
   return ;
}

/* �k���|�C���^�A�k��������`�F�b�N */
int app_strlen(char *cp)
{
   if (cp == NULL) {
      app_error(ERR_NULL_STR,1) ; /* �k���|�C���^�����o���� */
   }
   if (*cp == '\0') {
      return 0 ;
   }
   return (int)strlen(cp) ;
}

/* cp_line ���� cp_white ��ǂݔ�΂��Acp_delim �̂Ȃ��̂P�������݂���܂�
   �؂�o���� cp_dist �� copy                         */
char* gettoken(char *cp_line,
               char *cp_white,         /* �ǂݔ�΂��L�����N�^�Q */
               char *cp_delim,         /* �f���~�^�L�����N�^�Q */
               char *cp_dist)
{
   int i_tmpquote   = -1 ,               /* ���o�����N�I�[�g�L�����N�^ */
       i_quote      = FALSE ,            /* �N�I�[�g�����ǂ����̃t���O */
       i_replevel   = 0 ,                /* ���ʂ̃l�X�e�B���O���x�� */
       i ;

   while((i = *cp_line) != (int)'\x00') { /* �O�ɂ��Ă��邲�݂��΂� */
      if (strchr(cp_white,i) == NULL) {
         break ;
      }
      cp_line++ ;
   }

   while(((i = *cp_line) != (int)'\x00') && 
         ((strchr(cp_delim,(int)(*cp_line)) == NULL) || 
          (i_replevel > 0) || 
          (i_quote == TRUE))
        ) {

      if (i_quote == FALSE) {
         if (strchr(QUOTES,i) != NULL) { /* �N�I�[�g�𔭌����� */
            i_quote = TRUE ;
            i_tmpquote = i ; /* �N�I�[�g�L�����N�^���L�����Ă��� */
         }
         if (strchr(REPBGN,i) != NULL) { /* ���ʂ𔭌����� */
            i_replevel++ ;
         } else {
            if ((i_replevel > 0) && (strchr(REPEND,i) != NULL)) {
               i_replevel-- ;
            }
         }
      } else {
         if (i_tmpquote == i) { /* �N�I�[�g�̏I���𔭌����� */
            i_quote = FALSE ;
         }
      }

      *cp_dist++ = *cp_line++ ;
   }
   *cp_dist = '\x00' ;

   if (i_replevel > 0) {
      app_error(REP_ERROR,1) ;
   }
   if (i_quote == TRUE) {
      app_error(QUOTE_ERROR,1) ;
   }

   return cp_line ;
}

/* cp_line ���� cp_delim �̂Ȃ��̂P�����ȊO���݂���܂�
   �؂�o���� cp_dist �� copy          ~~~~               */
char* gettoken2(char *cp_line,char *cp_white,char *cp_delim,char *cp_dist)
{
   char *cp ;
   int i ;

   while((i = *cp_line) != (int)'\x00') { /* �O�ɂ��Ă��邲�݂��΂� */
      if (strchr(cp_white,i) == NULL) {
         break ;
      }
      cp_line++ ;
   }

   while((i = *cp_line) != (int)'\x00') { /* NULL �łȂ������� */
      if (strchr(cp_delim,i) == NULL) { /* �f���~�^��Ɋ܂܂�Ă��邩�H */
         *cp_dist = '\x00' ;
         return cp_line ;
      }
      *cp_dist++ = *cp_line++ ;
   }
   *cp_dist = '\x00' ;
   return cp_line ;
}

/* _ �ł͂��܂� APP �\�����̐؂�o�� */
char *get_appresw(char *cp_line,char *ca_resw)
{
   char *cp_pou ,             /* �A���_�[�o�[�̈ʒu */
        *cp_poc ;             /* �R�����g�L���̈ʒu */

   /* �o�b�t�@�N���A */
   ca_resw[0] = '\0' ;

   /* �R�����g�L����T�� */
   cp_poc = app_strchr(cp_line,COMMENT,'\0') ;

   /* �A���_�[�o�[��������Ȃ��� */
   if ((cp_pou = app_strchr(cp_line,UNDERBAR,COMMENT)) == NULL) {
      return NULL ;
   }

   /* �A���_�[�o�[�����������A�R�����g�L������ */
   if ((cp_poc != NULL) && (cp_pou > cp_poc)) {
      return NULL ;
   }

   /* �z���C�g�X�y�[�X���A�R�����g�L���܂Ő؂�o�� */
   return gettoken(cp_pou+1,WHITESPACE,LDELIM,ca_resw) ;
}

/* ������̑O��ɂ��Ă��邲�݂���菜�� */
void cutspcs(char *cp_buf,const char *cp_spcs)
{
   char *cp_save = cp_buf ;
   int  i_strlen = app_strlen(cp_buf) ;

   /* �k��������? */
   if (i_strlen == 0) {
      return ;
   }

   /* �k���̎�O�̈ʒu�� */
   cp_buf += (i_strlen - 1) ;

   /* ���K�����ꂢ�� */
   while (strchr(cp_spcs,*cp_buf) != NULL) {
      cp_buf-- ;
   }
   *(cp_buf+1) = '\0' ;

   /* �������ꂢ�łȂ������炸�炷 */
   cp_buf = cp_save ;
   while ((strchr(cp_spcs,*cp_buf) != NULL) && (*cp_buf != '\0')) {
      cp_buf++ ;
   }

   if (cp_buf != cp_save) {
      while (*cp_buf != '\0') {
         *cp_save++ = *cp_buf++ ;
      }
      *cp_save = '\0' ;
   }
}

/* cp_line ���� ����L�����N�^���܂܂�邩�ǂ�����T��
   �i�N�I�[�g�������݁j*/
char* app_strchr(char *cp_line,int i_char,int i_delim)
{
   int i_tmpquote   = -1 ,               /* ���o�����N�I�[�g�L�����N�^ */
       i_quote      = FALSE ,            /* �N�I�[�g�����ǂ����̃t���O */
       i ;

   while((i = (int)(*cp_line)) != (int)'\x00') {
      if (i_quote == FALSE) {
         if ((i == i_char) || (i == i_delim)) {
             break ;                     /* �ړI�̃L�����N�^�����o */
         }
         if (strchr(QUOTES,i) != NULL) { /* �N�I�[�g�𔭌����� */
            i_quote = TRUE ;
            i_tmpquote = i ; /* �N�I�[�g�L�����N�^���L�����Ă��� */
         }
      } else {
         if (i_tmpquote == i) { /* �N�I�[�g�̏I���𔭌����� */
            i_quote = FALSE ;
         }
      }
      cp_line++ ;
   }

   if (i != i_char) {
      cp_line = NULL ; /* �����ł��Ȃ����� */
   }

   return cp_line ;
}

/* �����񂪐��l���ǂ������ׂ� (�����Ȃ� TRUE ��Ԃ�) */
int app_isnum(char *cp)
{
   int i = app_strlen(cp) ;

   if (i > 0) {
      if (isdigit(cp[0])) { /* �擪������ */
         i-- ;              /* �k���̎�O */
         if (isdigit(cp[i]) || (cp[i] == 'H') || (cp[i] == 'h')
                            || (cp[i] == 'B') || (cp[i] == 'b')) {
            return TRUE ;
         }
      }
   }
   return FALSE ;
}

/* ������e�[�u�����T�[�`���A�G���g���ԍ���Ԃ� */
int app_scantbl(char **cpp_table,char *cp)
{
   int i ;

   for (i = 0 ; cpp_table[i] != NULL ; i++) {
      if (strcmp(cpp_table[i],cp) == 0) {
         return ++i ; /* �������� */
      }
   }
   return 0 ; /* ������Ȃ����� */
}

/* �e�[�u�����̕����񂪊܂܂�邩�ǂ������T�[�`���A�G���g���ԍ���Ԃ�
                                                 �i������J�b�g���j */
int app_scantbl_with_cut(char **cpp_table,char *cp)
{
   int i ;
   char *cp_r ;

   for (i = 0 ; cpp_table[i] != NULL ; i++) {
      if ((cp_r = strstr(cp,cpp_table[i])) != NULL) { /* �����������H */
         char *cp_from = &cp_r[strlen(cpp_table[i])] ;
         while (*cp_from != '\0') { /* �Y�������̃J�b�g */
             *cp_r++ = *cp_from++ ;
         }
         *cp_r = '\0' ;
         return ++i ;
      }
   }
   return 0 ;
}

/* ������ cp_t ���� cp_s1 �ƈ�v���镶����S�Ă� cp_s2 �ɒu������
   �i�A�� cp_t ����n�܂�̈�͏\���傫�����Ɓj */
int str_replace(char *cp_t,char *cp_s1,char *cp_s2)
{
   int i_s1len = app_strlen(cp_s1) ,
       i_s2len = app_strlen(cp_s2) ,
       i_count = 0 ,              /* �u���񐔃J�E���^ */
       i_diff ,
       i ;
   char *cp ;

   i_diff = i_s1len - i_s2len ;
   while((cp = strstr(cp_t,cp_s1)) != NULL){
      /* ���炷 */
      if (i_diff > 0) { /* �Z���Ȃ�ꍇ */
         char *cp_ptr ;
         for (cp_ptr = cp ; cp_ptr[i_s1len] != '\0' ; cp_ptr++) {
            cp_ptr[i_s2len] = cp_ptr[i_s1len] ;
         }
         cp_ptr[i_s2len] = cp_ptr[i_s1len] ; /* NULL CHAR */
      }
      if (i_diff < 0) { /* �����Ȃ�ꍇ */
         char *cp_ptr = &cp_t[app_strlen(cp_t)] ;
         for (cp_ptr = cp_t + app_strlen(cp_t) ; cp_ptr >= cp + i_s1len ;cp_ptr--){
            cp_ptr[-i_diff] = cp_ptr[0] ;
         }
      }
      /* �u�� */
      for (i = 0 ; i < i_s2len ; i++) {
         cp[i] = cp_s2[i] ;
      }
      i_count++ ;
   }
   return i_count ;
}

/* �p�X������h�b�g�̈ʒu��T�� */
char *getdotpos(char *cp)
{
   int i_len = app_strlen(cp) ;

   if ((i_len == 0) || (strchr(cp,'.') == NULL)) {
      return NULL ;
   }
   i_len-- ;
   for(;strchr("./\\",cp[i_len]) == NULL;i_len--) ;
   if (strchr("/\\",cp[i_len]) != NULL) {
      return NULL ;
   }

   return &cp[i_len] ;
}
