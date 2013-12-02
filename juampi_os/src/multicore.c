#include <scrn.h>
#include <exception.h>
#include <multicore.h>
#include <asserts.h>
#include <utils.h>

typedef struct {
	uint start, end;
} mem_zone;

static mem_zone
get_ebda_zone() 
{
	//De acuerdo a OSDEV, y en GRUB parece asumir lo mismo
	//	http://linux4u.jinr.ru/pub/misc/sys/grub/SVN/grub/grub2/commands/i386/pc/acpi.c	
	//El start de la EDBA puede en la mayoria de los casos tomarse de la
	//posicion 0x40E, o usando BIOS INT 12. Como estamos en Modo Protegido, 
	//usamos la memoria. TODO: Averiguar si GRUB ya te da este valor.

	//Como aparentemente la ebda mide al menos un kilobyte segun
	//	http://www.nondot.org/sabre/os/files/Booting/BIOS_SEG.txt
	//Y solo nos interesa el primer KB por la MPSpec, no nos fijamos nada.
	uint ebda_start = (*((ushort *) 0x40e)) << 4;

	mem_zone res = { ebda_start, ebda_start + 0xFFF };
	return res;
}

//Hace el checksum signado de los bytes. La suma tiene que dar 0, 
//considerando suma signada de byte con overflow.
static bool
do_checksum(void * p, unsigned int len)
{
	char * bytes =(char *) p;
	char sum = 0;

	for(uint i = 0; i < len; i++){
		sum += bytes[i];	
	}

	return sum == 0;
}

static bool
check_valid_mpfs(mp_float_struct * mpfs)
{
	return do_checksum(mpfs,mpfs->length * 16);
}

static mp_float_struct *
find_floating_pointer_struct()
{
	//Header y zonas a revisar
	static const char MPSIG[]	= "_MP_";
	static const uint MPLEN		= sizeof(MPSIG)-1;
	static const uint ZONES		= 3;

	//Direcciones son inclusive (intervalo cerrado).
	mem_zone zones[] = {
		//Primer kilobyte de Extended Data BIOS Area
		get_ebda_zone(),
		//El ultimo KB de memoria base. Asumimos que 640 KB es el minimo
		//ignorando que obviamente eso debiera alcanzarle a cualquiera.
		{ (639 << 10), (639 << 10) + 0xFFF },
		//Direcciones de BIOS ROM
		{ 0xF0000, 0xFFFFF }
	};

	//Buscar header en las zonas indicadas
	mp_float_struct * mpfs = NULL;
	for(uint i = 0; i < ZONES; i++){
		uchar * st = (uchar *) zones[i].start;
		uchar * en = (uchar *) zones[i].end;
		for(uchar * p = st; p <= en; p++){
			if(memcmp(p,MPSIG,MPLEN) == 0){
				mpfs = (mp_float_struct *) p;
				goto found;
			}
		}
	}

found:
	if(mpfs == NULL){
		scrn_printf("Estructura MPFS no encontrada");
		return NULL;
	}

	if(!check_valid_mpfs(mpfs)){
		scrn_printf("Estructura MPFS con checksum invalido");
		return NULL;
	}

	return mpfs;
}

//Chequea que el checksum de la tabla de configuracion de Multi Processor sea
//correcta. 
static bool
check_valid_mpct(mp_config_table * mpct)
{
	return do_checksum(mpct,mpct->length);
}

//Procesa la entrada de procesador para configurar este core
static void
configure_processor(processor_entry * entry)
{
	scrn_printf("\tEntrada de procesador: LAPIC (%d)\n",entry->local_apic_id);
}

//Tamaño de las entradas de la tabla de configuracion de MP
static uint entry_sizes[] = {
	[PROCESSOR]		= 20,
	[BUS]			= 8,
	[IOAPIC]		= 8,
	[IOINTR]		= 8,
	[LOCAL_IOINTR]	= 8
};

//Determinar posicion de la proxima entrada de la tabla
static mp_entry *
next_mp_entry(mp_config_table * mpct, mp_entry * p)
{
	char * ptr = (char *) p;
	fail_unless(p->entry_type < MP_ENTRY_TYPES);
	return (mp_entry *) (ptr + entry_sizes[p->entry_type]);
}

//Determina la configuracion del procesador si hay una tabla de configuracion
//es decir si no hay una configuracion default en uso.
static void 
determine_cpu_configuration(mp_float_struct * mpfs)
{
	mp_config_table * mpct = mpfs->config;
	fail_if(!check_valid_mpct(mpct));

	//Seguimos las entradas de la tabla de configuracion
	fail_unless(mpct->entry_count > 0);
	mp_entry * entry = mpct->entries;

	for(uint entryi = 0; entryi <= mpct->entry_count; entryi++){	
		if(entry->entry_type == PROCESSOR){
			configure_processor(&entry->chunk.processor);
		}
		//Por ahora ignoramos todas las demas entradas.
		entry = next_mp_entry(mpct,entry);
	}
}

static void
determine_default_configuration()
{
}

//Revisar que las estructuras sean del tamaño correcto
static void
check_struct_sizes()
{
	fail_unless(sizeof(processor_entry) != entry_sizes[PROCESSOR]);
	fail_unless(sizeof(ioapic_entry) != entry_sizes[IOAPIC]);
	fail_unless(sizeof(bus_entry) != entry_sizes[BUS]);
	fail_unless(sizeof(intr_assign_entry) != entry_sizes[IOINTR]);
	fail_unless(sizeof(local_intr_assign_entry) != entry_sizes[LOCAL_IOINTR]);
}

void multiprocessor_init()
{
	scrn_cls();
	check_struct_sizes();

	//Detectar MPFS
	mp_float_struct * mpfs = find_floating_pointer_struct();
	if(mpfs == NULL){
		return;
	}

	scrn_printf("\tEstructura MPFS encontrada: %u\n", mpfs);

	//Recorrer estructura y determinar los cores
	if(mpfs->config != 0){
		//Configuracion hay que determinarla
		scrn_printf("\tConfiguracion a determinar\n");
		determine_cpu_configuration(mpfs);
	}else if(mpfs->mp_features1 != 0){
		//La configuracion ya esta definida y es una estandar
		scrn_printf("\tConfiguracion default numero: %d",mpfs->mp_features1);
		determine_default_configuration(mpfs);	
	}else{
		kernel_panic("Configuracion MPFS invalida");	
	}
}
