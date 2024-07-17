#pragma once

#include <btu/bsa/archive.hpp>
#include <btu/bsa/settings.hpp>

void write_single_archive(const btu::Path &directory_path,
                          btu::bsa::Archive &&archive,
                          const btu::bsa::Settings &bsa_sets,
                          const bool remove_files,
                          const bool make_override);
