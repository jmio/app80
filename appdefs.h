/*
                         Generic APP Source File

                          Filename : APPDEFS.H
                          Version  : 95/03/01
                            Written by Mio

                      Generic APP �萔��`�t�@�C��
*/

#ifndef APPDEFS_H
#define APPDEFS_H

/* �p�����[�^ */

#define RESWBUFSIZ 200          /* �\���X�L�����p�o�b�t�@�T�C�Y */
#define PNLENGTH   128          /* �p�X���̍ő咷 */
#define LBLBFLEN   40           /* ���x�������p�o�b�t�@�T�C�Y */

/* �萔 */

#define OFF         0
#define ON          (!OFF)

#define FALSE       0
#define TRUE        (!FALSE)

#ifdef __MSDOS__
# define PATH_SEPL  '\\'
#else
# define PATH_SEPL  '/'
#endif

/* �e���|�����E���x���̓� */
#define TMPL_HEAD   "$_"	

/* ���ꏈ��������i�����̌�ɓ��e�������j
   �R�}���h���C���E�I�v�V�����L�����N�^ */
#define INCLUDE_OPTION 'I'
#define DEFINE_OPTION  'D'
#define HELP_OPTION    '?'

typedef unsigned char BYTE ;

#endif /* of APPDEFS_H */
