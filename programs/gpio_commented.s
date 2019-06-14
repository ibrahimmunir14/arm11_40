// set pin 16 as output
ldr r0, =0x20200004
ldr r2, =0x40000
str r2, [r0]

ldr r0, =0x20200028 // r0 = pin clearing address
ldr r1, =0x2020001C // r1 = pin setting address
ldr r2, =0x10000    // r2 = bit 16 setting value

loop:

str r2, [r0]        // turn off pin 16
ldr r3, =0x1000000  // r3 = countdown register
waita:              // count down r3 to 0, to 'pause'
sub r3, r3, #1
cmp r3, #0
bne waita

str r2, [r1]        // turn off pin 16
ldr r3, =0x1000000  // r3 = countdown register
waitb:              // count down r3 to 0, to 'pause'
sub r3, r3, #1
cmp r3, #0
bne waitb

b loop              // repeat on/off process forever
andeq r0, r0, r0

/* Note: There is duplicate code here, for the wait loops.
         If we had implemented branch with link, it would
          have been easy to reuse a single wait loop.
         This file is here because our assembler does not
          work with comments.

