/*
                         Generic APP Source File

                          Filename : APPSUB.H
                          Version  : 95/03/01
                            Written by Mio

                    APP �]�����[�`���p�T�u���[�`���Q
*/

#ifndef APPSUB_H
#define APPSUB_H

/* include_file �̃��C���o�b�t�@�T�C�Y */
#define LINEBUFSIZ 256           /* �s�ǂݎ��p�o�b�t�@�T�C�Y */

/* gettoken �ŗp����萔 */
#define WHITESPACE        " \t"         /* �ǂݔ�΂��L�����N�^�Q */
#define LDELIM            " \t\n;"      /* �f�t�H���g�s�f���~�^ */

#define QUOTES            "\"\'"        /* �N�I�[�g */
#define REPBGN            "("           /* ���� */
#define REPEND            ")"           /* ���ʕ� */

/* ����L�����N�^ */
#define UNDERBAR          '_'
#define COMMENT           ';'

#define cutspc(x)         cutspcs((x)," \t\n")

#include "appsub.prt"                   /* �֐��v���g�^�C�v */

#endif /* of APPSUB_H */
