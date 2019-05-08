#include <lx_emul.h>

#include <linux/phy.h>

#if 0
#define TRACE \
	do { \
		lx_printf("%s not implemented\n", __func__); \
	} while (0)
#else
#define TRACE do { ; } while (0)
#endif

#define TRACE_AND_STOP \
	do { \
		lx_printf("%s not implemented\n", __func__); \
		BUG(); \
	} while (0)

#if SKIP_VERBOSE
#define SKIP lx_printf("\033[34m%s\033[0m: skipped\n", __PRETTY_FUNCTION__)
#else
#define SKIP
#endif

struct page *alloc_pages(gfp_t gfp_mask, unsigned int order)
{
	TRACE_AND_STOP;
	return NULL;
}

u16 bitrev16(u16 in)
{
	TRACE_AND_STOP;
	return -1;
}

struct usb_cdc_parsed_header;
struct usb_interface;
int cdc_parse_cdc_header(struct usb_cdc_parsed_header *hdr, struct usb_interface *intf, u8 *buffer, int buflen)
{
	TRACE_AND_STOP;
	return -1;
}

u16 crc16(u16 crc, const u8 *buffer, size_t len)
{
	TRACE_AND_STOP;
	return -1;
}

__sum16 csum_fold(__wsum csum)
{
	TRACE_AND_STOP;
	return -1;
}

__wsum csum_partial(const void *buff, int len, __wsum sum)
{
	TRACE_AND_STOP;
	return -1;
}

void * dev_get_drvdata(const struct device *dev)
{
	TRACE_AND_STOP;
	return NULL;
}

int device_set_wakeup_enable(struct device *dev, bool enable)
{
	TRACE;
	return -1;
}

struct sk_buff;
void dev_kfree_skb_any(struct sk_buff *skb)
{
	TRACE_AND_STOP;
}

void dst_release(struct dst_entry *dst)
{
	TRACE_AND_STOP;
}

struct net_device;
u32 ethtool_op_get_link(struct net_device *dev)
{
	TRACE_AND_STOP;
	return -1;
}

int ethtool_op_get_ts_info(struct net_device *dev, struct ethtool_ts_info *eti)
{
	TRACE_AND_STOP;
	return -1;
}

bool file_ns_capable(const struct file *file, struct user_namespace *ns, int cap)
{
	TRACE_AND_STOP;
	return -1;
}

void free_netdev(struct net_device * ndev)
{
	TRACE_AND_STOP;
}

void free_uid(struct user_struct * user)
{
	TRACE_AND_STOP;
}

struct mii_if_info;
int generic_mii_ioctl(struct mii_if_info *mii_if, struct mii_ioctl_data *mii_data, int cmd, unsigned int *duplex_changed)
{
	TRACE_AND_STOP;
	return -1;
}

void get_page(struct page *page)
{
	TRACE_AND_STOP;
}

bool gfpflags_allow_blocking(const gfp_t gfp_flags)
{
	TRACE_AND_STOP;
	return -1;
}

bool gfp_pfmemalloc_allowed(gfp_t gfp_flags)
{
	TRACE_AND_STOP;
	return -1;
}

int hex2bin(u8 *dst, const char *src, size_t count)
{
	TRACE_AND_STOP;
	return -1;
}

int in_irq()
{
	TRACE_AND_STOP;
	return -1;
}

void *kmap_atomic(struct page *page)
{
	TRACE_AND_STOP;
	return NULL;
}

int memcmp(const void *s1, const void *s2, size_t size)
{
	TRACE_AND_STOP;
	return -1;
}

void mii_ethtool_get_link_ksettings( struct mii_if_info *mii, struct ethtool_link_ksettings *cmd)
{
	TRACE_AND_STOP;
}

int mii_ethtool_set_link_ksettings( struct mii_if_info *mii, const struct ethtool_link_ksettings *cmd)
{
	TRACE_AND_STOP;
	return -1;
}

unsigned netdev_mc_count(struct net_device * dev)
{
	TRACE;
	return 1;
}

int netdev_mc_empty(struct net_device * ndev)
{
	TRACE;
	return 1;
}

void netdev_stats_to_stats64(struct rtnl_link_stats64 *stats64, const struct net_device_stats *netdev_stats)
{
	TRACE_AND_STOP;
}

bool netdev_uses_dsa(struct net_device *dev)
{
	TRACE;
	return false;
}

