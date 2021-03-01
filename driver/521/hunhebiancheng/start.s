.text
.global _start
_start:

ldr sp,=stackend
b main
.global mycopy
mycopy:
loop:
ldrb r1,[r0],#1
strb r1,[r2],#1  @为什么交换顺序后，就看不出来了
cmp r1,#0
bne loop
mov r0,#0xff
mov pc,lr


.if 0
mov r0,#9
mov r1,#6
bl fun
mov r5,#0xff
.endif
.data
	stackstart:
		.space 128
	stackend:
.end
