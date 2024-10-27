	.file	"test.c"
	.text
	.globl	plus
	.type	plus, @function
plus:
	addi	sp, sp, -12
	sw	ra, 8(sp)
	sw	fp, 4(sp)
	addi	fp, sp, 12
	sw	a0, -12(fp)
	lw	t0, -12(fp)
	mv	a0, t0
	j	.Lplus_exit
.Lplus_exit:
	lw	ra, 8(sp)
	lw	fp, 4(sp)
	addi	sp, sp, 12
	ret
	.globl	minus
	.type	minus, @function
minus:
	addi	sp, sp, -12
	sw	ra, 8(sp)
	sw	fp, 4(sp)
	addi	fp, sp, 12
	sw	a0, -12(fp)
	lw	t0, -12(fp)
	sub	t0, zero, t0
	mv	a0, t0
	j	.Lminus_exit
.Lminus_exit:
	lw	ra, 8(sp)
	lw	fp, 4(sp)
	addi	sp, sp, 12
	ret
	.globl	add
	.type	add, @function
add:
	addi	sp, sp, -16
	sw	ra, 12(sp)
	sw	fp, 8(sp)
	addi	fp, sp, 16
	sw	a0, -16(fp)
	sw	a1, -12(fp)
	lw	t0, -16(fp)
	lw	t1, -12(fp)
	add	t2, t0, t1
	mv	a0, t2
	j	.Ladd_exit
.Ladd_exit:
	lw	ra, 12(sp)
	lw	fp, 8(sp)
	addi	sp, sp, 16
	ret
	.globl	sub
	.type	sub, @function
sub:
	addi	sp, sp, -16
	sw	ra, 12(sp)
	sw	fp, 8(sp)
	addi	fp, sp, 16
	sw	a0, -16(fp)
	sw	a1, -12(fp)
	lw	t0, -16(fp)
	lw	t1, -12(fp)
	sub	t2, t0, t1
	mv	a0, t2
	j	.Lsub_exit
.Lsub_exit:
	lw	ra, 12(sp)
	lw	fp, 8(sp)
	addi	sp, sp, 16
	ret
	.globl	mul
	.type	mul, @function
mul:
	addi	sp, sp, -16
	sw	ra, 12(sp)
	sw	fp, 8(sp)
	addi	fp, sp, 16
	sw	a0, -16(fp)
	sw	a1, -12(fp)
	lw	t0, -16(fp)
	lw	t1, -12(fp)
	mul	t2, t0, t1
	mv	a0, t2
	j	.Lmul_exit
.Lmul_exit:
	lw	ra, 12(sp)
	lw	fp, 8(sp)
	addi	sp, sp, 16
	ret
	.globl	div
	.type	div, @function
div:
	addi	sp, sp, -16
	sw	ra, 12(sp)
	sw	fp, 8(sp)
	addi	fp, sp, 16
	sw	a0, -16(fp)
	sw	a1, -12(fp)
	lw	t0, -16(fp)
	lw	t1, -12(fp)
	div	t2, t0, t1
	mv	a0, t2
	j	.Ldiv_exit
.Ldiv_exit:
	lw	ra, 12(sp)
	lw	fp, 8(sp)
	addi	sp, sp, 16
	ret
	.globl	or
	.type	or, @function
or:
	addi	sp, sp, -16
	sw	ra, 12(sp)
	sw	fp, 8(sp)
	addi	fp, sp, 16
	sw	a0, -16(fp)
	sw	a1, -12(fp)
	lw	t0, -16(fp)
	lw	t1, -12(fp)
	li	t2, 1
	bne	t0, zero, .L2
.L1:
	sltu	t2, zero, t1
.L2:
	andi	t2, t2, 1
	mv	a0, t2
	j	.Lor_exit
.Lor_exit:
	lw	ra, 12(sp)
	lw	fp, 8(sp)
	addi	sp, sp, 16
	ret
	.globl	and
	.type	and, @function
and:
	addi	sp, sp, -16
	sw	ra, 12(sp)
	sw	fp, 8(sp)
	addi	fp, sp, 16
	sw	a0, -16(fp)
	sw	a1, -12(fp)
	lw	t0, -16(fp)
	lw	t1, -12(fp)
	li	t2, 0
	beq	t0, zero, .L4
.L3:
	sltu	t2, zero, t1
.L4:
	andi	t2, t2, 1
	mv	a0, t2
	j	.Land_exit
.Land_exit:
	lw	ra, 12(sp)
	lw	fp, 8(sp)
	addi	sp, sp, 16
	ret
	.globl	eq
	.type	eq, @function
eq:
	addi	sp, sp, -16
	sw	ra, 12(sp)
	sw	fp, 8(sp)
	addi	fp, sp, 16
	sw	a0, -16(fp)
	sw	a1, -12(fp)
	lw	t0, -16(fp)
	lw	t1, -12(fp)
	xor	t2, t0, t1
	sltiu	t2, t2, 1
	mv	a0, t2
	j	.Leq_exit
