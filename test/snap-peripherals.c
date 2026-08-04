#include "config.h"

#include <stdio.h>
#include <string.h>

#include "internals.h"
#include "common.h"
#include "test.h"

test_return_t
snap_specdrum_active_flag_and_signed_dac_getter_setter( void )
{
  /* libspectrum_snap: SpecDrum active flag and DAC getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_specdrum_active_flag_and_signed_dac_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_specdrum_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_specdrum_active_flag_and_signed_dac_getter_setter: default specdrum_active should be 0, got %d\n",
             progname, libspectrum_snap_specdrum_active( snap ) );
    goto done;
  }

  libspectrum_snap_set_specdrum_active( snap, 1 );
  if( libspectrum_snap_specdrum_active( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_specdrum_active_flag_and_signed_dac_getter_setter: expected specdrum_active=1, got %d\n",
             progname, libspectrum_snap_specdrum_active( snap ) );
    goto done;
  }

  if( libspectrum_snap_specdrum_dac( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_specdrum_active_flag_and_signed_dac_getter_setter: default specdrum_dac should be 0, got %d\n",
             progname, (int)libspectrum_snap_specdrum_dac( snap ) );
    goto done;
  }

  libspectrum_snap_set_specdrum_dac( snap, -42 );
  if( libspectrum_snap_specdrum_dac( snap ) != -42 ) {
    fprintf( stderr, "%s: snap_specdrum_active_flag_and_signed_dac_getter_setter: expected specdrum_dac=-42, got %d\n",
             progname, (int)libspectrum_snap_specdrum_dac( snap ) );
    goto done;
  }

  libspectrum_snap_set_specdrum_dac( snap, 100 );
  if( libspectrum_snap_specdrum_dac( snap ) != 100 ) {
    fprintf( stderr, "%s: snap_specdrum_active_flag_and_signed_dac_getter_setter: expected specdrum_dac=100, got %d\n",
             progname, (int)libspectrum_snap_specdrum_dac( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_fuller_box_active_flag_getter_setter( void )
{
  /* libspectrum_snap: Fuller Box active flag getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_fuller_box_active_flag_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_fuller_box_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_fuller_box_active_flag_getter_setter: default fuller_box_active should be 0, got %d\n",
             progname, libspectrum_snap_fuller_box_active( snap ) );
    goto done;
  }

  libspectrum_snap_set_fuller_box_active( snap, 1 );
  if( libspectrum_snap_fuller_box_active( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_fuller_box_active_flag_getter_setter: expected fuller_box_active=1, got %d\n",
             progname, libspectrum_snap_fuller_box_active( snap ) );
    goto done;
  }

  libspectrum_snap_set_fuller_box_active( snap, 0 );
  if( libspectrum_snap_fuller_box_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_fuller_box_active_flag_getter_setter: expected fuller_box_active=0, got %d\n",
             progname, libspectrum_snap_fuller_box_active( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_multiface_active_paged_model_disabled_and_software_lockout_getter_setter( void )
{
  /* libspectrum_snap: Multiface interface flags getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_multiface_active_paged_model_disabled_and_software_lockout_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_multiface_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_multiface_active_paged_model_disabled_and_software_lockout_getter_setter: default multiface_active should be 0, got %d\n",
             progname, libspectrum_snap_multiface_active( snap ) );
    goto done;
  }

  libspectrum_snap_set_multiface_active( snap, 1 );
  if( libspectrum_snap_multiface_active( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_multiface_active_paged_model_disabled_and_software_lockout_getter_setter: expected multiface_active=1, got %d\n",
             progname, libspectrum_snap_multiface_active( snap ) );
    goto done;
  }

  if( libspectrum_snap_multiface_paged( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_multiface_active_paged_model_disabled_and_software_lockout_getter_setter: default multiface_paged should be 0, got %d\n",
             progname, libspectrum_snap_multiface_paged( snap ) );
    goto done;
  }

  libspectrum_snap_set_multiface_paged( snap, 1 );
  if( libspectrum_snap_multiface_paged( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_multiface_active_paged_model_disabled_and_software_lockout_getter_setter: expected multiface_paged=1, got %d\n",
             progname, libspectrum_snap_multiface_paged( snap ) );
    goto done;
  }

  if( libspectrum_snap_multiface_model_one( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_multiface_active_paged_model_disabled_and_software_lockout_getter_setter: default multiface_model_one should be 0, got %d\n",
             progname, libspectrum_snap_multiface_model_one( snap ) );
    goto done;
  }

  libspectrum_snap_set_multiface_model_one( snap, 1 );
  if( libspectrum_snap_multiface_model_one( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_multiface_active_paged_model_disabled_and_software_lockout_getter_setter: expected multiface_model_one=1, got %d\n",
             progname, libspectrum_snap_multiface_model_one( snap ) );
    goto done;
  }

  if( libspectrum_snap_multiface_model_128( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_multiface_active_paged_model_disabled_and_software_lockout_getter_setter: default multiface_model_128 should be 0, got %d\n",
             progname, libspectrum_snap_multiface_model_128( snap ) );
    goto done;
  }

  libspectrum_snap_set_multiface_model_128( snap, 1 );
  if( libspectrum_snap_multiface_model_128( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_multiface_active_paged_model_disabled_and_software_lockout_getter_setter: expected multiface_model_128=1, got %d\n",
             progname, libspectrum_snap_multiface_model_128( snap ) );
    goto done;
  }

  if( libspectrum_snap_multiface_disabled( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_multiface_active_paged_model_disabled_and_software_lockout_getter_setter: default multiface_disabled should be 0, got %d\n",
             progname, libspectrum_snap_multiface_disabled( snap ) );
    goto done;
  }

  libspectrum_snap_set_multiface_disabled( snap, 1 );
  if( libspectrum_snap_multiface_disabled( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_multiface_active_paged_model_disabled_and_software_lockout_getter_setter: expected multiface_disabled=1, got %d\n",
             progname, libspectrum_snap_multiface_disabled( snap ) );
    goto done;
  }

  if( libspectrum_snap_multiface_software_lockout( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_multiface_active_paged_model_disabled_and_software_lockout_getter_setter: default multiface_software_lockout should be 0, got %d\n",
             progname, libspectrum_snap_multiface_software_lockout( snap ) );
    goto done;
  }

  libspectrum_snap_set_multiface_software_lockout( snap, 1 );
  if( libspectrum_snap_multiface_software_lockout( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_multiface_active_paged_model_disabled_and_software_lockout_getter_setter: expected multiface_software_lockout=1, got %d\n",
             progname, libspectrum_snap_multiface_software_lockout( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_spectranet_boolean_int_flags_getter_setter( void )
{
  /* libspectrum_snap: Spectranet boolean int flags getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_spectranet_boolean_int_flags_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_spectranet_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_spectranet_boolean_int_flags_getter_setter: default spectranet_active should be 0, got %d\n",
             progname, libspectrum_snap_spectranet_active( snap ) );
    goto done;
  }
  libspectrum_snap_set_spectranet_active( snap, 1 );
  if( libspectrum_snap_spectranet_active( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_spectranet_boolean_int_flags_getter_setter: expected spectranet_active=1, got %d\n",
             progname, libspectrum_snap_spectranet_active( snap ) );
    goto done;
  }

  if( libspectrum_snap_spectranet_paged( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_spectranet_boolean_int_flags_getter_setter: default spectranet_paged should be 0, got %d\n",
             progname, libspectrum_snap_spectranet_paged( snap ) );
    goto done;
  }
  libspectrum_snap_set_spectranet_paged( snap, 1 );
  if( libspectrum_snap_spectranet_paged( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_spectranet_boolean_int_flags_getter_setter: expected spectranet_paged=1, got %d\n",
             progname, libspectrum_snap_spectranet_paged( snap ) );
    goto done;
  }

  if( libspectrum_snap_spectranet_paged_via_io( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_spectranet_boolean_int_flags_getter_setter: default spectranet_paged_via_io should be 0, got %d\n",
             progname, libspectrum_snap_spectranet_paged_via_io( snap ) );
    goto done;
  }
  libspectrum_snap_set_spectranet_paged_via_io( snap, 1 );
  if( libspectrum_snap_spectranet_paged_via_io( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_spectranet_boolean_int_flags_getter_setter: expected spectranet_paged_via_io=1, got %d\n",
             progname, libspectrum_snap_spectranet_paged_via_io( snap ) );
    goto done;
  }

  if( libspectrum_snap_spectranet_nmi_flipflop( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_spectranet_boolean_int_flags_getter_setter: default spectranet_nmi_flipflop should be 0, got %d\n",
             progname, libspectrum_snap_spectranet_nmi_flipflop( snap ) );
    goto done;
  }
  libspectrum_snap_set_spectranet_nmi_flipflop( snap, 1 );
  if( libspectrum_snap_spectranet_nmi_flipflop( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_spectranet_boolean_int_flags_getter_setter: expected spectranet_nmi_flipflop=1, got %d\n",
             progname, libspectrum_snap_spectranet_nmi_flipflop( snap ) );
    goto done;
  }

  if( libspectrum_snap_spectranet_programmable_trap_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_spectranet_boolean_int_flags_getter_setter: default spectranet_programmable_trap_active should be 0, got %d\n",
             progname, libspectrum_snap_spectranet_programmable_trap_active( snap ) );
    goto done;
  }
  libspectrum_snap_set_spectranet_programmable_trap_active( snap, 1 );
  if( libspectrum_snap_spectranet_programmable_trap_active( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_spectranet_boolean_int_flags_getter_setter: expected spectranet_programmable_trap_active=1, got %d\n",
             progname, libspectrum_snap_spectranet_programmable_trap_active( snap ) );
    goto done;
  }

  if( libspectrum_snap_spectranet_programmable_trap_msb( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_spectranet_boolean_int_flags_getter_setter: default spectranet_programmable_trap_msb should be 0, got %d\n",
             progname, libspectrum_snap_spectranet_programmable_trap_msb( snap ) );
    goto done;
  }
  libspectrum_snap_set_spectranet_programmable_trap_msb( snap, 1 );
  if( libspectrum_snap_spectranet_programmable_trap_msb( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_spectranet_boolean_int_flags_getter_setter: expected spectranet_programmable_trap_msb=1, got %d\n",
             progname, libspectrum_snap_spectranet_programmable_trap_msb( snap ) );
    goto done;
  }

  if( libspectrum_snap_spectranet_all_traps_disabled( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_spectranet_boolean_int_flags_getter_setter: default spectranet_all_traps_disabled should be 0, got %d\n",
             progname, libspectrum_snap_spectranet_all_traps_disabled( snap ) );
    goto done;
  }
  libspectrum_snap_set_spectranet_all_traps_disabled( snap, 1 );
  if( libspectrum_snap_spectranet_all_traps_disabled( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_spectranet_boolean_int_flags_getter_setter: expected spectranet_all_traps_disabled=1, got %d\n",
             progname, libspectrum_snap_spectranet_all_traps_disabled( snap ) );
    goto done;
  }

  if( libspectrum_snap_spectranet_rst8_trap_disabled( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_spectranet_boolean_int_flags_getter_setter: default spectranet_rst8_trap_disabled should be 0, got %d\n",
             progname, libspectrum_snap_spectranet_rst8_trap_disabled( snap ) );
    goto done;
  }
  libspectrum_snap_set_spectranet_rst8_trap_disabled( snap, 1 );
  if( libspectrum_snap_spectranet_rst8_trap_disabled( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_spectranet_boolean_int_flags_getter_setter: expected spectranet_rst8_trap_disabled=1, got %d\n",
             progname, libspectrum_snap_spectranet_rst8_trap_disabled( snap ) );
    goto done;
  }

  if( libspectrum_snap_spectranet_deny_downstream_a15( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_spectranet_boolean_int_flags_getter_setter: default spectranet_deny_downstream_a15 should be 0, got %d\n",
             progname, libspectrum_snap_spectranet_deny_downstream_a15( snap ) );
    goto done;
  }
  libspectrum_snap_set_spectranet_deny_downstream_a15( snap, 1 );
  if( libspectrum_snap_spectranet_deny_downstream_a15( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_spectranet_boolean_int_flags_getter_setter: expected spectranet_deny_downstream_a15=1, got %d\n",
             progname, libspectrum_snap_spectranet_deny_downstream_a15( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_spectranet_page_a_b_programmable_trap_word_and_memory_pointers_getter_setter( void )
{
  /* libspectrum_snap: Spectranet page_a/b, programmable_trap (word), and memory pointer fields */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  libspectrum_byte *w5100, *flash, *ram;
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_spectranet_page_a_b_programmable_trap_word_and_memory_pointers_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_spectranet_page_a( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_spectranet_page_a_b_programmable_trap_word_and_memory_pointers_getter_setter: default spectranet_page_a should be 0, got %d\n",
             progname, libspectrum_snap_spectranet_page_a( snap ) );
    goto done;
  }
  libspectrum_snap_set_spectranet_page_a( snap, 3 );
  if( libspectrum_snap_spectranet_page_a( snap ) != 3 ) {
    fprintf( stderr, "%s: snap_spectranet_page_a_b_programmable_trap_word_and_memory_pointers_getter_setter: expected spectranet_page_a=3, got %d\n",
             progname, libspectrum_snap_spectranet_page_a( snap ) );
    goto done;
  }

  if( libspectrum_snap_spectranet_page_b( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_spectranet_page_a_b_programmable_trap_word_and_memory_pointers_getter_setter: default spectranet_page_b should be 0, got %d\n",
             progname, libspectrum_snap_spectranet_page_b( snap ) );
    goto done;
  }
  libspectrum_snap_set_spectranet_page_b( snap, 7 );
  if( libspectrum_snap_spectranet_page_b( snap ) != 7 ) {
    fprintf( stderr, "%s: snap_spectranet_page_a_b_programmable_trap_word_and_memory_pointers_getter_setter: expected spectranet_page_b=7, got %d\n",
             progname, libspectrum_snap_spectranet_page_b( snap ) );
    goto done;
  }

  if( libspectrum_snap_spectranet_programmable_trap( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_spectranet_page_a_b_programmable_trap_word_and_memory_pointers_getter_setter: default spectranet_programmable_trap should be 0, got 0x%04x\n",
             progname, libspectrum_snap_spectranet_programmable_trap( snap ) );
    goto done;
  }
  libspectrum_snap_set_spectranet_programmable_trap( snap, 0x1234 );
  if( libspectrum_snap_spectranet_programmable_trap( snap ) != 0x1234 ) {
    fprintf( stderr, "%s: snap_spectranet_page_a_b_programmable_trap_word_and_memory_pointers_getter_setter: expected spectranet_programmable_trap=0x1234, got 0x%04x\n",
             progname, libspectrum_snap_spectranet_programmable_trap( snap ) );
    goto done;
  }

  if( libspectrum_snap_spectranet_w5100( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: snap_spectranet_page_a_b_programmable_trap_word_and_memory_pointers_getter_setter: default spectranet_w5100[0] should be NULL\n", progname );
    goto done;
  }
  w5100 = libspectrum_new( libspectrum_byte, 0x400 );
  w5100[0] = 0xab;
  libspectrum_snap_set_spectranet_w5100( snap, 0, w5100 );
  if( libspectrum_snap_spectranet_w5100( snap, 0 ) != w5100 ) {
    fprintf( stderr, "%s: snap_spectranet_page_a_b_programmable_trap_word_and_memory_pointers_getter_setter: spectranet_w5100[0] pointer mismatch after set\n", progname );
    libspectrum_free( w5100 );
    goto done;
  }

  if( libspectrum_snap_spectranet_flash( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: snap_spectranet_page_a_b_programmable_trap_word_and_memory_pointers_getter_setter: default spectranet_flash[0] should be NULL\n", progname );
    goto done;
  }
  flash = libspectrum_new( libspectrum_byte, 0x20000 );
  flash[0] = 0xcd;
  libspectrum_snap_set_spectranet_flash( snap, 0, flash );
  if( libspectrum_snap_spectranet_flash( snap, 0 ) != flash ) {
    fprintf( stderr, "%s: snap_spectranet_page_a_b_programmable_trap_word_and_memory_pointers_getter_setter: spectranet_flash[0] pointer mismatch after set\n", progname );
    libspectrum_free( flash );
    goto done;
  }

  if( libspectrum_snap_spectranet_ram( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: snap_spectranet_page_a_b_programmable_trap_word_and_memory_pointers_getter_setter: default spectranet_ram[0] should be NULL\n", progname );
    goto done;
  }
  ram = libspectrum_new( libspectrum_byte, 0x20000 );
  ram[0] = 0xef;
  libspectrum_snap_set_spectranet_ram( snap, 0, ram );
  if( libspectrum_snap_spectranet_ram( snap, 0 ) != ram ) {
    fprintf( stderr, "%s: snap_spectranet_page_a_b_programmable_trap_word_and_memory_pointers_getter_setter: spectranet_ram[0] pointer mismatch after set\n", progname );
    libspectrum_free( ram );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_ulaplus_active_palette_enabled_current_register_palette_and_ff_register_getter_setter( void )
{
  /* libspectrum_snap: ULAplus flags, current_register, palette array, and ff_register getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  libspectrum_byte *palette;
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_ulaplus_active_palette_enabled_current_register_palette_and_ff_register_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_ulaplus_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_ulaplus_active_palette_enabled_current_register_palette_and_ff_register_getter_setter: default ulaplus_active should be 0, got %d\n",
             progname, libspectrum_snap_ulaplus_active( snap ) );
    goto done;
  }
  libspectrum_snap_set_ulaplus_active( snap, 1 );
  if( libspectrum_snap_ulaplus_active( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_ulaplus_active_palette_enabled_current_register_palette_and_ff_register_getter_setter: expected ulaplus_active=1, got %d\n",
             progname, libspectrum_snap_ulaplus_active( snap ) );
    goto done;
  }

  if( libspectrum_snap_ulaplus_palette_enabled( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_ulaplus_active_palette_enabled_current_register_palette_and_ff_register_getter_setter: default ulaplus_palette_enabled should be 0, got %d\n",
             progname, libspectrum_snap_ulaplus_palette_enabled( snap ) );
    goto done;
  }
  libspectrum_snap_set_ulaplus_palette_enabled( snap, 1 );
  if( libspectrum_snap_ulaplus_palette_enabled( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_ulaplus_active_palette_enabled_current_register_palette_and_ff_register_getter_setter: expected ulaplus_palette_enabled=1, got %d\n",
             progname, libspectrum_snap_ulaplus_palette_enabled( snap ) );
    goto done;
  }

  if( libspectrum_snap_ulaplus_current_register( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_ulaplus_active_palette_enabled_current_register_palette_and_ff_register_getter_setter: default ulaplus_current_register should be 0, got 0x%02x\n",
             progname, libspectrum_snap_ulaplus_current_register( snap ) );
    goto done;
  }
  libspectrum_snap_set_ulaplus_current_register( snap, 0x3f );
  if( libspectrum_snap_ulaplus_current_register( snap ) != 0x3f ) {
    fprintf( stderr, "%s: snap_ulaplus_active_palette_enabled_current_register_palette_and_ff_register_getter_setter: expected ulaplus_current_register=0x3f, got 0x%02x\n",
             progname, libspectrum_snap_ulaplus_current_register( snap ) );
    goto done;
  }

  if( libspectrum_snap_ulaplus_ff_register( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_ulaplus_active_palette_enabled_current_register_palette_and_ff_register_getter_setter: default ulaplus_ff_register should be 0, got 0x%02x\n",
             progname, libspectrum_snap_ulaplus_ff_register( snap ) );
    goto done;
  }
  libspectrum_snap_set_ulaplus_ff_register( snap, 0xc0 );
  if( libspectrum_snap_ulaplus_ff_register( snap ) != 0xc0 ) {
    fprintf( stderr, "%s: snap_ulaplus_active_palette_enabled_current_register_palette_and_ff_register_getter_setter: expected ulaplus_ff_register=0xc0, got 0x%02x\n",
             progname, libspectrum_snap_ulaplus_ff_register( snap ) );
    goto done;
  }

  if( libspectrum_snap_ulaplus_palette( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: snap_ulaplus_active_palette_enabled_current_register_palette_and_ff_register_getter_setter: default ulaplus_palette[0] should be NULL\n", progname );
    goto done;
  }

  palette = libspectrum_new( libspectrum_byte, 64 );
  palette[0]  = 0xaa;
  palette[63] = 0xbb;

  libspectrum_snap_set_ulaplus_palette( snap, 0, palette );
  if( libspectrum_snap_ulaplus_palette( snap, 0 ) != palette ) {
    fprintf( stderr, "%s: snap_ulaplus_active_palette_enabled_current_register_palette_and_ff_register_getter_setter: ulaplus_palette[0] pointer mismatch after set\n", progname );
    libspectrum_free( palette );
    goto done;
  }
  if( libspectrum_snap_ulaplus_palette( snap, 0 )[0]  != 0xaa ||
      libspectrum_snap_ulaplus_palette( snap, 0 )[63] != 0xbb ) {
    fprintf( stderr, "%s: snap_ulaplus_active_palette_enabled_current_register_palette_and_ff_register_getter_setter: ulaplus_palette[0] data mismatch\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_zx_printer_active_getter_setter( void )
{
  /* libspectrum_snap: zx_printer_active getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_zx_printer_active_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_zx_printer_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_zx_printer_active_getter_setter: default zx_printer_active should be 0, got %d\n",
             progname, libspectrum_snap_zx_printer_active( snap ) );
    goto done;
  }
  libspectrum_snap_set_zx_printer_active( snap, 1 );
  if( libspectrum_snap_zx_printer_active( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_zx_printer_active_getter_setter: expected zx_printer_active=1, got %d\n",
             progname, libspectrum_snap_zx_printer_active( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_covox_active_and_covox_dac_getter_setter( void )
{
  /* libspectrum_snap: covox_active and covox_dac getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_covox_active_and_covox_dac_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_covox_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_covox_active_and_covox_dac_getter_setter: default covox_active should be 0, got %d\n",
             progname, libspectrum_snap_covox_active( snap ) );
    goto done;
  }
  libspectrum_snap_set_covox_active( snap, 1 );
  if( libspectrum_snap_covox_active( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_covox_active_and_covox_dac_getter_setter: expected covox_active=1, got %d\n",
             progname, libspectrum_snap_covox_active( snap ) );
    goto done;
  }

  if( libspectrum_snap_covox_dac( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_covox_active_and_covox_dac_getter_setter: default covox_dac should be 0, got 0x%02x\n",
             progname, libspectrum_snap_covox_dac( snap ) );
    goto done;
  }
  libspectrum_snap_set_covox_dac( snap, 0x80 );
  if( libspectrum_snap_covox_dac( snap ) != 0x80 ) {
    fprintf( stderr, "%s: snap_covox_active_and_covox_dac_getter_setter: expected covox_dac=0x80, got 0x%02x\n",
             progname, libspectrum_snap_covox_dac( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_multiface_red_button_disabled_getter_setter( void )
{
  /* libspectrum_snap: multiface_red_button_disabled getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_multiface_red_button_disabled_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_multiface_red_button_disabled( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_multiface_red_button_disabled_getter_setter: default multiface_red_button_disabled should be 0, got %d\n",
             progname, libspectrum_snap_multiface_red_button_disabled( snap ) );
    goto done;
  }
  libspectrum_snap_set_multiface_red_button_disabled( snap, 1 );
  if( libspectrum_snap_multiface_red_button_disabled( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_multiface_red_button_disabled_getter_setter: expected multiface_red_button_disabled=1, got %d\n",
             progname, libspectrum_snap_multiface_red_button_disabled( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_multiface_ram_pointer_and_multiface_ram_length_getter_setter( void )
{
  /* libspectrum_snap: multiface_ram pointer and multiface_ram_length getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  libspectrum_byte *ram;
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_multiface_ram_pointer_and_multiface_ram_length_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_multiface_ram( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: snap_multiface_ram_pointer_and_multiface_ram_length_getter_setter: default multiface_ram[0] should be NULL\n", progname );
    goto done;
  }

  if( libspectrum_snap_multiface_ram_length( snap, 0 ) != 0 ) {
    fprintf( stderr, "%s: snap_multiface_ram_pointer_and_multiface_ram_length_getter_setter: default multiface_ram_length[0] should be 0, got %zu\n",
             progname, libspectrum_snap_multiface_ram_length( snap, 0 ) );
    goto done;
  }

  ram = libspectrum_new( libspectrum_byte, 0x2000 );
  ram[0]      = 0xde;
  ram[0x1fff] = 0xad;

  libspectrum_snap_set_multiface_ram( snap, 0, ram );
  libspectrum_snap_set_multiface_ram_length( snap, 0, 0x2000 );

  if( libspectrum_snap_multiface_ram( snap, 0 ) != ram ) {
    fprintf( stderr, "%s: snap_multiface_ram_pointer_and_multiface_ram_length_getter_setter: multiface_ram[0] pointer mismatch after set\n", progname );
    libspectrum_free( ram );
    goto done;
  }
  if( libspectrum_snap_multiface_ram_length( snap, 0 ) != 0x2000 ) {
    fprintf( stderr, "%s: snap_multiface_ram_pointer_and_multiface_ram_length_getter_setter: expected multiface_ram_length[0]=0x2000, got %zu\n",
             progname, libspectrum_snap_multiface_ram_length( snap, 0 ) );
    goto done;
  }
  if( libspectrum_snap_multiface_ram( snap, 0 )[0]      != 0xde ||
      libspectrum_snap_multiface_ram( snap, 0 )[0x1fff] != 0xad ) {
    fprintf( stderr, "%s: snap_multiface_ram_pointer_and_multiface_ram_length_getter_setter: multiface_ram[0] data mismatch\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_zxmmc_active_getter_setter( void )
{
  /* libspectrum_snap: zxmmc_active getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_zxmmc_active_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_zxmmc_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_zxmmc_active_getter_setter: default zxmmc_active should be 0, got %d\n",
             progname, libspectrum_snap_zxmmc_active( snap ) );
    goto done;
  }
  libspectrum_snap_set_zxmmc_active( snap, 1 );
  if( libspectrum_snap_zxmmc_active( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_zxmmc_active_getter_setter: expected zxmmc_active=1, got %d\n",
             progname, libspectrum_snap_zxmmc_active( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_ttx2000s_active_getter_setter( void )
{
  /* libspectrum_snap: ttx2000s_active getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_ttx2000s_active_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_ttx2000s_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_ttx2000s_active_getter_setter: default ttx2000s_active should be 0, got %d\n",
             progname, libspectrum_snap_ttx2000s_active( snap ) );
    goto done;
  }
  libspectrum_snap_set_ttx2000s_active( snap, 1 );
  if( libspectrum_snap_ttx2000s_active( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_ttx2000s_active_getter_setter: expected ttx2000s_active=1, got %d\n",
             progname, libspectrum_snap_ttx2000s_active( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_kempston_mouse_active_getter_setter( void )
{
  /* libspectrum_snap: kempston_mouse_active getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_kempston_mouse_active_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_kempston_mouse_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_kempston_mouse_active_getter_setter: default kempston_mouse_active should be 0, got %d\n",
             progname, libspectrum_snap_kempston_mouse_active( snap ) );
    goto done;
  }

  libspectrum_snap_set_kempston_mouse_active( snap, 1 );
  if( libspectrum_snap_kempston_mouse_active( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_kempston_mouse_active_getter_setter: expected kempston_mouse_active=1, got %d\n",
             progname, libspectrum_snap_kempston_mouse_active( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_interface1_custom_rom_rom_pointer_and_rom_length_getter_setter( void )
{
  /* libspectrum_snap: interface1 custom_rom flag, ROM pointer, and ROM length getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  libspectrum_byte *rom;
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_interface1_custom_rom_rom_pointer_and_rom_length_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_interface1_custom_rom( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_interface1_custom_rom_rom_pointer_and_rom_length_getter_setter: default interface1_custom_rom should be 0, got %d\n",
             progname, libspectrum_snap_interface1_custom_rom( snap ) );
    goto done;
  }

  libspectrum_snap_set_interface1_custom_rom( snap, 1 );
  if( libspectrum_snap_interface1_custom_rom( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_interface1_custom_rom_rom_pointer_and_rom_length_getter_setter: expected interface1_custom_rom=1, got %d\n",
             progname, libspectrum_snap_interface1_custom_rom( snap ) );
    goto done;
  }

  if( libspectrum_snap_interface1_rom( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: snap_interface1_custom_rom_rom_pointer_and_rom_length_getter_setter: default interface1_rom[0] should be NULL\n",
             progname );
    goto done;
  }

  if( libspectrum_snap_interface1_rom_length( snap, 0 ) != 0 ) {
    fprintf( stderr, "%s: snap_interface1_custom_rom_rom_pointer_and_rom_length_getter_setter: default interface1_rom_length[0] should be 0, got %zu\n",
             progname, libspectrum_snap_interface1_rom_length( snap, 0 ) );
    goto done;
  }

  rom = libspectrum_new( libspectrum_byte, 0x2000 );
  rom[0]      = 0xbe;
  rom[0x1fff] = 0xef;

  libspectrum_snap_set_interface1_rom( snap, 0, rom );
  libspectrum_snap_set_interface1_rom_length( snap, 0, 0x2000 );

  if( libspectrum_snap_interface1_rom( snap, 0 ) != rom ) {
    fprintf( stderr, "%s: snap_interface1_custom_rom_rom_pointer_and_rom_length_getter_setter: interface1_rom[0] pointer mismatch after set\n",
             progname );
    libspectrum_free( rom );
    goto done;
  }

  if( libspectrum_snap_interface1_rom_length( snap, 0 ) != 0x2000 ) {
    fprintf( stderr, "%s: snap_interface1_custom_rom_rom_pointer_and_rom_length_getter_setter: expected interface1_rom_length[0]=0x2000, got %zu\n",
             progname, libspectrum_snap_interface1_rom_length( snap, 0 ) );
    goto done;
  }

  if( libspectrum_snap_interface1_rom( snap, 0 )[0]      != 0xbe ||
      libspectrum_snap_interface1_rom( snap, 0 )[0x1fff] != 0xef ) {
    fprintf( stderr, "%s: snap_interface1_custom_rom_rom_pointer_and_rom_length_getter_setter: interface1_rom[0] data mismatch\n",
             progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_melodik_active_getter_setter( void )
{
  /* libspectrum_snap: melodik_active getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_melodik_active_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_melodik_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_melodik_active_getter_setter: default melodik_active should be 0, got %d\n",
             progname, libspectrum_snap_melodik_active( snap ) );
    goto done;
  }

  libspectrum_snap_set_melodik_active( snap, 1 );
  if( libspectrum_snap_melodik_active( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_melodik_active_getter_setter: expected melodik_active=1, got %d\n",
             progname, libspectrum_snap_melodik_active( snap ) );
    goto done;
  }

  libspectrum_snap_set_melodik_active( snap, 0 );
  if( libspectrum_snap_melodik_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_melodik_active_getter_setter: expected melodik_active=0, got %d\n",
             progname, libspectrum_snap_melodik_active( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_simpleide_active_getter_setter( void )
{
  /* libspectrum_snap: simpleide_active getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_simpleide_active_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_simpleide_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_simpleide_active_getter_setter: default simpleide_active should be 0, got %d\n",
             progname, libspectrum_snap_simpleide_active( snap ) );
    goto done;
  }

  libspectrum_snap_set_simpleide_active( snap, 1 );
  if( libspectrum_snap_simpleide_active( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_simpleide_active_getter_setter: expected simpleide_active=1, got %d\n",
             progname, libspectrum_snap_simpleide_active( snap ) );
    goto done;
  }

  libspectrum_snap_set_simpleide_active( snap, 0 );
  if( libspectrum_snap_simpleide_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_simpleide_active_getter_setter: expected simpleide_active=0, got %d\n",
             progname, libspectrum_snap_simpleide_active( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_issue2_getter_setter( void )
{
  /* libspectrum_snap: issue2 keyboard mode getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_issue2_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_issue2( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_issue2_getter_setter: default issue2 should be 0, got %d\n",
             progname, libspectrum_snap_issue2( snap ) );
    goto done;
  }

  libspectrum_snap_set_issue2( snap, 1 );
  if( libspectrum_snap_issue2( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_issue2_getter_setter: expected issue2=1, got %d\n",
             progname, libspectrum_snap_issue2( snap ) );
    goto done;
  }

  libspectrum_snap_set_issue2( snap, 0 );
  if( libspectrum_snap_issue2( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_issue2_getter_setter: expected issue2=0, got %d\n",
             progname, libspectrum_snap_issue2( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_interface2_active_and_rom_getter_setter( void )
{
  /* libspectrum_snap: interface2_active flag and interface2_rom pointer getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  libspectrum_byte *rom;
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_interface2_active_and_rom_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_interface2_active( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_interface2_active_and_rom_getter_setter: default interface2_active should be 0, got %d\n",
             progname, libspectrum_snap_interface2_active( snap ) );
    goto done;
  }

  libspectrum_snap_set_interface2_active( snap, 1 );
  if( libspectrum_snap_interface2_active( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_interface2_active_and_rom_getter_setter: expected interface2_active=1, got %d\n",
             progname, libspectrum_snap_interface2_active( snap ) );
    goto done;
  }

  if( libspectrum_snap_interface2_rom( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: snap_interface2_active_and_rom_getter_setter: default interface2_rom[0] should be NULL\n",
             progname );
    goto done;
  }

  rom = libspectrum_new( libspectrum_byte, 0x4000 );
  rom[0]      = 0xc3;
  rom[0x3fff] = 0xff;

  libspectrum_snap_set_interface2_rom( snap, 0, rom );

  if( libspectrum_snap_interface2_rom( snap, 0 ) != rom ) {
    fprintf( stderr, "%s: snap_interface2_active_and_rom_getter_setter: interface2_rom[0] pointer mismatch after set\n",
             progname );
    libspectrum_free( rom );
    goto done;
  }

  if( libspectrum_snap_interface2_rom( snap, 0 )[0]      != 0xc3 ||
      libspectrum_snap_interface2_rom( snap, 0 )[0x3fff] != 0xff ) {
    fprintf( stderr, "%s: snap_interface2_active_and_rom_getter_setter: interface2_rom[0] data mismatch\n",
             progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_joystick_active_count_list_and_inputs_getter_setter( void )
{
  /* libspectrum_snap: joystick_active_count, joystick_list, and joystick_inputs getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_joystick_active_count_list_and_inputs_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_joystick_active_count( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_joystick_active_count_list_and_inputs_getter_setter: default joystick_active_count should be 0, got %zu\n",
             progname, libspectrum_snap_joystick_active_count( snap ) );
    goto done;
  }

  libspectrum_snap_set_joystick_active_count( snap, 1 );
  if( libspectrum_snap_joystick_active_count( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_joystick_active_count_list_and_inputs_getter_setter: expected joystick_active_count=1, got %zu\n",
             progname, libspectrum_snap_joystick_active_count( snap ) );
    goto done;
  }

  if( libspectrum_snap_joystick_list( snap, 0 ) != LIBSPECTRUM_JOYSTICK_NONE ) {
    fprintf( stderr, "%s: snap_joystick_active_count_list_and_inputs_getter_setter: default joystick_list[0] should be NONE\n",
             progname );
    goto done;
  }

  libspectrum_snap_set_joystick_list( snap, 0, LIBSPECTRUM_JOYSTICK_KEMPSTON );
  if( libspectrum_snap_joystick_list( snap, 0 ) != LIBSPECTRUM_JOYSTICK_KEMPSTON ) {
    fprintf( stderr, "%s: snap_joystick_active_count_list_and_inputs_getter_setter: expected joystick_list[0]=KEMPSTON\n",
             progname );
    goto done;
  }

  if( libspectrum_snap_joystick_inputs( snap, 0 ) != 0 ) {
    fprintf( stderr, "%s: snap_joystick_active_count_list_and_inputs_getter_setter: default joystick_inputs[0] should be 0, got %d\n",
             progname, libspectrum_snap_joystick_inputs( snap, 0 ) );
    goto done;
  }

  libspectrum_snap_set_joystick_inputs( snap, 0, LIBSPECTRUM_JOYSTICK_INPUT_JOYSTICK_1 );
  if( libspectrum_snap_joystick_inputs( snap, 0 ) != LIBSPECTRUM_JOYSTICK_INPUT_JOYSTICK_1 ) {
    fprintf( stderr, "%s: snap_joystick_active_count_list_and_inputs_getter_setter: expected joystick_inputs[0]=JOYSTICK_1\n",
             progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_multiface_model_3_getter_setter( void )
{
  /* libspectrum_snap: multiface_model_3 flag getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_multiface_model_3_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_multiface_model_3( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_multiface_model_3_getter_setter: default multiface_model_3 should be 0, got %d\n",
             progname, libspectrum_snap_multiface_model_3( snap ) );
    goto done;
  }

  libspectrum_snap_set_multiface_model_3( snap, 1 );
  if( libspectrum_snap_multiface_model_3( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_multiface_model_3_getter_setter: expected multiface_model_3=1, got %d\n",
             progname, libspectrum_snap_multiface_model_3( snap ) );
    goto done;
  }

  libspectrum_snap_set_multiface_model_3( snap, 0 );
  if( libspectrum_snap_multiface_model_3( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_multiface_model_3_getter_setter: expected multiface_model_3=0 after reset, got %d\n",
             progname, libspectrum_snap_multiface_model_3( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}
