; ti83+ assembly program
; for receiving a number over SPI
; and displaying it
; compiled on calc by Mimas assembler

 ORG    userMem-2
 DB     $BB,$6D
Start:
 CALL   GetByte
 LD     H,0
 LD     L,C
 BCALL  DispHL
 BCALL  NewLine
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
