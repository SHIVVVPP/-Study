[ORG 0x00]          ; 코드의 시작 어드레스를 0x00으로 설정한다.
[BITS 16]           ; 이하의 코드는 16비트 코드로 설정한다.

SECTION .text       ; text 섹션(세그먼트)를 정의한다.

jmp 0x07C0:START    ; CS 세그먼트 레지스터에 0x07C0을 복사하면서, START 레이블로 이동한다.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   CYNOS64 OS에 관련된 환경 설정 값       
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
TOTALSECTORCOUNT: dw 1024       ; 부트 로더를 제외한 CYNOS64 OS 이미지의 크기
                                ; 최대 1152 섹터(0x90000byte)까지 가능하다.


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       코드 영역
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
START:
    mov ax, 0x07C0      ; 부트 로더의 시작 어드레스(0x07C0)를 세그먼트 레지스터 값을 변환한다.
    mov ds, ax          ; DS 세그먼트 레지스터에 설정
    mov ax, 0xB800      ; 비디오 메모리의 시작 어드레스(0xB800)를 세그먼트 레지스터 값으로 변환한다.
    mov es, ax          ; ES 세그먼트 레지스터에 설정한다.
    
    ; 스택을 0x0000:0000~0x0000:FFFF 영역 64KB 크기로 생성한다.
    mov ax, 0x0000      ; 스택 세그먼트의 시작 어드레스(0x0000)를 세그먼트 레지스터 값으로 변환한다.
    mov ss, ax          ; SS 세그먼트 레지스터에 설정한다.
    mov sp, 0xFFFE      ; SP 레지스터의 어드레스를 0xFFFE로 설정한다.
    mov bp, 0xFFFE      ; BP 레지스터의 어드레스를 0xFFFE로 설정한다.

        
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;       화면을 모두 지우고, 속성값을 녹색으로 설정
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    mov si, 0                       ; SI 레지스터(문자열 원본 인덱스 레지스터)를 초기화한다.

.SCREENCLEARLOOP:                   ; 화면을 지우는 루프
    mov byte [ es: si ], 0          ; 비디오 메모리의 문자가 위치하는 어드레스에
                                    ; 0을 복사하여 문자를 삭제한다.
    mov byte [ es: si + 1], 0x0A    ; 비디오 메모리의 속성이 위치하는 어드레스에
                                    ; 0x0A(검은 바탕에 밝은 녹색)을 복사한다.
    
    add si, 2                       ; 문자와 속성을 설정했으므로 다음 위치로 이동한다.                       

    cmp si, 80 * 25 * 2             ; 화면 전체의 크기는 80 문자 * 25 라인이다.
                                    ; 출력한 문자의 수를 의미하는 SI 레지스터와 비교한다.
    jl .SCREENCLEARLOOP             ; SI 레지스터가 80 * 25 * 2 보다 작다면 아직 지우지 못한 영역이 있으므로
                                    ; .SCREENCLEARLOOP 레이블로 이동한다.

        
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;       화면 상단에 시작 메시지 출력       
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    push MESSAGE1                   ; 출력할 메시지의 어드레스를 스택에 삽입한다.
    push 0                          ; 화면 Y 좌표(1)를 스택에 삽입한다.
    push 0                          ; 화면 X 좌표(0)를 스택에 삽입한다.
    call PRINTMESSAGE               ; PRINTMESSAGE 함수를 호출한다.
    add sp, 6                       ; 삽입한 파라미터가 제거한다.

        
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;       OS 이미지를 로딩한다는 메시지 출력
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    push IMAGELOADINGMESSAGE
    push 1
    push 0
    call PRINTMESSAGE
    add sp, 6

        
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;       디스크에서 OS 이미지를 로딩한다.
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;       디스크를 읽기 전에 먼저 리셋한다.
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

