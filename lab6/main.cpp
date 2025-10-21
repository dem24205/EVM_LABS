#include <libusb.h>
#include <stdio.h>

using namespace std;

const char* get_device_class_name(uint8_t class_code);
const char* get_vendor_name(uint16_t vendor_id);
const char* get_product_name(uint16_t vendor_id, uint16_t product_id);
void print_device_info(libusb_device* device, int device_number);
void print_usb_devices_list(libusb_device** devices, ssize_t device_count);

int main() {
    libusb_context* usb_context = nullptr;
    libusb_device** devices_list = nullptr;
    int operation_result;
    ssize_t devices_count;

    operation_result = libusb_init(&usb_context);
    if (operation_result < 0) {
        fprintf(stderr, "Error: libusb initialization failed! Error code: %d\n", operation_result);
        return 1;
    }
    // libusb_set_option(usb_context, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_DEBUG);
    devices_count = libusb_get_device_list(usb_context, &devices_list);
    if (devices_count < 0) {
        fprintf(stderr, "Error: Failed to get USB devices list!\n");
        libusb_exit(usb_context);
        return 1;
    }

    printf("\n");
    printf("Detected USB devices: %zd\n", devices_count);
    printf("===========================================================\n\n");

    if (devices_count > 0) {
        print_usb_devices_list(devices_list, devices_count);
    }
    else {
        printf("No USB devices detected\n");
    }

    printf("\n===========================================================\n");

    libusb_free_device_list(devices_list, 1);
    libusb_exit(usb_context);
    return 0;
}

const char* get_device_class_name(uint8_t class_code) {
    switch (class_code) {
    case 0x00: return "Interface Specific";
    case 0x01: return "Audio Device";
    case 0x02: return "Network Adapter";
    case 0x03: return "HID (Keyboard, Mouse, Joystick)";
    case 0x05: return "Physical Interface";
    case 0x06: return "Image Processing";
    case 0x07: return "Printer";
    case 0x08: return "Mass Storage (Flash Drive, HDD)";
    case 0x09: return "USB Hub";
    case 0x0A: return "CDC Data (Modem, COM Port)";
    case 0x0B: return "Smart Card";
    case 0x0D: return "Security Device";
    case 0x0E: return "Video Device (Camera)";
    case 0x0F: return "Personal Healthcare";
    case 0x10: return "Audio/Video Device";
    case 0x11: return "USB Billboard";
    case 0x12: return "USB Type-C Bridge";
    case 0xDC: return "Diagnostic Device";
    case 0xE0: return "Wireless Controller (Bluetooth, Wi-Fi)";
    case 0xEF: return "Miscellaneous";
    case 0xFE: return "Application Specific";
    case 0xFF: return "Vendor Specific";
    default:   return "Unknown Class";
    }
}

