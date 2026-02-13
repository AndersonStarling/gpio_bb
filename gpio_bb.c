#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/gpio/driver.h>
#include <linux/device/devres.h>
#include <linux/property.h>
#include <linux/clk.h>
#include <linux/irq.h>

#define GPIO_REVISION 0x00u
#define GPIO_SYSCONFIG 0x10u
#define GPIO_EOI 0x20u
#define GPIO_IRQSTATUS_RAW_0 0x24u
#define GPIO_IRQSTATUS_RAW_1 0x28u
#define GPIO_IRQSTATUS_0 0x2Cu
#define GPIO_IRQSTATUS_1 0x30u
#define GPIO_IRQSTATUS_SET_0 0x34u
#define GPIO_IRQSTATUS_SET_1 0x38u
#define GPIO_IRQSTATUS_CLR_0 0x3Cu
#define GPIO_IRQSTATUS_CLR_1 0x40u
#define GPIO_IRQWAKEN_0 0x44u
#define GPIO_IRQWAKEN_1 0x48u
#define GPIO_SYSSTATUS 0x114u
#define GPIO_CTRL 0x130u
#define GPIO_OE 0x134u
#define GPIO_DATAIN 0x138u
#define GPIO_DATAOUT 0x13Cu
#define GPIO_LEVELDETECT_0 0x140u
#define GPIO_LEVELDETECT_1 0x144u
#define GPIO_RISINGDETECT 0x148u
#define GPIO_FALLINGDETECT 0x14Cu
#define GPIO_DEBOUNCENABLE 0x150u
#define GPIO_DEBOUNCINGTIME 0x154u
#define GPIO_CLEARDATAOUT 0x190u
#define GPIO_SETDATAOUT 0x194u

static struct platform_driver gpio_bb_driver;
struct irq_chip gpio_bb_irq;

static int gpio_bb_probe(struct platform_device *pdev);
static void gpio_bb_remove(struct platform_device *pdev);
static int gpio_bb_get_direction(struct gpio_chip *gc, unsigned int offset);
static int gpio_bb_direction_input(struct gpio_chip *gc, unsigned int offset);
static int gpio_bb_direction_output(struct gpio_chip *gc, unsigned int offset, int output_value);
static void gpio_bb_set(struct gpio_chip *gc, unsigned int offset, int value);
static int gpio_bb_get(struct gpio_chip *gc, unsigned int offset);

int	gpio_bb_to_irq(struct gpio_chip *gc, unsigned int offset);
unsigned int gpio_bb_irq_startup(struct irq_data *data);
void gpio_bb_irq_shutdown(struct irq_data *data);
void gpio_bb_irq_enable(struct irq_data *data);
void gpio_bb_irq_disable(struct irq_data *data);
int gpio_bb_irq_set_type(struct irq_data *data, unsigned int flow_type);


struct gpio_bb_data_struct_t
{
    void __iomem * base;
    struct platform_device * pdev;
};


static int gpio_bb_probe(struct platform_device *pdev)
{
    struct device * dev = &pdev->dev;
    struct gpio_bb_data_struct_t * gpio_data;
    struct gpio_chip * gpio_chip;
    struct gpio_irq_chip * gpio_irq;
    struct clk * fck;
    u32 ngpio = 0;
    int ret = -1;

    printk("%s called\n", __func__);

    gpio_data = devm_kzalloc(dev, sizeof(*gpio_data), GFP_KERNEL);
    if(gpio_data == NULL)
    {
        return -ENOMEM;
    }

    gpio_data->pdev = pdev;
    printk("pdev = %u\n", gpio_data->pdev);
    gpio_data->base = devm_platform_get_and_ioremap_resource(pdev, 0, NULL);
    if(gpio_data->base == NULL)
    {
        return -ENOMEM;
    }

    gpio_chip = devm_kzalloc(dev, sizeof(*gpio_chip), GFP_KERNEL);
    if(gpio_chip == NULL)
    {
        return -ENOMEM;
    }

    gpio_chip->base = -1;

    gpio_chip->get_direction    = gpio_bb_get_direction;
    gpio_chip->direction_input  = gpio_bb_direction_input;
    gpio_chip->direction_output = gpio_bb_direction_output;
    gpio_chip->set              = gpio_bb_set;
    gpio_chip->get              = gpio_bb_get;
    gpio_chip->to_irq           = gpio_bb_to_irq;
    ret = device_property_read_u32(dev, "ngpios", &ngpio);
    if(ret < 0)
    {
        return -1;
    }
    gpio_chip->ngpio = ngpio;
    printk("gpio_chip->ngpio = %d\n", gpio_chip->ngpio);

    /* enable clock */
    fck = devm_clk_get(dev, "fck");
    if (IS_ERR(fck)) {
        dev_err(dev, "Failed to get fck clock\n");
        return PTR_ERR(fck);
    }

    ret = clk_prepare_enable(fck);
    if (ret) {
        dev_err(&pdev->dev, "Failed to enable clock\n");
        return ret;
    }

    gpiochip_add_data(gpio_chip, gpio_data);
    gpio_irq_chip_set_chip(&gpio_chip->irq, &gpio_bb_irq); 

    return 0;
}

static void gpio_bb_remove(struct platform_device *pdev)
{
    printk("%s called\n", __func__);
    platform_driver_unregister(&gpio_bb_driver);
    return;
}

