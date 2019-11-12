@
@ Sistemas Empotrados
@ El "hola mundo" en la Redwire EconoTAG
@

@
@ Constantes
@

        @ Registro de control de dirección del GPIO32-GPIO63
        .set GPIO_PAD_DIR1,    0x80000004

        @ Registro de activación de bits del GPIO32-GPIO63
        .set GPIO_DATA_SET1,   0x8000004c

        @ Registro de limpieza de bits del GPIO32-GPIO63
        .set GPIO_DATA_RESET1, 0x80000054

        @ El led rojo está en el GPIO 44 (el bit 12 de los registros GPIO_X_1)
        .set LED_RED_MASK,     (1 << (44-32))

        @ El led verde está en el GPIO 45 (el bit 13 de los registros GPIO_X_1)
        .set LED_GREEN_MASK,     (1 << (45-32))

        @ Registro de activación de bits del GPIO32-GPIO63
        .set GPIO_DIR_SET1,   0x8000005C

        @ Retardo para el parpadeo
        .set DELAY,            0x00200000

        @ Registro de control de dirección del GPIO00-GPIO31
        .set GPI_PAD_DIR0,      0x80000000

        @ Registro de activación de bits del GPIO00-GPIO31
        .set GPIO_DATA_SET0,   0x80000048       

        .set GPIO_DATA0,  0x80000008

        .set PIN_BUTTON_1, 0x08000000
        .set PIN_BUTTON_2, 0x04000000

        .set PIN_BUTTONS, 0x00C00000

@
@ Punto de entrada
@

        .code 32
        .text
        .global _start
        .type   _start, %function

_start:
        bl gpio_init

loop:

	bl test_buttons

        @ Pausa corta
        ldr     r0, =DELAY
        bl      pause

	str	r5, [r6]

        @ Pausa corta
        ldr     r0, =DELAY
        bl      pause

        @ Apagamos el led
	str	r5, [r7]

        @ Bucle infinito
        b       loop
        
@
@ Función que produce un retardo
@ r0: iteraciones del retardo
@
        .type   pause, %function
pause:
        subs    r0, r0, #1
        bne     pause
        mov     pc, lr

        .type test_buttons, %function
test_buttons:
        
        ldr r1, =GPIO_DATA0
	ldr r1, [r1]        

        tst r1, #PIN_BUTTON_1
        bne led_green_on

        tst r1, #PIN_BUTTON_2
        bne led_red_on

	mov pc, lr

        .type led_red, %function
led_red_on:

	ldr	r4, =LED_GREEN_MASK
	ldr	r5, =LED_RED_MASK
	str	r5, [r7]
        mov     pc, lr

        .type led_green, %function
led_green_on:

	ldr	r5, =LED_GREEN_MASK
	ldr	r4, =LED_RED_MASK
	str	r4, [r7]
        mov     pc, lr

        .type gpio_init, %function
gpio_init:

        ldr     r8, =GPIO_DIR_SET1
	//ldr	r8, =GPIO_PAD_DIR1

        @ Configuramos el GPIO45 para que sea de salida
        ldr     r4, =LED_GREEN_MASK
        str     r4, [r8]

        @ Configuramos el GPIO44 para que sea de salida
        ldr     r5, =LED_RED_MASK
        str     r5, [r8]      

        @ Direcciones de los registros GPIO_DATA_SET1 y GPIO_DATA_RESET1
        ldr     r6, =GPIO_DATA_SET1
        ldr     r7, =GPIO_DATA_RESET1        
        
        ldr     r2, =GPIO_DATA_SET0
        ldr     r3, =PIN_BUTTONS       
        str     r3, [r2]

	str	r5, [r6]

        mov     pc, lr
