	.file	"multest.c"
	.text
	.p2align 2
	.globl multiply
	.type	multiply, @function
multiply:
	enter	#12
	ldi:8 	#252, r1
	extsb	r1
	addn	fp, r1
	st    	r4, @r1
	ldi:8 	#248, r1
	extsb	r1
	addn	fp, r1
	st    	r5, @r1
	ldi:8 	#252, r2
	extsb	r2
	addn	fp, r2
	ldi:8 	#248, r1
	extsb	r1
	addn	fp, r1
	ld    	@r2, r2
	ld    	@r1, r1
	mul	r1, r2
	mov	mdl, r1
	mov   	r1, r4
	leave
	ret
	.size	multiply, .-multiply
	.p2align 2
	.globl main
	.type	main, @function
main:
	st	r8, @-r15
	st	rp, @-r15
	enter	#16
	ldi:8 	#252, r1
	extsb	r1
	addn	fp, r1
	ldi:8 	#0, r2
	st    	r2, @r1
	ldi:8 	#248, r1
	extsb	r1
	addn	fp, r1
	ldi:8 	#10, r2
	st    	r2, @r1
	ldi:8 	#244, r1
	extsb	r1
	addn	fp, r1
	ldi:8 	#5, r2
	st    	r2, @r1
	ldi:8 	#252, r8
	extsb	r8
	addn	fp, r8
	ldi:8 	#244, r2
	extsb	r2
	addn	fp, r2
	ldi:8 	#248, r1
	extsb	r1
	addn	fp, r1
	ld    	@r2, r5
	ld    	@r1, r4
	ldi:32	multiply, r1
	call	@r1
	st    	r4, @r8
	ldi:8 	#0, r1
	mov   	r1, r4
	leave
	ld	@r15+, rp
	ld	@r15+, r8
	ret
	.size	main, .-main
	.ident	"GCC: (GNU) 14.2.0"
