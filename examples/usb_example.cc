// https://www.beyondlogic.org/usbnutshell/usb5.shtml
#include <bits/stdc++.h>
#include <libusb-1.0/libusb.h>
using namespace std;

#define PIXELX 1

#define ABORT_ON(ret, msg)                                                     \
  do {                                                                         \
    if (ret < 0) {                                                             \
      fprintf(stderr, "%s:%d", __FILE__, __LINE__);                            \
      fprintf(stderr, " Error:%d %s\n", ret, msg);                             \
      abort();                                                                 \
    }                                                                          \
  } while (0);

void print_devices(libusb_device **devices) {
  if (devices == NULL)
    return;

  int ret = 0;
  libusb_device *curr = NULL;
  struct libusb_device_descriptor desc;
  struct device_info {
    char VID[32];
    char PID[32];
    uint32_t bus;
    uint32_t addr;
  } device_info;

  for (int i = 0; (curr = devices[i]) != NULL; i++) {
    memset(&device_info, 0, sizeof(device_info));
    ret = libusb_get_device_descriptor(curr, &desc);
    ABORT_ON(ret, "libusb_get_device_descriptor");
    sprintf(device_info.VID, "%04X", desc.idVendor);
    sprintf(device_info.PID, "%04X", desc.idProduct);
    device_info.bus = libusb_get_bus_number(curr);
    device_info.addr = libusb_get_device_address(curr);

    printf("%d - %s:%s(Bus%d, device%d)\n", i, device_info.VID, device_info.PID,
           device_info.bus, device_info.addr);
  }
}

void list_devices() {
  libusb_device **devices; // array of libusb_device pointers, null terminated
  cout << "List of USB devices:" << endl;
  int ret = libusb_init(NULL);
  ABORT_ON(ret, "libusb_init");
  // libusb_set_option(NULL, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_DEBUG);
  ret = libusb_get_device_list(NULL, &devices);
  print_devices(devices);
  libusb_free_device_list(devices, 1);
}

libusb_device *find_device(int index) {
  libusb_device **devices;
  int ret = libusb_get_device_list(NULL, &devices);
  if (index < 0 || index >= ret)
    return NULL;
  return devices[index];
}

void print_endpoint(const struct libusb_endpoint_descriptor *endpoint) {
  cout << endl << "\t\tENDPOINT" << endl << endl;
  printf("\t\t bEndpointAddress: 0x%02X\n", endpoint->bEndpointAddress);
  printf("\t\t bmAttributes: %X\n", endpoint->bmAttributes);
  printf("\t\t wMaxPacketSize: %d\n", endpoint->wMaxPacketSize);
  printf("\t\t bInterval: %d\n", endpoint->bInterval);
  printf("\t\t bRefresh: %d\n", endpoint->bRefresh);
  printf("\t\t bSynchAddress: %d\n", endpoint->bSynchAddress);
}

void print_altsetting(const struct libusb_interface_descriptor *interface) {
  cout << endl << "\tINTERFACE" << endl << endl;
  printf("\t bInterfaceNumber: %d\n", interface->bInterfaceNumber);
  printf("\t bAlternateSetting: %d\n", interface->bAlternateSetting);
  printf("\t bNumEndpoints: %d\n", interface->bNumEndpoints);
  printf("\t bInterfaceClass: %d\n", interface->bInterfaceClass);
  printf("\t bInterfaceSubClass: %d\n", interface->bInterfaceSubClass);
  printf("\t bInterfaceProtocol: %d\n", interface->bInterfaceProtocol);
  printf("\t iInterface: %d\n", interface->iInterface);

  for (int i = 0; i < interface->bNumEndpoints; i++) {
    print_endpoint(interface->endpoint + i);
  }
}

void print_interface(const struct libusb_interface *interface) {
  for (int i = 0; i < interface->num_altsetting; i++)
    print_altsetting(interface->altsetting + i);
}

