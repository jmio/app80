/*
                         Generic APP Source File

                          Filename : FUNCTION.H
                          Version  : 95/03/09
                            Written by Mio

                              �֐��̏���
*/

#ifndef FUNCTION_H
#define FUNCTION_H

#define MAXFNLEN   50               /* �֐����̍ő�̒��� */
#define MAXARGS    10               /* �֐��̈������̍ő� */
#define MAXARGLEN  256              /* �֐��ʂ̈������̒��� */

#define FUNDELIMS    ":(;\n"        /* �֐����̃f���~�^ */
#define ARGDELIMS    ")"            /* �����u���b�N�̃f���~�^ */
#define ARGCUTDELIM  ","            /* �����̋�؂� */
#define REGDELIMS    ";\n"          /* �ۑ��ϐ��u���b�N�̃f���~�^ */

/* �w�b�_�����I�v�V�����Ŏg�p�����w�b�_������ */
#define OLD_FUNC_H  "@_function"
#define OLD_PROC_H  "@_procedure"

#define NEW_FUNC_H  "_function_h"
#define NEW_PROC_H  "_procedure_h"

#define LABEL_FEND  "%s_END"        /* �֐��o���̃��x���̏��������� */

#define PUSH_CHAR            '^'    /* �֐��`���̕ϐ��ۑ��w�� */
#define LEA_CHAR             '*'    /* �֐��`���̕ϐ� LEA �n���w�� */

#include "function.prt"

#endif /* of FUNCTION_H */
