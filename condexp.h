/*
                         Generic APP Source File

                          Filename : CONDEXP.H
                          Version  : 95/03/01
                            Written by Mio

                              ����������
*/

#ifndef CONDEXP_H
#define CONDEXP_H

#define ARGSTRMAX 80            /* ���������ӂ̍ő咷 */

/* ���̗v�f��؂�o�����̃f���~�^ */
#define ARG1_DELIM             "=<>|&!~;\n"
#define COND_UDELIM            "=<>|&!~"
#define ARG2_DELIM             "&|;\n"
#define REST_DELIM             ";\n"
#define FLAG_DELIM             " ;\t\n"

/* �_�����Z�q */
#define OP_LOR                 "||"
#define OP_LAND                "&&"

/* ��r���Z�q */
#define OP_EQUAL               "=="
#define OP_EQUAL2              "="
#define OP_NOT_E               "!="
#define OP_NOT_E2              "<>"
#define OP_ABOVE               ">"
#define OP_BELOW               "<"
#define OP_A_OR_E              ">="
#define OP_B_OR_E              "<="
#define OP_TEST                "&"
#define OP_NOT_T               "~&"
#define OP_NOT_T2              "&~"
#define OP_AND                 "&="
#define OP_NOT_A               "~&="
#define OP_LET                 "="

/* �e���|�����E���x�� */
#define TMPL_COND    ".CTMP"

#include "condexp.prt"          /* �֐��v���g�^�C�v */

#endif /* of IFCOND_H */
