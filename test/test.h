#ifndef TEST_TEST_H
#define TEST_TEST_H

#include "libspectrum.h"

extern const char *progname;

typedef enum test_return_t {
  TEST_PASS,
  TEST_FAIL,
  TEST_INCOMPLETE,
  TEST_SKIPPED,
} test_return_t;

typedef struct test_edge_sequence_t {

  libspectrum_dword length;
  size_t count;
  int flags;

} test_edge_sequence_t;

#define STATIC_TEST_PATH(x) SRCDIR "/test/" x
#define DYNAMIC_TEST_PATH(x) "test/" x

test_return_t check_edges( const char *filename, test_edge_sequence_t *edges,
			   int flags_mask );

test_return_t tape_with_unknown_block( void );
test_return_t tzx_turbo_data_with_zero_pilot_pulses_and_zero_data( void );
test_return_t writing_empty_tap_file( void );
test_return_t invalid_compressed_file_1( void );
test_return_t invalid_compressed_file_2( void );
test_return_t pointer_wraparound_in_szx_file( void );
test_return_t invalid_tzx_gdb( void );
test_return_t empty_tzx_drb( void );
test_return_t invalid_tzx_archive_info_block( void );
test_return_t invalid_hardware_info_block_causes_memory_leak( void );
test_return_t invalid_warajevo_tape_file( void );
test_return_t invalid_tzx_custom_info_block_causes_memory_leak( void );
test_return_t tzx_loop_end_block_with_loop_start_block( void );
test_return_t tzx_loop_blocks( void );
test_return_t complete_tzx_file( void );
test_return_t tzx_loop_blocks_2( void );
test_return_t tzx_jump_blocks( void );
test_return_t csw_empty_file( void );
test_return_t complete_tzx_to_tap_conversion( void );
test_return_t sna_file_with_sp_0x4000( void );
test_return_t sna_file_with_sp_0xffff( void );
test_return_t mdr_write_protection_1( void );
test_return_t mdr_write_protection_2( void );
test_return_t complete_tzx_timings( void );
test_return_t writing_sna_file( void );
test_return_t writing_plus_3_z80_file( void );
test_return_t reading_old_szx_file( void );
test_return_t rzx_invalid_frame_data_error_does_not_free_repeat_frame_pointer( void );
test_return_t zero_tail_length_pzx_file( void );
test_return_t no_pilot_pulse_gdb_tzx_file( void );
test_return_t csw_conversion( void );
test_return_t write_szx_z80r_chunk( void );
test_return_t write_szx_spcr_chunk( void );
test_return_t write_szx_joy_chunk( void );
test_return_t write_szx_keyb_chunk( void );
test_return_t write_szx_zxpr_chunk( void );
test_return_t write_szx_ay_chunk( void );
test_return_t write_szx_scld_chunk( void );
test_return_t write_szx_zxat_chunk( void );
test_return_t write_szx_zxcf_chunk( void );
test_return_t write_szx_amxm_chunk( void );
test_return_t write_szx_side_chunk( void );
test_return_t write_szx_drum_chunk( void );
test_return_t write_szx_covx_chunk( void );
test_return_t read_szx_z80r_chunk( void );
test_return_t read_szx_spcr_chunk( void );
test_return_t read_szx_joy_chunk( void );
test_return_t read_szx_keyb_chunk( void );
test_return_t read_szx_zxpr_chunk( void );
test_return_t read_szx_ay_chunk( void );
test_return_t read_szx_scld_chunk( void );
test_return_t read_szx_zxat_chunk( void );
test_return_t read_szx_zxcf_chunk( void );
test_return_t read_szx_amxm_chunk( void );
test_return_t read_szx_side_chunk( void );
test_return_t read_szx_drum_chunk( void );
test_return_t read_szx_covx_chunk( void );
test_return_t write_szx_zmmc_chunk( void );
test_return_t read_szx_zmmc_chunk( void );
test_return_t write_szx_uspe_chunk( void );
test_return_t read_szx_uspe_chunk( void );
test_return_t write_szx_pltt_chunk( void );
test_return_t read_szx_pltt_chunk( void );
test_return_t write_szx_ramp_chunk( void );
test_return_t read_szx_ramp_chunk( void );
test_return_t write_szx_atrp_chunk( void );
test_return_t write_szx_cfrp_chunk( void );
test_return_t read_szx_atrp_chunk( void );
test_return_t read_szx_cfrp_chunk( void );
test_return_t write_uncompressed_szx_ramp_chunk( void );
test_return_t write_uncompressed_szx_atrp_chunk( void );
test_return_t write_uncompressed_szx_cfrp_chunk( void );
test_return_t read_uncompressed_szx_ramp_chunk( void );
test_return_t read_uncompressed_szx_atrp_chunk( void );
test_return_t read_uncompressed_szx_cfrp_chunk( void );
test_return_t write_szx_b128_chunk( void );
test_return_t read_szx_b128_chunk( void );
test_return_t write_szx_snet_chunk( void );
test_return_t read_szx_snet_chunk( void );
test_return_t write_szx_mfce_chunk( void );
test_return_t read_szx_mfce_chunk( void );
test_return_t write_szx_dide_chunk( void );
test_return_t read_szx_dide_chunk( void );
test_return_t write_szx_dmmc_chunk( void );
test_return_t read_szx_dmmc_chunk( void );
test_return_t write_szx_opus_chunk( void );
test_return_t read_szx_opus_chunk( void );
test_return_t write_szx_plsd_chunk( void );
test_return_t read_szx_plsd_chunk( void );
test_return_t write_szx_dock_chunk( void );
test_return_t read_szx_dock_chunk( void );
test_return_t write_szx_dirp_chunk( void );
test_return_t read_szx_dirp_chunk( void );
test_return_t write_szx_dmrp_chunk( void );
test_return_t read_szx_dmrp_chunk( void );
test_return_t write_szx_if2r_chunk( void );
test_return_t read_szx_if2r_chunk( void );
test_return_t write_szx_snef_chunk( void );
test_return_t read_szx_snef_chunk( void );
test_return_t write_szx_sner_chunk( void );
test_return_t read_szx_sner_chunk( void );
test_return_t write_rzx_with_incompressible_snap( void );
test_return_t tape_peek_next_block( void );
test_return_t read_tzx_raw_block_edge_handling( void );
test_return_t trailing_pause_block_tzx_file( void );
test_return_t read_mono_wav_threshold_fixture( void );
test_return_t read_stereo_wav_mixdown_fixture( void );
test_return_t buffer_write_byte_stores_single_byte( void );
test_return_t buffer_write_word_stores_little_endian_word( void );
test_return_t buffer_write_dword_stores_little_endian_dword( void );
test_return_t buffer_write_buffer_copies_src_contents_to_dest( void );
test_return_t buffer_empty_predicates( void );
test_return_t buffer_clear_resets_data_size_to_zero( void );
test_return_t buffer_set_fills_n_bytes_with_given_value( void );
test_return_t buffer_append_copies_src_to_raw_byte_buffer( void );
test_return_t buffer_write_raw_stores_arbitrary_bytes( void );
test_return_t buffer_auto_grows_beyond_initial_capacity( void );
test_return_t buffer_null_accessors_return_safe_values( void );
test_return_t creator_alloc_free_and_program_getter_setter( void );
test_return_t creator_major_and_minor_version_getter_setter( void );
test_return_t creator_competition_code_and_custom_data_getter_setter( void );
test_return_t snap_main_z80_register_getter_setter_a_f_bc_de_hl_alternates( void );
test_return_t snap_index_and_special_register_getter_setter_ix_iy_i_r_sp_pc( void );
test_return_t snap_z80_status_getter_setter_iff1_iff2_im_tstates_halted( void );
test_return_t microdrive_alloc_free_and_write_protect_getter_setter( void );
test_return_t microdrive_cartridge_len_and_data_getter_setter( void );
test_return_t microdrive_mdr_write_mdr_read_roundtrip( void );
test_return_t snap_machine_type_getter_setter_and_default_value( void );
test_return_t snap_memptr_getter_setter( void );
test_return_t snap_ula_128k_memory_port_and_ay_register_port_getter_setter( void );
test_return_t snap_last_instruction_ei_and_set_f_getter_setter( void );
test_return_t snap_out_plus3_memoryport_and_scld_hsr_dec_getter_setter( void );
test_return_t snap_custom_rom_flag_and_custom_rom_pages_getter_setter( void );
test_return_t snap_ram_pages_getter_setter( void );
test_return_t snap_interface1_active_paged_and_drive_count_getter_setter( void );
test_return_t snap_divide_active_eprom_writeprotect_paged_and_control_getter_setter( void );
test_return_t snap_specdrum_active_flag_and_signed_dac_getter_setter( void );
test_return_t snap_fuller_box_active_flag_getter_setter( void );
test_return_t snap_multiface_active_paged_model_disabled_and_software_lockout_getter_setter( void );
test_return_t snap_ay_registers_array_getter_setter_all_16_registers( void );
test_return_t snap_beta_disk_active_paged_autoboot_and_drive_count_getter_setter( void );
test_return_t snap_zxatasp_active_upload_and_writeprotect_getter_setter( void );
test_return_t snap_zxatasp_port_a_b_c_and_control_getter_setter( void );
test_return_t snap_zxatasp_pages_and_current_page_getter_setter( void );
test_return_t pzx_archive_info_tags_title_and_author_correctly_parsed( void );
test_return_t snap_divide_pages_count_and_divide_eprom_pointer_getter_setter( void );
test_return_t snap_divide_ram_page_pointer_array_getter_setter( void );
test_return_t snap_divmmc_active_eprom_writeprotect_paged_and_control_getter_setter( void );
test_return_t snap_divmmc_pages_count_and_divmmc_eprom_pointer_getter_setter( void );
test_return_t snap_plus_d_active_paged_drive_count_custom_rom_and_direction_getter_setter( void );
test_return_t snap_plus_d_fdc_byte_registers_control_track_sector_data_status_getter_setter( void );
test_return_t snap_plus_d_rom_and_ram_single_pointer_getter_setter( void );
test_return_t snap_zxatasp_ram_page_pointer_array_getter_setter( void );
test_return_t snap_opus_discovery_active_paged_drive_count_custom_rom_and_direction_getter_setter( void );
test_return_t snap_opus_discovery_fdc_byte_registers_track_sector_data_status_via_ports_getter_setter( void );
test_return_t snap_opus_discovery_rom_and_ram_single_pointer_getter_setter( void );
test_return_t snap_spectranet_boolean_int_flags_getter_setter( void );
test_return_t snap_spectranet_page_a_b_programmable_trap_word_and_memory_pointers_getter_setter( void );
test_return_t snap_disciple_active_paged_inhibit_button_drive_count_custom_rom_and_direction_getter_setter( void );
test_return_t snap_disciple_fdc_byte_registers_control_track_sector_data_status_getter_setter( void );
test_return_t snap_disciple_rom_pointer_rom_length_and_ram_pointer_getter_setter( void );
test_return_t snap_usource_active_paged_custom_rom_int_flags_getter_setter( void );
test_return_t snap_usource_rom_pointer_and_rom_length_getter_setter( void );
test_return_t snap_uspeeched_active_and_paged_getter_setter( void );
test_return_t snap_didaktik80_int_flags_and_fdc_byte_registers_getter_setter( void );
test_return_t snap_didaktik80_rom_pointer_rom_length_and_ram_pointer_getter_setter( void );
test_return_t snap_ulaplus_active_palette_enabled_current_register_palette_and_ff_register_getter_setter( void );
test_return_t snap_late_timings_getter_setter( void );
test_return_t snap_zx_printer_active_getter_setter( void );
test_return_t snap_covox_active_and_covox_dac_getter_setter( void );
test_return_t snap_multiface_red_button_disabled_getter_setter( void );
test_return_t snap_multiface_ram_pointer_and_multiface_ram_length_getter_setter( void );
test_return_t snap_zxmmc_active_getter_setter( void );
test_return_t snap_ttx2000s_active_getter_setter( void );
test_return_t snap_kempston_mouse_active_getter_setter( void );
test_return_t snap_interface1_custom_rom_rom_pointer_and_rom_length_getter_setter( void );
test_return_t snap_melodik_active_getter_setter( void );
test_return_t snap_simpleide_active_getter_setter( void );
test_return_t snap_issue2_getter_setter( void );
test_return_t snap_interface2_active_and_rom_getter_setter( void );
test_return_t snap_joystick_active_count_list_and_inputs_getter_setter( void );
test_return_t tape_turbo_block_pilot_length_sync1_length_sync2_length_getter_setter( void );
test_return_t tape_turbo_block_bit0_length_bit1_length_pilot_pulses_pause_getter_setter( void );
test_return_t tape_pure_tone_block_pulse_length_and_count_getter_setter( void );
test_return_t tape_pure_data_block_bit0_length_bit1_length_bits_in_last_byte_pause_getter_setter( void );
test_return_t tape_pause_block_pause_length_and_level_getter_setter( void );
test_return_t tape_rom_block_data_data_length_and_pause_getter_setter( void );
test_return_t tzx_pulse_sequence_over_255_splits_into_multiple_pulses_blocks( void );
test_return_t tape_pause_tstates_getter_setter_across_block_types( void );
test_return_t tape_rle_pulse_block_scale_data_and_data_length_getter_setter( void );
test_return_t tape_pulses_block_count_and_pulse_lengths_getter_setter( void );
test_return_t tape_pulse_sequence_block_count_pulse_lengths_and_pulse_repeats_getter_setter( void );
test_return_t tape_raw_data_block_bit_length_bits_in_last_byte_data_data_length_and_pause_getter_setter( void );
test_return_t tape_data_block_count_tail_length_level_data_and_bit_pulses_getter_setter( void );
test_return_t tape_group_start_block_text_getter_setter( void );
test_return_t tape_loop_start_block_count_getter_setter( void );
test_return_t tape_set_signal_level_block_level_getter_setter( void );
test_return_t rzx_alloc_and_free_lifecycle( void );
test_return_t rzx_start_input_stop_input_and_tstates_accessor( void );
test_return_t rzx_store_frame_and_iterator_get_frames_count( void );
test_return_t rzx_store_frame_repeat_frame_detection( void );
test_return_t rzx_add_snap_inserts_snapshot_block( void );
test_return_t rzx_iterator_begin_next_last_with_snap_and_input_blocks( void );
test_return_t rzx_get_keyid_returns_zero_with_no_signature_block( void );
test_return_t rzx_iterator_get_frames_returns_size_t_max_for_non_input_block( void );
test_return_t rzx_read_uncompressed_input_respects_declared_block_length( void );
test_return_t rzx_read_input_rejects_block_length_less_than_18( void );
test_return_t machine_capabilities_16k_and_48k_have_no_capabilities( void );
test_return_t machine_capabilities_48k_ntsc_has_ntsc_only( void );
test_return_t machine_capabilities_tc2048_has_timex_and_kempston( void );
test_return_t machine_capabilities_tc2068_has_ay_timex_and_dock( void );
test_return_t machine_capabilities_ts2068_has_ay_timex_dock_and_ntsc( void );
test_return_t machine_capabilities_128k_has_ay_and_128_memory( void );
test_return_t machine_capabilities_plus2_has_ay_128_memory_and_sinclair_joystick( void );
test_return_t machine_capabilities_plus2a_has_ay_128_plus3_memory_and_sinclair_joystick( void );
test_return_t machine_capabilities_plus3_and_plus3e_have_full_plus3_capabilities( void );
test_return_t machine_capabilities_128e_has_ay_128_plus3_memory_and_sinclair_joystick( void );
test_return_t machine_capabilities_pent_has_ay_128_memory_and_trdos( void );
test_return_t machine_capabilities_pent512_has_ay_128_trdos_and_pent512_memory( void );
test_return_t machine_capabilities_pent1024_has_ay_128_trdos_pent512_and_pent1024_memory( void );
test_return_t machine_capabilities_scorp_has_ay_128_trdos_scorp_memory_and_even_m1( void );
test_return_t machine_capabilities_se_has_ay_128_memory_timex_video_kempston_and_se_memory( void );
test_return_t machine_name_returns_correct_string_for_all_machines( void );
test_return_t joystick_name_returns_correct_string_for_all_types( void );
test_return_t snap_beta_disk_custom_rom_direction_and_fdc_byte_registers_getter_setter( void );
test_return_t snap_beta_disk_rom_single_pointer_getter_setter( void );
test_return_t snap_divmmc_ram_page_pointer_array_getter_setter( void );
test_return_t snap_tc2068_dock_exrom_active_cart_pointer_and_ram_flag_getter_setter( void );
test_return_t tape_comment_block_text_getter_setter( void );
test_return_t tape_message_block_text_and_pause_getter_setter( void );
test_return_t tape_archive_info_block_count_ids_and_texts_getter_setter( void );
test_return_t tape_hardware_block_count_types_ids_and_values_getter_setter( void );
test_return_t tape_select_block_count_offsets_and_texts_getter_setter( void );
test_return_t tape_custom_block_text_data_and_data_length_getter_setter( void );
test_return_t tape_jump_block_offset_getter_setter( void );
test_return_t tape_block_description_returns_correct_string_for_all_types( void );
test_return_t snap_custom_roms_pointer_array_and_rom_length_getter_setter( void );
test_return_t snap_slt_level_data_length_screen_and_screen_level_getter_setter( void );
test_return_t snap_multiface_model_3_getter_setter( void );
test_return_t snap_zxcf_active_upload_memctl_and_pages_getter_setter( void );
test_return_t snap_zxcf_ram_page_pointer_array_getter_setter( void );