RESETDISK:    ; 디스크를 리셋하는 코드의 시작
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;     BIOS Reset Function 호출  
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    
    ; 서비스 번호 0, 드라이브 번호(0=Floppy)
    mov ax, 0
    mov dl, 0
    int 0x13

    ; 에러가 발생하면 에러 처리로 이동한다.
    jc HANDLEDISKERROR

        
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;       디스크에서 섹터를 읽는다.
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    
    ; 디스크의 내용을 메모리로 복사할 어드레스(ES:BX)를 0x10000으로 설정한다.
    mov si, 0x1000              ; OS 이미지를 복사할 어드레스(0x10000)를
                                ; 세그먼트 레지스터 값으로 변환한다.
    
    mov es, si                  ; ES 세그먼트 레지스터에 값을 설정한다.
    mov bx, 0x0000              ; BX 레지스터에 0x0000을 설정하여 복사할
                                ; 어드레스를 0x1000:0000(0x10000)으로 최종 설정한다.


    mov di, word [ TOTALSECTORCOUNT ]   ; 복사할 OS 이미지를 섹터 수를 DI 레지스터에 설정한다.

READDATA:
    ; 모든 섹터를 다 읽었는지 확인
    cmp di, 0                   ; 복사할 OS 이미지의 섹터 수를 0과 비교
    je READEND                  ; 복사할 섹터 수가 0이라면 다 복사 했으므로 READEND로 이동
    sub di, 0x1                 ; 복사할 섹터 수를 1감소
        
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;   BIOS Read Function 호출       
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    mov ah, 0x02                    ; BIOS 서비스 번호 2(Read Sector)
    mov al, 0x1                     ; 읽을 섹터 수는 1
    mov ch, byte [ TRACKNUMBER ]    ; 읽을 트랙 번호 설정
    mov cl, byte [ SECTORNUMBER ]   ; 읽을 섹터 번호 설정
    mov dh, byte [ HEADNUMBER ]     ; 읽을 헤드 번호 설정
    mov dl, 0x00                    ; 읽을 드라이브 번호(0=Floppy) 설정
    int 0x13                        ; 인터럽트 서비스 수행
    jc HANDLEDISKERROR              ; 에러가 발생했다면 HANDLEDISKERROR로 이동한다.
        
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;   복사할 어드레스와 트랙, 헤드, 섹터 어드레스 계산       
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    add si, 0x0020                  ; 512(0x200)바이트 만큼 읽었으므로, 이를 세그먼트 레지스터 값으로 변환한다.

    mov es, si                      ; ES 세그먼트 레지스터에 더해서 어드레스를 한 섹터 만큼 증가시킨다.

    ; 한 섹터를 읽었으므로 섹터 번호를 증가시키고 마지막 섹터(18)까지 읽었는지 판단한다.
    ; 마지막 섹터가 아니면 섹터 읽기로 이동해서 다시 섹터 읽기를 수행한다.
    mov al, byte [ SECTORNUMBER ]   ; 섹터 번호를 AL 레지스터에 설정한다.
    add al, 0x01                    ; 섹터 번호를 1 증가시킨다.
    mov byte [ SECTORNUMBER ], al   ; 증가시킨 섹터 번호를 SECTORNUMBER에 다시 설정한다.
    cmp al, 19                      ; 증가시킨 섹터 번호를 19와 비교한다.
    jl READDATA                     ; 섹터 번호가 19 미만이라면 READDATA로 이동한다.

    ; 마지막 섹터까지 읽었으면(섹터 번호가 19이면) 헤드를 토글( 0->1 1->0)하고,
    ; 섹터 번호를 1로 설정한다.
    xor byte [ HEADNUMBER ], 0x01   ; 헤드 번호를 0X01과 XOR하여 토글(0->1 1->0)
    mov byte [ SECTORNUMBER ], 0x01 ; 섹터 번호를 다시 1로 설정한다.

    ; 만약 헤드가 1->0로 바뀌었으면 양쪽 헤드를 모두 읽은 것이므로 아래로 이동하여
    ; 트랙 번호를 1 증가시킨다.
    cmp byte [ HEADNUMBER ], 0x01   ; 헤드 번호를 0x00과 비교한다.
    jne READDATA                    ; 헤드 번호가 0이 아니면 READDATA로 이동한다.

    ; 트랙을 1 증가시킨 후 다시 섹터 읽기로 이동한다.
    add byte [ TRACKNUMBER ], 0x01  ; 트랙 번호를 1증가시킨다.
    jmp READDATA                    ; READDATA로 이동한다.


