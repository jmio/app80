/*
                         Generic APP Source File

                          Filename : MISCLOOP.H
                          Version  : 95/03/01
                            Written by Mio

                            �e�탋�[�v����
*/

#ifndef MISCLOOP_H
#define MISCLOOP_H

#define LPNESTMAX         100          /* loop �̍ő�l�X�e�B���O���x�� */

#define TMPL_BEGIN        ".BEGIN"
#define TMPL_CONTINUE     ".CONT"
#define TMPL_EXIT         ".EXIT"
#define TMPL_FORBEGIN     ".FORBG"

extern int i_loopnum ;              /* loop ���x���E�J�E���^ */
extern int i_looplev ;              /* loop �u���b�N�l�X�e�B���O���x�� */
extern char *cp_lcondstk[] ,        /* loop �����X�^�b�N1 */
            *cp_lcondstk2[] ;       /* loop �����X�^�b�N2 (for) */
extern int ia_loopn[] ,             /* loop ���x���ԍ��X�^�b�N */
           ia_contf[] ,             /* continue �t���O�X�^�b�N */
           ia_exitf[] ;             /* exit �t���O�X�^�b�N */

#include "miscloop.prt"

#endif /* of MISCLOOP_H */