/* utilities.c */
test_return_t utilities_zx_string_to_utf8_null_source_is_invalid( void );
test_return_t utilities_zx_string_to_utf8_plain_ascii( void );
test_return_t utilities_zx_string_to_utf8_trailing_spaces_stripped( void );
test_return_t utilities_zx_string_to_utf8_special_chars( void );
test_return_t utilities_zx_string_to_utf8_udg_char( void );
test_return_t utilities_zx_string_to_utf8_spectrum_token( void );
test_return_t utilities_zx_string_to_utf8_buffer_too_short_is_invalid( void );

/* szx.c (IF1) */
test_return_t write_szx_if1_chunk( void );
test_return_t read_szx_if1_chunk( void );

/* timings.c */
test_return_t timings_48k_processor_speed_and_frame_timing( void );
test_return_t timings_128k_processor_speed_and_frame_timing( void );
test_return_t timings_pentagon_processor_speed_and_frame_timing( void );
test_return_t timings_ts2068_processor_speed_and_frame_timing( void );
test_return_t timings_unknown_machine_returns_zero_for_all_frame_timings( void );

/* tape.c: tape_present and tape_clear */
test_return_t tape_present_returns_false_for_empty_tape( void );
test_return_t tape_present_true_after_load_and_false_after_clear( void );

