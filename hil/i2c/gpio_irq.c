/*
* In the final iteration for I2C Sniffer, private vars include SDA, SCL, IRQ_SDA, IRQ_SCL, *buffer_holding, m_buffer.
* In the final iteration for I2C Sniffer, public vars include count, which maybe subject to change a private var with public accessor method
* Final Iteration will have start() and stop() through ioctl, all of which are public methods.
*/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/string.h>

#define BUFFER_SIZE 1000

unsigned int SDA = 514;
unsigned int SCL = 515;
unsigned int IRQ_SDA;
unsigned int IRQ_SCL;
char m_buffer[BUFFER_SIZE];
int global_counter = 0;
typedef enum sniffer_state{
	WAIT_FOR_SDA_FALL,
	WAIT_FOR_SCL_FALL,
	PACKET_SNIFF,
	WAIT_FOR_SDA_RISE,
	END_OF_COMMS
} sniffer_state_t;

sniffer_state_t currentState;

void PACKET_SNIFFING_PROTOCOL(int sda_value);
void PACKET_STORE_PROTOCOL(void);

void PACKET_SNIFFING_PROTOCOL(int sda_value){
	if(global_counter>=999){
		m_buffer[999] = '\0';
		currentState = END_OF_COMMS;
		return;
	}
	m_buffer[global_counter] = (sda_value == 1) ? '1':'0';
	global_counter++;
	return;
}

void PACKET_STORE_PROTOCOL(){
	if(global_counter>=999){
		m_buffer[999] = '\0';
		currentState = END_OF_COMMS;
		return;
	}
	m_buffer[global_counter] = '\n';
	global_counter++;
	return;
}

static irqreturn_t HANDLER_SDA(int irq, void *dev_id)
{
	switch (currentState)
	{
		case WAIT_FOR_SDA_FALL:
			irq_set_irq_type(IRQ_SDA, IRQF_TRIGGER_RISING);
			currentState = WAIT_FOR_SCL_FALL;
			break;
		
		case WAIT_FOR_SCL_FALL:
			irq_set_irq_type(IRQ_SDA, IRQF_TRIGGER_FALLING);
			currentState = WAIT_FOR_SDA_FALL;
			break;
		case PACKET_SNIFF:
			break;
		case WAIT_FOR_SDA_RISE:
			PACKET_STORE_PROTOCOL();/*store-packet logic here*/
			irq_set_irq_type(IRQ_SDA, IRQF_TRIGGER_FALLING);
			currentState = WAIT_FOR_SDA_FALL;
			break;
		case END_OF_COMMS:
			/*Equivalent of stop irqs*/

			break;
	}
	return IRQ_HANDLED;
}
static irqreturn_t HANDLER_SCL(int irq, void *dev_id){
	switch (currentState)
	{
		case WAIT_FOR_SDA_FALL:
			break;
		case WAIT_FOR_SCL_FALL:
			irq_set_irq_type(IRQ_SCL, IRQF_TRIGGER_RISING);
			currentState = PACKET_SNIFF;
			break;
		case PACKET_SNIFF:
			PACKET_SNIFFING_PROTOCOL(gpio_get_value(SDA)); /*packet-sniffing logic here*/
			irq_set_irq_type(IRQ_SCL, IRQF_TRIGGER_FALLING);
			currentState = WAIT_FOR_SDA_RISE;
			break;
		case WAIT_FOR_SDA_RISE:
			irq_set_irq_type(IRQ_SCL, IRQF_TRIGGER_RISING);
			currentState = PACKET_SNIFF;
			break;
		case END_OF_COMMS:
			/*Euivalent of stop irqs*/
			printk("buffer %s\r\n", m_buffer);
			break;
	}
    return IRQ_HANDLED;
}

static int __init ModuleInit(void) {
//	printk("I2C Sniffer: Loading module... ");

	// Setup SCL
	if(gpio_request(SCL, "I2C-Sniffer-SCL")) {
		printk("Error!\nCan not allocate GPIO %d\n", SCL);
		return -1;
	}

	if(gpio_direction_input(SCL)) {
		printk("Error!\nCan not set GPIO %d to input!\n", SCL);
		gpio_free(SCL);
		return -1;
	}
    // Setup SDA
    if(gpio_request(SDA, "I2C-Sniffer-SDA")) {
		printk("Error!\nCan not allocate GPIO %d\n", SDA);
        gpio_free(SCL);
		return -1;
	}
	if(gpio_direction_input(SDA)) {
		printk("Error!\nCan not set GPIO %d to input!\n", SDA);
		gpio_free(SDA);
        gpio_free(SCL);
		return -1;
	}
	
	IRQ_SCL = gpio_to_irq(SCL);
	IRQ_SDA = gpio_to_irq(SDA);

	if(request_irq(IRQ_SDA, HANDLER_SDA, IRQF_TRIGGER_FALLING, "I2C_SDA_HANDLER", NULL) != 0){
		printk("Error!\nCan not request interrupt nr.: %d\n", IRQ_SDA);
		gpio_free(SDA);
		return -1;
	}
    if(request_irq(IRQ_SCL, HANDLER_SCL,IRQF_TRIGGER_RISING, "I2C_SCL_HANDLER", NULL) != 0){
		printk("Error!\nCan not request interrupt nr.: %d\n", IRQ_SCL);
        free_irq(IRQ_SDA, NULL);
		gpio_free(SCL);
		return -1;
	}

//	printk("Done!\n");
	currentState = WAIT_FOR_SDA_FALL;

	return 0;
}
static void __exit ModuleExit(void) {
	m_buffer[global_counter] = '\0';
	printk("Global Counter: %d", global_counter);
	printk("State: %d", currentState);
	printk("Buffer contents:\n%s\n", m_buffer);
	printk("I2C Sniffer: Unloading module... \n");
	
	free_irq(IRQ_SDA, NULL);
    free_irq(IRQ_SCL, NULL);
	gpio_free(SCL);
    gpio_free(SDA);
    printk("Done!\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);

// Meta Data
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ameen Durani");
MODULE_DESCRIPTION("I2C Sniffer");