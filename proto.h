/*
                         Generic APP Source File

                          Filename : PROTO.H
                          Version  : 95/03/01
                            Written by Mio

                    コード生成関数共通ヘッダファイル
*/

/* 条件ジャンプ出力 */
void jcc_output(char *cp_s,char *cp_n,char *cp_label,int i_num) ;

/* cp_1 と cp_2 を 比較した後、equal 条件でジャンプするコード */
void cond_equal(char *cp_1,char *cp_2,char *cp_label,int i_num) ;

/* cp_1 と cp_2 を 比較した後、not equal 条件でジャンプするコード */
void cond_not_e(char *cp_1,char *cp_2,char *cp_label,int i_num) ;

/* cp_1 と cp_2 を 比較した後、below 条件でジャンプするコード */
void cond_below(char *cp_1,char *cp_2,char *cp_label,int i_num) ;

/* cp_1 と cp_2 を 比較した後、below or equal 条件でジャンプするコード */
void cond_b_or_e(char *cp_1,char *cp_2,char *cp_label,int i_num) ;

/* cp_1 と cp_2 を 比較した後、above 条件でジャンプするコード */
void cond_above(char *cp_1,char *cp_2,char *cp_label,int i_num) ;

/* cp_1 と cp_2 を 比較した後、above or equal 条件でジャンプするコード */
void cond_a_or_e(char *cp_1,char *cp_2,char *cp_label,int i_num) ;

/* cp_1 と cp_2 を test した後、zero 条件でジャンプするコード */
void cond_test(char *cp_1,char *cp_2,char *cp_label,int i_num) ;

/* cp_1 と cp_2 を test した後、not zero 条件でジャンプするコード */
void cond_not_t(char *cp_1,char *cp_2,char *cp_label,int i_num) ;

/* cp_1 と cp_2 を and した後、zero 条件でジャンプするコード */
void cond_and(char *cp_1,char *cp_2,char *cp_label,int i_num) ;

/* cp_1 と cp_2 を and した後、not zero 条件でジャンプするコード */
void cond_not_a(char *cp_1,char *cp_2,char *cp_label,int i_num) ;

/* フラグによるジャンプ */
void cond_flags(char *cp_1,int i_tf,char *cp_label,int i_num) ;

/* ラベルの生成 */
void label_output(char *cp) ;

/* MOV の出力（評価前） */
void mov_output(char *cp1,char *cp2) ;

/* LEA の出力（評価前） */
void lea_output(char *cp1,char *cp2) ;

/* 関数の戻り値代入 MOV 出力（評価前） */
void retval_output(char *cp1) ;

/* CMP の出力（評価前） */
void cmp_output(char *cp1,char *cp2) ;

/* AND の出力（評価前） */
void and_output(char *cp1,char *cp2) ;

/* TEST の出力（評価前） */
void test_output(char *cp1,char *cp2) ;

/* 無条件 JMP の出力 */
void jmp_output(char *cp) ;

/* 無条件 JMP の出力（SHORT） */
void short_jmp_output(char *cp) ;

/* JCXZ の出力 */
void jcxz_output(char *cp) ;

/* LOOP の出力 */
void loop_output(char *cp) ;

/* サブルーチン CALL の出力 */
void call_output(char *cp) ;

/* サブルーチン RET の出力 */
void ret_output(void) ;

/* 割り込み IRET の出力 */
void iret_output(void) ;

/* 変数の保存(PUSH)出力 */
void push_output(char *cp) ;

/* 変数の保存(POP)出力 */
void pop_output(char *cp) ;

/* 変数のインクリメント出力（評価ずみ） */
void inc_output(char *cp) ;

/* 変数のデクリメント出力（評価ずみ） */
void dec_output(char *cp) ;

/* PUBLIC 宣言 */
void public_output(char *cp) ;

/* EXTERNAL 宣言 */
void extern_output(char *cp) ;

/* 80 向け LOOP */
void loop80_output(char *cp1,char *cpl,int i_num) ;
