include(FetchContent)

FetchContent_Declare(
    abseil
    # "Abseil recommends users "live-at-head" (update to the latest commit from the master branch as often as possible)."
    # Commits on Feb 17, 2022
    GIT_REPOSITORY  https://github.com/abseil/abseil-cpp.git
    GIT_TAG     7f850b3167fb38e6b4a9ce1824e6fabd733b5d62
)

# avoids a warning:
# A future Abseil release will default ABSL_PROPAGATE_CXX_STD to ON for CMake
# [build]   3.8 and up.  We recommend enabling this option to ensure your project still
# [build]   builds correctly.
option(ABSL_PROPAGATE_CXX_STD
  "Use CMake C++ standard meta features (e.g. cxx_std_11) that propagate to targets that link to Abseil"
  ON) # default to OFF

FetchContent_MakeAvailable(abseil)

# cmake --build . --target help | grep absl
set(ABSL_TARGETS_LIST
absl_bad_any_cast_impl
absl_bad_optional_access
absl_bad_variant_access
absl_base
absl_city
absl_civil_time
absl_cord
absl_cord_internal
absl_cordz_functions
absl_cordz_handle
absl_cordz_info
absl_cordz_sample_token
absl_debugging_internal
absl_demangle_internal
absl_examine_stack
absl_exponential_biased
absl_failure_signal_handler
absl_flags
absl_flags_commandlineflag
absl_flags_commandlineflag_internal
absl_flags_config
absl_flags_internal
absl_flags_marshalling
absl_flags_parse
absl_flags_private_handle_accessor
absl_flags_program_name
absl_flags_reflection
absl_flags_usage
absl_flags_usage_internal
absl_graphcycles_internal
absl_hash
absl_hashtablez_sampler
absl_int128
absl_leak_check
absl_leak_check_disable
absl_log_severity
absl_low_level_hash
absl_malloc_internal
absl_periodic_sampler
absl_random_distributions
absl_random_internal_distribution_test_util
absl_random_internal_platform
absl_random_internal_pool_urbg
absl_random_internal_randen
absl_random_internal_randen_hwaes
absl_random_internal_randen_hwaes_impl
absl_random_internal_randen_slow
absl_random_internal_seed_material
absl_random_seed_gen_exception
absl_random_seed_sequences
absl_raw_hash_set
absl_raw_logging_internal
absl_scoped_set_env
absl_spinlock_wait
absl_stacktrace
absl_status
absl_statusor
absl_str_format_internal
absl_strerror
absl_strings
absl_strings_internal
absl_symbolize
absl_synchronization
absl_throw_delegate
absl_time
absl_time_zone
)

foreach(ABSL_TARGET IN LISTS ABSL_TARGETS_LIST)
    set_target_properties(${ABSL_TARGET} PROPERTIES
      # disable auto-running clang-tidy
      CXX_CLANG_TIDY ""
  )
endforeach()
