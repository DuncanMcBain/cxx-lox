project(
  'cxx_lox',
  ['cpp'],
  version: files('VERSION'),
  default_options: [
    'cpp_std=c++20',
  ],
)

absl_dep = dependency(
  'absl_strings', 'absl_inlined_vector', 'absl_base',
  fallback: 'absl',
  version: '>= 20211102',
)
fmt_dep = dependency('fmt', fallback: 'fmt')

subdir('src')
