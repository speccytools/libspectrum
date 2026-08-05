#include "config.h"

#include <stdio.h>

#include "test.h"

test_return_t
snap_custom_rom_flag_and_custom_rom_pages_getter_setter( void )
{
  /* libspectrum_snap: custom_rom flag and custom_rom_pages getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_custom_rom_flag_and_custom_rom_pages_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_custom_rom( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_custom_rom_flag_and_custom_rom_pages_getter_setter: default custom_rom should be 0, got %d\n",
             progname, libspectrum_snap_custom_rom( snap ) );
    goto done;
  }

  libspectrum_snap_set_custom_rom( snap, 1 );
  if( libspectrum_snap_custom_rom( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_custom_rom_flag_and_custom_rom_pages_getter_setter: expected custom_rom=1, got %d\n",
             progname, libspectrum_snap_custom_rom( snap ) );
    goto done;
  }

  libspectrum_snap_set_custom_rom( snap, 0 );
  if( libspectrum_snap_custom_rom( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_custom_rom_flag_and_custom_rom_pages_getter_setter: expected custom_rom=0, got %d\n",
             progname, libspectrum_snap_custom_rom( snap ) );
    goto done;
  }

  if( libspectrum_snap_custom_rom_pages( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_custom_rom_flag_and_custom_rom_pages_getter_setter: default custom_rom_pages should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_snap_custom_rom_pages( snap ) );
    goto done;
  }

  libspectrum_snap_set_custom_rom_pages( snap, 2 );
  if( libspectrum_snap_custom_rom_pages( snap ) != 2 ) {
    fprintf( stderr, "%s: snap_custom_rom_flag_and_custom_rom_pages_getter_setter: expected custom_rom_pages=2, got %lu\n",
             progname, (unsigned long)libspectrum_snap_custom_rom_pages( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_ram_pages_getter_setter( void )
{
  /* libspectrum_snap: RAM pages getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  libspectrum_byte *page;
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_ram_pages_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_pages( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: snap_ram_pages_getter_setter: default pages[0] should be NULL\n", progname );
    goto done;
  }

  page = libspectrum_new( libspectrum_byte, 0x4000 );
  page[0] = 0xaa;
  page[0x3fff] = 0x55;

  libspectrum_snap_set_pages( snap, 0, page );
  if( libspectrum_snap_pages( snap, 0 ) != page ) {
    fprintf( stderr, "%s: snap_ram_pages_getter_setter: pages[0] pointer mismatch after set\n", progname );
    libspectrum_free( page );
    goto done;
  }

  if( libspectrum_snap_pages( snap, 0 )[0] != 0xaa ||
      libspectrum_snap_pages( snap, 0 )[0x3fff] != 0x55 ) {
    fprintf( stderr, "%s: snap_ram_pages_getter_setter: pages[0] data mismatch\n", progname );
    goto done;
  }

  if( libspectrum_snap_pages( snap, 1 ) != NULL ) {
    fprintf( stderr, "%s: snap_ram_pages_getter_setter: pages[1] should still be NULL\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  /* snap_free frees all pages including the one we set */
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_interface1_active_paged_and_drive_count_getter_setter( void )
{
  /* libspectrum_snap: interface1 active, paged, and drive_count getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_interface1_active_paged_and_drive_count_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_interface1_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_interface1_active_paged_and_drive_count_getter_setter: default interface1_active should be 0, got %d\n",
             progname, libspectrum_snap_interface1_active( snap ) );
    goto done;
  }

  libspectrum_snap_set_interface1_active( snap, 1 );
  if( libspectrum_snap_interface1_active( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_interface1_active_paged_and_drive_count_getter_setter: expected interface1_active=1, got %d\n",
             progname, libspectrum_snap_interface1_active( snap ) );
    goto done;
  }

  if( libspectrum_snap_interface1_paged( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_interface1_active_paged_and_drive_count_getter_setter: default interface1_paged should be 0, got %d\n",
             progname, libspectrum_snap_interface1_paged( snap ) );
    goto done;
  }

  libspectrum_snap_set_interface1_paged( snap, 1 );
  if( libspectrum_snap_interface1_paged( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_interface1_active_paged_and_drive_count_getter_setter: expected interface1_paged=1, got %d\n",
             progname, libspectrum_snap_interface1_paged( snap ) );
    goto done;
  }

  if( libspectrum_snap_interface1_drive_count( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_interface1_active_paged_and_drive_count_getter_setter: default interface1_drive_count should be 0, got %d\n",
             progname, libspectrum_snap_interface1_drive_count( snap ) );
    goto done;
  }

  libspectrum_snap_set_interface1_drive_count( snap, 4 );
  if( libspectrum_snap_interface1_drive_count( snap ) != 4 ) {
    fprintf( stderr, "%s: snap_interface1_active_paged_and_drive_count_getter_setter: expected interface1_drive_count=4, got %d\n",
             progname, libspectrum_snap_interface1_drive_count( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_ay_registers_array_getter_setter_all_16_registers( void )
{
  /* libspectrum_snap: ay_registers array getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;
  int i;

  if( !snap ) {
    fprintf( stderr, "%s: snap_ay_registers_array_getter_setter_all_16_registers: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  /* Default: all 16 AY registers should be zero */
  for( i = 0; i < 16; i++ ) {
    if( libspectrum_snap_ay_registers( snap, i ) != 0 ) {
      fprintf( stderr, "%s: snap_ay_registers_array_getter_setter_all_16_registers: default ay_registers[%d] should be 0, got %d\n",
               progname, i, libspectrum_snap_ay_registers( snap, i ) );
      goto done;
    }
  }

  /* Set each register to a unique value and verify round-trip */
  for( i = 0; i < 16; i++ )
    libspectrum_snap_set_ay_registers( snap, i, (libspectrum_byte)(i + 1) );

  for( i = 0; i < 16; i++ ) {
    if( libspectrum_snap_ay_registers( snap, i ) != (libspectrum_byte)(i + 1) ) {
      fprintf( stderr, "%s: snap_ay_registers_array_getter_setter_all_16_registers: expected ay_registers[%d]=%d, got %d\n",
               progname, i, i + 1, libspectrum_snap_ay_registers( snap, i ) );
      goto done;
    }
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_divide_pages_count_and_divide_eprom_pointer_getter_setter( void )
{
  /* libspectrum_snap: DivIDE pages count and divide_eprom single-pointer */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  libspectrum_byte *eprom;
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_divide_pages_count_and_divide_eprom_pointer_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_divide_pages( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_divide_pages_count_and_divide_eprom_pointer_getter_setter: default divide_pages should be 0, got %zu\n",
             progname, libspectrum_snap_divide_pages( snap ) );
    goto done;
  }
  libspectrum_snap_set_divide_pages( snap, 3 );
  if( libspectrum_snap_divide_pages( snap ) != 3 ) {
    fprintf( stderr, "%s: snap_divide_pages_count_and_divide_eprom_pointer_getter_setter: expected divide_pages=3, got %zu\n",
             progname, libspectrum_snap_divide_pages( snap ) );
    goto done;
  }

  if( libspectrum_snap_divide_eprom( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: snap_divide_pages_count_and_divide_eprom_pointer_getter_setter: default divide_eprom[0] should be NULL\n", progname );
    goto done;
  }

  eprom = libspectrum_new( libspectrum_byte, 0x2000 );
  eprom[0]      = 0xde;
  eprom[0x1fff] = 0xad;

  libspectrum_snap_set_divide_eprom( snap, 0, eprom );
  if( libspectrum_snap_divide_eprom( snap, 0 ) != eprom ) {
    fprintf( stderr, "%s: snap_divide_pages_count_and_divide_eprom_pointer_getter_setter: divide_eprom[0] pointer mismatch after set\n", progname );
    libspectrum_free( eprom );
    goto done;
  }
  if( libspectrum_snap_divide_eprom( snap, 0 )[0]      != 0xde ||
      libspectrum_snap_divide_eprom( snap, 0 )[0x1fff] != 0xad ) {
    fprintf( stderr, "%s: snap_divide_pages_count_and_divide_eprom_pointer_getter_setter: divide_eprom[0] data mismatch\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_divide_ram_page_pointer_array_getter_setter( void )
{
  /* libspectrum_snap: DivIDE RAM page pointer array (SNAPSHOT_DIVIDE_PAGES pages) */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  libspectrum_byte *page0, *page1;
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_divide_ram_page_pointer_array_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_divide_ram( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: snap_divide_ram_page_pointer_array_getter_setter: default divide_ram[0] should be NULL\n", progname );
    goto done;
  }

  page0 = libspectrum_new( libspectrum_byte, 0x2000 );
  page0[0]      = 0x11;
  page0[0x1fff] = 0x22;

  libspectrum_snap_set_divide_ram( snap, 0, page0 );
  if( libspectrum_snap_divide_ram( snap, 0 ) != page0 ) {
    fprintf( stderr, "%s: snap_divide_ram_page_pointer_array_getter_setter: divide_ram[0] pointer mismatch after set\n", progname );
    libspectrum_free( page0 );
    goto done;
  }
  if( libspectrum_snap_divide_ram( snap, 0 )[0]      != 0x11 ||
      libspectrum_snap_divide_ram( snap, 0 )[0x1fff] != 0x22 ) {
    fprintf( stderr, "%s: snap_divide_ram_page_pointer_array_getter_setter: divide_ram[0] data mismatch\n", progname );
    goto done;
  }

  page1 = libspectrum_new( libspectrum_byte, 0x2000 );
  page1[0] = 0x33;
  libspectrum_snap_set_divide_ram( snap, 1, page1 );

  if( libspectrum_snap_divide_ram( snap, 2 ) != NULL ) {
    fprintf( stderr, "%s: snap_divide_ram_page_pointer_array_getter_setter: divide_ram[2] should still be NULL\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_divmmc_active_eprom_writeprotect_paged_and_control_getter_setter( void )
{
  /* libspectrum_snap: DivMMC flags (active, eprom_writeprotect, paged, control) */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_divmmc_active_eprom_writeprotect_paged_and_control_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_divmmc_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_divmmc_active_eprom_writeprotect_paged_and_control_getter_setter: default divmmc_active should be 0, got %d\n",
             progname, libspectrum_snap_divmmc_active( snap ) );
    goto done;
  }
  libspectrum_snap_set_divmmc_active( snap, 1 );
  if( libspectrum_snap_divmmc_active( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_divmmc_active_eprom_writeprotect_paged_and_control_getter_setter: expected divmmc_active=1, got %d\n",
             progname, libspectrum_snap_divmmc_active( snap ) );
    goto done;
  }

  if( libspectrum_snap_divmmc_eprom_writeprotect( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_divmmc_active_eprom_writeprotect_paged_and_control_getter_setter: default divmmc_eprom_writeprotect should be 0, got %d\n",
             progname, libspectrum_snap_divmmc_eprom_writeprotect( snap ) );
    goto done;
  }
  libspectrum_snap_set_divmmc_eprom_writeprotect( snap, 1 );
  if( libspectrum_snap_divmmc_eprom_writeprotect( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_divmmc_active_eprom_writeprotect_paged_and_control_getter_setter: expected divmmc_eprom_writeprotect=1, got %d\n",
             progname, libspectrum_snap_divmmc_eprom_writeprotect( snap ) );
    goto done;
  }

  if( libspectrum_snap_divmmc_paged( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_divmmc_active_eprom_writeprotect_paged_and_control_getter_setter: default divmmc_paged should be 0, got %d\n",
             progname, libspectrum_snap_divmmc_paged( snap ) );
    goto done;
  }
  libspectrum_snap_set_divmmc_paged( snap, 1 );
  if( libspectrum_snap_divmmc_paged( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_divmmc_active_eprom_writeprotect_paged_and_control_getter_setter: expected divmmc_paged=1, got %d\n",
             progname, libspectrum_snap_divmmc_paged( snap ) );
    goto done;
  }

  if( libspectrum_snap_divmmc_control( snap ) != 0x00 ) {
    fprintf( stderr, "%s: snap_divmmc_active_eprom_writeprotect_paged_and_control_getter_setter: default divmmc_control should be 0, got 0x%02x\n",
             progname, libspectrum_snap_divmmc_control( snap ) );
    goto done;
  }
  libspectrum_snap_set_divmmc_control( snap, 0xc7 );
  if( libspectrum_snap_divmmc_control( snap ) != 0xc7 ) {
    fprintf( stderr, "%s: snap_divmmc_active_eprom_writeprotect_paged_and_control_getter_setter: expected divmmc_control=0xc7, got 0x%02x\n",
             progname, libspectrum_snap_divmmc_control( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_divmmc_pages_count_and_divmmc_eprom_pointer_getter_setter( void )
{
  /* libspectrum_snap: DivMMC pages count and divmmc_eprom single-pointer */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  libspectrum_byte *eprom;
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_divmmc_pages_count_and_divmmc_eprom_pointer_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_divmmc_pages( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_divmmc_pages_count_and_divmmc_eprom_pointer_getter_setter: default divmmc_pages should be 0, got %zu\n",
             progname, libspectrum_snap_divmmc_pages( snap ) );
    goto done;
  }
  libspectrum_snap_set_divmmc_pages( snap, 16 );
  if( libspectrum_snap_divmmc_pages( snap ) != 16 ) {
    fprintf( stderr, "%s: snap_divmmc_pages_count_and_divmmc_eprom_pointer_getter_setter: expected divmmc_pages=16, got %zu\n",
             progname, libspectrum_snap_divmmc_pages( snap ) );
    goto done;
  }

  if( libspectrum_snap_divmmc_eprom( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: snap_divmmc_pages_count_and_divmmc_eprom_pointer_getter_setter: default divmmc_eprom[0] should be NULL\n", progname );
    goto done;
  }

  eprom = libspectrum_new( libspectrum_byte, 0x4000 );
  eprom[0]      = 0xbe;
  eprom[0x3fff] = 0xef;

  libspectrum_snap_set_divmmc_eprom( snap, 0, eprom );
  if( libspectrum_snap_divmmc_eprom( snap, 0 ) != eprom ) {
    fprintf( stderr, "%s: snap_divmmc_pages_count_and_divmmc_eprom_pointer_getter_setter: divmmc_eprom[0] pointer mismatch after set\n", progname );
    libspectrum_free( eprom );
    goto done;
  }
  if( libspectrum_snap_divmmc_eprom( snap, 0 )[0]      != 0xbe ||
      libspectrum_snap_divmmc_eprom( snap, 0 )[0x3fff] != 0xef ) {
    fprintf( stderr, "%s: snap_divmmc_pages_count_and_divmmc_eprom_pointer_getter_setter: divmmc_eprom[0] data mismatch\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_zxatasp_ram_page_pointer_array_getter_setter( void )
{
  /* libspectrum_snap: ZXATASP RAM page pointer array (SNAPSHOT_ZXATASP_PAGES pages) */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  libspectrum_byte *page0, *page1;
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_zxatasp_ram_page_pointer_array_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_zxatasp_ram( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: snap_zxatasp_ram_page_pointer_array_getter_setter: default zxatasp_ram[0] should be NULL\n", progname );
    goto done;
  }

  page0 = libspectrum_new( libspectrum_byte, 0x4000 );
  page0[0]      = 0x11;
  page0[0x3fff] = 0x22;

  libspectrum_snap_set_zxatasp_ram( snap, 0, page0 );
  if( libspectrum_snap_zxatasp_ram( snap, 0 ) != page0 ) {
    fprintf( stderr, "%s: snap_zxatasp_ram_page_pointer_array_getter_setter: zxatasp_ram[0] pointer mismatch after set\n", progname );
    libspectrum_free( page0 );
    goto done;
  }
  if( libspectrum_snap_zxatasp_ram( snap, 0 )[0]      != 0x11 ||
      libspectrum_snap_zxatasp_ram( snap, 0 )[0x3fff] != 0x22 ) {
    fprintf( stderr, "%s: snap_zxatasp_ram_page_pointer_array_getter_setter: zxatasp_ram[0] data mismatch\n", progname );
    goto done;
  }

  page1 = libspectrum_new( libspectrum_byte, 0x4000 );
  page1[0] = 0x33;
  libspectrum_snap_set_zxatasp_ram( snap, 1, page1 );

  if( libspectrum_snap_zxatasp_ram( snap, 2 ) != NULL ) {
    fprintf( stderr, "%s: snap_zxatasp_ram_page_pointer_array_getter_setter: zxatasp_ram[2] should still be NULL\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_divmmc_ram_page_pointer_array_getter_setter( void )
{
  /* libspectrum_snap: DivMMC RAM page pointer array (SNAPSHOT_DIVMMC_PAGES pages) */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  libspectrum_byte *page0, *page1;
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_divmmc_ram_page_pointer_array_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_divmmc_ram( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: snap_divmmc_ram_page_pointer_array_getter_setter: default divmmc_ram[0] should be NULL\n", progname );
    goto done;
  }

  page0 = libspectrum_new( libspectrum_byte, 0x2000 );
  page0[0]      = 0x11;
  page0[0x1fff] = 0x22;

  libspectrum_snap_set_divmmc_ram( snap, 0, page0 );
  if( libspectrum_snap_divmmc_ram( snap, 0 ) != page0 ) {
    fprintf( stderr, "%s: snap_divmmc_ram_page_pointer_array_getter_setter: divmmc_ram[0] pointer mismatch after set\n", progname );
    libspectrum_free( page0 );
    goto done;
  }
  if( libspectrum_snap_divmmc_ram( snap, 0 )[0]      != 0x11 ||
      libspectrum_snap_divmmc_ram( snap, 0 )[0x1fff] != 0x22 ) {
    fprintf( stderr, "%s: snap_divmmc_ram_page_pointer_array_getter_setter: divmmc_ram[0] data mismatch\n", progname );
    goto done;
  }

  page1 = libspectrum_new( libspectrum_byte, 0x2000 );
  page1[0] = 0x33;
  libspectrum_snap_set_divmmc_ram( snap, 1, page1 );

  if( libspectrum_snap_divmmc_ram( snap, 2 ) != NULL ) {
    fprintf( stderr, "%s: snap_divmmc_ram_page_pointer_array_getter_setter: divmmc_ram[2] should still be NULL\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_tc2068_dock_exrom_active_cart_pointer_and_ram_flag_getter_setter( void )
{
  /* libspectrum_snap: TC2068 DOCK/EXROM dock_active flag, cart pointers, and ram (writeable) flags */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  libspectrum_byte *dock_data, *exrom_data;
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_tc2068_dock_exrom_active_cart_pointer_and_ram_flag_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_dock_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_tc2068_dock_exrom_active_cart_pointer_and_ram_flag_getter_setter: default dock_active should be 0, got %d\n",
             progname, libspectrum_snap_dock_active( snap ) );
    goto done;
  }
  libspectrum_snap_set_dock_active( snap, 1 );
  if( libspectrum_snap_dock_active( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_tc2068_dock_exrom_active_cart_pointer_and_ram_flag_getter_setter: expected dock_active=1, got %d\n",
             progname, libspectrum_snap_dock_active( snap ) );
    goto done;
  }

  if( libspectrum_snap_dock_ram( snap, 0 ) != 0 ) {
    fprintf( stderr, "%s: snap_tc2068_dock_exrom_active_cart_pointer_and_ram_flag_getter_setter: default dock_ram[0] should be 0, got %d\n",
             progname, libspectrum_snap_dock_ram( snap, 0 ) );
    goto done;
  }
  libspectrum_snap_set_dock_ram( snap, 0, 1 );
  if( libspectrum_snap_dock_ram( snap, 0 ) != 1 ) {
    fprintf( stderr, "%s: snap_tc2068_dock_exrom_active_cart_pointer_and_ram_flag_getter_setter: expected dock_ram[0]=1, got %d\n",
             progname, libspectrum_snap_dock_ram( snap, 0 ) );
    goto done;
  }

  if( libspectrum_snap_dock_cart( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: snap_tc2068_dock_exrom_active_cart_pointer_and_ram_flag_getter_setter: default dock_cart[0] should be NULL\n", progname );
    goto done;
  }

  dock_data = libspectrum_new( libspectrum_byte, 0x2000 );
  dock_data[0]      = 0xab;
  dock_data[0x1fff] = 0xcd;

  libspectrum_snap_set_dock_cart( snap, 0, dock_data );
  if( libspectrum_snap_dock_cart( snap, 0 ) != dock_data ) {
    fprintf( stderr, "%s: snap_tc2068_dock_exrom_active_cart_pointer_and_ram_flag_getter_setter: dock_cart[0] pointer mismatch after set\n", progname );
    libspectrum_free( dock_data );
    goto done;
  }
  if( libspectrum_snap_dock_cart( snap, 0 )[0]      != 0xab ||
      libspectrum_snap_dock_cart( snap, 0 )[0x1fff] != 0xcd ) {
    fprintf( stderr, "%s: snap_tc2068_dock_exrom_active_cart_pointer_and_ram_flag_getter_setter: dock_cart[0] data mismatch\n", progname );
    goto done;
  }

  if( libspectrum_snap_exrom_ram( snap, 0 ) != 0 ) {
    fprintf( stderr, "%s: snap_tc2068_dock_exrom_active_cart_pointer_and_ram_flag_getter_setter: default exrom_ram[0] should be 0, got %d\n",
             progname, libspectrum_snap_exrom_ram( snap, 0 ) );
    goto done;
  }
  libspectrum_snap_set_exrom_ram( snap, 0, 1 );
  if( libspectrum_snap_exrom_ram( snap, 0 ) != 1 ) {
    fprintf( stderr, "%s: snap_tc2068_dock_exrom_active_cart_pointer_and_ram_flag_getter_setter: expected exrom_ram[0]=1, got %d\n",
             progname, libspectrum_snap_exrom_ram( snap, 0 ) );
    goto done;
  }

  if( libspectrum_snap_exrom_cart( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: snap_tc2068_dock_exrom_active_cart_pointer_and_ram_flag_getter_setter: default exrom_cart[0] should be NULL\n", progname );
    goto done;
  }

  exrom_data = libspectrum_new( libspectrum_byte, 0x2000 );
  exrom_data[0]      = 0xef;
  exrom_data[0x1fff] = 0x01;

  libspectrum_snap_set_exrom_cart( snap, 0, exrom_data );
  if( libspectrum_snap_exrom_cart( snap, 0 ) != exrom_data ) {
    fprintf( stderr, "%s: snap_tc2068_dock_exrom_active_cart_pointer_and_ram_flag_getter_setter: exrom_cart[0] pointer mismatch after set\n", progname );
    libspectrum_free( exrom_data );
    goto done;
  }
  if( libspectrum_snap_exrom_cart( snap, 0 )[0]      != 0xef ||
      libspectrum_snap_exrom_cart( snap, 0 )[0x1fff] != 0x01 ) {
    fprintf( stderr, "%s: snap_tc2068_dock_exrom_active_cart_pointer_and_ram_flag_getter_setter: exrom_cart[0] data mismatch\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_custom_roms_pointer_array_and_rom_length_getter_setter( void )
{
  /* libspectrum_snap: roms[4] custom ROM pointer array and rom_length[4] size array getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  libspectrum_byte *rom0, *rom3;
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_custom_roms_pointer_array_and_rom_length_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_roms( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: snap_custom_roms_pointer_array_and_rom_length_getter_setter: default roms[0] should be NULL\n", progname );
    goto done;
  }

  if( libspectrum_snap_rom_length( snap, 0 ) != 0 ) {
    fprintf( stderr, "%s: snap_custom_roms_pointer_array_and_rom_length_getter_setter: default rom_length[0] should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_snap_rom_length( snap, 0 ) );
    goto done;
  }

  rom0 = libspectrum_new( libspectrum_byte, 0x4000 );
  rom0[0]      = 0xf3;
  rom0[0x3fff] = 0xfe;

  libspectrum_snap_set_roms( snap, 0, rom0 );
  libspectrum_snap_set_rom_length( snap, 0, 0x4000 );

  if( libspectrum_snap_roms( snap, 0 ) != rom0 ) {
    fprintf( stderr, "%s: snap_custom_roms_pointer_array_and_rom_length_getter_setter: roms[0] pointer mismatch after set\n", progname );
    libspectrum_free( rom0 );
    goto done;
  }
  if( libspectrum_snap_roms( snap, 0 )[0]      != 0xf3 ||
      libspectrum_snap_roms( snap, 0 )[0x3fff] != 0xfe ) {
    fprintf( stderr, "%s: snap_custom_roms_pointer_array_and_rom_length_getter_setter: roms[0] data mismatch\n", progname );
    goto done;
  }
  if( libspectrum_snap_rom_length( snap, 0 ) != 0x4000 ) {
    fprintf( stderr, "%s: snap_custom_roms_pointer_array_and_rom_length_getter_setter: rom_length[0] should be 0x4000, got %lu\n",
             progname, (unsigned long)libspectrum_snap_rom_length( snap, 0 ) );
    goto done;
  }

  if( libspectrum_snap_roms( snap, 1 ) != NULL ) {
    fprintf( stderr, "%s: snap_custom_roms_pointer_array_and_rom_length_getter_setter: roms[1] should still be NULL\n", progname );
    goto done;
  }

  rom3 = libspectrum_new( libspectrum_byte, 0x2000 );
  rom3[0]      = 0x11;
  rom3[0x1fff] = 0x22;

  libspectrum_snap_set_roms( snap, 3, rom3 );
  libspectrum_snap_set_rom_length( snap, 3, 0x2000 );

  if( libspectrum_snap_roms( snap, 3 ) != rom3 ) {
    fprintf( stderr, "%s: snap_custom_roms_pointer_array_and_rom_length_getter_setter: roms[3] pointer mismatch after set\n", progname );
    libspectrum_free( rom3 );
    goto done;
  }
  if( libspectrum_snap_rom_length( snap, 3 ) != 0x2000 ) {
    fprintf( stderr, "%s: snap_custom_roms_pointer_array_and_rom_length_getter_setter: rom_length[3] should be 0x2000, got %lu\n",
             progname, (unsigned long)libspectrum_snap_rom_length( snap, 3 ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_slt_level_data_length_screen_and_screen_level_getter_setter( void )
{
  /* libspectrum_snap: slt[] pointer array, slt_length[] size array, slt_screen pointer, and slt_screen_level getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  libspectrum_byte *level_data, *screen;
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_slt_level_data_length_screen_and_screen_level_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_slt( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: snap_slt_level_data_length_screen_and_screen_level_getter_setter: default slt[0] should be NULL\n", progname );
    goto done;
  }

  if( libspectrum_snap_slt_length( snap, 0 ) != 0 ) {
    fprintf( stderr, "%s: snap_slt_level_data_length_screen_and_screen_level_getter_setter: default slt_length[0] should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_snap_slt_length( snap, 0 ) );
    goto done;
  }

  level_data = libspectrum_new( libspectrum_byte, 0x1000 );
  level_data[0]      = 0xab;
  level_data[0x0fff] = 0xcd;

  libspectrum_snap_set_slt( snap, 0, level_data );
  libspectrum_snap_set_slt_length( snap, 0, 0x1000 );

  if( libspectrum_snap_slt( snap, 0 ) != level_data ) {
    fprintf( stderr, "%s: snap_slt_level_data_length_screen_and_screen_level_getter_setter: slt[0] pointer mismatch after set\n", progname );
    libspectrum_free( level_data );
    goto done;
  }
  if( libspectrum_snap_slt( snap, 0 )[0]      != 0xab ||
      libspectrum_snap_slt( snap, 0 )[0x0fff] != 0xcd ) {
    fprintf( stderr, "%s: snap_slt_level_data_length_screen_and_screen_level_getter_setter: slt[0] data mismatch\n", progname );
    goto done;
  }
  if( libspectrum_snap_slt_length( snap, 0 ) != 0x1000 ) {
    fprintf( stderr, "%s: snap_slt_level_data_length_screen_and_screen_level_getter_setter: slt_length[0] should be 0x1000, got %lu\n",
             progname, (unsigned long)libspectrum_snap_slt_length( snap, 0 ) );
    goto done;
  }

  if( libspectrum_snap_slt( snap, 1 ) != NULL ) {
    fprintf( stderr, "%s: snap_slt_level_data_length_screen_and_screen_level_getter_setter: slt[1] should still be NULL\n", progname );
    goto done;
  }

  if( libspectrum_snap_slt_screen( snap ) != NULL ) {
    fprintf( stderr, "%s: snap_slt_level_data_length_screen_and_screen_level_getter_setter: default slt_screen should be NULL\n", progname );
    goto done;
  }

  screen = libspectrum_new( libspectrum_byte, 6912 );
  screen[0]    = 0x55;
  screen[6911] = 0xaa;

  libspectrum_snap_set_slt_screen( snap, screen );

  if( libspectrum_snap_slt_screen( snap ) != screen ) {
    fprintf( stderr, "%s: snap_slt_level_data_length_screen_and_screen_level_getter_setter: slt_screen pointer mismatch after set\n", progname );
    libspectrum_free( screen );
    goto done;
  }
  if( libspectrum_snap_slt_screen( snap )[0]    != 0x55 ||
      libspectrum_snap_slt_screen( snap )[6911] != 0xaa ) {
    fprintf( stderr, "%s: snap_slt_level_data_length_screen_and_screen_level_getter_setter: slt_screen data mismatch\n", progname );
    goto done;
  }

  if( libspectrum_snap_slt_screen_level( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_slt_level_data_length_screen_and_screen_level_getter_setter: default slt_screen_level should be 0, got %d\n",
             progname, libspectrum_snap_slt_screen_level( snap ) );
    goto done;
  }

  libspectrum_snap_set_slt_screen_level( snap, 3 );
  if( libspectrum_snap_slt_screen_level( snap ) != 3 ) {
    fprintf( stderr, "%s: snap_slt_level_data_length_screen_and_screen_level_getter_setter: expected slt_screen_level=3, got %d\n",
             progname, libspectrum_snap_slt_screen_level( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_zxcf_active_upload_memctl_and_pages_getter_setter( void )
{
  /* libspectrum_snap: ZXCF flags (active, upload, memctl, pages) getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_zxcf_active_upload_memctl_and_pages_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_zxcf_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_zxcf_active_upload_memctl_and_pages_getter_setter: default zxcf_active should be 0, got %d\n",
             progname, libspectrum_snap_zxcf_active( snap ) );
    goto done;
  }
  libspectrum_snap_set_zxcf_active( snap, 1 );
  if( libspectrum_snap_zxcf_active( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_zxcf_active_upload_memctl_and_pages_getter_setter: expected zxcf_active=1, got %d\n",
             progname, libspectrum_snap_zxcf_active( snap ) );
    goto done;
  }

  if( libspectrum_snap_zxcf_upload( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_zxcf_active_upload_memctl_and_pages_getter_setter: default zxcf_upload should be 0, got %d\n",
             progname, libspectrum_snap_zxcf_upload( snap ) );
    goto done;
  }
  libspectrum_snap_set_zxcf_upload( snap, 1 );
  if( libspectrum_snap_zxcf_upload( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_zxcf_active_upload_memctl_and_pages_getter_setter: expected zxcf_upload=1, got %d\n",
             progname, libspectrum_snap_zxcf_upload( snap ) );
    goto done;
  }

  if( libspectrum_snap_zxcf_memctl( snap ) != 0x00 ) {
    fprintf( stderr, "%s: snap_zxcf_active_upload_memctl_and_pages_getter_setter: default zxcf_memctl should be 0, got 0x%02x\n",
             progname, libspectrum_snap_zxcf_memctl( snap ) );
    goto done;
  }
  libspectrum_snap_set_zxcf_memctl( snap, 0x37 );
  if( libspectrum_snap_zxcf_memctl( snap ) != 0x37 ) {
    fprintf( stderr, "%s: snap_zxcf_active_upload_memctl_and_pages_getter_setter: expected zxcf_memctl=0x37, got 0x%02x\n",
             progname, libspectrum_snap_zxcf_memctl( snap ) );
    goto done;
  }

  if( libspectrum_snap_zxcf_pages( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_zxcf_active_upload_memctl_and_pages_getter_setter: default zxcf_pages should be 0, got %zu\n",
             progname, libspectrum_snap_zxcf_pages( snap ) );
    goto done;
  }
  libspectrum_snap_set_zxcf_pages( snap, 16 );
  if( libspectrum_snap_zxcf_pages( snap ) != 16 ) {
    fprintf( stderr, "%s: snap_zxcf_active_upload_memctl_and_pages_getter_setter: expected zxcf_pages=16, got %zu\n",
             progname, libspectrum_snap_zxcf_pages( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_zxcf_ram_page_pointer_array_getter_setter( void )
{
  /* libspectrum_snap: ZXCF RAM page pointer array (SNAPSHOT_ZXCF_PAGES pages) */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  libspectrum_byte *page0, *page1;
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_zxcf_ram_page_pointer_array_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_zxcf_ram( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: snap_zxcf_ram_page_pointer_array_getter_setter: default zxcf_ram[0] should be NULL\n", progname );
    goto done;
  }

  page0 = libspectrum_new( libspectrum_byte, 0x4000 );
  page0[0]      = 0xab;
  page0[0x3fff] = 0xcd;

  libspectrum_snap_set_zxcf_ram( snap, 0, page0 );
  if( libspectrum_snap_zxcf_ram( snap, 0 ) != page0 ) {
    fprintf( stderr, "%s: snap_zxcf_ram_page_pointer_array_getter_setter: zxcf_ram[0] pointer mismatch after set\n", progname );
    libspectrum_free( page0 );
    goto done;
  }
  if( libspectrum_snap_zxcf_ram( snap, 0 )[0]      != 0xab ||
      libspectrum_snap_zxcf_ram( snap, 0 )[0x3fff] != 0xcd ) {
    fprintf( stderr, "%s: snap_zxcf_ram_page_pointer_array_getter_setter: zxcf_ram[0] data mismatch\n", progname );
    goto done;
  }

  page1 = libspectrum_new( libspectrum_byte, 0x4000 );
  page1[0] = 0xef;
  libspectrum_snap_set_zxcf_ram( snap, 1, page1 );

  if( libspectrum_snap_zxcf_ram( snap, 2 ) != NULL ) {
    fprintf( stderr, "%s: snap_zxcf_ram_page_pointer_array_getter_setter: zxcf_ram[2] should still be NULL\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}