void netif_device_attach(struct net_device *dev)
{
	TRACE;
}

void netif_device_detach(struct net_device *dev)
{
	TRACE_AND_STOP;
}

int  netif_device_present(struct net_device * d)
{
	TRACE;
	return 1;
}

u32 netif_msg_init(int arg0, int arg1)
{
	TRACE;
	return 0;
}

void netif_start_queue(struct net_device *dev)
{
	TRACE;
}

void netif_stop_queue(struct net_device *dev)
{
	TRACE_AND_STOP;
}

void netif_trans_update(struct net_device *dev)
{
	TRACE;
}

void netif_tx_wake_all_queues(struct net_device *dev)
{
	TRACE_AND_STOP;
}

void netif_wake_queue(struct net_device * d)
{
	TRACE;
}

const void *of_get_mac_address(struct device_node *np)
{
	TRACE;
	return NULL;
}

void pm_runtime_enable(struct device *dev)
{
	TRACE;
}

void read_lock_bh(rwlock_t * l)
{
	TRACE_AND_STOP;
}

void read_unlock_bh(rwlock_t * l)
{
	TRACE_AND_STOP;
}

void unregister_netdev(struct net_device * dev)
{
	TRACE_AND_STOP;
}

void secpath_reset(struct sk_buff * skb)
{
	TRACE;
}

void __set_current_state(int state)
{
	TRACE_AND_STOP;
}

void sg_init_table(struct scatterlist *sg, unsigned int arg)
{
	TRACE_AND_STOP;
}

void sg_set_buf(struct scatterlist *sg, const void *buf, unsigned int buflen)
{
	TRACE_AND_STOP;
}

void sg_set_page(struct scatterlist *sg, struct page *page, unsigned int len, unsigned int offset)
{
	TRACE_AND_STOP;
}

void sk_free(struct sock *sk)
{
	TRACE_AND_STOP;
}

void sock_efree(struct sk_buff *skb)
{
	TRACE_AND_STOP;
}

void spin_lock(spinlock_t *lock) { SKIP; }

void spin_lock_irq(spinlock_t *lock)
{
	TRACE_AND_STOP;
}

void spin_lock_nested(spinlock_t *lock, int subclass)
{
	TRACE;
}

void spin_unlock_irq(spinlock_t *lock)
{
	TRACE_AND_STOP;
}

size_t strlcpy(char *dest, const char *src, size_t size)
{
	TRACE_AND_STOP;
	return -1;
}

void tasklet_init(struct tasklet_struct *t, void (*f)(unsigned long), unsigned long d)
{
	t->func    = f;
	t->data    = d;
}

void tasklet_kill(struct tasklet_struct *t)
{
	TRACE_AND_STOP;
}

void trace_consume_skb(struct sk_buff *skb)
{
	TRACE;
}

void trace_kfree_skb(struct sk_buff *skb, void *arg)
{
	TRACE;
}

unsigned int u64_stats_fetch_begin_irq(const struct u64_stats_sync *p)
{
	TRACE_AND_STOP;
	return -1;
}

bool u64_stats_fetch_retry_irq(const struct u64_stats_sync *p, unsigned int s)
{
	TRACE_AND_STOP;
	return -1;
}

struct usb_device;
int usb_clear_halt(struct usb_device *dev, int pipe)
{
	TRACE_AND_STOP;
	return -1;
}

struct usb_driver;
int usb_driver_claim_interface(struct usb_driver *driver, struct usb_interface *iface, void *priv)
{
	TRACE_AND_STOP;
	return -1;
}

void usb_driver_release_interface(struct usb_driver *driver, struct usb_interface *iface)
{
	TRACE_AND_STOP;
}

struct usb_anchor;
struct urb *usb_get_from_anchor(struct usb_anchor *anchor)
{
	TRACE_AND_STOP;
	return NULL;
}

struct urb *usb_get_urb(struct urb *urb)
{
	TRACE_AND_STOP;
	return NULL;
}

int usb_interrupt_msg(struct usb_device *usb_dev, unsigned int pipe, void *data, int len, int *actual_length, int timeout)
{
	TRACE_AND_STOP;
	return -1;
}

void usb_scuttle_anchored_urbs(struct usb_anchor *anchor)
{
	TRACE_AND_STOP;
}

int usb_string(struct usb_device *dev, int index, char *buf, size_t size)
{
	TRACE_AND_STOP;
	return -1;
}