.Leq_exit:
	lw	ra, 12(sp)
	lw	fp, 8(sp)
	addi	sp, sp, 16
	ret
	.globl	ne
	.type	ne, @function
ne:
	addi	sp, sp, -16
	sw	ra, 12(sp)
	sw	fp, 8(sp)
	addi	fp, sp, 16
	sw	a0, -16(fp)
	sw	a1, -12(fp)
	lw	t0, -16(fp)
	lw	t1, -12(fp)
	xor	t2, t0, t1
	sltu	t2, zero, t2
	mv	a0, t2
	j	.Lne_exit
.Lne_exit:
	lw	ra, 12(sp)
	lw	fp, 8(sp)
	addi	sp, sp, 16
	ret
	.globl	lt
	.type	lt, @function
lt:
	addi	sp, sp, -16
	sw	ra, 12(sp)
	sw	fp, 8(sp)
	addi	fp, sp, 16
	sw	a0, -16(fp)
	sw	a1, -12(fp)
	lw	t0, -16(fp)
	lw	t1, -12(fp)
	slt	t2, t0, t1
	mv	a0, t2
	j	.Llt_exit
.Llt_exit:
	lw	ra, 12(sp)
	lw	fp, 8(sp)
	addi	sp, sp, 16
	ret
	.globl	gt
	.type	gt, @function
gt:
	addi	sp, sp, -16
	sw	ra, 12(sp)
	sw	fp, 8(sp)
	addi	fp, sp, 16
	sw	a0, -16(fp)
	sw	a1, -12(fp)
	lw	t0, -16(fp)
	lw	t1, -12(fp)
	slt	t2, t1, t0
	mv	a0, t2
	j	.Lgt_exit
.Lgt_exit:
	lw	ra, 12(sp)
	lw	fp, 8(sp)
	addi	sp, sp, 16
	ret
	.globl	le
	.type	le, @function
le:
	addi	sp, sp, -16
	sw	ra, 12(sp)
	sw	fp, 8(sp)
	addi	fp, sp, 16
	sw	a0, -16(fp)
	sw	a1, -12(fp)
	lw	t0, -16(fp)
	lw	t1, -12(fp)
	slt	t2, t1, t0
	xori	t2, t2, 1
	mv	a0, t2
	j	.Lle_exit
.Lle_exit:
	lw	ra, 12(sp)
	lw	fp, 8(sp)
	addi	sp, sp, 16
	ret
	.globl	ge
	.type	ge, @function
ge:
	addi	sp, sp, -16
	sw	ra, 12(sp)
	sw	fp, 8(sp)
	addi	fp, sp, 16
	sw	a0, -16(fp)
	sw	a1, -12(fp)
	lw	t0, -16(fp)
	lw	t1, -12(fp)
	slt	t2, t0, t1
	xori	t2, t2, 1
	mv	a0, t2
	j	.Lge_exit
.Lge_exit:
	lw	ra, 12(sp)
	lw	fp, 8(sp)
	addi	sp, sp, 16
	ret
	.globl	if_cond
	.type	if_cond, @function
if_cond:
	addi	sp, sp, -20
	sw	ra, 16(sp)
	sw	fp, 12(sp)
	addi	fp, sp, 20
	sw	a0, -20(fp)
	sw	a1, -16(fp)
	sw	a2, -12(fp)
	lw	t0, -20(fp)
	beq	t0, zero, .L5
	lw	t1, -16(fp)
	mv	a0, t1
	j	.Lif_cond_exit
	j	.L6
.L5:
	lw	t1, -12(fp)
	mv	a0, t1
	j	.Lif_cond_exit
.L6:
.Lif_cond_exit:
	lw	ra, 16(sp)
	lw	fp, 12(sp)
	addi	sp, sp, 20
	ret
	.globl	while_sum
	.type	while_sum, @function
while_sum:
	addi	sp, sp, -20
	sw	ra, 16(sp)
	sw	fp, 12(sp)
	addi	fp, sp, 20
	sw	a0, -20(fp)
	li	t0, 0
	sw	t0, -16(fp)
	li	t0, 1
	sw	t0, -12(fp)
.L7:
	lw	t0, -12(fp)
	lw	t1, -20(fp)
	slt	t2, t1, t0
	xori	t2, t2, 1
	beq	t2, zero, .L8
	lw	t0, -16(fp)
	lw	t1, -12(fp)
	add	t3, t0, t1
	sw	t3, -16(fp)
	lw	t0, -12(fp)
	li	t1, 1
	add	t3, t0, t1
	sw	t3, -12(fp)
	j	.L7
