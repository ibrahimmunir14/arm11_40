ldr r0, =0x20200004
ldr r2, =0x40000
str r2, [r0]

ldr r0, =0x20200028
ldr r1, =0x2020001C
ldr r2, =0x10000

loop:

str r2, [r0]
ldr r3, =0x1000000
waita:
sub r3, r3, #1
cmp r3, #0
bne waita

str r2, [r1]
ldr r3, =0x1000000
waitb:
sub r3, r3, #1
cmp r3, #0
bne waitb

b loop
andeq r0, r0, r0