/* identify.c */
test_return_t identify_class_unknown_returns_class_unknown( void );
test_return_t identify_class_tape_tap_returns_class_tape( void );
test_return_t identify_class_tape_tzx_returns_class_tape( void );
test_return_t identify_class_tape_pzx_returns_class_tape( void );
test_return_t identify_class_tape_csw_returns_class_tape( void );
test_return_t identify_class_snapshot_sna_returns_class_snapshot( void );
test_return_t identify_class_snapshot_szx_returns_class_snapshot( void );
test_return_t identify_class_snapshot_z80_returns_class_snapshot( void );
test_return_t identify_class_recording_rzx_returns_class_recording( void );
test_return_t identify_class_compressed_gz_returns_class_compressed( void );
test_return_t identify_class_compressed_bz2_returns_class_compressed( void );
test_return_t identify_class_disk_dsk_returns_class_disk_plus3( void );
test_return_t identify_class_disk_trd_returns_class_disk_trdos( void );
test_return_t identify_class_disk_mdr_returns_class_microdrive( void );
test_return_t identify_class_disk_img_returns_class_disk_plusd( void );
test_return_t identify_class_harddisk_hdf_returns_class_harddisk( void );
test_return_t identify_class_cartridge_dck_returns_class_cartridge_timex( void );
test_return_t identify_class_cartridge_if2_returns_class_cartridge_if2( void );
test_return_t identify_class_disk_udi_returns_class_disk_generic( void );
test_return_t identify_class_pok_returns_class_auxiliary( void );
test_return_t identify_class_screen_scr_returns_class_screenshot( void );
test_return_t identify_class_disk_opd_returns_class_disk_opus( void );
test_return_t identify_class_disk_d80_returns_class_disk_didaktik( void );

#endif
