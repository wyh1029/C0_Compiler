.data
MAX_NUM: .word 40
String0: .asciiz " x = "
String1: .asciiz " y = "
String2: .asciiz " SWAP x = "
String3: .asciiz " SWAP y = "
String4: .asciiz " OVERFLOW!          "
String5: .asciiz " complete number: "
String6: .asciiz "  "
String7: .asciiz "          "
String8: .asciiz " ---------------------------------------------------------------          "
String9: .asciiz " b = "
String10: .asciiz " ---------------------------------------------------------------          "
String11: .asciiz " "
String12: .asciiz "          "
String13: .asciiz "          The total is "
String14: .asciiz " 10! = "
.text
subi $sp, $sp, 52
j main
factorial:
subi $sp, $sp, 48
addi $s7, $sp, 52
subi $s1, $s7, 4
lw $s2, 0($s1)
li $s1, 1
sub $s0, $s2, $s1
bgtz $s0, label0
li $s0, 1
move $v0, $s0
jr $ra
label0:
subi $s0, $s7, 4
lw $s1, 0($s0)
li $s0, 1
subu $t0, $s1, $s0
subi $sp, $sp, 4
sw $t0, 0($sp)
sw $ra, 44($sp)
sw $s7, 48($sp)
jal factorial
move $sp, $s7
lw $ra, 40($sp)
lw $s7, 44($sp)
move $t0, $v0
subi $s0, $s7, 4
lw $s1, 0($s0)
mul $t1, $s1, $t0
move $v0, $t1
jr $ra
jr $ra
mod:
subi $sp, $sp, 48
addi $s7, $sp, 56
subi $s0, $s7, 4
lw $s1, 0($s0)
subi $s0, $s7, 8
lw $s2, 0($s0)
div $t0, $s1, $s2
subi $s0, $s7, 8
lw $s1, 0($s0)
mul $t0, $t0, $s1
subi $s0, $s7, 4
lw $s1, 0($s0)
subu $t1, $s1, $t0
subi $s0, $s7, 4
sw $t1, 0($s0)
subi $s0, $s7, 4
lw $s1, 0($s0)
move $v0, $s1
jr $ra
jr $ra
swap:
subi $sp, $sp, 52
addi $s7, $sp, 60
la $s0, String0
move $a0, $s0
li $v0, 4
syscall
subi $s0, $s7, 4
lw $s1, 0($s0)
move $a0, $s1
li $v0, 1
syscall
la $s0, String1
move $a0, $s0
li $v0, 4
syscall
subi $s0, $s7, 8
lw $s1, 0($s0)
move $a0, $s1
li $v0, 1
syscall
subi $s0, $s7, 4
lw $s1, 0($s0)
subi $s0, $s7, 12
sw $s1, 0($s0)
subi $s0, $s7, 8
lw $s1, 0($s0)
subi $s0, $s7, 4
sw $s1, 0($s0)
subi $s0, $s7, 12
lw $s1, 0($s0)
subi $s0, $s7, 8
sw $s1, 0($s0)
la $s0, String2
move $a0, $s0
li $v0, 4
syscall
subi $s0, $s7, 4
lw $s1, 0($s0)
move $a0, $s1
li $v0, 1
syscall
la $s0, String3
move $a0, $s0
li $v0, 4
syscall
subi $s0, $s7, 8
lw $s1, 0($s0)
move $a0, $s1
li $v0, 1
syscall
jr $ra
complete_num:
subi $sp, $sp, 612
addi $s7, $sp, 616
subi $s0, $s7, 4
lw $s1, 0($s0)
subi $s0, $s7, 524
sw $s1, 0($s0)
label1:
subi $s1, $s7, 524
lw $s2, 0($s1)
la $s1, MAX_NUM
lw $s3, 0($s1)
sub $s0, $s2, $s3
bgez $s0, label2
li $s0, 0
li $s1, 1
subu $t0, $s0, $s1
subi $s0, $s7, 528
sw $t0, 0($s0)
subi $s0, $s7, 524
lw $s1, 0($s0)
subi $s0, $s7, 532
sw $s1, 0($s0)
li $s0, 1
subi $s1, $s7, 520
sw $s0, 0($s1)
label3:
subi $s1, $s7, 520
lw $s2, 0($s1)
subi $s1, $s7, 524
lw $s3, 0($s1)
sub $s0, $s2, $s3
bgez $s0, label4
subi $s0, $s7, 524
lw $s1, 0($s0)
subi $s0, $s7, 520
lw $s2, 0($s0)
div $t0, $s1, $s2
subi $s0, $s7, 520
lw $s1, 0($s0)
mul $t0, $t0, $s1
subi $s0, $s7, 536
sw $t0, 0($s0)
subi $sp, $sp, 4
subi $s0, $s7, 524
lw $s1, 0($s0)
sw $s1, 0($sp)
subi $sp, $sp, 4
subi $s0, $s7, 520
lw $s1, 0($s0)
sw $s1, 0($sp)
sw $ra, 48($sp)
sw $s7, 52($sp)
jal mod
move $sp, $s7
lw $ra, 40($sp)
lw $s7, 44($sp)
move $t0, $v0
li $s0, 0
bne $t0, $s0, label5
subi $s0, $s7, 528
lw $s1, 0($s0)
li $s0, 1
addu $t0, $s1, $s0
subi $s0, $s7, 528
sw $t0, 0($s0)
subi $s0, $s7, 532
lw $s1, 0($s0)
subi $s0, $s7, 520
lw $s2, 0($s0)
subu $t0, $s1, $s2
subi $s0, $s7, 532
sw $t0, 0($s0)
subi $s1, $s7, 528
lw $s2, 0($s1)
li $s1, 128
sub $s0, $s2, $s1
bltz $s0, label6
la $s0, String4
move $a0, $s0
li $v0, 4
syscall
label6:
subi $s1, $s7, 528
lw $s2, 0($s1)
li $s1, 128
sub $s0, $s2, $s1
bgez $s0, label7
subi $s0, $s7, 520
lw $s1, 0($s0)
subi $s0, $s7, 528
lw $s2, 0($s0)
subi $s0, $s7, 516
sll $s3, $s2, 2
add $s0, $s0, $s3
sw $s1, 0($s0)
label7:
label5:
subi $s0, $s7, 520
lw $s1, 0($s0)
li $s0, 1
addu $t0, $s1, $s0
subi $s0, $s7, 520
sw $t0, 0($s0)
j label3
label4:
subi $s0, $s7, 532
lw $s1, 0($s0)
li $s0, 0
bne $s1, $s0, label8
la $s0, String5
move $a0, $s0
li $v0, 4
syscall
subi $s0, $s7, 524
lw $s1, 0($s0)
move $a0, $s1
li $v0, 1
syscall
li $s0, 0
subi $s1, $s7, 520
sw $s0, 0($s1)
label9:
subi $s1, $s7, 520
lw $s2, 0($s1)
subi $s1, $s7, 528
lw $s3, 0($s1)
sub $s0, $s2, $s3
bgtz $s0, label10
la $s0, String6
move $a0, $s0
li $v0, 4
syscall
subi $s0, $s7, 520
lw $s1, 0($s0)
subi $s0, $s7, 516
sll $s2, $s1, 2
add $s0, $s0, $s2
lw $s1, 0($s0)
move $t0, $s1
move $a0, $t0
li $v0, 1
syscall
subi $s0, $s7, 520
lw $s1, 0($s0)
li $s0, 1
addu $t0, $s1, $s0
subi $s0, $s7, 520
sw $t0, 0($s0)
j label9
label10:
la $s0, String7
move $a0, $s0
li $v0, 4
syscall
label8:
subi $s0, $s7, 524
lw $s1, 0($s0)
li $s0, 1
addu $t0, $s1, $s0
subi $s0, $s7, 524
sw $t0, 0($s0)
j label1
label2:
la $s0, String8
move $a0, $s0
li $v0, 4
syscall
li $s0, 0
subi $s1, $s7, 560
sw $s0, 0($s1)
li $s0, 0
subi $s1, $s7, 520
sw $s0, 0($s1)
label11:
subi $s1, $s7, 520
lw $s2, 0($s1)
la $s1, MAX_NUM
lw $s3, 0($s1)
sub $s0, $s2, $s3
bgez $s0, label12
subi $s0, $s7, 560
lw $s1, 0($s0)
subi $s0, $s7, 520
lw $s2, 0($s0)
mul $t0, $s1, $s2
subi $s0, $s7, 564
sw $t0, 0($s0)
subi $s0, $s7, 524
lw $s1, 0($s0)
subi $s0, $s7, 528
lw $s2, 0($s0)
mul $t0, $s1, $s2
subi $s0, $s7, 568
sw $t0, 0($s0)
subi $s0, $s7, 560
lw $s1, 0($s0)
subi $s0, $s7, 520
lw $s2, 0($s0)
mul $t0, $s1, $s2
subi $s0, $s7, 560
sw $t0, 0($s0)
subi $s0, $s7, 560
lw $s1, 0($s0)
subi $s0, $s7, 564
lw $s2, 0($s0)
addu $t0, $s1, $s2
subi $s0, $s7, 568
lw $s1, 0($s0)
addu $t0, $t0, $s1
subi $s0, $s7, 560
sw $t0, 0($s0)
subi $s0, $s7, 520
lw $s1, 0($s0)
li $s0, 1
addu $t0, $s1, $s0
subi $s0, $s7, 520
sw $t0, 0($s0)
j label11
label12:
subi $s0, $s7, 560
lw $s1, 0($s0)
subi $s0, $s7, 564
sw $s1, 0($s0)
li $s0, 0
subi $s1, $s7, 520
sw $s0, 0($s1)
label13:
subi $s1, $s7, 520
lw $s2, 0($s1)
la $s1, MAX_NUM
lw $s3, 0($s1)
sub $s0, $s2, $s3
bgez $s0, label14
subi $s0, $s7, 564
lw $s1, 0($s0)
subi $s0, $s7, 520
lw $s2, 0($s0)
addu $t0, $s1, $s2
subi $s0, $s7, 564
sw $t0, 0($s0)
subi $s0, $s7, 520
lw $s1, 0($s0)
li $s0, 1
addu $t0, $s1, $s0
subi $s0, $s7, 520
sw $t0, 0($s0)
j label13
label14:
la $s0, String9
move $a0, $s0
li $v0, 4
syscall
subi $s0, $s7, 564
lw $s1, 0($s0)
move $a0, $s1
li $v0, 1
syscall
la $s0, String10
move $a0, $s0
li $v0, 4
syscall
li $s0, 0
subi $s1, $s7, 548
sw $s0, 0($s1)
li $s0, 1
subi $s1, $s7, 552
sw $s0, 0($s1)
li $s0, 2
subi $s1, $s7, 540
sw $s0, 0($s1)
label15:
subi $s1, $s7, 540
lw $s2, 0($s1)
la $s1, MAX_NUM
lw $s3, 0($s1)
sub $s0, $s2, $s3
bgtz $s0, label16
subi $s0, $s7, 540
lw $s1, 0($s0)
li $s0, 2
div $t0, $s1, $s0
subi $s0, $s7, 544
sw $t0, 0($s0)
li $s0, 2
subi $s1, $s7, 520
sw $s0, 0($s1)
label17:
subi $s1, $s7, 520
lw $s2, 0($s1)
subi $s1, $s7, 544
lw $s3, 0($s1)
sub $s0, $s2, $s3
bgtz $s0, label18
subi $s0, $s7, 540
lw $s1, 0($s0)
subi $s0, $s7, 520
lw $s2, 0($s0)
div $t0, $s1, $s2
subi $s0, $s7, 520
lw $s1, 0($s0)
mul $t0, $t0, $s1
subi $s0, $s7, 556
sw $t0, 0($s0)
subi $sp, $sp, 4
subi $s0, $s7, 540
lw $s1, 0($s0)
sw $s1, 0($sp)
subi $sp, $sp, 4
subi $s0, $s7, 520
lw $s1, 0($s0)
sw $s1, 0($sp)
sw $ra, 48($sp)
sw $s7, 52($sp)
jal mod
move $sp, $s7
lw $ra, 40($sp)
lw $s7, 44($sp)
move $t0, $v0
li $s0, 0
bne $t0, $s0, label19
li $s0, 0
subi $s1, $s7, 552
sw $s0, 0($s1)
label19:
subi $s0, $s7, 520
lw $s1, 0($s0)
li $s0, 1
addu $t0, $s1, $s0
subi $s0, $s7, 520
sw $t0, 0($s0)
j label17
label18:
subi $s0, $s7, 552
lw $s1, 0($s0)
li $s0, 1
bne $s1, $s0, label20
la $s0, String11
move $a0, $s0
li $v0, 4
syscall
subi $s0, $s7, 540
lw $s1, 0($s0)
move $a0, $s1
li $v0, 1
syscall
subi $s0, $s7, 548
lw $s1, 0($s0)
li $s0, 1
addu $t0, $s1, $s0
subi $s0, $s7, 548
sw $t0, 0($s0)
subi $s0, $s7, 548
lw $s1, 0($s0)
li $s0, 10
div $t0, $s1, $s0
li $s0, 10
mul $t0, $t0, $s0
subi $s0, $s7, 556
sw $t0, 0($s0)
subi $s0, $s7, 556
lw $s1, 0($s0)
subi $s0, $s7, 548
lw $s2, 0($s0)
bne $s1, $s2, label21
la $s0, String12
move $a0, $s0
li $v0, 4
syscall
label21:
label20:
li $s0, 1
subi $s1, $s7, 552
sw $s0, 0($s1)
subi $s0, $s7, 540
lw $s1, 0($s0)
li $s0, 1
addu $t0, $s1, $s0
subi $s0, $s7, 540
sw $t0, 0($s0)
j label15
label16:
la $s0, String13
move $a0, $s0
li $v0, 4
syscall
subi $s0, $s7, 548
lw $s1, 0($s0)
move $a0, $s1
li $v0, 1
syscall
jr $ra
main:
subi $sp, $sp, 52
addi $s7, $sp, 52
subi $sp, $sp, 4
li $s0, 10
sw $s0, 0($sp)
sw $ra, 44($sp)
sw $s7, 48($sp)
jal factorial
move $sp, $s7
lw $ra, 40($sp)
lw $s7, 44($sp)
move $t0, $v0
subi $s0, $s7, 4
sw $t0, 0($s0)
la $s0, String14
move $a0, $s0
li $v0, 4
syscall
subi $s0, $s7, 4
lw $s1, 0($s0)
move $a0, $s1
li $v0, 1
syscall
subi $sp, $sp, 4
li $s0, 5
sw $s0, 0($sp)
subi $sp, $sp, 4
li $s0, 10
sw $s0, 0($sp)
sw $ra, 48($sp)
sw $s7, 52($sp)
jal swap
move $sp, $s7
lw $ra, 40($sp)
lw $s7, 44($sp)
subi $sp, $sp, 4
li $s0, 2
sw $s0, 0($sp)
sw $ra, 44($sp)
sw $s7, 48($sp)
jal complete_num
move $sp, $s7
lw $ra, 40($sp)
lw $s7, 44($sp)
jr $ra