.L8:
	lw	t0, -16(fp)
	mv	a0, t0
	j	.Lwhile_sum_exit
.Lwhile_sum_exit:
	lw	ra, 16(sp)
	lw	fp, 12(sp)
	addi	sp, sp, 20
	ret
	.globl	for_sum
	.type	for_sum, @function
for_sum:
	addi	sp, sp, -20
	sw	ra, 16(sp)
	sw	fp, 12(sp)
	addi	fp, sp, 20
	sw	a0, -20(fp)
	li	t0, 0
	sw	t0, -16(fp)
	li	t0, 1
	sw	t0, -12(fp)
.L9:
	lw	t0, -12(fp)
	lw	t1, -20(fp)
	slt	t2, t1, t0
	xori	t2, t2, 1
	beq	t2, zero, .L10
	lw	t0, -16(fp)
	lw	t1, -12(fp)
	add	t3, t0, t1
	sw	t3, -16(fp)
	lw	t0, -12(fp)
	li	t1, 1
	add	t3, t0, t1
	sw	t3, -12(fp)
	j	.L9
.L10:
	lw	t0, -16(fp)
	mv	a0, t0
	j	.Lfor_sum_exit
.Lfor_sum_exit:
	lw	ra, 16(sp)
	lw	fp, 12(sp)
	addi	sp, sp, 20
	ret
	.globl	increase_one
	.type	increase_one, @function
increase_one:
	addi	sp, sp, -12
	sw	ra, 8(sp)
	sw	fp, 4(sp)
	addi	fp, sp, 12
	sw	a0, -12(fp)
	lw	t0, -12(fp)
	li	t1, 1
	add	t2, t0, t1
	mv	a0, t2
	j	.Lincrease_one_exit
.Lincrease_one_exit:
	lw	ra, 8(sp)
	lw	fp, 4(sp)
	addi	sp, sp, 12
	ret
	.globl	increase_two
	.type	increase_two, @function
increase_two:
	addi	sp, sp, -12
	sw	ra, 8(sp)
	sw	fp, 4(sp)
	addi	fp, sp, 12
	sw	a0, -12(fp)
	lw	t0, -12(fp)
	mv	a0, t0
	call	increase_one
	call	increase_one
	j	.Lincrease_two_exit
.Lincrease_two_exit:
	lw	ra, 8(sp)
	lw	fp, 4(sp)
	addi	sp, sp, 12
	ret
	.globl	gcd
	.type	gcd, @function
gcd:
	addi	sp, sp, -24
	sw	ra, 20(sp)
	sw	fp, 16(sp)
	addi	fp, sp, 24
	sw	a0, -24(fp)
	sw	a1, -20(fp)
	li	t1, 1
	sw	t1, -16(fp)
.L11:
	lw	t1, -16(fp)
	lw	t2, -24(fp)
	slt	t3, t2, t1
	xori	t3, t3, 1
	lw	t1, -16(fp)
	lw	t2, -20(fp)
	slt	t4, t2, t1
	xori	t4, t4, 1
	li	t1, 0
	beq	t3, zero, .L14
.L13:
	sltu	t1, zero, t4
.L14:
	andi	t1, t1, 1
	beq	t1, zero, .L12
	lw	t2, -24(fp)
	lw	t3, -16(fp)
	rem	t4, t2, t3
	li	t2, 0
	xor	t3, t4, t2
	sltiu	t3, t3, 1
	lw	t2, -20(fp)
	lw	t4, -16(fp)
	rem	t5, t2, t4
	li	t2, 0
	xor	t4, t5, t2
	sltiu	t4, t4, 1
	li	t2, 0
	beq	t3, zero, .L18
.L17:
	sltu	t2, zero, t4
.L18:
	andi	t2, t2, 1
	beq	t2, zero, .L15
	lw	t3, -16(fp)
	sw	t3, -12(fp)
	j	.L16
.L15:
.L16:
	lw	t2, -16(fp)
	li	t3, 1
	add	t4, t2, t3
	sw	t4, -16(fp)
	j	.L11
.L12:
	lw	t1, -12(fp)
	mv	a0, t1
	j	.Lgcd_exit
.Lgcd_exit:
	lw	ra, 20(sp)
	lw	fp, 16(sp)
	addi	sp, sp, 24
	ret
	.globl	band
	.type	band, @function
band:
	addi	sp, sp, -16
	sw	ra, 12(sp)
	sw	fp, 8(sp)
	addi	fp, sp, 16
	sw	a0, -16(fp)
	sw	a1, -12(fp)
	lw	t1, -16(fp)
	lw	t2, -12(fp)
	and	t3, t1, t2
	mv	a0, t3
	j	.Lband_exit
.Lband_exit:
	lw	ra, 12(sp)
	lw	fp, 8(sp)
	addi	sp, sp, 16
	ret