const char* get_vendor_name(uint16_t vendor_id) {
    switch (vendor_id) {
    case 0x2b7e: return "Sony";
    case 0x8086: return "Intel";
    case 0x8087: return "Intel";
    case 0x04f2: return "Chicony";
    case 0x1d6b: return "Linux Foundation";
    case 0x046d: return "Logitech";
    case 0x045e: return "Microsoft";
    case 0x0bda: return "Realtek";
    case 0x0781: return "SanDisk";
    case 0x0951: return "Kingston";
    case 0x413c: return "Dell";
    case 0x0489: return "Foxconn";
    case 0x04e8: return "Samsung";
    case 0x0cf3: return "Qualcomm";
    case 0x0a5c: return "Broadcom";
    case 0x1050: return "Yubico";
    case 0x18d1: return "Google";
    default: return "Unknown Vendor";
    }
}
const char* get_product_name(uint16_t vendor_id, uint16_t product_id) {
    if (vendor_id == 0x8086) {
        switch (product_id) {
        case 0x51ed: return "USB Hub";
        case 0x461e: return "USB Controller";
        case 0x02e8: return "Thunderbolt";
        case 0x0aa7: return "Wireless Hub";
        default: return "Intel Device";
        }
    }
    else if (vendor_id == 0x8087) {
        switch (product_id) {
        case 0x0026: return "Bluetooth Controller";
        case 0x0033: return "Wireless Controller";
        case 0x07dc: return "Wireless Adapter";
        case 0x0a2b: return "Bluetooth Radio";
        default: return "Intel Wireless Device";
        }
    }
    else if (vendor_id == 0x2b7e) {
        switch (product_id) {
        case 0xb685: return "USB Composite Device";
        case 0x0900: return "PlayStation Controller";
        case 0x0901: return "DualShock Controller";
        default: return "Sony Device";
        }
    }
    else if (vendor_id == 0x04f2) {
        switch (product_id) {
        case 0x0729: return "USB Camera";
        case 0xb5d9: return "Webcam";
        case 0xb6a5: return "Keyboard";
        default: return "Chicony Device";
        }
    }
    else if (vendor_id == 0x1d6b) {
        switch (product_id) {
        case 0x0002: return "USB Hub";
        case 0x0003: return "USB Host Controller";
        case 0x0001: return "USB Device";
        default: return "System Device";
        }
    }
    else if (vendor_id == 0x046d) {
        switch (product_id) {
        case 0xc52b: return "Unifying Receiver";
        case 0xc539: return "Keyboard";
        case 0xc07d: return "Mouse";
        default: return "Logitech Device";
        }
    }

    return "Unknown Device";
}

void print_device_info(libusb_device* device, int device_number) {
    libusb_device_descriptor descriptor;
    libusb_config_descriptor* config = nullptr;
    libusb_device_handle* device_handle = nullptr;
    unsigned char serial_buffer[256];
    int operation_result;

    operation_result = libusb_get_device_descriptor(device, &descriptor);
    if (operation_result < 0) {
        fprintf(stderr, "Error: Failed to get device descriptor! Code: %d\n", operation_result);
        return;
    }

    libusb_get_config_descriptor(device, 0, &config);

    const char* serial_number = "[not available]";
    int string_length = 0;

    if (libusb_open(device, &device_handle) == 0 && descriptor.iSerialNumber) {
        string_length = libusb_get_string_descriptor_ascii(
            device_handle,
            descriptor.iSerialNumber,
            serial_buffer,
            sizeof(serial_buffer)
        );
        if (string_length > 0) {
            serial_number = (char*)serial_buffer;
        }
        else {
            serial_number = "[no access]";
        }
    }

    const char* vendor_name = get_vendor_name(descriptor.idVendor);
    const char* product_name = get_product_name(descriptor.idVendor, descriptor.idProduct);
    const char* class_name = get_device_class_name(descriptor.bDeviceClass);

    printf("Device %d:\n", device_number);
    printf("  Configurations:  %d\n", descriptor.bNumConfigurations);
    printf("  Class:           %d (0x%02x) - %s\n",
        descriptor.bDeviceClass, descriptor.bDeviceClass, class_name);
    printf("  Vendor:          %04x (%s)\n", descriptor.idVendor, vendor_name);
    printf("  Product:         %04x (%s)\n", descriptor.idProduct, product_name);
    printf("  Interfaces:      %d\n", config ? config->bNumInterfaces : 0);
    printf("  Serial number:   %s\n", serial_number);
    printf("  Bus:             %d\n", libusb_get_bus_number(device));
    printf("  Address:         %d\n", libusb_get_device_address(device));

    if (device_handle) {
        libusb_close(device_handle);
    }

    if (config) {
        libusb_free_config_descriptor(config);
    }
}
void print_usb_devices_list(libusb_device** devices, ssize_t device_count) {
    for (ssize_t i = 0; i < device_count; i++) {
        print_device_info(devices[i], i + 1);
        if (i < device_count - 1) {
            printf("-----------------------------------------------------------\n");
        }
    }
}
