/*
                         Generic APP Source File

                          Filename : APPMAIN.H
                          Version  : 95/03/01
                            Written by Mio

                    Generic APP ���C�����[�`���t�@�C��
*/

#ifndef APPMAIN_H
#define APPMAIN_H

#define MAXFNAMES 2            /* �R�}���h���C���Ŏw��ł���t�@�C���l�[���� */
                               /* �����ύX���鎞�� main �� app_getopt ��
                                  �킽���߂�l�o�b�t�@�z��𑝂₷���Ƃ��K�v */

#define MAXPARAMS 100          /* �t�@�C�����ȊO�̍ő�p�����[�^�� */

/* �b�v���g�^�C�v���[�J�Ƃ̋��p�I�v�V���� */
#if !defined(CPROTO) && !defined(MKDEP)
# include "version.h"
# ifdef APP86
#  define DEFAULT_IN_EXT               ".app"
#  define DEFAULT_OUT_EXT              ".asm"
#  define DEFAULT_HD_EXT               ".h"
# else
#  define DEFAULT_IN_EXT               ".app"
#  define DEFAULT_OUT_EXT              ".asm"
#  define DEFAULT_HD_EXT               ".h"
# endif
#endif

#ifdef CPROTO
# include "cprtver.h"
# define DEFAULT_IN_EXT               ".c"
# define DEFAULT_OUT_EXT              ".prt"
# define DEFAULT_HD_EXT               ".prt"   /* DUMMY */
#endif

#ifdef MKDEP
# include "mkdepver.h"
# define DEFAULT_IN_EXT               ".c"
# define DEFAULT_OUT_EXT              ".dep"
# define DEFAULT_HD_EXT               ".dep"   /* DUMMY */
#endif

/* �O���[�o���ϐ� */
extern FILE *fp_out ;                  /* �o�̓t�@�C�� */
extern int ia_option[] ;               /* �I�v�V���� on/off �t���O */

#include "appmain.prt"                 /* �֐��v���g�^�C�v */

#endif /* of APPMAIN_H */