void print_configuration(const struct libusb_config_descriptor *config) {
  cout << endl << endl << "CONFIGURATION" << endl << endl;
  printf(" wTotalLength: %d\n", config->wTotalLength);
  printf(" bNumInterfaces: %d\n", config->bNumInterfaces);
  printf(" bConfigurationValue: %d\n", config->bConfigurationValue);
  printf(" iConfiguration: %d\n", config->iConfiguration);
  printf(" bmAttributes: 0x%X\n", config->bmAttributes);
  printf(" MaxPower: %dmA\n", config->MaxPower);

  for (int i = 0; i < config->bNumInterfaces; i++) {
    print_interface(config->interface + i);
  }
}

libusb_device_handle *print_device_detail(libusb_device *device) {
  if (device == NULL)
    return NULL;
  int ret = -1;
  libusb_device_handle *handle = NULL;
  libusb_device_descriptor desc;
  unsigned char data[512] = {0};

  ret = libusb_get_device_descriptor(device, &desc);
  ABORT_ON(ret, "libusb_get_device_descriptor");
  ret = libusb_open(device, &handle);
  ABORT_ON(ret, "libusb_open");

  /* manufacturer and productid */
  ret = libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, data,
                                           sizeof(data));
  ABORT_ON(ret, "libusb_get_string_descriptor_ascii");
  string manufacturer(reinterpret_cast<const char *>(data));
  memset(data, 0, sizeof(data));

  ret = libusb_get_string_descriptor_ascii(handle, desc.iProduct, data,
                                           sizeof(data));
  ABORT_ON(ret, "libusb_get_string_descriptor_ascii");
  string product(reinterpret_cast<const char *>(data));
  memset(data, 0, sizeof(data));

  cout << endl << "-Manufacturer: " << manufacturer << endl;
  cout << "-ProductID: " << product << endl;

  /* Configuration Descriptor */
  for (int i = 0; i < desc.bNumConfigurations; i++) {
    struct libusb_config_descriptor *config;
    ret = libusb_get_config_descriptor(device, i, &config);
    ABORT_ON(ret, "libusb_get_config_descriptor");
    print_configuration(config);
    libusb_free_config_descriptor(config);
  }
  return handle;
}

int main(int argc, char *argv[]) {
  int device_index = 0;
  int ret = 0;
  libusb_device_handle *handle = NULL;

  list_devices();
  cout << "Select a device whose index is: ";
  cin >> device_index;
  libusb_device *device = find_device(device_index);
  if (device == NULL) {
    printf("index out of range! exit...\n");
    return -1;
  }
  handle = print_device_detail(device);
  if (handle == NULL)
    return -1;


#if PIXELX
  /* transfer data, preset config for PIXELX.inc */
  uint8_t data[2] = {0x09, 0x80}; // expose command
  unsigned char ENDPOINT_WR = 0x08;
  unsigned char ENDPOINT_RD = 0x86;
  int INTERFACE_NUM = 0;
  int transfered = 0;

  ret = libusb_claim_interface(handle, INTERFACE_NUM);
  ABORT_ON(ret, "libusb_claim_interface");
  ret = libusb_bulk_transfer(handle, ENDPOINT_WR, data, sizeof(data),
                             &transfered, 1000); // WR
  ABORT_ON(ret, "libusb_bulk_transfer");
  libusb_release_interface(handle, 0);
  printf("WRITE:\nInterface:%d Endpoint=0x%02X Result=%d "
         "Transfered=%d\nMessage=\n%02X %02X\n",
         INTERFACE_NUM, ENDPOINT_WR, ret, transfered, data[0], data[1]);

  transfered = -1;
  memset(data, 0, sizeof(data));

  ret = libusb_claim_interface(handle, INTERFACE_NUM);
  ABORT_ON(ret, "libusb_claim_interface");
  ret = libusb_bulk_transfer(handle, ENDPOINT_RD, data, sizeof(data),
                             &transfered, 1000); // RD
  ABORT_ON(ret, "libusb_bulk_transfer");
  libusb_release_interface(handle, 0);
  printf("READ:\nInterface:%d Endpoint=0x%02X Result=%d "
         "Transfered=%d\nMessage=\n%02X %02X\n",
         INTERFACE_NUM, ENDPOINT_WR, ret, transfered, data[0], data[1]);
#endif

  libusb_exit(NULL);
  return 0;
}