static int gpio_bb_get_direction(struct gpio_chip *gc, unsigned int offset)
{
    struct gpio_bb_data_struct_t * gpio_data;
    u32 ret = 0;

    gpio_data = (struct gpio_bb_data_struct_t *)gpiochip_get_data(gc);
    if(gpio_data == NULL)
    {
        return -ENOMEM;
    }

    ret = readl(gpio_data->base + GPIO_OE);
    ret = (ret >> offset) & 0x01;

    return ret;
}

static int gpio_bb_direction_input(struct gpio_chip *gc, unsigned int offset)
{
    struct gpio_bb_data_struct_t * gpio_data;
    u32 reg = 0;

    gpio_data = (struct gpio_bb_data_struct_t *)gpiochip_get_data(gc);
    if(gpio_data == NULL)
    {
        return -ENOMEM;
    }

    reg = readl(gpio_data->base + GPIO_OE);
    reg |= (1 << offset);

    writel(reg, gpio_data->base + GPIO_OE);

    return 0;
}

static int gpio_bb_direction_output(struct gpio_chip *gc, unsigned int offset, int output_value)
{
    struct gpio_bb_data_struct_t * gpio_data;
    u32 reg = 0;

    gpio_data = (struct gpio_bb_data_struct_t *)gpiochip_get_data(gc);
    if(gpio_data == NULL)
    {
        return -ENOMEM;
    }

    gc->set(gc, offset, output_value);
    reg = readl(gpio_data->base + GPIO_OE);
    reg &= ~(1 << offset);
    writel(reg, gpio_data->base + GPIO_OE);

    return 0;
}

static void gpio_bb_set(struct gpio_chip *gc, unsigned int offset, int value)
{
    struct gpio_bb_data_struct_t * gpio_data;
    u32 reg = 0;

    gpio_data = (struct gpio_bb_data_struct_t *)gpiochip_get_data(gc);
    if(gpio_data == NULL)
    {
        return;
    }

    switch(value)
    {
        case 0:
            reg = readl(gpio_data->base + GPIO_CLEARDATAOUT);
            reg |= (1 << offset);
            writel(reg, gpio_data->base + GPIO_CLEARDATAOUT);
            break;
        case 1:
            reg = readl(gpio_data->base + GPIO_SETDATAOUT);
            reg |= (1 << offset);
            writel(reg, gpio_data->base + GPIO_SETDATAOUT);
            break;
        default:
            printk("%s value error\n", __func__);
            break;
    }

    return;
}


static int gpio_bb_get(struct gpio_chip *gc, unsigned int offset)
{
    struct gpio_bb_data_struct_t * gpio_data;
    u32 reg = 0;
    u32 ret = 0;
    u32 direction = 0;

    gpio_data = (struct gpio_bb_data_struct_t *)gpiochip_get_data(gc);
    if(gpio_data == NULL)
    {
        return -ENOMEM;
    }

    direction = gc->get_direction(gc, offset);
    switch(direction)
    {
        /* output */
        case 0:
            reg = readl(gpio_data->base + GPIO_DATAOUT);
            break;
        /* input */
        case 1:
            reg = readl(gpio_data->base + GPIO_DATAIN);
            break;
        default:
            printk("%s direction error\n", __func__);
            break;
    }

    ret = (reg >> offset) & 0x01;

    return ret;
}

int	gpio_bb_to_irq(struct gpio_chip *gc, unsigned int offset)
{
    int irq;
    struct platform_device *pdev;
    struct gpio_bb_data_struct_t * gpio_data;

    gpio_data = (struct gpio_bb_data_struct_t *)gpiochip_get_data(gc);

    pdev = gpio_data->pdev;
    printk("pdev = %u\n", pdev);

    switch(offset)
    {
        case 0 ... 14:
            irq = platform_get_irq(pdev, 0);
            break;
        case 15 ... 31:
            irq = platform_get_irq(pdev, 1);
            break;
        default:
            printk("offset is error\n");
            break;
    }

    printk("%s irq = %d\n", __func__, irq);

    return irq;
}

unsigned int gpio_bb_irq_startup(struct irq_data *data)
{
    printk("%s called\n", __func__);
    printk("data->mask = %u\n", data->mask);
    printk("data->irq = %u\n", data->irq);
    printk("data->hwirq = %u\n", data->hwirq);
    printk("----------------------");

    return 0;
}

void gpio_bb_irq_shutdown(struct irq_data *data)
{
    printk("%s called\n", __func__);
}

void gpio_bb_irq_enable(struct irq_data *data)
{
    printk("%s called\n", __func__);
}

void gpio_bb_irq_disable(struct irq_data *data)
{
    printk("%s called\n", __func__);
}

int gpio_bb_irq_set_type(struct irq_data *data, unsigned int flow_type)
{
    printk("%s called\n", __func__);
    printk("flow_type %u\n", flow_type);

    return 0;
}

struct irq_chip gpio_bb_irq = 
{
    .irq_startup = gpio_bb_irq_startup,
    .irq_shutdown = gpio_bb_irq_shutdown,
    .irq_enable = gpio_bb_irq_enable,
    .irq_disable = gpio_bb_irq_disable,
    .irq_set_type = gpio_bb_irq_set_type
};

static const struct of_device_id gpio_bb_match[] = {
	{
		.compatible = "gpio-bb",
	},
	{ },
};

static struct platform_driver gpio_bb_driver = {
	.probe		= gpio_bb_probe,
	.remove		= gpio_bb_remove,
	.driver		= {
        .name = "gpio-bb",
		.of_match_table = gpio_bb_match,
	},
};

module_platform_driver(gpio_bb_driver);

MODULE_DESCRIPTION("gpio bb driver");
MODULE_ALIAS("platform:gpio-bb");
MODULE_LICENSE("GPL v2");