ktime_t ktime_get_real(void)
{
	TRACE_AND_STOP;
	return -1;
}

void kunmap_atomic(void *addr)
{
	TRACE_AND_STOP;
}

void might_sleep()
{
	TRACE_AND_STOP;
}

bool page_is_pfmemalloc(struct page *page)
{
	TRACE_AND_STOP;
	return -1;
}

void put_page(struct page *page)
{
	TRACE_AND_STOP;
}

struct usb_interface *usb_ifnum_to_if(const struct usb_device *dev, unsigned ifnum)
{
	TRACE_AND_STOP;
	return NULL;
}

void usb_kill_urb(struct urb *urb)
{
	TRACE_AND_STOP;
}

int usb_set_interface(struct usb_device *dev, int ifnum, int alternate)
{
	TRACE;
	return 0;
}

int usb_unlink_urb(struct urb *urb)
{
	TRACE_AND_STOP;
	return -1;
}

void usleep_range(unsigned long min, unsigned long max)
{
	TRACE;
}

int sysctl_tstamp_allow_data;
struct user_namespace init_user_ns;


char  *strncpy(char *dst, const char *src, size_t s) { TRACE; return NULL; }

void ethtool_convert_legacy_u32_to_link_mode(unsigned long *dst, u32 legacy_u32) { TRACE; }
bool ethtool_convert_link_mode_to_legacy_u32(u32 *legacy_u32, const unsigned long *src) { TRACE; return true; }
int generic_handle_irq(unsigned int irqnum) { TRACE_AND_STOP; return -1; }

int phy_read_mmd(struct phy_device *phydev, int devad, u32 regnum) { TRACE_AND_STOP; return -1; }
int phy_write_mmd(struct phy_device *phydev, int devad, u32 regnum, u16 val) { TRACE_AND_STOP; return -1; }

int irq_set_chip_data(unsigned int irq, void *data) { TRACE_AND_STOP; return -1; }
void irq_set_chip_and_handler_name(unsigned int irq, struct irq_chip *chip, irq_flow_handler_t handle, const char *name) { TRACE; }
void irq_set_chip_and_handler(unsigned int irq, struct irq_chip *chip, irq_flow_handler_t handle) { TRACE; }
void irq_set_noprobe(unsigned int irq) { TRACE; }
void *irq_data_get_irq_chip_data(struct irq_data *d) { TRACE_AND_STOP; return NULL; }
unsigned int irq_create_mapping(struct irq_domain *domain, irq_hw_number_t hwirq) { TRACE; return 0xdeadbeef; }
void irq_dispose_mapping(unsigned int virq) { TRACE; }
void irq_domain_remove_irq(int virq) { TRACE; }
struct irq_domain *irq_domain_add_simple(struct device_node *of_node, unsigned int size, unsigned int first_irq, const struct irq_domain_ops *ops, void *host_data) { TRACE; return 0xdeadbeef; }
void irq_domain_remove(struct irq_domain *domain) { TRACE; }

void handle_simple_irq(struct irq_desc *desc) { TRACE; }

void pm_runtime_set_autosuspend_delay(struct device *dev, int delay) { TRACE; }

struct usb_device *usb_get_dev(struct usb_device *dev) { TRACE; return dev; }
void usb_put_dev(struct usb_device *dev) { TRACE; }

bool in_interrupt(void) { TRACE; return 0; }

int genphy_c45_restart_aneg(struct phy_device *phydev)
{
	TRACE_AND_STOP;
	return -1;
}

struct pci_dev *pci_get_device(unsigned int vendor, unsigned int device, struct pci_dev *from)
{
	TRACE_AND_STOP;
	return NULL;
}

char *kobject_name(const struct kobject *kobj)
{
	TRACE_AND_STOP;
	return NULL;
}

struct mdio_board_info;
struct mii_bus;
void mdiobus_setup_mdiodev_from_board_info(struct mii_bus *bus, int (*cb) (struct mii_bus *bus, struct mdio_board_info *bi))
{
	TRACE;
}

int mdio_device_bus_match(struct device *dev, struct device_driver *drv)
{
	TRACE_AND_STOP;
	return -1;
}

struct mdio_device *mdio_device_create(struct mii_bus *bus, int addr)
{
	TRACE_AND_STOP;
	return NULL;
}

