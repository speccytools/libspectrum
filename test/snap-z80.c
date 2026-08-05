#include "config.h"

#include <stdio.h>

#include "test.h"

test_return_t
snap_main_z80_register_getter_setter_a_f_bc_de_hl_alternates( void )
{
  /* libspectrum_snap: main Z80 register getter/setter (a, f, bc, de, hl, alternates) */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_main_z80_register_getter_setter_a_f_bc_de_hl_alternates: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  libspectrum_snap_set_a( snap, 0x12 );
  libspectrum_snap_set_f( snap, 0x34 );
  libspectrum_snap_set_bc( snap, 0x5678 );
  libspectrum_snap_set_de( snap, 0x9abc );
  libspectrum_snap_set_hl( snap, 0xdef0 );
  libspectrum_snap_set_a_( snap, 0xaa );
  libspectrum_snap_set_f_( snap, 0xbb );
  libspectrum_snap_set_bc_( snap, 0x1122 );
  libspectrum_snap_set_de_( snap, 0x3344 );
  libspectrum_snap_set_hl_( snap, 0x5566 );

  if( libspectrum_snap_a( snap ) != 0x12 ) {
    fprintf( stderr, "%s: snap_main_z80_register_getter_setter_a_f_bc_de_hl_alternates: expected a=0x12, got 0x%02x\n", progname,
             libspectrum_snap_a( snap ) );
    goto done;
  }
  if( libspectrum_snap_f( snap ) != 0x34 ) {
    fprintf( stderr, "%s: snap_main_z80_register_getter_setter_a_f_bc_de_hl_alternates: expected f=0x34, got 0x%02x\n", progname,
             libspectrum_snap_f( snap ) );
    goto done;
  }
  if( libspectrum_snap_bc( snap ) != 0x5678 ) {
    fprintf( stderr, "%s: snap_main_z80_register_getter_setter_a_f_bc_de_hl_alternates: expected bc=0x5678, got 0x%04x\n", progname,
             libspectrum_snap_bc( snap ) );
    goto done;
  }
  if( libspectrum_snap_de( snap ) != 0x9abc ) {
    fprintf( stderr, "%s: snap_main_z80_register_getter_setter_a_f_bc_de_hl_alternates: expected de=0x9abc, got 0x%04x\n", progname,
             libspectrum_snap_de( snap ) );
    goto done;
  }
  if( libspectrum_snap_hl( snap ) != 0xdef0 ) {
    fprintf( stderr, "%s: snap_main_z80_register_getter_setter_a_f_bc_de_hl_alternates: expected hl=0xdef0, got 0x%04x\n", progname,
             libspectrum_snap_hl( snap ) );
    goto done;
  }
  if( libspectrum_snap_a_( snap ) != 0xaa ) {
    fprintf( stderr, "%s: snap_main_z80_register_getter_setter_a_f_bc_de_hl_alternates: expected a_=0xaa, got 0x%02x\n", progname,
             libspectrum_snap_a_( snap ) );
    goto done;
  }
  if( libspectrum_snap_f_( snap ) != 0xbb ) {
    fprintf( stderr, "%s: snap_main_z80_register_getter_setter_a_f_bc_de_hl_alternates: expected f_=0xbb, got 0x%02x\n", progname,
             libspectrum_snap_f_( snap ) );
    goto done;
  }
  if( libspectrum_snap_bc_( snap ) != 0x1122 ) {
    fprintf( stderr, "%s: snap_main_z80_register_getter_setter_a_f_bc_de_hl_alternates: expected bc_=0x1122, got 0x%04x\n", progname,
             libspectrum_snap_bc_( snap ) );
    goto done;
  }
  if( libspectrum_snap_de_( snap ) != 0x3344 ) {
    fprintf( stderr, "%s: snap_main_z80_register_getter_setter_a_f_bc_de_hl_alternates: expected de_=0x3344, got 0x%04x\n", progname,
             libspectrum_snap_de_( snap ) );
    goto done;
  }
  if( libspectrum_snap_hl_( snap ) != 0x5566 ) {
    fprintf( stderr, "%s: snap_main_z80_register_getter_setter_a_f_bc_de_hl_alternates: expected hl_=0x5566, got 0x%04x\n", progname,
             libspectrum_snap_hl_( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_index_and_special_register_getter_setter_ix_iy_i_r_sp_pc( void )
{
  /* libspectrum_snap: index and special register getter/setter (ix, iy, i, r, sp, pc) */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_index_and_special_register_getter_setter_ix_iy_i_r_sp_pc: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  libspectrum_snap_set_ix( snap, 0x1234 );
  libspectrum_snap_set_iy( snap, 0x5678 );
  libspectrum_snap_set_i( snap, 0xfe );
  libspectrum_snap_set_r( snap, 0xdc );
  libspectrum_snap_set_sp( snap, 0xba98 );
  libspectrum_snap_set_pc( snap, 0x7654 );

  if( libspectrum_snap_ix( snap ) != 0x1234 ) {
    fprintf( stderr, "%s: snap_index_and_special_register_getter_setter_ix_iy_i_r_sp_pc: expected ix=0x1234, got 0x%04x\n", progname,
             libspectrum_snap_ix( snap ) );
    goto done;
  }
  if( libspectrum_snap_iy( snap ) != 0x5678 ) {
    fprintf( stderr, "%s: snap_index_and_special_register_getter_setter_ix_iy_i_r_sp_pc: expected iy=0x5678, got 0x%04x\n", progname,
             libspectrum_snap_iy( snap ) );
    goto done;
  }
  if( libspectrum_snap_i( snap ) != 0xfe ) {
    fprintf( stderr, "%s: snap_index_and_special_register_getter_setter_ix_iy_i_r_sp_pc: expected i=0xfe, got 0x%02x\n", progname,
             libspectrum_snap_i( snap ) );
    goto done;
  }
  if( libspectrum_snap_r( snap ) != 0xdc ) {
    fprintf( stderr, "%s: snap_index_and_special_register_getter_setter_ix_iy_i_r_sp_pc: expected r=0xdc, got 0x%02x\n", progname,
             libspectrum_snap_r( snap ) );
    goto done;
  }
  if( libspectrum_snap_sp( snap ) != 0xba98 ) {
    fprintf( stderr, "%s: snap_index_and_special_register_getter_setter_ix_iy_i_r_sp_pc: expected sp=0xba98, got 0x%04x\n", progname,
             libspectrum_snap_sp( snap ) );
    goto done;
  }
  if( libspectrum_snap_pc( snap ) != 0x7654 ) {
    fprintf( stderr, "%s: snap_index_and_special_register_getter_setter_ix_iy_i_r_sp_pc: expected pc=0x7654, got 0x%04x\n", progname,
             libspectrum_snap_pc( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_z80_status_getter_setter_iff1_iff2_im_tstates_halted( void )
{
  /* libspectrum_snap: Z80 status getter/setter (iff1, iff2, im, tstates, halted) */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_z80_status_getter_setter_iff1_iff2_im_tstates_halted: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  libspectrum_snap_set_iff1( snap, 1 );
  libspectrum_snap_set_iff2( snap, 0 );
  libspectrum_snap_set_im( snap, 2 );
  libspectrum_snap_set_tstates( snap, 69888 );
  libspectrum_snap_set_halted( snap, 1 );

  if( libspectrum_snap_iff1( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_z80_status_getter_setter_iff1_iff2_im_tstates_halted: expected iff1=1, got %d\n", progname,
             libspectrum_snap_iff1( snap ) );
    goto done;
  }
  if( libspectrum_snap_iff2( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_z80_status_getter_setter_iff1_iff2_im_tstates_halted: expected iff2=0, got %d\n", progname,
             libspectrum_snap_iff2( snap ) );
    goto done;
  }
  if( libspectrum_snap_im( snap ) != 2 ) {
    fprintf( stderr, "%s: snap_z80_status_getter_setter_iff1_iff2_im_tstates_halted: expected im=2, got %d\n", progname,
             libspectrum_snap_im( snap ) );
    goto done;
  }
  if( libspectrum_snap_tstates( snap ) != 69888 ) {
    fprintf( stderr, "%s: snap_z80_status_getter_setter_iff1_iff2_im_tstates_halted: expected tstates=69888, got %lu\n", progname,
             (unsigned long)libspectrum_snap_tstates( snap ) );
    goto done;
  }
  if( libspectrum_snap_halted( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_z80_status_getter_setter_iff1_iff2_im_tstates_halted: expected halted=1, got %d\n", progname,
             libspectrum_snap_halted( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_machine_type_getter_setter_and_default_value( void )
{
  /* libspectrum_snap: machine type getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_machine_type_getter_setter_and_default_value: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_machine( snap ) != LIBSPECTRUM_MACHINE_UNKNOWN ) {
    fprintf( stderr, "%s: snap_machine_type_getter_setter_and_default_value: default machine should be UNKNOWN, got %d\n",
             progname, (int)libspectrum_snap_machine( snap ) );
    goto done;
  }

  libspectrum_snap_set_machine( snap, LIBSPECTRUM_MACHINE_48 );
  if( libspectrum_snap_machine( snap ) != LIBSPECTRUM_MACHINE_48 ) {
    fprintf( stderr, "%s: snap_machine_type_getter_setter_and_default_value: expected MACHINE_48, got %d\n",
             progname, (int)libspectrum_snap_machine( snap ) );
    goto done;
  }

  libspectrum_snap_set_machine( snap, LIBSPECTRUM_MACHINE_128 );
  if( libspectrum_snap_machine( snap ) != LIBSPECTRUM_MACHINE_128 ) {
    fprintf( stderr, "%s: snap_machine_type_getter_setter_and_default_value: expected MACHINE_128, got %d\n",
             progname, (int)libspectrum_snap_machine( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_memptr_getter_setter( void )
{
  /* libspectrum_snap: memptr getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_memptr_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  libspectrum_snap_set_memptr( snap, 0xabcd );
  if( libspectrum_snap_memptr( snap ) != 0xabcd ) {
    fprintf( stderr, "%s: snap_memptr_getter_setter: expected memptr=0xabcd, got 0x%04x\n",
             progname, libspectrum_snap_memptr( snap ) );
    goto done;
  }

  libspectrum_snap_set_memptr( snap, 0x0000 );
  if( libspectrum_snap_memptr( snap ) != 0x0000 ) {
    fprintf( stderr, "%s: snap_memptr_getter_setter: expected memptr=0x0000, got 0x%04x\n",
             progname, libspectrum_snap_memptr( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_ula_128k_memory_port_and_ay_register_port_getter_setter( void )
{
  /* libspectrum_snap: ULA and 128K memory port getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_ula_128k_memory_port_and_ay_register_port_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  libspectrum_snap_set_out_ula( snap, 0x07 );
  if( libspectrum_snap_out_ula( snap ) != 0x07 ) {
    fprintf( stderr, "%s: snap_ula_128k_memory_port_and_ay_register_port_getter_setter: expected out_ula=0x07, got 0x%02x\n",
             progname, libspectrum_snap_out_ula( snap ) );
    goto done;
  }

  libspectrum_snap_set_out_128_memoryport( snap, 0x05 );
  if( libspectrum_snap_out_128_memoryport( snap ) != 0x05 ) {
    fprintf( stderr, "%s: snap_ula_128k_memory_port_and_ay_register_port_getter_setter: expected out_128_memoryport=0x05, got 0x%02x\n",
             progname, libspectrum_snap_out_128_memoryport( snap ) );
    goto done;
  }

  libspectrum_snap_set_out_ay_registerport( snap, 0x0e );
  if( libspectrum_snap_out_ay_registerport( snap ) != 0x0e ) {
    fprintf( stderr, "%s: snap_ula_128k_memory_port_and_ay_register_port_getter_setter: expected out_ay_registerport=0x0e, got 0x%02x\n",
             progname, libspectrum_snap_out_ay_registerport( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_last_instruction_ei_and_set_f_getter_setter( void )
{
  /* libspectrum_snap: last_instruction_ei and last_instruction_set_f getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_last_instruction_ei_and_set_f_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_last_instruction_ei( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_last_instruction_ei_and_set_f_getter_setter: default last_instruction_ei should be 0, got %d\n",
             progname, libspectrum_snap_last_instruction_ei( snap ) );
    goto done;
  }
  libspectrum_snap_set_last_instruction_ei( snap, 1 );
  if( libspectrum_snap_last_instruction_ei( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_last_instruction_ei_and_set_f_getter_setter: expected last_instruction_ei=1, got %d\n",
             progname, libspectrum_snap_last_instruction_ei( snap ) );
    goto done;
  }

  if( libspectrum_snap_last_instruction_set_f( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_last_instruction_ei_and_set_f_getter_setter: default last_instruction_set_f should be 0, got %d\n",
             progname, libspectrum_snap_last_instruction_set_f( snap ) );
    goto done;
  }
  libspectrum_snap_set_last_instruction_set_f( snap, 1 );
  if( libspectrum_snap_last_instruction_set_f( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_last_instruction_ei_and_set_f_getter_setter: expected last_instruction_set_f=1, got %d\n",
             progname, libspectrum_snap_last_instruction_set_f( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_out_plus3_memoryport_and_scld_hsr_dec_getter_setter( void )
{
  /* libspectrum_snap: out_plus3_memoryport, out_scld_hsr, and out_scld_dec getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_out_plus3_memoryport_and_scld_hsr_dec_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_out_plus3_memoryport( snap ) != 0x08 ) {
    fprintf( stderr, "%s: snap_out_plus3_memoryport_and_scld_hsr_dec_getter_setter: default out_plus3_memoryport should be 0x08, got 0x%02x\n",
             progname, libspectrum_snap_out_plus3_memoryport( snap ) );
    goto done;
  }
  libspectrum_snap_set_out_plus3_memoryport( snap, 0x28 );
  if( libspectrum_snap_out_plus3_memoryport( snap ) != 0x28 ) {
    fprintf( stderr, "%s: snap_out_plus3_memoryport_and_scld_hsr_dec_getter_setter: expected out_plus3_memoryport=0x28, got 0x%02x\n",
             progname, libspectrum_snap_out_plus3_memoryport( snap ) );
    goto done;
  }

  if( libspectrum_snap_out_scld_hsr( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_out_plus3_memoryport_and_scld_hsr_dec_getter_setter: default out_scld_hsr should be 0, got 0x%02x\n",
             progname, libspectrum_snap_out_scld_hsr( snap ) );
    goto done;
  }
  libspectrum_snap_set_out_scld_hsr( snap, 0x49 );
  if( libspectrum_snap_out_scld_hsr( snap ) != 0x49 ) {
    fprintf( stderr, "%s: snap_out_plus3_memoryport_and_scld_hsr_dec_getter_setter: expected out_scld_hsr=0x49, got 0x%02x\n",
             progname, libspectrum_snap_out_scld_hsr( snap ) );
    goto done;
  }

  if( libspectrum_snap_out_scld_dec( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_out_plus3_memoryport_and_scld_hsr_dec_getter_setter: default out_scld_dec should be 0, got 0x%02x\n",
             progname, libspectrum_snap_out_scld_dec( snap ) );
    goto done;
  }
  libspectrum_snap_set_out_scld_dec( snap, 0x9d );
  if( libspectrum_snap_out_scld_dec( snap ) != 0x9d ) {
    fprintf( stderr, "%s: snap_out_plus3_memoryport_and_scld_hsr_dec_getter_setter: expected out_scld_dec=0x9d, got 0x%02x\n",
             progname, libspectrum_snap_out_scld_dec( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

test_return_t
snap_late_timings_getter_setter( void )
{
  /* libspectrum_snap: late_timings getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: snap_late_timings_getter_setter: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_late_timings( snap ) != 0 ) {
    fprintf( stderr, "%s: snap_late_timings_getter_setter: default late_timings should be 0, got %d\n",
             progname, libspectrum_snap_late_timings( snap ) );
    goto done;
  }
  libspectrum_snap_set_late_timings( snap, 1 );
  if( libspectrum_snap_late_timings( snap ) != 1 ) {
    fprintf( stderr, "%s: snap_late_timings_getter_setter: expected late_timings=1, got %d\n",
             progname, libspectrum_snap_late_timings( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}
