/*
 * Sistemas operativos empotrados
 * Driver para el controlador de interrupciones del MC1322x
 */

#include "system.h"

/*****************************************************************************/

/**
 * Acceso estructurado a los registros de control del ITC del MC1322x
 */
typedef struct
{
	/* ESTA ESTRUCTURA SE DEFINIRÁ EN LA PRÁCTICA 6 */
	uint32_t INTCNTL;		// Interrupt Control Register
	uint32_t NIMASK;		// Normal Interrupt Mask Register
	uint32_t INTENNUM;		// Interrupt Enable Number Register
	uint32_t INTDISNUM;		// Interrupt Disable Number Register
	uint32_t INTENABLE;		// Interrupt Enable Register
	uint32_t INTTYPE;		// Interrupt Type Register
	uint32_t reserved[4];	// 
	uint32_t NIVECTOR;		// Normal Interrupt Vector
	uint32_t FIVECTOR;		// Fast Interrupt Vector
	uint32_t INTSRC;		// Interrupt Source Register
	uint32_t INTFRC;		// Interrupt Force Register
	uint32_t NIPEND;		// Normal Interrupt Pending Register
	uint32_t FIPEND;		// Fast Interrupt Pending Register

} itc_regs_t;

static volatile itc_regs_t* const itc_regs = ITC_BASE;

static uint32_t itc_enable;

/**
 * Tabla de manejadores de interrupción.
 */
static itc_handler_t itc_handlers[itc_src_max];

/*****************************************************************************/

/**
 * Inicializa el controlador de interrupciones.
 * Deshabilita los bits I y F de la CPU, inicializa la tabla de manejadores a NULL,
 * y habilita el arbitraje de interrupciones Normales y rápidas en el controlador
 * de interupciones.
 */
inline void itc_init ()
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 6 */
	uint32_t i;

	for(i = 0; i < itc_src_max; i++){	//Inicializamos el vector itc_handlers
		itc_handlers[i] = (uint32_t) NULL;
	}

	// No provocar ninguna interrupción simulada al arrancar
	itc_regs->intfrc = (uint32_t) 0;

	// Deshabilitar todas las fuentes de interrupción al activar el controlador
	itc_regs->intenable = (uint32_t) 0;
	
	// Activar arbitraje de interrupciones IRQ
	itc_regs->intcntl = ~(uint32_t)(1 << 19);
	
	// Activar arbitraje de interrupciones FIQ
	itc_regs->intcntl = ~(uint32_t)(1 << 20);
}

/*****************************************************************************/

/**
 * Deshabilita el envío de peticiones de interrupción a la CPU
 * Permite implementar regiones críticas en modo USER
 */
inline void itc_disable_ints ()
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 6 */
	itc_enable = itc_regs->INTENABLE;
	itc_regs->INTENABLE = (uint32_t) 0;
}

/*****************************************************************************/

/**
 * Vuelve a habilitar el envío de peticiones de interrupción a la CPU
 * Permite implementar regiones críticas en modo USER
 */
inline void itc_restore_ints ()
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 6 */	
	itc_regs->INTENABLE = itc_enable;
}

/*****************************************************************************/

/**
 * Asigna un manejador de interrupción
 * @param src		Identificador de la fuente
 * @param handler	Manejador
 */
inline void itc_set_handler (itc_src_t src, itc_handler_t handler)
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 6 */
	itc_handlers[src] = handler;
}

/*****************************************************************************/

/**
 * Asigna una prioridad (normal o fast) a una fuente de interrupción
 * @param src		Identificador de la fuente
 * @param priority	Tipo de prioridad
 */
inline void itc_set_priority (itc_src_t src, itc_priority_t priority)
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 6 */
	if(priority)
		itc_regs->INTTYPE = (1 << src);
	else
		itc_regs->INTTYPE &= ~(1 << src);
}

/*****************************************************************************/

/**
 * Habilita las interrupciones de una determinda fuente
 * @param src		Identificador de la fuente
 */
inline void itc_enable_interrupt (itc_src_t src)
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 6 */
	itc_regs->INTENNUM = src;
}

/*****************************************************************************/

/**
 * Deshabilita las interrupciones de una determinda fuente
 * @param src		Identificador de la fuente
 */
inline void itc_disable_interrupt (itc_src_t src)
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 6 */
	itc_reg->INTDISNUM = src;
}

/*****************************************************************************/

/**
 * Fuerza una interrupción con propósitos de depuración
 * @param src		Identificador de la fuente
 */
inline void itc_force_interrupt (itc_src_t src)
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 6 */
	itc_regs->intfrc |= (uint32_t)(1 << src);	//Ponemos el bit deseado a 1
}

/*****************************************************************************/

/**
 * Desfuerza una interrupción con propósitos de depuración
 * @param src		Identificador de la fuente
 */
inline void itc_unforce_interrupt (itc_src_t src)
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 6 */
	itc_regs->intfrc &= ~(uint32_t)(1 << src);	//Ponemos el bit deseado a 0 para desactivarlo
}

/*****************************************************************************/

/**
 * Da servicio a la interrupción normal pendiente de más prioridad.
 * En el caso de usar un manejador de excepciones IRQ que permita interrupciones
 * anidadas, debe deshabilitar las IRQ de menor prioridad hasta que se haya
 * completado el servicio de la IRQ para evitar inversiones de prioridad
 */
void itc_service_normal_interrupt ()
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 6 */
	itc_handlers[itc_regs->NIVECTOR];
}

/*****************************************************************************/

/**
 * Da servicio a la interrupción rápida pendiente de más prioridad
 */
void itc_service_fast_interrupt ()
{
	/* ESTA FUNCIÓN SE DEFINIRÁ EN LA PRÁCTICA 6 */
	itc_handlers[itc_regs->FIVECTOR];
}

/*****************************************************************************/
