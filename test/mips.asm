.data
a: .word 0
b: .word 0
temp1: .word 0
max: .word 0

.text
.globl main
j main

main:
	# ��ȡ������ a
	li $v0, 5
	syscall
	sw $v0, a
	# ��ȡ������ b
	li $v0, 5
	syscall
	sw $v0, b
	# temp1 = (a < b)
	lw $t0, a
	lw $t1, b
	slt $t2, $t0, $t1
	sw $t2, temp1
	# ��� temp1 == 0 ����ת�� label0
	lw $t0, temp1
	beqz $t0, label0
	# max = b
	lw $t0, b
	sw $t0, max
	# ��������ת�� label1
	j label1
label0:
	# max = a
	lw $t0, a
	sw $t0, max
label1:
	# ��� max ��ֵ
	li $v0, 1
	lw $a0, max
	syscall
	li $v0, 11
	li $a0, 10
	syscall
	# �������
	li $v0, 10
	syscall