READEND:    
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;   OS 이미지가 완료되었다는 메시지를 출력한다.       
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    push LOADINGCOMPLETEMESSAGE     ; 출력할 메시지의 어드레스를 스택에 삽입한다.
    push 1                          ; 화면 Y 좌표(1)를 스택에 삽입한다.
    push 20                         ; 화면 X 좌표(20)를 스택에 삽입한다.
    call PRINTMESSAGE               ; PRINTMESSAGE 함수를 호출한다.
    add sp, 6                       ; 삽입한 파라미터를 제거한다.
        
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;   로딩한 가상 OS 이미지 실행       
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    jmp 0x10000:0x0000

    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       함수 코드 영역
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; 디스크 에러를 처리하는 함수이다.
HANDLEDISKERROR:
    push DISKERRORMESSAGE       ; 에러 문자열의 어드레스를 스택에 삽입한다.
    push 1                      ; 화면 Y 좌표(1)를 스택에 삽입한다.
    push 20                     ; 화면 X 좌표(20)를 스택에 삽입한다.
    call PRINTMESSAGE           ; PRINTMESSAGE 함수를 호출 한다.

    jmp $                       ; 현재 위치에서 무한 루프를 수행한다.

; 메시지를 출력하는 함수
;   PARAM : x 좌표, y 좌표, 문자열
PRINTMESSAGE:                   
    push bp                     ; 베이스 포인터 레지스터(BP)를 스택에 삽입한다.
    mov bp, sp                  ; 베이스 포인터 레지스터(BP)에 스택 포인터 레지스터(SP)의 값을 설정한다.
                                ; 베이스 포인터 레지스터(BP)를 이용해서 파라미터에 접근하려는 목적이다.


    push es                     ; ES 세그먼트 레지스터부터 DX 레지스터까지 스택에 삽입한다.
    push si                     ; 함수에서 임시로 사용하는 레지스터로 함수의 마지막 부분에서
    push di                     ; 스택에 삽입된 값을 꺼내 원래 값으로 복원한다.
    push ax
    push cx 
    push dx

    ; ES 세그먼트 레지스터에 비디오 모드 어드레스를 설정한다.
    mov ax, 0xB800              ; 비디오 메모리 시작 어드레스(0x0B8000)를
                                ; 세그먼트 레지스터 값으로 변환한다.
    mov es, ax                  ; ES 세그먼트 레지스터에 설정한다.
        
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;   X, Y의 좌표로 비디오 메모리의 어드레스를 계산한다.       
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    ; Y 좌표를 이용해서 먼저 라인 어드레스를 구한다.
    mov ax, word [ bp + 6 ]     ; 파라미터 2(화면 좌표 Y)를 AX 레지스터에 설정한다.
    mov si, 160                 ; 한 라인의 바이트 수( 2 * 80 행)를 SI 레지스터에 설정한다.
    mul si                      ; AX 레지스터와 SI 레지스터를 곱하면 화면 Y 어드레스를 계산할 수 있다.
    mov di, ax                  ; 화면 Y 어드레스와 계산된 X 어드레스를 더해서
                                ; 실제 비디오 메모리 어드레스를 계산한다.

    ; X 좌표를 이용해서 2를 곱한 후 최종 어드레스를 구한다.
    mov ax, word [ bp + 4 ]     ; 파라미터 1(화면 좌표 X)를 AX 레지스터에 설정한다.
    mov si, 2                   ; 한 문자를 나타내는 바이트 수(2)를 SI 레지스터에 설정한다.
    mul si                      ; AX 레지스터와 SI 레지스터를 곱하여 화면 X 어드레스를 계산할 수 있다.
    add di, ax                  ; 화면 Y 어드레스와 계산된 X 어드레스를 더해서
                                ; 실제 비디오 메모리 어드레스를 계산한다.

    ; 출력할 문자열의 어드레스
    mov si, word [ bp + 8 ]     ; 파라미터 3( 출력할 문자열의 어드레스 )

