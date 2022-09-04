/*
                         Generic APP Source File

                          Filename : PROTO.H
                          Version  : 95/03/01
                            Written by Mio

                    �R�[�h�����֐����ʃw�b�_�t�@�C��
*/

/* �����W�����v�o�� */
void jcc_output(char *cp_s,char *cp_n,char *cp_label,int i_num) ;

/* cp_1 �� cp_2 �� ��r������Aequal �����ŃW�����v����R�[�h */
void cond_equal(char *cp_1,char *cp_2,char *cp_label,int i_num) ;

/* cp_1 �� cp_2 �� ��r������Anot equal �����ŃW�����v����R�[�h */
void cond_not_e(char *cp_1,char *cp_2,char *cp_label,int i_num) ;

/* cp_1 �� cp_2 �� ��r������Abelow �����ŃW�����v����R�[�h */
void cond_below(char *cp_1,char *cp_2,char *cp_label,int i_num) ;

/* cp_1 �� cp_2 �� ��r������Abelow or equal �����ŃW�����v����R�[�h */
void cond_b_or_e(char *cp_1,char *cp_2,char *cp_label,int i_num) ;

/* cp_1 �� cp_2 �� ��r������Aabove �����ŃW�����v����R�[�h */
void cond_above(char *cp_1,char *cp_2,char *cp_label,int i_num) ;

/* cp_1 �� cp_2 �� ��r������Aabove or equal �����ŃW�����v����R�[�h */
void cond_a_or_e(char *cp_1,char *cp_2,char *cp_label,int i_num) ;

/* cp_1 �� cp_2 �� test ������Azero �����ŃW�����v����R�[�h */
void cond_test(char *cp_1,char *cp_2,char *cp_label,int i_num) ;

/* cp_1 �� cp_2 �� test ������Anot zero �����ŃW�����v����R�[�h */
void cond_not_t(char *cp_1,char *cp_2,char *cp_label,int i_num) ;

/* cp_1 �� cp_2 �� and ������Azero �����ŃW�����v����R�[�h */
void cond_and(char *cp_1,char *cp_2,char *cp_label,int i_num) ;

/* cp_1 �� cp_2 �� and ������Anot zero �����ŃW�����v����R�[�h */
void cond_not_a(char *cp_1,char *cp_2,char *cp_label,int i_num) ;

/* �t���O�ɂ��W�����v */
void cond_flags(char *cp_1,int i_tf,char *cp_label,int i_num) ;

/* ���x���̐��� */
void label_output(char *cp) ;

/* MOV �̏o�́i�]���O�j */
void mov_output(char *cp1,char *cp2) ;

/* LEA �̏o�́i�]���O�j */
void lea_output(char *cp1,char *cp2) ;

/* �֐��̖߂�l��� MOV �o�́i�]���O�j */
void retval_output(char *cp1) ;

/* CMP �̏o�́i�]���O�j */
void cmp_output(char *cp1,char *cp2) ;

/* AND �̏o�́i�]���O�j */
void and_output(char *cp1,char *cp2) ;

/* TEST �̏o�́i�]���O�j */
void test_output(char *cp1,char *cp2) ;

/* ������ JMP �̏o�� */
void jmp_output(char *cp) ;

/* ������ JMP �̏o�́iSHORT�j */
void short_jmp_output(char *cp) ;

/* JCXZ �̏o�� */
void jcxz_output(char *cp) ;

/* LOOP �̏o�� */
void loop_output(char *cp) ;

/* �T�u���[�`�� CALL �̏o�� */
void call_output(char *cp) ;

/* �T�u���[�`�� RET �̏o�� */
void ret_output(void) ;

/* ���荞�� IRET �̏o�� */
void iret_output(void) ;

/* �ϐ��̕ۑ�(PUSH)�o�� */
void push_output(char *cp) ;

/* �ϐ��̕ۑ�(POP)�o�� */
void pop_output(char *cp) ;

/* �ϐ��̃C���N�������g�o�́i�]�����݁j */
void inc_output(char *cp) ;

/* �ϐ��̃f�N�������g�o�́i�]�����݁j */
void dec_output(char *cp) ;

/* PUBLIC �錾 */
void public_output(char *cp) ;

/* EXTERNAL �錾 */
void extern_output(char *cp) ;

/* 80 ���� LOOP */
void loop80_output(char *cp1,char *cpl,int i_num) ;
