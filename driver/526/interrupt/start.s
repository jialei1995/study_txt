.text
.global _start
_start:

b reset
nop
nop
nop
nop
nop
b irq_handler
nop


reset:
@set interrupt vector to 0x40008000
ldr r0,=0x40008000
@协处理器，配置cache，有自己的指令集.p15一共16个
@0代表操作结果不可知，将r0地址赋给c12，与c0，最后的0可省略
mcr p15,0,r0,c12,c0,0

@set model irq 
mrs r0,cpsr
bic r0,#0x1f
orr r0,#0x12
msr cpsr,r0

@set irq_models stack
ldr sp,=irq_end

@set model irq -->user
mrs r0,cpsr
bic r0,#0x9f @允许irq,注意改一下cpsr
orr r0,#0x10
msr cpsr,r0

ldr sp,=user_end
b main

irq_handler:
sub lr,lr,#4   @pc指向的位置命令并没有执行，lr放的pc下一地址指令
stmfd sp!,{r0-r12,lr}
bl doirq
ldmfd sp!,{r0-r12,pc}^

.data
	irq_start:
	.space 128
	irq_end:
	user_start:
	.space 128
	user_end:
.end
