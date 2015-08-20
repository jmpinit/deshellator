; ti83+ assembly program
; for receiving an image over SPI
; and displaying it
; compiled on calc by Mimas assembler

 ORG    userMem-2
 DB     $BB,$6D
Start:
 DI
Wait:
 LD     A,$FE
 OUT    (1),A
 NOP
 NOP
 IN     A,(1)
 CP     $F7
 JP     Z,Go
 JP     Wait
Go:
 BCALL  GrBufClr
 LD     HL,plotSScreen
 LD     DE,768
Draw:
 CALL   GetByte
 LD     (HL),A
 INC    HL
 DEC    DE
 LD     A,0
 CP     E
 JR     NZ,Draw
 CP     D
 JR     NZ,Draw
 BCALL  GrBufCpy
 EI
 RET
GetByte:
 LD     C,0
 LD     B,8
GetBit:
 SLA    C
ClockDown:
 LD     A, $D2
 OUT    (0),A
 CALL   DelayL
Sample:
 IN     A,(0)
 BIT    0,A
 JR     Z,GetZero
GetOne:
 SET    0,C
GetZero:
ClockUp:
 LD     A,$D0
 OUT    (0),A
 CALL   DelayL
 DJNZ   GetBit
 LD     A,C
 RET
DelayL:
 CALL   Delay
 CALL   Delay
 CALL   Delay
 CALL   Delay
 RET
Delay:
 NOP
 NOP
 NOP
 NOP
 NOP
 NOP
 NOP
 NOP
 RET