.MESSAGELOOP:                   ; 메시지를 출력하는 루프
    mov cl, byte [ si ]         ; SI 레지스터가 가리키는 문자열 위치에서 한 문자를
                                ; CL 레지스터에 복사한다.
                                ; CL 레지스터는 CX 레지스터의 하위 1바이트를 의미한다.
                                ; 문자열 1바이트면 충분하므로 CX 레지스터의 하위 1바이트만 사용한다.

 
    cmp cl, 0                   ; 복사된 문자의 0을 비교한다.
    je .MESSAGEEND              ; 복사한 문자의 값이 0이면 문자열이 종료되었음을
                                ; 의미하므로 .MESSAGEEND로 이동하여 문자 출력을 종료한다.


    mov byte [ es: di ], cl     ; 0이 아니라면 비디오 메모리 어드레스 0xB800:di에 문자를 출력한다.

    add si, 1                   ; SI 레지스터에 1을 더하여 다음 문자열로 이동한다.
    add di, 2                   ; DI 레지스터에 2를 더하여 비디오 메모리의 다음 문자 위치로 이동한다.
                                ; 비디오 메모리는 (문자, 속성)의 쌍으로 구성되므로 문자만 출력하려면
                                ; 2를 더해야 한다.

    jmp .MESSAGELOOP            ; 메시지 출력 루프로 이동하여 다음 문자를 출력한다.

.MESSAGEEND:
    pop dx                      ; 함수에서 사용이 끝난 DX 레지스터부터 ES 레지스터까지를 스택에
    pop cx                      ; 삽입된 값을 이용해서 복원한다.
    pop ax                      ; 스택은 가장 마지막에 들어간 데이터가 가장 먼저 나오는
    pop di                      ; 자료구조(Last-In, First-Out)이므로 삽입(push)의 역순으로
    pop si                      ; 제거해야 한다.

    pop es
    pop bp                      ; 베이스 포인터 레지스터(BP)를 복원한다.
    ret                         ; 함수를 호출한 다음 코드의 위치로 복귀한다.

    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       데이터 영역
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 부트로더 시작 메시지
MESSAGE1:   db 'CYNOS64 OS Boot Loader Start !!', 0 ; 출력할 메시지를 정의한다.
                                                    ; 마지막은 0으로 설정하여 .MESSAGELOOP에서
                                                    ; 문자열이 종료되었음을 알 수 있게 한다.

DISKERRORMESSAGE:   db  'DISK Error!', 0                
IMAGELOADINGMESSAGE:    db 'OS Image Loading...', 0
LOADINGCOMPLETEMESSAGE: db 'Complete!!', 0

SECTORNUMBER:       db 0x02 ; OS 이미지가 시작하는 섹터 번호를 저장하는 영역
HEADNUMBER:         db 0x00 ; OS 이미지가 시작하는 헤드 번호를 저장하는 영역
TRACKNUMBER:        db 0x00 ; OS 이미지가 시작하는 트랙 번호를 저장하는 영역

times 510 - ( $ - $$ )  db  0x00    ; $: 현재 라인의 어드레스
                                    ; $$: 현재 섹션(.text)의 시작 어드레스
                                    ; $ - $$: 현재 섹션을 기준으로 하는 오프셋
                                    ; 510 - ( $ - $$ ): 현재부터 어드레스 510까지
                                    ; db 0x00: 1바이트를 선언하고 값은 0x00
                                    ; time: 반복수행
                                    ; 현재 위치에서 어드레스 510까지 0x00으로 채운다.



    db 0x55                         ; 1바이트를 선언하고 값은 0x55
    db 0xAA                         ; 1바이트를 선언하고 값은 0xAA
                                    ; 어드레스 511, 512에 0x55, 0xAA를 써서 부트 섹터임을 표기한다.








