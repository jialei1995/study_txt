.text
.global _start
_start:

b reset
nop
b swi_handler
nop
nop
nop
nop
nop

reset:
ldr sp,=stackend
mrs r0,cpsr
bic r0,#0xffffffff
orr r0,#0x10
msr cpsr,r0  @in user model

ldr r0,=0x11111111
ldr r1,=0x22222222
ldr r2,=0x33333333

swi 88
ldr r0,=0xffffffff

swi_handler:
stmfd sp!,{r0-r12,lr}
sub r0,lr,#4
ldr r1,[r0]
bic r1,#0xff000000
cmp r1,#88
bleq func
ldr r0,=0x88888888
ldr r1,=0x77777777
ldr r2,=0x99999999

ldmfd sp!,{r0-r12,pc}^

func:
ldr r0,=0xaaaaaaaa
mov pc,lr


.data
	stackstart:
		.space 160
	stackend:
.end
