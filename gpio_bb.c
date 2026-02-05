#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/gpio/driver.h>
#include <linux/device/devres.h>

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
static int gpio_bb_probe(struct platform_device *pdev);
static void gpio_bb_remove(struct platform_device *pdev);
static int gpio_bb_get_direction(struct gpio_chip *gc, unsigned int offset);
static void gpio_bb_set(struct gpio_chip *gc, unsigned int offset, int value);
static int gpio_bb_get(struct gpio_chip *gc, unsigned int offset);


struct gpio_bb_data_struct_t
{
    void __iomem * base;
};


static int gpio_bb_probe(struct platform_device *pdev)
{
    struct device * dev = &pdev->dev;
    struct gpio_bb_data_struct_t * gpio_data;
    struct gpio_chip * gpio_chip;

    printk("%s called\n", __func__);

    // gpio_data = devm_kzalloc(dev, sizeof(*gpio_data), GFP_KERNEL);
    // if(gpio_data == NULL)
    // {
    //     return -ENOMEM;
    // }

    // gpio_data->base = devm_platform_get_and_ioremap_resource(pdev, 0, NULL);
    // if(gpio_data->base == NULL)
    // {
    //     return -ENOMEM;
    // }

    // gpio_chip = devm_kzalloc(dev, sizeof(*gpio_chip), GFP_KERNEL);
    // if(gpio_chip == NULL)
    // {
    //     return -ENOMEM;
    // }

    // gpio_chip->get_direction = gpio_bb_get_direction;
    // gpio_chip->set = gpio_bb_set;
    // gpio_chip->get = gpio_bb_get;
    // gpiochip_add_data(gpio_chip, gpio_data);

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

static void gpio_bb_set(struct gpio_chip *gc, unsigned int offset, int value)
{
    struct gpio_bb_data_struct_t * gpio_data;
    u32 reg = 0;

    gpio_data = (struct gpio_bb_data_struct_t *)gpiochip_get_data(gc);
    if(gpio_data == NULL)
    {
        return;
    }

    reg = readl(gpio_data->base + GPIO_DATAOUT);
    reg |= (value << offset);
    writel(reg, gpio_data->base + GPIO_DATAOUT);

    return;
}


static int gpio_bb_get(struct gpio_chip *gc, unsigned int offset)
{
    struct gpio_bb_data_struct_t * gpio_data;
    u32 reg = 0;

    gpio_data = (struct gpio_bb_data_struct_t *)gpiochip_get_data(gc);
    if(gpio_data == NULL)
    {
        return -ENOMEM;
    }

    reg = readl(gpio_data->base + GPIO_DATAIN);

    return reg;
}

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














