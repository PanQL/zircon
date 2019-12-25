#define HAS_DEVICE_TREE 1

// FIXME
static const zbi_platform_id_t platform_id = {
    .vid = PDEV_VID_ARM,
    .pid = PDEV_PID_HIKEY960,
    .board_name = "hikey960",
};

static void append_board_boot_item(zbi_header_t* bootdata) {
}
