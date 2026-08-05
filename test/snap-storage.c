#include "config.h"

#include <stdio.h>

#include "test.h"

test_return_t
snap_divide_active_eprom_writeprotect_paged_and_control_getter_setter( void )
{
  /* libspectrum_snap: DivIDE interface flags getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_divide_active_eprom_writeprotect_paged_and_control_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_divide_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_divide_active_eprom_writeprotect_paged_and_control_getter_setter: default divide_active should be 0, got %d\n",
             progname, libspectrum_snap_divide_active( snap ) );
    goto done;
  }

  libspectrum_snap_set_divide_active( snap, 1 );
  if( libspectrum_snap_divide_active( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_divide_active_eprom_writeprotect_paged_and_control_getter_setter: expected divide_active=1, got %d\n",
             progname, libspectrum_snap_divide_active( snap ) );
    goto done;
  }

  if( libspectrum_snap_divide_eprom_writeprotect( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_divide_active_eprom_writeprotect_paged_and_control_getter_setter: default divide_eprom_writeprotect should be 0, got %d\n",
             progname, libspectrum_snap_divide_eprom_writeprotect( snap ) );
    goto done;
  }

  libspectrum_snap_set_divide_eprom_writeprotect( snap, 1 );
  if( libspectrum_snap_divide_eprom_writeprotect( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_divide_active_eprom_writeprotect_paged_and_control_getter_setter: expected divide_eprom_writeprotect=1, got %d\n",
             progname, libspectrum_snap_divide_eprom_writeprotect( snap ) );
    goto done;
  }

  if( libspectrum_snap_divide_paged( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_divide_active_eprom_writeprotect_paged_and_control_getter_setter: default divide_paged should be 0, got %d\n",
             progname, libspectrum_snap_divide_paged( snap ) );
    goto done;
  }

  libspectrum_snap_set_divide_paged( snap, 1 );
  if( libspectrum_snap_divide_paged( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_divide_active_eprom_writeprotect_paged_and_control_getter_setter: expected divide_paged=1, got %d\n",
             progname, libspectrum_snap_divide_paged( snap ) );
    goto done;
  }

  if( libspectrum_snap_divide_control( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_divide_active_eprom_writeprotect_paged_and_control_getter_setter: default divide_control should be 0, got 0x%02x\n",
             progname, libspectrum_snap_divide_control( snap ) );
    goto done;
  }

  libspectrum_snap_set_divide_control( snap, 0xa3 );
  if( libspectrum_snap_divide_control( snap ) != 0xa3 ) {
    fprintf( stderr, "%s: snap_divide_active_eprom_writeprotect_paged_and_control_getter_setter: expected divide_control=0xa3, got 0x%02x\n",
             progname, libspectrum_snap_divide_control( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_beta_disk_active_paged_autoboot_and_drive_count_getter_setter( void )
{
  /* libspectrum_snap: beta disk interface getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_beta_disk_active_paged_autoboot_and_drive_count_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_beta_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_beta_disk_active_paged_autoboot_and_drive_count_getter_setter: default beta_active should be 0, got %d\n",
             progname, libspectrum_snap_beta_active( snap ) );
    goto done;
  }
  libspectrum_snap_set_beta_active( snap, 1 );
  if( libspectrum_snap_beta_active( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_beta_disk_active_paged_autoboot_and_drive_count_getter_setter: expected beta_active=1, got %d\n",
             progname, libspectrum_snap_beta_active( snap ) );
    goto done;
  }

  if( libspectrum_snap_beta_paged( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_beta_disk_active_paged_autoboot_and_drive_count_getter_setter: default beta_paged should be 0, got %d\n",
             progname, libspectrum_snap_beta_paged( snap ) );
    goto done;
  }
  libspectrum_snap_set_beta_paged( snap, 1 );
  if( libspectrum_snap_beta_paged( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_beta_disk_active_paged_autoboot_and_drive_count_getter_setter: expected beta_paged=1, got %d\n",
             progname, libspectrum_snap_beta_paged( snap ) );
    goto done;
  }

  if( libspectrum_snap_beta_autoboot( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_beta_disk_active_paged_autoboot_and_drive_count_getter_setter: default beta_autoboot should be 0, got %d\n",
             progname, libspectrum_snap_beta_autoboot( snap ) );
    goto done;
  }
  libspectrum_snap_set_beta_autoboot( snap, 1 );
  if( libspectrum_snap_beta_autoboot( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_beta_disk_active_paged_autoboot_and_drive_count_getter_setter: expected beta_autoboot=1, got %d\n",
             progname, libspectrum_snap_beta_autoboot( snap ) );
    goto done;
  }

  if( libspectrum_snap_beta_drive_count( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_beta_disk_active_paged_autoboot_and_drive_count_getter_setter: default beta_drive_count should be 0, got %d\n",
             progname, libspectrum_snap_beta_drive_count( snap ) );
    goto done;
  }
  libspectrum_snap_set_beta_drive_count( snap, 4 );
  if( libspectrum_snap_beta_drive_count( snap ) != 4 ) {
    fprintf( stderr, "%s: snap_beta_disk_active_paged_autoboot_and_drive_count_getter_setter: expected beta_drive_count=4, got %d\n",
             progname, libspectrum_snap_beta_drive_count( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_zxatasp_active_upload_and_writeprotect_getter_setter( void )
{
  /* libspectrum_snap: ZXATASP active/upload/writeprotect getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_zxatasp_active_upload_and_writeprotect_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_zxatasp_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_zxatasp_active_upload_and_writeprotect_getter_setter: default zxatasp_active should be 0, got %d\n",
             progname, libspectrum_snap_zxatasp_active( snap ) );
    goto done;
  }
  libspectrum_snap_set_zxatasp_active( snap, 1 );
  if( libspectrum_snap_zxatasp_active( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_zxatasp_active_upload_and_writeprotect_getter_setter: expected zxatasp_active=1, got %d\n",
             progname, libspectrum_snap_zxatasp_active( snap ) );
    goto done;
  }

  if( libspectrum_snap_zxatasp_upload( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_zxatasp_active_upload_and_writeprotect_getter_setter: default zxatasp_upload should be 0, got %d\n",
             progname, libspectrum_snap_zxatasp_upload( snap ) );
    goto done;
  }
  libspectrum_snap_set_zxatasp_upload( snap, 1 );
  if( libspectrum_snap_zxatasp_upload( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_zxatasp_active_upload_and_writeprotect_getter_setter: expected zxatasp_upload=1, got %d\n",
             progname, libspectrum_snap_zxatasp_upload( snap ) );
    goto done;
  }

  if( libspectrum_snap_zxatasp_writeprotect( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_zxatasp_active_upload_and_writeprotect_getter_setter: default zxatasp_writeprotect should be 0, got %d\n",
             progname, libspectrum_snap_zxatasp_writeprotect( snap ) );
    goto done;
  }
  libspectrum_snap_set_zxatasp_writeprotect( snap, 1 );
  if( libspectrum_snap_zxatasp_writeprotect( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_zxatasp_active_upload_and_writeprotect_getter_setter: expected zxatasp_writeprotect=1, got %d\n",
             progname, libspectrum_snap_zxatasp_writeprotect( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_zxatasp_port_a_b_c_and_control_getter_setter( void )
{
  /* libspectrum_snap: ZXATASP port_a/b/c and control getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_zxatasp_port_a_b_c_and_control_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_zxatasp_port_a( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_zxatasp_port_a_b_c_and_control_getter_setter: default zxatasp_port_a should be 0, got 0x%02x\n",
             progname, libspectrum_snap_zxatasp_port_a( snap ) );
    goto done;
  }
  libspectrum_snap_set_zxatasp_port_a( snap, 0xaa );
  if( libspectrum_snap_zxatasp_port_a( snap ) != 0xaa ) {
    fprintf( stderr, "%s: snap_zxatasp_port_a_b_c_and_control_getter_setter: expected zxatasp_port_a=0xaa, got 0x%02x\n",
             progname, libspectrum_snap_zxatasp_port_a( snap ) );
    goto done;
  }

  if( libspectrum_snap_zxatasp_port_b( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_zxatasp_port_a_b_c_and_control_getter_setter: default zxatasp_port_b should be 0, got 0x%02x\n",
             progname, libspectrum_snap_zxatasp_port_b( snap ) );
    goto done;
  }
  libspectrum_snap_set_zxatasp_port_b( snap, 0xbb );
  if( libspectrum_snap_zxatasp_port_b( snap ) != 0xbb ) {
    fprintf( stderr, "%s: snap_zxatasp_port_a_b_c_and_control_getter_setter: expected zxatasp_port_b=0xbb, got 0x%02x\n",
             progname, libspectrum_snap_zxatasp_port_b( snap ) );
    goto done;
  }

  if( libspectrum_snap_zxatasp_port_c( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_zxatasp_port_a_b_c_and_control_getter_setter: default zxatasp_port_c should be 0, got 0x%02x\n",
             progname, libspectrum_snap_zxatasp_port_c( snap ) );
    goto done;
  }
  libspectrum_snap_set_zxatasp_port_c( snap, 0xcc );
  if( libspectrum_snap_zxatasp_port_c( snap ) != 0xcc ) {
    fprintf( stderr, "%s: snap_zxatasp_port_a_b_c_and_control_getter_setter: expected zxatasp_port_c=0xcc, got 0x%02x\n",
             progname, libspectrum_snap_zxatasp_port_c( snap ) );
    goto done;
  }

  if( libspectrum_snap_zxatasp_control( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_zxatasp_port_a_b_c_and_control_getter_setter: default zxatasp_control should be 0, got 0x%02x\n",
             progname, libspectrum_snap_zxatasp_control( snap ) );
    goto done;
  }
  libspectrum_snap_set_zxatasp_control( snap, 0x9b );
  if( libspectrum_snap_zxatasp_control( snap ) != 0x9b ) {
    fprintf( stderr, "%s: snap_zxatasp_port_a_b_c_and_control_getter_setter: expected zxatasp_control=0x9b, got 0x%02x\n",
             progname, libspectrum_snap_zxatasp_control( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_zxatasp_pages_and_current_page_getter_setter( void )
{
  /* libspectrum_snap: ZXATASP pages and current_page getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_zxatasp_pages_and_current_page_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_zxatasp_pages( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_zxatasp_pages_and_current_page_getter_setter: default zxatasp_pages should be 0, got %zu\n",
             progname, libspectrum_snap_zxatasp_pages( snap ) );
    goto done;
  }
  libspectrum_snap_set_zxatasp_pages( snap, 8 );
  if( libspectrum_snap_zxatasp_pages( snap ) != 8 ) {
    fprintf( stderr, "%s: snap_zxatasp_pages_and_current_page_getter_setter: expected zxatasp_pages=8, got %zu\n",
             progname, libspectrum_snap_zxatasp_pages( snap ) );
    goto done;
  }

  if( libspectrum_snap_zxatasp_current_page( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_zxatasp_pages_and_current_page_getter_setter: default zxatasp_current_page should be 0, got %zu\n",
             progname, libspectrum_snap_zxatasp_current_page( snap ) );
    goto done;
  }
  libspectrum_snap_set_zxatasp_current_page( snap, 5 );
  if( libspectrum_snap_zxatasp_current_page( snap ) != 5 ) {
    fprintf( stderr, "%s: snap_zxatasp_pages_and_current_page_getter_setter: expected zxatasp_current_page=5, got %zu\n",
             progname, libspectrum_snap_zxatasp_current_page( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_plus_d_active_paged_drive_count_custom_rom_and_direction_getter_setter( void )
{
  /* libspectrum_snap: Plus D active, paged, drive_count, custom_rom, direction */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_plus_d_active_paged_drive_count_custom_rom_and_direction_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_plusd_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_plus_d_active_paged_drive_count_custom_rom_and_direction_getter_setter: default plusd_active should be 0, got %d\n",
             progname, libspectrum_snap_plusd_active( snap ) );
    goto done;
  }
  libspectrum_snap_set_plusd_active( snap, 1 );
  if( libspectrum_snap_plusd_active( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_plus_d_active_paged_drive_count_custom_rom_and_direction_getter_setter: expected plusd_active=1, got %d\n",
             progname, libspectrum_snap_plusd_active( snap ) );
    goto done;
  }

  if( libspectrum_snap_plusd_paged( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_plus_d_active_paged_drive_count_custom_rom_and_direction_getter_setter: default plusd_paged should be 0, got %d\n",
             progname, libspectrum_snap_plusd_paged( snap ) );
    goto done;
  }
  libspectrum_snap_set_plusd_paged( snap, 1 );
  if( libspectrum_snap_plusd_paged( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_plus_d_active_paged_drive_count_custom_rom_and_direction_getter_setter: expected plusd_paged=1, got %d\n",
             progname, libspectrum_snap_plusd_paged( snap ) );
    goto done;
  }

  if( libspectrum_snap_plusd_drive_count( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_plus_d_active_paged_drive_count_custom_rom_and_direction_getter_setter: default plusd_drive_count should be 0, got %d\n",
             progname, libspectrum_snap_plusd_drive_count( snap ) );
    goto done;
  }
  libspectrum_snap_set_plusd_drive_count( snap, 2 );
  if( libspectrum_snap_plusd_drive_count( snap ) != 2 ) {
    fprintf( stderr, "%s: snap_plus_d_active_paged_drive_count_custom_rom_and_direction_getter_setter: expected plusd_drive_count=2, got %d\n",
             progname, libspectrum_snap_plusd_drive_count( snap ) );
    goto done;
  }

  if( libspectrum_snap_plusd_custom_rom( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_plus_d_active_paged_drive_count_custom_rom_and_direction_getter_setter: default plusd_custom_rom should be 0, got %d\n",
             progname, libspectrum_snap_plusd_custom_rom( snap ) );
    goto done;
  }
  libspectrum_snap_set_plusd_custom_rom( snap, 1 );
  if( libspectrum_snap_plusd_custom_rom( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_plus_d_active_paged_drive_count_custom_rom_and_direction_getter_setter: expected plusd_custom_rom=1, got %d\n",
             progname, libspectrum_snap_plusd_custom_rom( snap ) );
    goto done;
  }

  if( libspectrum_snap_plusd_direction( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_plus_d_active_paged_drive_count_custom_rom_and_direction_getter_setter: default plusd_direction should be 0, got %d\n",
             progname, libspectrum_snap_plusd_direction( snap ) );
    goto done;
  }
  libspectrum_snap_set_plusd_direction( snap, 1 );
  if( libspectrum_snap_plusd_direction( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_plus_d_active_paged_drive_count_custom_rom_and_direction_getter_setter: expected plusd_direction=1, got %d\n",
             progname, libspectrum_snap_plusd_direction( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_plus_d_fdc_byte_registers_control_track_sector_data_status_getter_setter( void )
{
  /* libspectrum_snap: Plus D FDC byte registers (control, track, sector, data, status) */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_plus_d_fdc_byte_registers_control_track_sector_data_status_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_plusd_control( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_plus_d_fdc_byte_registers_control_track_sector_data_status_getter_setter: default plusd_control should be 0, got 0x%02x\n",
             progname, libspectrum_snap_plusd_control( snap ) );
    goto done;
  }
  libspectrum_snap_set_plusd_control( snap, 0xd4 );
  if( libspectrum_snap_plusd_control( snap ) != 0xd4 ) {
    fprintf( stderr, "%s: snap_plus_d_fdc_byte_registers_control_track_sector_data_status_getter_setter: expected plusd_control=0xd4, got 0x%02x\n",
             progname, libspectrum_snap_plusd_control( snap ) );
    goto done;
  }

  if( libspectrum_snap_plusd_track( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_plus_d_fdc_byte_registers_control_track_sector_data_status_getter_setter: default plusd_track should be 0, got %d\n",
             progname, libspectrum_snap_plusd_track( snap ) );
    goto done;
  }
  libspectrum_snap_set_plusd_track( snap, 39 );
  if( libspectrum_snap_plusd_track( snap ) != 39 ) {
    fprintf( stderr, "%s: snap_plus_d_fdc_byte_registers_control_track_sector_data_status_getter_setter: expected plusd_track=39, got %d\n",
             progname, libspectrum_snap_plusd_track( snap ) );
    goto done;
  }

  if( libspectrum_snap_plusd_sector( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_plus_d_fdc_byte_registers_control_track_sector_data_status_getter_setter: default plusd_sector should be 0, got %d\n",
             progname, libspectrum_snap_plusd_sector( snap ) );
    goto done;
  }
  libspectrum_snap_set_plusd_sector( snap, 9 );
  if( libspectrum_snap_plusd_sector( snap ) != 9 ) {
    fprintf( stderr, "%s: snap_plus_d_fdc_byte_registers_control_track_sector_data_status_getter_setter: expected plusd_sector=9, got %d\n",
             progname, libspectrum_snap_plusd_sector( snap ) );
    goto done;
  }

  if( libspectrum_snap_plusd_data( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_plus_d_fdc_byte_registers_control_track_sector_data_status_getter_setter: default plusd_data should be 0, got 0x%02x\n",
             progname, libspectrum_snap_plusd_data( snap ) );
    goto done;
  }
  libspectrum_snap_set_plusd_data( snap, 0x5a );
  if( libspectrum_snap_plusd_data( snap ) != 0x5a ) {
    fprintf( stderr, "%s: snap_plus_d_fdc_byte_registers_control_track_sector_data_status_getter_setter: expected plusd_data=0x5a, got 0x%02x\n",
             progname, libspectrum_snap_plusd_data( snap ) );
    goto done;
  }

  if( libspectrum_snap_plusd_status( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_plus_d_fdc_byte_registers_control_track_sector_data_status_getter_setter: default plusd_status should be 0, got 0x%02x\n",
             progname, libspectrum_snap_plusd_status( snap ) );
    goto done;
  }
  libspectrum_snap_set_plusd_status( snap, 0x81 );
  if( libspectrum_snap_plusd_status( snap ) != 0x81 ) {
    fprintf( stderr, "%s: snap_plus_d_fdc_byte_registers_control_track_sector_data_status_getter_setter: expected plusd_status=0x81, got 0x%02x\n",
             progname, libspectrum_snap_plusd_status( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_plus_d_rom_and_ram_single_pointer_getter_setter( void )
{
  /* libspectrum_snap: Plus D ROM and RAM single-pointer getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  libspectrum_byte *rom, *ram;
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_plus_d_rom_and_ram_single_pointer_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_plusd_rom( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: snap_plus_d_rom_and_ram_single_pointer_getter_setter: default plusd_rom[0] should be NULL\n", progname );
    goto done;
  }

  rom = libspectrum_new( libspectrum_byte, 0x2000 );
  rom[0]      = 0xc3;
  rom[0x1fff] = 0xff;

  libspectrum_snap_set_plusd_rom( snap, 0, rom );
  if( libspectrum_snap_plusd_rom( snap, 0 ) != rom ) {
    fprintf( stderr, "%s: snap_plus_d_rom_and_ram_single_pointer_getter_setter: plusd_rom[0] pointer mismatch after set\n", progname );
    libspectrum_free( rom );
    goto done;
  }
  if( libspectrum_snap_plusd_rom( snap, 0 )[0]      != 0xc3 ||
      libspectrum_snap_plusd_rom( snap, 0 )[0x1fff] != 0xff ) {
    fprintf( stderr, "%s: snap_plus_d_rom_and_ram_single_pointer_getter_setter: plusd_rom[0] data mismatch\n", progname );
    goto done;
  }

  if( libspectrum_snap_plusd_ram( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: snap_plus_d_rom_and_ram_single_pointer_getter_setter: default plusd_ram[0] should be NULL\n", progname );
    goto done;
  }

  ram = libspectrum_new( libspectrum_byte, 0x2000 );
  ram[0]      = 0x11;
  ram[0x1fff] = 0x22;

  libspectrum_snap_set_plusd_ram( snap, 0, ram );
  if( libspectrum_snap_plusd_ram( snap, 0 ) != ram ) {
    fprintf( stderr, "%s: snap_plus_d_rom_and_ram_single_pointer_getter_setter: plusd_ram[0] pointer mismatch after set\n", progname );
    libspectrum_free( ram );
    goto done;
  }
  if( libspectrum_snap_plusd_ram( snap, 0 )[0]      != 0x11 ||
      libspectrum_snap_plusd_ram( snap, 0 )[0x1fff] != 0x22 ) {
    fprintf( stderr, "%s: snap_plus_d_rom_and_ram_single_pointer_getter_setter: plusd_ram[0] data mismatch\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_opus_discovery_active_paged_drive_count_custom_rom_and_direction_getter_setter( void )
{
  /* libspectrum_snap: Opus Discovery int flags getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_opus_discovery_active_paged_drive_count_custom_rom_and_direction_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_opus_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_opus_discovery_active_paged_drive_count_custom_rom_and_direction_getter_setter: default opus_active should be 0, got %d\n",
             progname, libspectrum_snap_opus_active( snap ) );
    goto done;
  }
  libspectrum_snap_set_opus_active( snap, 1 );
  if( libspectrum_snap_opus_active( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_opus_discovery_active_paged_drive_count_custom_rom_and_direction_getter_setter: expected opus_active=1, got %d\n",
             progname, libspectrum_snap_opus_active( snap ) );
    goto done;
  }

  if( libspectrum_snap_opus_paged( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_opus_discovery_active_paged_drive_count_custom_rom_and_direction_getter_setter: default opus_paged should be 0, got %d\n",
             progname, libspectrum_snap_opus_paged( snap ) );
    goto done;
  }
  libspectrum_snap_set_opus_paged( snap, 1 );
  if( libspectrum_snap_opus_paged( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_opus_discovery_active_paged_drive_count_custom_rom_and_direction_getter_setter: expected opus_paged=1, got %d\n",
             progname, libspectrum_snap_opus_paged( snap ) );
    goto done;
  }

  if( libspectrum_snap_opus_drive_count( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_opus_discovery_active_paged_drive_count_custom_rom_and_direction_getter_setter: default opus_drive_count should be 0, got %d\n",
             progname, libspectrum_snap_opus_drive_count( snap ) );
    goto done;
  }
  libspectrum_snap_set_opus_drive_count( snap, 2 );
  if( libspectrum_snap_opus_drive_count( snap ) != 2 ) {
    fprintf( stderr, "%s: snap_opus_discovery_active_paged_drive_count_custom_rom_and_direction_getter_setter: expected opus_drive_count=2, got %d\n",
             progname, libspectrum_snap_opus_drive_count( snap ) );
    goto done;
  }

  if( libspectrum_snap_opus_custom_rom( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_opus_discovery_active_paged_drive_count_custom_rom_and_direction_getter_setter: default opus_custom_rom should be 0, got %d\n",
             progname, libspectrum_snap_opus_custom_rom( snap ) );
    goto done;
  }
  libspectrum_snap_set_opus_custom_rom( snap, 1 );
  if( libspectrum_snap_opus_custom_rom( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_opus_discovery_active_paged_drive_count_custom_rom_and_direction_getter_setter: expected opus_custom_rom=1, got %d\n",
             progname, libspectrum_snap_opus_custom_rom( snap ) );
    goto done;
  }

  if( libspectrum_snap_opus_direction( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_opus_discovery_active_paged_drive_count_custom_rom_and_direction_getter_setter: default opus_direction should be 0, got %d\n",
             progname, libspectrum_snap_opus_direction( snap ) );
    goto done;
  }
  libspectrum_snap_set_opus_direction( snap, 1 );
  if( libspectrum_snap_opus_direction( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_opus_discovery_active_paged_drive_count_custom_rom_and_direction_getter_setter: expected opus_direction=1, got %d\n",
             progname, libspectrum_snap_opus_direction( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_opus_discovery_fdc_byte_registers_track_sector_data_status_via_ports_getter_setter( void )
{
  /* libspectrum_snap: Opus Discovery FDC byte registers getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_opus_discovery_fdc_byte_registers_track_sector_data_status_via_ports_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_opus_track( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_opus_discovery_fdc_byte_registers_track_sector_data_status_via_ports_getter_setter: default opus_track should be 0, got 0x%02x\n",
             progname, libspectrum_snap_opus_track( snap ) );
    goto done;
  }
  libspectrum_snap_set_opus_track( snap, 0x12 );
  if( libspectrum_snap_opus_track( snap ) != 0x12 ) {
    fprintf( stderr, "%s: snap_opus_discovery_fdc_byte_registers_track_sector_data_status_via_ports_getter_setter: expected opus_track=0x12, got 0x%02x\n",
             progname, libspectrum_snap_opus_track( snap ) );
    goto done;
  }

  if( libspectrum_snap_opus_sector( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_opus_discovery_fdc_byte_registers_track_sector_data_status_via_ports_getter_setter: default opus_sector should be 0, got 0x%02x\n",
             progname, libspectrum_snap_opus_sector( snap ) );
    goto done;
  }
  libspectrum_snap_set_opus_sector( snap, 0x34 );
  if( libspectrum_snap_opus_sector( snap ) != 0x34 ) {
    fprintf( stderr, "%s: snap_opus_discovery_fdc_byte_registers_track_sector_data_status_via_ports_getter_setter: expected opus_sector=0x34, got 0x%02x\n",
             progname, libspectrum_snap_opus_sector( snap ) );
    goto done;
  }

  if( libspectrum_snap_opus_data( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_opus_discovery_fdc_byte_registers_track_sector_data_status_via_ports_getter_setter: default opus_data should be 0, got 0x%02x\n",
             progname, libspectrum_snap_opus_data( snap ) );
    goto done;
  }
  libspectrum_snap_set_opus_data( snap, 0x56 );
  if( libspectrum_snap_opus_data( snap ) != 0x56 ) {
    fprintf( stderr, "%s: snap_opus_discovery_fdc_byte_registers_track_sector_data_status_via_ports_getter_setter: expected opus_data=0x56, got 0x%02x\n",
             progname, libspectrum_snap_opus_data( snap ) );
    goto done;
  }

  if( libspectrum_snap_opus_status( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_opus_discovery_fdc_byte_registers_track_sector_data_status_via_ports_getter_setter: default opus_status should be 0, got 0x%02x\n",
             progname, libspectrum_snap_opus_status( snap ) );
    goto done;
  }
  libspectrum_snap_set_opus_status( snap, 0x78 );
  if( libspectrum_snap_opus_status( snap ) != 0x78 ) {
    fprintf( stderr, "%s: snap_opus_discovery_fdc_byte_registers_track_sector_data_status_via_ports_getter_setter: expected opus_status=0x78, got 0x%02x\n",
             progname, libspectrum_snap_opus_status( snap ) );
    goto done;
  }

  if( libspectrum_snap_opus_data_reg_a( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_opus_discovery_fdc_byte_registers_track_sector_data_status_via_ports_getter_setter: default opus_data_reg_a should be 0, got 0x%02x\n",
             progname, libspectrum_snap_opus_data_reg_a( snap ) );
    goto done;
  }
  libspectrum_snap_set_opus_data_reg_a( snap, 0x9a );
  if( libspectrum_snap_opus_data_reg_a( snap ) != 0x9a ) {
    fprintf( stderr, "%s: snap_opus_discovery_fdc_byte_registers_track_sector_data_status_via_ports_getter_setter: expected opus_data_reg_a=0x9a, got 0x%02x\n",
             progname, libspectrum_snap_opus_data_reg_a( snap ) );
    goto done;
  }

  if( libspectrum_snap_opus_data_dir_a( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_opus_discovery_fdc_byte_registers_track_sector_data_status_via_ports_getter_setter: default opus_data_dir_a should be 0, got 0x%02x\n",
             progname, libspectrum_snap_opus_data_dir_a( snap ) );
    goto done;
  }
  libspectrum_snap_set_opus_data_dir_a( snap, 0xbc );
  if( libspectrum_snap_opus_data_dir_a( snap ) != 0xbc ) {
    fprintf( stderr, "%s: snap_opus_discovery_fdc_byte_registers_track_sector_data_status_via_ports_getter_setter: expected opus_data_dir_a=0xbc, got 0x%02x\n",
             progname, libspectrum_snap_opus_data_dir_a( snap ) );
    goto done;
  }

  if( libspectrum_snap_opus_control_a( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_opus_discovery_fdc_byte_registers_track_sector_data_status_via_ports_getter_setter: default opus_control_a should be 0, got 0x%02x\n",
             progname, libspectrum_snap_opus_control_a( snap ) );
    goto done;
  }
  libspectrum_snap_set_opus_control_a( snap, 0xde );
  if( libspectrum_snap_opus_control_a( snap ) != 0xde ) {
    fprintf( stderr, "%s: snap_opus_discovery_fdc_byte_registers_track_sector_data_status_via_ports_getter_setter: expected opus_control_a=0xde, got 0x%02x\n",
             progname, libspectrum_snap_opus_control_a( snap ) );
    goto done;
  }

  if( libspectrum_snap_opus_data_reg_b( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_opus_discovery_fdc_byte_registers_track_sector_data_status_via_ports_getter_setter: default opus_data_reg_b should be 0, got 0x%02x\n",
             progname, libspectrum_snap_opus_data_reg_b( snap ) );
    goto done;
  }
  libspectrum_snap_set_opus_data_reg_b( snap, 0xf0 );
  if( libspectrum_snap_opus_data_reg_b( snap ) != 0xf0 ) {
    fprintf( stderr, "%s: snap_opus_discovery_fdc_byte_registers_track_sector_data_status_via_ports_getter_setter: expected opus_data_reg_b=0xf0, got 0x%02x\n",
             progname, libspectrum_snap_opus_data_reg_b( snap ) );
    goto done;
  }

  if( libspectrum_snap_opus_data_dir_b( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_opus_discovery_fdc_byte_registers_track_sector_data_status_via_ports_getter_setter: default opus_data_dir_b should be 0, got 0x%02x\n",
             progname, libspectrum_snap_opus_data_dir_b( snap ) );
    goto done;
  }
  libspectrum_snap_set_opus_data_dir_b( snap, 0x0f );
  if( libspectrum_snap_opus_data_dir_b( snap ) != 0x0f ) {
    fprintf( stderr, "%s: snap_opus_discovery_fdc_byte_registers_track_sector_data_status_via_ports_getter_setter: expected opus_data_dir_b=0x0f, got 0x%02x\n",
             progname, libspectrum_snap_opus_data_dir_b( snap ) );
    goto done;
  }

  if( libspectrum_snap_opus_control_b( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_opus_discovery_fdc_byte_registers_track_sector_data_status_via_ports_getter_setter: default opus_control_b should be 0, got 0x%02x\n",
             progname, libspectrum_snap_opus_control_b( snap ) );
    goto done;
  }
  libspectrum_snap_set_opus_control_b( snap, 0xaa );
  if( libspectrum_snap_opus_control_b( snap ) != 0xaa ) {
    fprintf( stderr, "%s: snap_opus_discovery_fdc_byte_registers_track_sector_data_status_via_ports_getter_setter: expected opus_control_b=0xaa, got 0x%02x\n",
             progname, libspectrum_snap_opus_control_b( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_opus_discovery_rom_and_ram_single_pointer_getter_setter( void )
{
  /* libspectrum_snap: Opus Discovery ROM and RAM single-pointer getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  libspectrum_byte *rom, *ram;
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_opus_discovery_rom_and_ram_single_pointer_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_opus_rom( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: snap_opus_discovery_rom_and_ram_single_pointer_getter_setter: default opus_rom[0] should be NULL\n", progname );
    goto done;
  }

  rom = libspectrum_new( libspectrum_byte, 0x4000 );
  rom[0]      = 0x11;
  rom[0x3fff] = 0x22;

  libspectrum_snap_set_opus_rom( snap, 0, rom );
  if( libspectrum_snap_opus_rom( snap, 0 ) != rom ) {
    fprintf( stderr, "%s: snap_opus_discovery_rom_and_ram_single_pointer_getter_setter: opus_rom[0] pointer mismatch after set\n", progname );
    libspectrum_free( rom );
    goto done;
  }
  if( libspectrum_snap_opus_rom( snap, 0 )[0]      != 0x11 ||
      libspectrum_snap_opus_rom( snap, 0 )[0x3fff] != 0x22 ) {
    fprintf( stderr, "%s: snap_opus_discovery_rom_and_ram_single_pointer_getter_setter: opus_rom[0] data mismatch\n", progname );
    goto done;
  }

  if( libspectrum_snap_opus_ram( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: snap_opus_discovery_rom_and_ram_single_pointer_getter_setter: default opus_ram[0] should be NULL\n", progname );
    goto done;
  }

  ram = libspectrum_new( libspectrum_byte, 0x800 );
  ram[0]     = 0x33;
  ram[0x7ff] = 0x44;

  libspectrum_snap_set_opus_ram( snap, 0, ram );
  if( libspectrum_snap_opus_ram( snap, 0 ) != ram ) {
    fprintf( stderr, "%s: snap_opus_discovery_rom_and_ram_single_pointer_getter_setter: opus_ram[0] pointer mismatch after set\n", progname );
    libspectrum_free( ram );
    goto done;
  }
  if( libspectrum_snap_opus_ram( snap, 0 )[0]     != 0x33 ||
      libspectrum_snap_opus_ram( snap, 0 )[0x7ff] != 0x44 ) {
    fprintf( stderr, "%s: snap_opus_discovery_rom_and_ram_single_pointer_getter_setter: opus_ram[0] data mismatch\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_disciple_active_paged_inhibit_button_drive_count_custom_rom_and_direction_getter_setter( void )
{
  /* libspectrum_snap: DISCiPLE int flags getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_disciple_active_paged_inhibit_button_drive_count_custom_rom_and_direction_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_disciple_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_disciple_active_paged_inhibit_button_drive_count_custom_rom_and_direction_getter_setter: default disciple_active should be 0, got %d\n",
             progname, libspectrum_snap_disciple_active( snap ) );
    goto done;
  }
  libspectrum_snap_set_disciple_active( snap, 1 );
  if( libspectrum_snap_disciple_active( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_disciple_active_paged_inhibit_button_drive_count_custom_rom_and_direction_getter_setter: expected disciple_active=1, got %d\n",
             progname, libspectrum_snap_disciple_active( snap ) );
    goto done;
  }

  if( libspectrum_snap_disciple_paged( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_disciple_active_paged_inhibit_button_drive_count_custom_rom_and_direction_getter_setter: default disciple_paged should be 0, got %d\n",
             progname, libspectrum_snap_disciple_paged( snap ) );
    goto done;
  }
  libspectrum_snap_set_disciple_paged( snap, 1 );
  if( libspectrum_snap_disciple_paged( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_disciple_active_paged_inhibit_button_drive_count_custom_rom_and_direction_getter_setter: expected disciple_paged=1, got %d\n",
             progname, libspectrum_snap_disciple_paged( snap ) );
    goto done;
  }

  if( libspectrum_snap_disciple_inhibit_button( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_disciple_active_paged_inhibit_button_drive_count_custom_rom_and_direction_getter_setter: default disciple_inhibit_button should be 0, got %d\n",
             progname, libspectrum_snap_disciple_inhibit_button( snap ) );
    goto done;
  }
  libspectrum_snap_set_disciple_inhibit_button( snap, 1 );
  if( libspectrum_snap_disciple_inhibit_button( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_disciple_active_paged_inhibit_button_drive_count_custom_rom_and_direction_getter_setter: expected disciple_inhibit_button=1, got %d\n",
             progname, libspectrum_snap_disciple_inhibit_button( snap ) );
    goto done;
  }

  if( libspectrum_snap_disciple_drive_count( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_disciple_active_paged_inhibit_button_drive_count_custom_rom_and_direction_getter_setter: default disciple_drive_count should be 0, got %d\n",
             progname, libspectrum_snap_disciple_drive_count( snap ) );
    goto done;
  }
  libspectrum_snap_set_disciple_drive_count( snap, 2 );
  if( libspectrum_snap_disciple_drive_count( snap ) != 2 ) {
    fprintf( stderr, "%s: snap_disciple_active_paged_inhibit_button_drive_count_custom_rom_and_direction_getter_setter: expected disciple_drive_count=2, got %d\n",
             progname, libspectrum_snap_disciple_drive_count( snap ) );
    goto done;
  }

  if( libspectrum_snap_disciple_custom_rom( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_disciple_active_paged_inhibit_button_drive_count_custom_rom_and_direction_getter_setter: default disciple_custom_rom should be 0, got %d\n",
             progname, libspectrum_snap_disciple_custom_rom( snap ) );
    goto done;
  }
  libspectrum_snap_set_disciple_custom_rom( snap, 1 );
  if( libspectrum_snap_disciple_custom_rom( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_disciple_active_paged_inhibit_button_drive_count_custom_rom_and_direction_getter_setter: expected disciple_custom_rom=1, got %d\n",
             progname, libspectrum_snap_disciple_custom_rom( snap ) );
    goto done;
  }

  if( libspectrum_snap_disciple_direction( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_disciple_active_paged_inhibit_button_drive_count_custom_rom_and_direction_getter_setter: default disciple_direction should be 0, got %d\n",
             progname, libspectrum_snap_disciple_direction( snap ) );
    goto done;
  }
  libspectrum_snap_set_disciple_direction( snap, 1 );
  if( libspectrum_snap_disciple_direction( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_disciple_active_paged_inhibit_button_drive_count_custom_rom_and_direction_getter_setter: expected disciple_direction=1, got %d\n",
             progname, libspectrum_snap_disciple_direction( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_disciple_fdc_byte_registers_control_track_sector_data_status_getter_setter( void )
{
  /* libspectrum_snap: DISCiPLE FDC byte registers getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_disciple_fdc_byte_registers_control_track_sector_data_status_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_disciple_control( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_disciple_fdc_byte_registers_control_track_sector_data_status_getter_setter: default disciple_control should be 0, got 0x%02x\n",
             progname, libspectrum_snap_disciple_control( snap ) );
    goto done;
  }
  libspectrum_snap_set_disciple_control( snap, 0x12 );
  if( libspectrum_snap_disciple_control( snap ) != 0x12 ) {
    fprintf( stderr, "%s: snap_disciple_fdc_byte_registers_control_track_sector_data_status_getter_setter: expected disciple_control=0x12, got 0x%02x\n",
             progname, libspectrum_snap_disciple_control( snap ) );
    goto done;
  }

  if( libspectrum_snap_disciple_track( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_disciple_fdc_byte_registers_control_track_sector_data_status_getter_setter: default disciple_track should be 0, got 0x%02x\n",
             progname, libspectrum_snap_disciple_track( snap ) );
    goto done;
  }
  libspectrum_snap_set_disciple_track( snap, 0x34 );
  if( libspectrum_snap_disciple_track( snap ) != 0x34 ) {
    fprintf( stderr, "%s: snap_disciple_fdc_byte_registers_control_track_sector_data_status_getter_setter: expected disciple_track=0x34, got 0x%02x\n",
             progname, libspectrum_snap_disciple_track( snap ) );
    goto done;
  }

  if( libspectrum_snap_disciple_sector( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_disciple_fdc_byte_registers_control_track_sector_data_status_getter_setter: default disciple_sector should be 0, got 0x%02x\n",
             progname, libspectrum_snap_disciple_sector( snap ) );
    goto done;
  }
  libspectrum_snap_set_disciple_sector( snap, 0x56 );
  if( libspectrum_snap_disciple_sector( snap ) != 0x56 ) {
    fprintf( stderr, "%s: snap_disciple_fdc_byte_registers_control_track_sector_data_status_getter_setter: expected disciple_sector=0x56, got 0x%02x\n",
             progname, libspectrum_snap_disciple_sector( snap ) );
    goto done;
  }

  if( libspectrum_snap_disciple_data( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_disciple_fdc_byte_registers_control_track_sector_data_status_getter_setter: default disciple_data should be 0, got 0x%02x\n",
             progname, libspectrum_snap_disciple_data( snap ) );
    goto done;
  }
  libspectrum_snap_set_disciple_data( snap, 0x78 );
  if( libspectrum_snap_disciple_data( snap ) != 0x78 ) {
    fprintf( stderr, "%s: snap_disciple_fdc_byte_registers_control_track_sector_data_status_getter_setter: expected disciple_data=0x78, got 0x%02x\n",
             progname, libspectrum_snap_disciple_data( snap ) );
    goto done;
  }

  if( libspectrum_snap_disciple_status( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_disciple_fdc_byte_registers_control_track_sector_data_status_getter_setter: default disciple_status should be 0, got 0x%02x\n",
             progname, libspectrum_snap_disciple_status( snap ) );
    goto done;
  }
  libspectrum_snap_set_disciple_status( snap, 0x9a );
  if( libspectrum_snap_disciple_status( snap ) != 0x9a ) {
    fprintf( stderr, "%s: snap_disciple_fdc_byte_registers_control_track_sector_data_status_getter_setter: expected disciple_status=0x9a, got 0x%02x\n",
             progname, libspectrum_snap_disciple_status( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_disciple_rom_pointer_rom_length_and_ram_pointer_getter_setter( void )
{
  /* libspectrum_snap: DISCiPLE ROM pointer, ROM length, and RAM pointer getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  libspectrum_byte *rom, *ram;
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_disciple_rom_pointer_rom_length_and_ram_pointer_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_disciple_rom( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: snap_disciple_rom_pointer_rom_length_and_ram_pointer_getter_setter: default disciple_rom[0] should be NULL\n", progname );
    goto done;
  }

  if( libspectrum_snap_disciple_rom_length( snap, 0 ) != 0 ) {
    fprintf( stderr, "%s: snap_disciple_rom_pointer_rom_length_and_ram_pointer_getter_setter: default disciple_rom_length[0] should be 0, got %zu\n",
             progname, libspectrum_snap_disciple_rom_length( snap, 0 ) );
    goto done;
  }

  rom = libspectrum_new( libspectrum_byte, 0x8000 );
  rom[0]      = 0x11;
  rom[0x7fff] = 0x22;

  libspectrum_snap_set_disciple_rom( snap, 0, rom );
  libspectrum_snap_set_disciple_rom_length( snap, 0, 0x8000 );

  if( libspectrum_snap_disciple_rom( snap, 0 ) != rom ) {
    fprintf( stderr, "%s: snap_disciple_rom_pointer_rom_length_and_ram_pointer_getter_setter: disciple_rom[0] pointer mismatch after set\n", progname );
    libspectrum_free( rom );
    goto done;
  }
  if( libspectrum_snap_disciple_rom_length( snap, 0 ) != 0x8000 ) {
    fprintf( stderr, "%s: snap_disciple_rom_pointer_rom_length_and_ram_pointer_getter_setter: expected disciple_rom_length[0]=0x8000, got %zu\n",
             progname, libspectrum_snap_disciple_rom_length( snap, 0 ) );
    goto done;
  }
  if( libspectrum_snap_disciple_rom( snap, 0 )[0]      != 0x11 ||
      libspectrum_snap_disciple_rom( snap, 0 )[0x7fff] != 0x22 ) {
    fprintf( stderr, "%s: snap_disciple_rom_pointer_rom_length_and_ram_pointer_getter_setter: disciple_rom[0] data mismatch\n", progname );
    goto done;
  }

  if( libspectrum_snap_disciple_ram( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: snap_disciple_rom_pointer_rom_length_and_ram_pointer_getter_setter: default disciple_ram[0] should be NULL\n", progname );
    goto done;
  }

  ram = libspectrum_new( libspectrum_byte, 0x2000 );
  ram[0]      = 0x33;
  ram[0x1fff] = 0x44;

  libspectrum_snap_set_disciple_ram( snap, 0, ram );
  if( libspectrum_snap_disciple_ram( snap, 0 ) != ram ) {
    fprintf( stderr, "%s: snap_disciple_rom_pointer_rom_length_and_ram_pointer_getter_setter: disciple_ram[0] pointer mismatch after set\n", progname );
    libspectrum_free( ram );
    goto done;
  }
  if( libspectrum_snap_disciple_ram( snap, 0 )[0]      != 0x33 ||
      libspectrum_snap_disciple_ram( snap, 0 )[0x1fff] != 0x44 ) {
    fprintf( stderr, "%s: snap_disciple_rom_pointer_rom_length_and_ram_pointer_getter_setter: disciple_ram[0] data mismatch\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_usource_active_paged_custom_rom_int_flags_getter_setter( void )
{
  /* libspectrum_snap: USource int flags getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_usource_active_paged_custom_rom_int_flags_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_usource_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_usource_active_paged_custom_rom_int_flags_getter_setter: default usource_active should be 0, got %d\n",
             progname, libspectrum_snap_usource_active( snap ) );
    goto done;
  }
  libspectrum_snap_set_usource_active( snap, 1 );
  if( libspectrum_snap_usource_active( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_usource_active_paged_custom_rom_int_flags_getter_setter: expected usource_active=1, got %d\n",
             progname, libspectrum_snap_usource_active( snap ) );
    goto done;
  }

  if( libspectrum_snap_usource_paged( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_usource_active_paged_custom_rom_int_flags_getter_setter: default usource_paged should be 0, got %d\n",
             progname, libspectrum_snap_usource_paged( snap ) );
    goto done;
  }
  libspectrum_snap_set_usource_paged( snap, 1 );
  if( libspectrum_snap_usource_paged( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_usource_active_paged_custom_rom_int_flags_getter_setter: expected usource_paged=1, got %d\n",
             progname, libspectrum_snap_usource_paged( snap ) );
    goto done;
  }

  if( libspectrum_snap_usource_custom_rom( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_usource_active_paged_custom_rom_int_flags_getter_setter: default usource_custom_rom should be 0, got %d\n",
             progname, libspectrum_snap_usource_custom_rom( snap ) );
    goto done;
  }
  libspectrum_snap_set_usource_custom_rom( snap, 1 );
  if( libspectrum_snap_usource_custom_rom( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_usource_active_paged_custom_rom_int_flags_getter_setter: expected usource_custom_rom=1, got %d\n",
             progname, libspectrum_snap_usource_custom_rom( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_usource_rom_pointer_and_rom_length_getter_setter( void )
{
  /* libspectrum_snap: USource ROM pointer and ROM length getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  libspectrum_byte *rom;
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_usource_rom_pointer_and_rom_length_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_usource_rom( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: snap_usource_rom_pointer_and_rom_length_getter_setter: default usource_rom[0] should be NULL\n", progname );
    goto done;
  }

  if( libspectrum_snap_usource_rom_length( snap, 0 ) != 0 ) {
    fprintf( stderr, "%s: snap_usource_rom_pointer_and_rom_length_getter_setter: default usource_rom_length[0] should be 0, got %zu\n",
             progname, libspectrum_snap_usource_rom_length( snap, 0 ) );
    goto done;
  }

  rom = libspectrum_new( libspectrum_byte, 0x4000 );
  rom[0]      = 0xab;
  rom[0x3fff] = 0xcd;

  libspectrum_snap_set_usource_rom( snap, 0, rom );
  libspectrum_snap_set_usource_rom_length( snap, 0, 0x4000 );

  if( libspectrum_snap_usource_rom( snap, 0 ) != rom ) {
    fprintf( stderr, "%s: snap_usource_rom_pointer_and_rom_length_getter_setter: usource_rom[0] pointer mismatch after set\n", progname );
    libspectrum_free( rom );
    goto done;
  }
  if( libspectrum_snap_usource_rom_length( snap, 0 ) != 0x4000 ) {
    fprintf( stderr, "%s: snap_usource_rom_pointer_and_rom_length_getter_setter: expected usource_rom_length[0]=0x4000, got %zu\n",
             progname, libspectrum_snap_usource_rom_length( snap, 0 ) );
    goto done;
  }
  if( libspectrum_snap_usource_rom( snap, 0 )[0]      != 0xab ||
      libspectrum_snap_usource_rom( snap, 0 )[0x3fff] != 0xcd ) {
    fprintf( stderr, "%s: snap_usource_rom_pointer_and_rom_length_getter_setter: usource_rom[0] data mismatch\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_uspeeched_active_and_paged_getter_setter( void )
{
  /* libspectrum_snap: USpeeched active and paged getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_uspeeched_active_and_paged_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_uspeech_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_uspeeched_active_and_paged_getter_setter: default uspeech_active should be 0, got %d\n",
             progname, libspectrum_snap_uspeech_active( snap ) );
    goto done;
  }
  libspectrum_snap_set_uspeech_active( snap, 1 );
  if( libspectrum_snap_uspeech_active( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_uspeeched_active_and_paged_getter_setter: expected uspeech_active=1, got %d\n",
             progname, libspectrum_snap_uspeech_active( snap ) );
    goto done;
  }

  if( libspectrum_snap_uspeech_paged( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_uspeeched_active_and_paged_getter_setter: default uspeech_paged should be 0, got %d\n",
             progname, libspectrum_snap_uspeech_paged( snap ) );
    goto done;
  }
  libspectrum_snap_set_uspeech_paged( snap, 1 );
  if( libspectrum_snap_uspeech_paged( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_uspeeched_active_and_paged_getter_setter: expected uspeech_paged=1, got %d\n",
             progname, libspectrum_snap_uspeech_paged( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_didaktik80_int_flags_and_fdc_byte_registers_getter_setter( void )
{
  /* libspectrum_snap: Didaktik80 int flags and FDC byte registers getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_didaktik80_int_flags_and_fdc_byte_registers_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_didaktik80_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_didaktik80_int_flags_and_fdc_byte_registers_getter_setter: default didaktik80_active should be 0, got %d\n",
             progname, libspectrum_snap_didaktik80_active( snap ) );
    goto done;
  }
  libspectrum_snap_set_didaktik80_active( snap, 1 );
  if( libspectrum_snap_didaktik80_active( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_didaktik80_int_flags_and_fdc_byte_registers_getter_setter: expected didaktik80_active=1, got %d\n",
             progname, libspectrum_snap_didaktik80_active( snap ) );
    goto done;
  }

  if( libspectrum_snap_didaktik80_paged( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_didaktik80_int_flags_and_fdc_byte_registers_getter_setter: default didaktik80_paged should be 0, got %d\n",
             progname, libspectrum_snap_didaktik80_paged( snap ) );
    goto done;
  }
  libspectrum_snap_set_didaktik80_paged( snap, 1 );
  if( libspectrum_snap_didaktik80_paged( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_didaktik80_int_flags_and_fdc_byte_registers_getter_setter: expected didaktik80_paged=1, got %d\n",
             progname, libspectrum_snap_didaktik80_paged( snap ) );
    goto done;
  }

  if( libspectrum_snap_didaktik80_drive_count( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_didaktik80_int_flags_and_fdc_byte_registers_getter_setter: default didaktik80_drive_count should be 0, got %d\n",
             progname, libspectrum_snap_didaktik80_drive_count( snap ) );
    goto done;
  }
  libspectrum_snap_set_didaktik80_drive_count( snap, 2 );
  if( libspectrum_snap_didaktik80_drive_count( snap ) != 2 ) {
    fprintf( stderr, "%s: snap_didaktik80_int_flags_and_fdc_byte_registers_getter_setter: expected didaktik80_drive_count=2, got %d\n",
             progname, libspectrum_snap_didaktik80_drive_count( snap ) );
    goto done;
  }

  if( libspectrum_snap_didaktik80_custom_rom( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_didaktik80_int_flags_and_fdc_byte_registers_getter_setter: default didaktik80_custom_rom should be 0, got %d\n",
             progname, libspectrum_snap_didaktik80_custom_rom( snap ) );
    goto done;
  }
  libspectrum_snap_set_didaktik80_custom_rom( snap, 1 );
  if( libspectrum_snap_didaktik80_custom_rom( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_didaktik80_int_flags_and_fdc_byte_registers_getter_setter: expected didaktik80_custom_rom=1, got %d\n",
             progname, libspectrum_snap_didaktik80_custom_rom( snap ) );
    goto done;
  }

  if( libspectrum_snap_didaktik80_direction( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_didaktik80_int_flags_and_fdc_byte_registers_getter_setter: default didaktik80_direction should be 0, got %d\n",
             progname, libspectrum_snap_didaktik80_direction( snap ) );
    goto done;
  }
  libspectrum_snap_set_didaktik80_direction( snap, 1 );
  if( libspectrum_snap_didaktik80_direction( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_didaktik80_int_flags_and_fdc_byte_registers_getter_setter: expected didaktik80_direction=1, got %d\n",
             progname, libspectrum_snap_didaktik80_direction( snap ) );
    goto done;
  }

  if( libspectrum_snap_didaktik80_track( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_didaktik80_int_flags_and_fdc_byte_registers_getter_setter: default didaktik80_track should be 0, got 0x%02x\n",
             progname, libspectrum_snap_didaktik80_track( snap ) );
    goto done;
  }
  libspectrum_snap_set_didaktik80_track( snap, 0x12 );
  if( libspectrum_snap_didaktik80_track( snap ) != 0x12 ) {
    fprintf( stderr, "%s: snap_didaktik80_int_flags_and_fdc_byte_registers_getter_setter: expected didaktik80_track=0x12, got 0x%02x\n",
             progname, libspectrum_snap_didaktik80_track( snap ) );
    goto done;
  }

  if( libspectrum_snap_didaktik80_sector( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_didaktik80_int_flags_and_fdc_byte_registers_getter_setter: default didaktik80_sector should be 0, got 0x%02x\n",
             progname, libspectrum_snap_didaktik80_sector( snap ) );
    goto done;
  }
  libspectrum_snap_set_didaktik80_sector( snap, 0x34 );
  if( libspectrum_snap_didaktik80_sector( snap ) != 0x34 ) {
    fprintf( stderr, "%s: snap_didaktik80_int_flags_and_fdc_byte_registers_getter_setter: expected didaktik80_sector=0x34, got 0x%02x\n",
             progname, libspectrum_snap_didaktik80_sector( snap ) );
    goto done;
  }

  if( libspectrum_snap_didaktik80_data( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_didaktik80_int_flags_and_fdc_byte_registers_getter_setter: default didaktik80_data should be 0, got 0x%02x\n",
             progname, libspectrum_snap_didaktik80_data( snap ) );
    goto done;
  }
  libspectrum_snap_set_didaktik80_data( snap, 0x56 );
  if( libspectrum_snap_didaktik80_data( snap ) != 0x56 ) {
    fprintf( stderr, "%s: snap_didaktik80_int_flags_and_fdc_byte_registers_getter_setter: expected didaktik80_data=0x56, got 0x%02x\n",
             progname, libspectrum_snap_didaktik80_data( snap ) );
    goto done;
  }

  if( libspectrum_snap_didaktik80_status( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_didaktik80_int_flags_and_fdc_byte_registers_getter_setter: default didaktik80_status should be 0, got 0x%02x\n",
             progname, libspectrum_snap_didaktik80_status( snap ) );
    goto done;
  }
  libspectrum_snap_set_didaktik80_status( snap, 0x78 );
  if( libspectrum_snap_didaktik80_status( snap ) != 0x78 ) {
    fprintf( stderr, "%s: snap_didaktik80_int_flags_and_fdc_byte_registers_getter_setter: expected didaktik80_status=0x78, got 0x%02x\n",
             progname, libspectrum_snap_didaktik80_status( snap ) );
    goto done;
  }

  if( libspectrum_snap_didaktik80_aux( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_didaktik80_int_flags_and_fdc_byte_registers_getter_setter: default didaktik80_aux should be 0, got 0x%02x\n",
             progname, libspectrum_snap_didaktik80_aux( snap ) );
    goto done;
  }
  libspectrum_snap_set_didaktik80_aux( snap, 0x9a );
  if( libspectrum_snap_didaktik80_aux( snap ) != 0x9a ) {
    fprintf( stderr, "%s: snap_didaktik80_int_flags_and_fdc_byte_registers_getter_setter: expected didaktik80_aux=0x9a, got 0x%02x\n",
             progname, libspectrum_snap_didaktik80_aux( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_didaktik80_rom_pointer_rom_length_and_ram_pointer_getter_setter( void )
{
  /* libspectrum_snap: Didaktik80 ROM pointer, ROM length, and RAM pointer getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  libspectrum_byte *rom, *ram;
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_didaktik80_rom_pointer_rom_length_and_ram_pointer_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_didaktik80_rom( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: snap_didaktik80_rom_pointer_rom_length_and_ram_pointer_getter_setter: default didaktik80_rom[0] should be NULL\n", progname );
    goto done;
  }

  if( libspectrum_snap_didaktik80_rom_length( snap, 0 ) != 0 ) {
    fprintf( stderr, "%s: snap_didaktik80_rom_pointer_rom_length_and_ram_pointer_getter_setter: default didaktik80_rom_length[0] should be 0, got %zu\n",
             progname, libspectrum_snap_didaktik80_rom_length( snap, 0 ) );
    goto done;
  }

  rom = libspectrum_new( libspectrum_byte, 0x4000 );
  rom[0]      = 0x11;
  rom[0x3fff] = 0x22;

  libspectrum_snap_set_didaktik80_rom( snap, 0, rom );
  libspectrum_snap_set_didaktik80_rom_length( snap, 0, 0x4000 );

  if( libspectrum_snap_didaktik80_rom( snap, 0 ) != rom ) {
    fprintf( stderr, "%s: snap_didaktik80_rom_pointer_rom_length_and_ram_pointer_getter_setter: didaktik80_rom[0] pointer mismatch after set\n", progname );
    libspectrum_free( rom );
    goto done;
  }
  if( libspectrum_snap_didaktik80_rom_length( snap, 0 ) != 0x4000 ) {
    fprintf( stderr, "%s: snap_didaktik80_rom_pointer_rom_length_and_ram_pointer_getter_setter: expected didaktik80_rom_length[0]=0x4000, got %zu\n",
             progname, libspectrum_snap_didaktik80_rom_length( snap, 0 ) );
    goto done;
  }
  if( libspectrum_snap_didaktik80_rom( snap, 0 )[0]      != 0x11 ||
      libspectrum_snap_didaktik80_rom( snap, 0 )[0x3fff] != 0x22 ) {
    fprintf( stderr, "%s: snap_didaktik80_rom_pointer_rom_length_and_ram_pointer_getter_setter: didaktik80_rom[0] data mismatch\n", progname );
    goto done;
  }

  if( libspectrum_snap_didaktik80_ram( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: snap_didaktik80_rom_pointer_rom_length_and_ram_pointer_getter_setter: default didaktik80_ram[0] should be NULL\n", progname );
    goto done;
  }

  ram = libspectrum_new( libspectrum_byte, 0x2000 );
  ram[0]      = 0x33;
  ram[0x1fff] = 0x44;

  libspectrum_snap_set_didaktik80_ram( snap, 0, ram );
  if( libspectrum_snap_didaktik80_ram( snap, 0 ) != ram ) {
    fprintf( stderr, "%s: snap_didaktik80_rom_pointer_rom_length_and_ram_pointer_getter_setter: didaktik80_ram[0] pointer mismatch after set\n", progname );
    libspectrum_free( ram );
    goto done;
  }
  if( libspectrum_snap_didaktik80_ram( snap, 0 )[0]      != 0x33 ||
      libspectrum_snap_didaktik80_ram( snap, 0 )[0x1fff] != 0x44 ) {
    fprintf( stderr, "%s: snap_didaktik80_rom_pointer_rom_length_and_ram_pointer_getter_setter: didaktik80_ram[0] data mismatch\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_beta_disk_custom_rom_direction_and_fdc_byte_registers_getter_setter( void )
{
  /* libspectrum_snap: BetaDisk custom_rom, direction, and FDC byte registers (system, track, sector, data, status) */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_beta_disk_custom_rom_direction_and_fdc_byte_registers_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_beta_custom_rom( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_beta_disk_custom_rom_direction_and_fdc_byte_registers_getter_setter: default beta_custom_rom should be 0, got %d\n",
             progname, libspectrum_snap_beta_custom_rom( snap ) );
    goto done;
  }
  libspectrum_snap_set_beta_custom_rom( snap, 1 );
  if( libspectrum_snap_beta_custom_rom( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_beta_disk_custom_rom_direction_and_fdc_byte_registers_getter_setter: expected beta_custom_rom=1, got %d\n",
             progname, libspectrum_snap_beta_custom_rom( snap ) );
    goto done;
  }

  if( libspectrum_snap_beta_direction( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_beta_disk_custom_rom_direction_and_fdc_byte_registers_getter_setter: default beta_direction should be 0, got %d\n",
             progname, libspectrum_snap_beta_direction( snap ) );
    goto done;
  }
  libspectrum_snap_set_beta_direction( snap, 1 );
  if( libspectrum_snap_beta_direction( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_beta_disk_custom_rom_direction_and_fdc_byte_registers_getter_setter: expected beta_direction=1, got %d\n",
             progname, libspectrum_snap_beta_direction( snap ) );
    goto done;
  }

  if( libspectrum_snap_beta_system( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_beta_disk_custom_rom_direction_and_fdc_byte_registers_getter_setter: default beta_system should be 0, got 0x%02x\n",
             progname, libspectrum_snap_beta_system( snap ) );
    goto done;
  }
  libspectrum_snap_set_beta_system( snap, 0x03 );
  if( libspectrum_snap_beta_system( snap ) != 0x03 ) {
    fprintf( stderr, "%s: snap_beta_disk_custom_rom_direction_and_fdc_byte_registers_getter_setter: expected beta_system=0x03, got 0x%02x\n",
             progname, libspectrum_snap_beta_system( snap ) );
    goto done;
  }

  if( libspectrum_snap_beta_track( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_beta_disk_custom_rom_direction_and_fdc_byte_registers_getter_setter: default beta_track should be 0, got %d\n",
             progname, libspectrum_snap_beta_track( snap ) );
    goto done;
  }
  libspectrum_snap_set_beta_track( snap, 42 );
  if( libspectrum_snap_beta_track( snap ) != 42 ) {
    fprintf( stderr, "%s: snap_beta_disk_custom_rom_direction_and_fdc_byte_registers_getter_setter: expected beta_track=42, got %d\n",
             progname, libspectrum_snap_beta_track( snap ) );
    goto done;
  }

  if( libspectrum_snap_beta_sector( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_beta_disk_custom_rom_direction_and_fdc_byte_registers_getter_setter: default beta_sector should be 0, got %d\n",
             progname, libspectrum_snap_beta_sector( snap ) );
    goto done;
  }
  libspectrum_snap_set_beta_sector( snap, 7 );
  if( libspectrum_snap_beta_sector( snap ) != 7 ) {
    fprintf( stderr, "%s: snap_beta_disk_custom_rom_direction_and_fdc_byte_registers_getter_setter: expected beta_sector=7, got %d\n",
             progname, libspectrum_snap_beta_sector( snap ) );
    goto done;
  }

  if( libspectrum_snap_beta_data( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_beta_disk_custom_rom_direction_and_fdc_byte_registers_getter_setter: default beta_data should be 0, got 0x%02x\n",
             progname, libspectrum_snap_beta_data( snap ) );
    goto done;
  }
  libspectrum_snap_set_beta_data( snap, 0xdb );
  if( libspectrum_snap_beta_data( snap ) != 0xdb ) {
    fprintf( stderr, "%s: snap_beta_disk_custom_rom_direction_and_fdc_byte_registers_getter_setter: expected beta_data=0xdb, got 0x%02x\n",
             progname, libspectrum_snap_beta_data( snap ) );
    goto done;
  }

  if( libspectrum_snap_beta_status( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_beta_disk_custom_rom_direction_and_fdc_byte_registers_getter_setter: default beta_status should be 0, got 0x%02x\n",
             progname, libspectrum_snap_beta_status( snap ) );
    goto done;
  }
  libspectrum_snap_set_beta_status( snap, 0x80 );
  if( libspectrum_snap_beta_status( snap ) != 0x80 ) {
    fprintf( stderr, "%s: snap_beta_disk_custom_rom_direction_and_fdc_byte_registers_getter_setter: expected beta_status=0x80, got 0x%02x\n",
             progname, libspectrum_snap_beta_status( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_beta_disk_rom_single_pointer_getter_setter( void )
{
  /* libspectrum_snap: BetaDisk ROM single-pointer getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  libspectrum_byte *rom;
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_beta_disk_rom_single_pointer_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_beta_rom( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: snap_beta_disk_rom_single_pointer_getter_setter: default beta_rom[0] should be NULL\n", progname );
    goto done;
  }

  rom = libspectrum_new( libspectrum_byte, 0x4000 );
  rom[0]      = 0xc3;
  rom[0x3fff] = 0xff;

  libspectrum_snap_set_beta_rom( snap, 0, rom );
  if( libspectrum_snap_beta_rom( snap, 0 ) != rom ) {
    fprintf( stderr, "%s: snap_beta_disk_rom_single_pointer_getter_setter: beta_rom[0] pointer mismatch after set\n", progname );
    libspectrum_free( rom );
    goto done;
  }
  if( libspectrum_snap_beta_rom( snap, 0 )[0]      != 0xc3 ||
      libspectrum_snap_beta_rom( snap, 0 )[0x3fff] != 0xff ) {
    fprintf( stderr, "%s: snap_beta_disk_rom_single_pointer_getter_setter: beta_rom[0] data mismatch\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}