int mdio_device_register(struct mdio_device *mdiodev)
{
	TRACE_AND_STOP;
	return -1;
}

void mdio_device_reset(struct mdio_device *mdiodev, int value)
{
	TRACE;
}

const char *phy_duplex_to_str(unsigned int duplex)
{
	TRACE_AND_STOP;
	return NULL;
}

int __phy_modify(struct phy_device *phydev, u32 regnum, u16 mask, u16 set)
{
	TRACE;
	return 0;
}

int phy_modify(struct phy_device *phydev, u32 regnum, u16 mask, u16 set)
{
	TRACE;
	return 0;
}

const char *phy_speed_to_str(int speed)
{
	TRACE_AND_STOP;
	return NULL;
}

int    strcmp(const char *s1, const char *s2)
{
	TRACE_AND_STOP;
	return -1;
}

struct module;
int try_module_get(struct module * mod)
{
	TRACE;
	return -1;
}

struct device *bus_find_device_by_name(struct bus_type *bus, struct device *start, const char *name)
{
	TRACE_AND_STOP;
	return NULL;
}

int  device_add(struct device *dev)
{
	TRACE;
	return 0;
}

int  device_bind_driver(struct device *dev)
{
	TRACE;
	return 0;
}

void device_del(struct device *dev)
{
	TRACE_AND_STOP;
}

void device_initialize(struct device *dev)
{
	TRACE;
}

int  device_register(struct device *dev)
{
	TRACE;
	return 0;
}

void device_release_driver(struct device *dev)
{
	TRACE_AND_STOP;
}

const char *dev_name(const struct device *dev)
{
	TRACE_AND_STOP;
	return NULL;
}

int dev_set_name(struct device *dev, const char *fmt, ...)
{
	TRACE;
	return 0;
}

void free_irq(unsigned int irq, void *dev_id)
{
	TRACE_AND_STOP;
}

struct device *get_device(struct device *dev)
{
	TRACE;
	return NULL;
}

irq_hw_number_t irqd_to_hwirq(struct irq_data *d)
{
	TRACE_AND_STOP;
	return -1;
}

int of_device_uevent_modalias(struct device *dev, struct kobj_uevent_env *env)
{
	TRACE_AND_STOP;
	return -1;
}

int of_driver_match_device(struct device *dev, const struct device_driver *drv)
{
	TRACE_AND_STOP;
	return -1;
}

void phy_led_trigger_change_speed(struct phy_device *phy)
{
	TRACE;
}

int phy_led_triggers_register(struct phy_device *phy)
{
	TRACE;
	return 0;
}

void phy_led_triggers_unregister(struct phy_device *phy)
{
	TRACE_AND_STOP;
}

void put_device(struct device *dev)
{
	TRACE_AND_STOP;
}

int request_module(const char *name, ...)
{
	TRACE;
	return 0;
}

int request_threaded_irq(unsigned int irq, irq_handler_t handler, irq_handler_t thread_fn, unsigned long flags, const char *name, void *dev)
{
	TRACE_AND_STOP;
	return -1;
}

int sysfs_create_link(struct kobject *kobj, struct kobject *target, const char *name)
{
	TRACE;
	return 0;
}

int sysfs_create_link_nowarn(struct kobject *kobj, struct kobject *target, const char *name)
{
	TRACE;
	return 0;
}

void sysfs_remove_link(struct kobject *kobj, const char *name)
{
	TRACE_AND_STOP;
}


struct workqueue_struct *system_power_efficient_wq;

struct phy_driver genphy_10g_driver;

void mdio_device_free(struct mdio_device *mdiodev)
{
	TRACE_AND_STOP;
}

void module_put(struct module *mod)
{
	TRACE_AND_STOP;
}

const struct phy_setting *
phy_lookup_setting(int speed, int duplex, const unsigned long *mask, size_t maxbit, bool exact)
{
	TRACE_AND_STOP;
	return NULL;
}

int driver_register(struct device_driver *drv)
{
	TRACE;
	return 0;
}

void driver_unregister(struct device_driver *drv)
{
	TRACE_AND_STOP;
}

int class_register(struct class *cls)
{
	TRACE;
	return 0;
}

void class_unregister(struct class *cls)
{
	TRACE_AND_STOP;
}

int  bus_register(struct bus_type *bus)
{
	TRACE;
	return 0;
}

void bus_unregister(struct bus_type *bus)
{
	TRACE_AND_STOP;
}

